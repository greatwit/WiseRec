

#include <sys/system_properties.h>

#include "CameraEncodec.h"

#include "ComDefine.h"
#define TAG "CameraEncodec"

#define SAVE_FILE true

#if SAVE_FILE
FILE *mFile = NULL;
#endif

CameraEncodec::CameraEncodec()
	    :mbRunning(false)
		,mFirstFrame(true)
{ 
	GLOGI("function %s,line:%d construct.",__FUNCTION__,__LINE__);

#if SAVE_FILE
	mFile = fopen("/sdcard/camen.h264", "wb+");
#endif
}

CameraEncodec::~CameraEncodec()
{
	GLOGI("function %s,line:%d Destructor.",__FUNCTION__,__LINE__);

	#if SAVE_FILE
		if(mFile) fclose(mFile);
	#endif
}

bool CameraEncodec::CreateCodec(JNIEnv *env, jobject thiz, const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, short sendPort)
{
	return true;
}

bool CameraEncodec::CreateCodec(jobject thiz, const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, int cameraId, jstring clientPackageName)
{
	bool bResult = false;
    //读取sdk版本
    char szSdkVer[32]={0};
    __system_property_get("ro.build.version.sdk", szSdkVer);
    GLOGW("sdk:%d",atoi(szSdkVer));
	
	JNIEnv *env = AndroidRuntime::getJNIEnv();
	//jstring clientPackageName = env->NewStringUTF(clientName);
	bResult = CameraLib::getInstance()->LoadCameraLib(atoi(szSdkVer));
	if(bResult)
	{
		int camSet = CameraLib::getInstance()->CameraSetup(this, cameraId, clientPackageName);
		if(camSet>=0)
		{
			bResult = CodecBaseLib::getInstance()->CodecCreate(format, surface, crypto, flags, true);
			if(bResult)
				CodecBaseLib::getInstance()->RegisterBufferCall(this);
			else
				GLOGE("function %s,line:%d CodecCreate failed.", __FUNCTION__, __LINE__);
		}
		else
		{
			GLOGE("function %s,line:%d CameraSetup failed.", __FUNCTION__, __LINE__);
			return false;
		}
	}
	else
		GLOGE("function %s,line:%d LoadCameraLib failed.", __FUNCTION__, __LINE__);
	
	return bResult;
}

bool CameraEncodec::DeInit()
{	
	//StopVideo();
	//mCodec = NULL;
	CameraLib::getInstance()->CameraRelease();
	CameraLib::getInstance()->ReleaseLib();

	return true;
}

void CameraEncodec::SetDisplayOrientation(int value)
{
	CameraLib::getInstance()->SetDisplayOrientation(value);
}

void CameraEncodec::SetCameraParameter(jstring params)
{
	CameraLib::getInstance()->SetCameraParameter(params);
}

jstring CameraEncodec::GetCameraParameter()
{
    return CameraLib::getInstance()->GetCameraParameter();
}

bool CameraEncodec::StartVideo(const sp<Surface> &cameraSurf)
{
	//mCodec->startCodec();
	CodecBaseLib::getInstance()->StartCodec();
	
	CameraLib::getInstance()->StartPreview(cameraSurf);

	GLOGD("function %s,line:%d",__FUNCTION__,__LINE__);

	return true; 
}

bool CameraEncodec::StopVideo()
{
	GLOGW("function %s,line:%d StopVideo 0",__FUNCTION__,__LINE__);

	//mCodec->stopCodec();
	CameraLib::getInstance()->StopPreview();
	
	CodecBaseLib::getInstance()->StopCodec();

	GLOGD("function %s,line:%d StopVideo 2",__FUNCTION__,__LINE__);

	return true;
}

//camera frame callback
void CameraEncodec::VideoSource(VideoFrame *pBuf)
{
	GLOGW("function %s,line:%d len:%d", __FUNCTION__, __LINE__, pBuf->length);

	char* data = (char*)pBuf->addrVirY;
	int ylen   = pBuf->length*2/3;
	int uvlen  = ylen/2;
	char tmp   = '/0';
	for(int i=0; i<uvlen;) //NV21 to NV12
	{
		tmp 			= data[ylen+i];
		data[ylen+i] 	= data[ylen+i+1];
		data[ylen+i+1] 	= tmp;
		i+=2;
	}
	
#if SAVE_FILE
	//fwrite(data, 1, pBuf->length, mFile);
#endif

	CodecBaseLib::getInstance()->AddBuffer(data, pBuf->length);
}

//codec frame callback
void CameraEncodec::onCodecBuffer(struct CodecBuffer& buff)
{
	GLOGW("function %s,line:%d onCodecBuffer size:%d flags:%d", __FUNCTION__, __LINE__, buff.size, buff.flags);

	#if SAVE_FILE
		fwrite(buff.buf, 1, buff.size, mFile);
	#endif

	//mpSender->sendBuffer(buff.buf, buff.size, MIME_H264, MIME_H264_LEN, 0);
}

//upload camera framebuffer
void CameraEncodec::AddDecodecSource(char *data, int len)
{
	//mCodec->addBuffer(data, len);
}



