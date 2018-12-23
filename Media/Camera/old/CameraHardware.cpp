

#include <fcntl.h> 
#include <sys/mman.h>
//#include <linux/videodev.h>
#include "videodev2.h"
#include <utils/threads.h>
#include <utils/Log.h>

#include "CameraHardware.h"


#define NB_BUFFER 5

#define V4L2_MODE_VIDEO				0x0002	/*  For video capture */
#define V4L2_MODE_IMAGE				0x0003	/*  For image capture */


#define CHECK_ERROR(err)												\
		if ((err) != OK)												\
		{																\
			ALOGE("CameraHardware ERROR: %s, line: %d", __FUNCTION__, __LINE__);		\
			return UNKNOWN_ERROR;										\
		}

namespace android 
{  

CameraHardware::CameraHardware() :
							BaseCamera(),
							mFirstFrame(true),
							mFrameId(0),
							mV4l2Handle(0),
							mBufferCnt(NB_BUFFER),
							mVideoWidth(1280),
							mVideoHeight(720),
							mTvdFlag(false),
							mTakingPicture(false),
							mCapturing(false),
							mCheckInitOK(false)
{
	memset(&mMapMem, 0, sizeof(v4l2_mem_map_t));
	ALOGV("mVideoWidth: %d, mVideoHeight: %d", mVideoWidth, mVideoHeight);
}

CameraHardware::~CameraHardware()
{
	ALOGV("CameraHardware, Destructor");
}

bool CameraHardware::InitCamera(int width, int height, int deviveid)
{
	int result = -1;
	if(checkInit())
		return true;

	mSleepTime = 1000*1000/VIDEO_FRAMERATE;
	mDeviceID  = deviveid;
	
	mVideoWidth  = width;
	mVideoHeight = height;
	
	if(OK == openCameraDev())
	{
		// 设置摄像头采集帧的宽／高, tryFmtSize可能修改mARect值
		// 在回显时，当UI的宽，高与mARect不一致时，要进行缩放
		if(OK != tryFmtSize(&width, &height))
		{
			ALOGE("mpCameraSource->tryFmtSize Failed,function %s,line:%d ", __FUNCTION__, __LINE__);
			return result;
		}
	}
	else
	{
		ALOGE("openCameraDev() Failed,function %s,line:%d",__FUNCTION__,__LINE__);
		return false;
	}

	result = v4l2Init();
	if(result <0)
		CHECK_ERROR(result);

	mCheckInitOK = true;

	return true;
}

void CameraHardware::DeInitCamera()
{
	closeCameraDev();
	mCheckInitOK = false;
}

bool CameraHardware::checkInit()
{
	return mCheckInitOK;
}

int CameraHardware::StartCamera( )
{
	ALOGE("TAG Enter startCamera:function %s,line:%d",__FUNCTION__,__LINE__);
	if(false == checkInit())
	{
		ALOGW("CameraHardware::startCamera: camera do not init 1.");
		return 0;
	}

	mFrameId = 0;

	startThread();

	ALOGE("TAG LEAVE startCamera:function %s,line:%d",__FUNCTION__,__LINE__);
	return OK;
}

int CameraHardware::StopCamera()
{	
	if(false == checkInit())
	{
		ALOGW("CameraHardware::startCamera: camera do not init 2.");
		return 0;
	}

	stopThread();		

	mFirstFrame = true;
	return OK;
}

void CameraHardware::SetPixelFormat(int format)
{
	mPixelFomat = format;
}
		
int CameraHardware::GetPixelFormat()
{
	return mPixelFomat;
}

int	CameraHardware::startThread()
{
	mCapturing = true;
	ALOGE("TAG 11:function %s,line:%d",__FUNCTION__,__LINE__);
	run("CameraHardware", PRIORITY_URGENT_DISPLAY);
	ALOGE("TAG 12:function %s,line:%d",__FUNCTION__,__LINE__);
	return 0;
}

void CameraHardware::stopThread()
{
	mCapturing = false;
	requestExitAndWait();
}

int	CameraHardware::getVideoWidth()
{
	return mVideoWidth;
}

int	CameraHardware::getVideoHeight()
{
	return mVideoHeight;
}

int CameraHardware::closeCameraDev()
{
	v4l2DeInit();
	if (mV4l2Handle != NULL)
	{
		close(mV4l2Handle);
		mV4l2Handle = NULL;
	}
	return OK;
}

int CameraHardware::videoCaptureGetFrame()
{
	int ret = UNKNOWN_ERROR;
	ret = v4l2WaitCamerReady();

	if (ret != 0) 
	{
		ALOGE("wait time out");
		return __LINE__;
	}

	// get one video frame
	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(v4l2_buffer));
	ret = getPreviewFrame(&buf);
	if (ret != OK)
	{
		usleep(10*1000);
		return ret;
	}

	mCurFrameTimestamp = (int64_t)((int64_t)buf.timestamp.tv_usec + (((int64_t)buf.timestamp.tv_sec) * 1000000));

	if (-1 == ioctl(mV4l2Handle, VIDIOC_QBUF, &buf))
	{
		ALOGE("VIDIOC_QBUF failed");
	}

	return OK;
}

void CameraHardware::videoCaptureReleaseFrame(int index)
{
	int ret = UNKNOWN_ERROR;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(v4l2_buffer));

	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index  = index;

	ALOGV("videoCaptureReleaseFrame,buf.index: %d", buf.index);
	ret = ioctl(mV4l2Handle, VIDIOC_QBUF, &buf);
	if (ret != 0)
	{
		// comment for temp, to do
		ALOGE("releasePreviewFrame: VIDIOC_QBUF Failed: index = %d, ret = %d, %s",
				buf.index, ret, strerror(errno));
	}
}

// check v4l2 device capabilities
int CameraHardware::v4l2CheckCapability()
{
	int ret = -1;
	struct v4l2_capability cap; 
	ret = ioctl (mV4l2Handle, VIDIOC_QUERYCAP, &cap); 
	if (ret < 0)
	{ 
		ALOGE("Error opening device: unable to query device.");
		return -1;
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
	{ 
		ALOGE("Error opening device: video capture not supported.");
		return -1;
	}

	if ((cap.capabilities & V4L2_CAP_STREAMING) == 0)
	{ 
		ALOGE("Capture device does not support streaming i/o");
		return -1;
	}


	if(strcmp((char*)cap.driver, "sun4i_csi") == 0) 
	{
		ALOGD("cap.driver: %s", cap.driver);
	} 
	else 
	{
		//mTvdFlag = true;
		mTvdFlag = false;
	}
	return 0;
}

int CameraHardware::v4l2Init()
{
	int iResult = 0;
	struct v4l2_input inp;
	inp.index = 0;
	if (-1 == ioctl (mV4l2Handle, VIDIOC_S_INPUT, &inp))
	{
		ALOGE("VIDIOC_S_INPUT error!\n");
		return -1;
	}

	// check v4l2 device capabilities
	v4l2CheckCapability();

	iResult = v4l2setCaptureParams(mTakingPicture);
	if(iResult<0)
	{
		CHECK_ERROR(iResult);
		return iResult;
	}

	// set v4l2 device parameters
	iResult = v4l2SetVideoParams();
	if(iResult<0)
	{
		CHECK_ERROR(iResult);
		return iResult;
	}

	// v4l2 request buffers
	iResult = v4l2ReqBufs();
	if(iResult<0)
	{
		CHECK_ERROR(iResult);
		return iResult;
	}

	// v4l2 query buffers
	iResult = v4l2QueryBuf();
	if(iResult<0)
	{
		CHECK_ERROR(iResult);
		return iResult;
	}

	// stream on the v4l2 device
	iResult = v4l2StartStreaming();
	if(iResult<0)
	{
		CHECK_ERROR(iResult);
		return iResult;
	}

	ALOGD("start camera ok");
	return OK;
}

void CameraHardware::v4l2DeInit()
{
	// v4l2 device stop stream
	v4l2StopStreaming();

	// v4l2 device unmap buffers
	v4l2UnmapBuf();
}

int CameraHardware::openCameraDev()
{
	//struct v4l2_input inp;
	// open V4L2 device

	if(mDeviceID == 0)
	{
		mV4l2Handle = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
	}
	else
	{
		mV4l2Handle = open("/dev/video1", O_RDWR | O_NONBLOCK, 0);
	}

	if (mV4l2Handle == -1) 
	{ 
		ALOGE("ERROR opening V4L interface: %s", strerror(errno));
		return -1; 
	} 

	ALOGV("mDeviceID:%d, mV4l2Handle:%d", mDeviceID, mV4l2Handle);

	return OK;
}

bool CameraHardware::threadLoop()
{
	getCaptureFrame();
	return true;
}

void CameraHardware::setFrameRate(int frame)
{
		if(0<frame&&frame<50)
			mSleepTime = 1000*1000/frame;
}

int	CameraHardware::getCaptureFrame()
{
	while (mCapturing) 
	{
		ALOGE("TAG 50:function %s,line:%d",__FUNCTION__,__LINE__);
		int ret = UNKNOWN_ERROR;
		ret = v4l2WaitCamerReady();

		usleep(mSleepTime);
		if(false == mCapturing)
			return 0;


		if (ret != 0) 
		{
			ALOGE("wait time out");
			return __LINE__;
		}
		ALOGE("TAG 51:function %s,line:%d",__FUNCTION__,__LINE__);
		// get one video frame
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(v4l2_buffer));
		ret = getPreviewFrame(&buf);
		if (ret != OK)
		{
			usleep(10*1000);
			return ret;
		}
		
		mCurFrameTimestamp = (int64_t)((int64_t)buf.timestamp.tv_usec + (((int64_t)buf.timestamp.tv_sec) * 1000000));

		V4L2BUF_t Buf;
		Buf.addrPhyY = buf.m.offset;//(int)mMapMem.mem[buf.index];//
		Buf.length   = buf.length;
		if(mStreamBase != NULL && Buf.length>1 )
		{
			mStreamBase->VideoSource(&Buf);
		}
		
		ALOGE("TAG 52:function %s,line:%d buffer length:%d ",__FUNCTION__,__LINE__, Buf.length);

		if(-1 == ioctl(mV4l2Handle, VIDIOC_QBUF, &buf))
		{
			ALOGE("VIDIOC_QBUF failed");
		}
	}
	 ALOGE("CameraHardware, mCurFrameTimestamp: %lld", mCurFrameTimestamp);
	return OK;
}
 
int CameraHardware::v4l2SetVideoParams()
{
	int ret = UNKNOWN_ERROR;
	struct v4l2_format format;
	int system;

	ALOGI("%s, line: %d, w: %d, h: %d, pfmt: %d", 
			__FUNCTION__, __LINE__, mVideoWidth, mVideoHeight, mPixelFomat);

	if(mTvdFlag)
	{
		memset(&format, 0, sizeof(format));
		format.type                = V4L2_BUF_TYPE_PRIVATE;
		if (-1 == ioctl (mV4l2Handle, VIDIOC_G_FMT, &format)) 
		{
			ALOGE("VIDIOC_G_FMT error!  a\n");
			ret = -1;
			return ret; 
		}
		if((format.fmt.raw_data[16 ] & 1) == 0)
		{
			ALOGE("format.fmt.raw_data[16], error");
			ret = -1;
			return ret; 
		}
		system = (format.fmt.raw_data[16]  & (1 << 4) ) != 0 ? 1: 0;

		memset(&format, 0, sizeof(format));
		format.type                = V4L2_BUF_TYPE_PRIVATE;
		format.fmt.raw_data[0] =0;//interface
		format.fmt.raw_data[1] = system;//system

		format.fmt.raw_data[2] =0;//format 1=mb, for test only		
		format.fmt.raw_data[8] =1;//row
		format.fmt.raw_data[9] =1;//column

		format.fmt.raw_data[10] =1;//channel_index
		format.fmt.raw_data[11] =0;//channel_index
		format.fmt.raw_data[12] =0;//channel_index
		format.fmt.raw_data[13] =0;//channel_index

		if (-1 == ioctl (mV4l2Handle, VIDIOC_S_FMT, &format)) //�����Զ���
		{
			ALOGE("VIDIOC_S_FMT error!  a\n");
			ret = -1;
			return ret; 
		}
	}

	// try to support this format: NV21, YUYV
	// we do not support mjpeg camera now
	if(mPixelFomat<0)
	{
		if (getFormatCapture(V4L2_PIX_FMT_NV21) == OK)
		{
			mPixelFomat = V4L2_PIX_FMT_NV21;
			ALOGV("capture format: V4L2_PIX_FMT_NV21");
		}
		else if(getFormatCapture(V4L2_PIX_FMT_NV12) == OK)
		{
			mPixelFomat = V4L2_PIX_FMT_NV12;
			ALOGV("capture format: V4L2_PIX_FMT_NV12");
		}
		else if(getFormatCapture(V4L2_PIX_FMT_YUYV) == OK)
		{
			mPixelFomat = V4L2_PIX_FMT_YUYV;		// maybe usb camera
			ALOGV("capture format: V4L2_PIX_FMT_YUYV");
		}
		else
		{
			ALOGE("driver should surpport NV21/NV12 or YUYV format, but it not!");//V4L2_PIX_FMT_NV16
			ret = -1;
			return ret;
		}
	}


	tryFmtSize(&mVideoWidth,&mVideoHeight);
	
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width  = mVideoWidth;
	format.fmt.pix.height = mVideoHeight;
	format.fmt.pix.pixelformat = mPixelFomat;
	format.fmt.pix.field = V4L2_FIELD_SEQ_BT;//V4L2_FIELD_NONE;

	ret = ioctl(mV4l2Handle, VIDIOC_S_FMT, &format); 
	if (ret < 0) 
	{ 
		ALOGE("VIDIOC_S_FMT Failed: %s", strerror(errno)); 
		return ret; 
	} 

	mVideoWidth = format.fmt.pix.width;
	mVideoHeight= format.fmt.pix.height;
	ALOGV("camera params: w: %d, h: %d, pfmt: %d, pfield: %d", 
			mVideoWidth, mVideoHeight, mPixelFomat, V4L2_FIELD_NONE);

	return OK;
}

/**
 *  向驱动申请帧缓冲，一般不超过5个
 *		mBufferCnt = NB_BUFFER;
 *		V4L2_MEMORY_MMAP　为内存类型
 */
int CameraHardware::v4l2ReqBufs()
{
	int ret = UNKNOWN_ERROR;
	struct v4l2_requestbuffers rb; 

	if (mTakingPicture)
	{
		mBufferCnt = 1;
	}
	else
	{
		mBufferCnt = NB_BUFFER;
	}

	ALOGV("TO VIDIOC_REQBUFS count: %d", mBufferCnt);

	memset(&rb, 0, sizeof(rb));
	rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	rb.memory = V4L2_MEMORY_MMAP;
	rb.count  = mBufferCnt;

	ret = ioctl(mV4l2Handle, VIDIOC_REQBUFS, &rb); 
	if (ret < 0)
	{ 
		ALOGE("Init: VIDIOC_REQBUFS failed: %s", strerror(errno));
		return ret;
	}

	if (mBufferCnt != rb.count)
	{
		mBufferCnt = rb.count;
		ALOGV("VIDIOC_REQBUFS count: %d", mBufferCnt);
	}

	return OK;
}

/***
 *   v4l2QueryBuf
 *
 *　　把VIDIOC_REQBUFS中分配的数据缓存物理地址转换成虚拟地址
 *
 */

int CameraHardware::v4l2QueryBuf()
{
	int ret = UNKNOWN_ERROR;
	struct v4l2_buffer buf;

	for (int i = 0; i < mBufferCnt; i++) 
	{  
		memset (&buf, 0, sizeof (struct v4l2_buffer));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
		buf.memory = V4L2_MEMORY_MMAP; 
		buf.index  = i; 

		ret = ioctl (mV4l2Handle, VIDIOC_QUERYBUF, &buf); 
		if (ret < 0)
		{ 
			ALOGE("Unable to query buffer (%s)", strerror(errno));
			return ret;
		}

		mMapMem.mem[i] = mmap(0, buf.length,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				mV4l2Handle,
				buf.m.offset);
		mMapMem.length = buf.length;
		ALOGI("index: %d, mem: %x, len: %d, offset: %x", i, (int)mMapMem.mem[i], buf.length, buf.m.offset);

		if (mMapMem.mem[i] == MAP_FAILED)
		{ 
			ALOGE("Unable to map buffer (%s)", strerror(errno)); 
			return -1;
		}

		// start with all buffers in queue
		ret = ioctl(mV4l2Handle, VIDIOC_QBUF, &buf);
		if (ret < 0)
		{ 
			ALOGE("VIDIOC_QBUF Failed");
			return ret;
		}
	} 

	return OK;
}

int CameraHardware::v4l2setCaptureParams(bool bTakePicture)
{
	int ret = -1;

		// set capture mode
	struct v4l2_streamparm params;
	params.parm.capture.timeperframe.numerator = 1;
	params.parm.capture.timeperframe.denominator = 30;
	params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (bTakePicture)
	{
		params.parm.capture.capturemode = V4L2_MODE_IMAGE;
	}
	else
	{
		params.parm.capture.capturemode = V4L2_MODE_VIDEO;
	}

	ret = ioctl(mV4l2Handle, VIDIOC_S_PARM, &params);
	if (ret < 0)
		ALOGE("v4l2setCaptureParams failed!");
	else 
		ALOGV("v4l2setCaptureParams ok");

	return ret;
}

int CameraHardware::v4l2StartStreaming()
{
	int ret = UNKNOWN_ERROR; 
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 

	ret = ioctl (mV4l2Handle, VIDIOC_STREAMON, &type);
	if (ret < 0) 
	{ 
		ALOGE("StartStreaming: Unable to start capture: %s", strerror(errno)); 
		return ret; 
	} 

	return OK;
}

int CameraHardware::v4l2StopStreaming()
{
	int ret = UNKNOWN_ERROR; 
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 

	ret = ioctl (mV4l2Handle, VIDIOC_STREAMOFF, &type); 
	if (ret < 0) 
	{ 
		ALOGE("StopStreaming: Unable to stop capture: %s", strerror(errno)); 
		return ret; 
	} 
	ALOGD("V4L2Camera::v4l2StopStreaming OK");

	return OK;
}

int CameraHardware::v4l2UnmapBuf()
{
	int ret = UNKNOWN_ERROR;

	for (int i = 0; i < mBufferCnt; i++) 
	{
		ret = munmap(mMapMem.mem[i], mMapMem.length);
		if (ret < 0)
		{
			ALOGE("v4l2CloseBuf Unmap failed");
			return ret;
		}
	}

	return OK;
}

int CameraHardware::v4l2WaitCamerReady()
{
	fd_set fds;		
	struct timeval tv;
	int r;
	 
	FD_ZERO(&fds);
	FD_SET(mV4l2Handle, &fds);		

	/* Timeout */
	tv.tv_sec  = 2;
	tv.tv_usec = 0;

	r = select(mV4l2Handle + 1, &fds, NULL, NULL, &tv);

	if (r == -1) 
	{
		ALOGE("select err");
		return -1;
	} 
	else if (r == 0) 
	{
		ALOGE("select timeout");
		return -1;
	}

	return 0;
}

int CameraHardware::getPreviewFrame(v4l2_buffer *buf)
{
	int ret = UNKNOWN_ERROR;

	buf->type   = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	buf->memory = V4L2_MEMORY_MMAP;

	ret = ioctl(mV4l2Handle, VIDIOC_DQBUF, buf);
	ALOGD("ioctl result:%d", ret);
	if (ret < 0)
	{ 
		switch (errno)  
		{  
			case EAGAIN:  
				ALOGD("GetPreviewFrame: VIDIOC_DQBUF Failed:EAGAIN");
				break;  
			case EIO:  
				/* Could ignore EIO, see spec. */  
				/* fall through */  
				ALOGD("GetPreviewFrame: VIDIOC_DQBUF Failed:EIO");
				break;  
			case EINVAL: 
				/* Could ignore EIO, see spec. */  
				/* fall through */  
				ALOGD("GetPreviewFrame: VIDIOC_DQBUF Failed:EINVAL");
				break;  
			case EPIPE: 
				/* Could ignore EIO, see spec. */  
				/* fall through */  
				ALOGD("GetPreviewFrame: VIDIOC_DQBUF Failed:EPIPE");
				break; 
			default:  
				ALOGD("GetPreviewFrame: VIDIOC_DQBUF Failed:default");
				//errno_exit("VIDIOC_DQBUF"); 
			break;  		  
		}
		ALOGE("GetPreviewFrame: VIDIOC_DQBUF Failed");
		return __LINE__; 			// can not return false
	}
	return OK;
}

int CameraHardware::tryFmtSize(int * width, int * height)
{
	int ret = -1;
	struct v4l2_format fmt;

	ALOGV("V4L2Camera::TryFmtSize: w: %d, h: %d", *width, *height);

	memset(&fmt, 0, sizeof(fmt));

	//get pixelformat
	if (getFormatCapture(V4L2_PIX_FMT_NV21) == OK)
	{
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21;
		ALOGV("capture format: V4L2_PIX_FMT_NV21");
	}
	else if(getFormatCapture(V4L2_PIX_FMT_YUYV) == OK)
	{
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;		// maybe usb camera
		ALOGV("capture format: V4L2_PIX_FMT_YUYV");
	}
	else if(getFormatCapture(V4L2_PIX_FMT_NV12) == OK)
	{
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
		ALOGV("capture format: V4L2_PIX_FMT_NV12");
	}
	else
	{
		ALOGE("driver should surpport NV21/NV12 or YUYV format, but it not!");
		ret = -1;
		return ret;
	}
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width  = *width;
	fmt.fmt.pix.height = *height;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	ret = ioctl(mV4l2Handle, VIDIOC_TRY_FMT, &fmt); 
	if (ret < 0) 
	{ 
		ALOGE("VIDIOC_TRY_FMT Failed: %s", strerror(errno)); 
		return ret; 
	} 

	// driver surpport this size
	*width = fmt.fmt.pix.width; 
	*height = fmt.fmt.pix.height;

	return OK;
}

int CameraHardware::setWhiteBalance(int wb)
{
	struct v4l2_control ctrl;
	int ret = -1;

	ctrl.id = V4L2_CID_DO_WHITE_BALANCE;
	ctrl.value = wb;
	ret = ioctl(mV4l2Handle, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0)
		ALOGV("setWhiteBalance failed!");
	else 
		ALOGV("setWhiteBalance ok");

	return ret;
}

int CameraHardware::setExposure(int exp)
{
	int ret = -1;
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_EXPOSURE;
	ctrl.value = exp;
	ret = ioctl(mV4l2Handle, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0)
		ALOGV("setExposure failed!");
	else 
		ALOGV("setExposure ok");

	return ret;
}

/**
 *  	判断当期摄像头支持的格式
 *      当前USB摄像头支持YUYV（4:2:2）,JPEG
 */
int CameraHardware::getFormatCapture(int format)
{
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i = 0; i < 12; i++)
	{
		fmtdesc.index = i;
		if (-1 == ioctl(mV4l2Handle, VIDIOC_ENUM_FMT, &fmtdesc))
		{
			break;
		}
		ALOGV("format index = %d, name = %s, v4l2 pixel format = %x\n",
				i, fmtdesc.description, fmtdesc.pixelformat);

		if (fmtdesc.pixelformat == format)
		{
			return OK;
		}
	}

	return -1;
}

}; // namespace android


