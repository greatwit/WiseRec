#ifndef CAMERA_HARDWARE_H
#define CAMERA_HARDWARE_H


#include "capture.h"
#include "videodev2.h"


#include "BaseCamera.h"

#define TAG "CameraHardware"
#include "ComDefine.h"

namespace android 
{

class CameraHardware: public Thread, public BaseCamera
{
	public:	
		CameraHardware();
		~CameraHardware();

		bool			InitCamera(int width, int height, int deviveid);
		void			DeInitCamera();
		
		int				StartCamera();
		int				StopCamera();
		
		void			SetPixelFormat(int format);
		int				GetPixelFormat();
	

	protected:
		int				getVideoWidth();
		int				getVideoHeight();
		void 			setFrameRate(int frame);
		
		int				openCameraDev();
		int				closeCameraDev();

		int				tryFmtSize(int * width, int * height);
		int				getFormatCapture(int format);
		int				setWhiteBalance(int wb);
		int				setExposure(int exp);


		void			videoCaptureReleaseFrame(int index);

		int				startThread();
		void			stopThread();
		virtual bool	threadLoop();
		int				getCaptureFrame();

	private:
		bool			checkInit();
	
		int						v4l2Init();
		void					v4l2DeInit();


		int                     v4l2CheckCapability();
		int 					v4l2SetVideoParams();
		int 					v4l2ReqBufs();
		int 					v4l2QueryBuf();
		int  					v4l2setCaptureParams(bool bTakePicture);
		int 					v4l2StartStreaming(); 
		int 					v4l2StopStreaming(); 
		int 					v4l2UnmapBuf();
		int						getFrameRate();								

		int						v4l2WaitCamerReady();
		int						getPreviewFrame(v4l2_buffer *buf);
		int						videoCaptureGetFrame();

		int						mDeviceID;
		bool					mCheckInitOK;
		bool					mFirstFrame;
		int						mFrameId;

		int						mVideoWidth;
		int						mVideoHeight;

		int						mV4l2Handle;
	
		// actually buffer counts
		int						mBufferCnt;
		int						mSleepTime;

	
		typedef struct v4l2_mem_map_t
		{
			void *	mem[5];
			int 	length;
		}v4l2_mem_map_t;
		v4l2_mem_map_t			mMapMem;
	
		bool					mCameraStarted;
		int                     overlayStarted;
		int                     videoLayer;

		bool					mOverlayFirstFrame;
		int64_t 				mCurFrameTimestamp;
		bool 					mTakingPicture;
		bool					mTvdFlag;

		bool					mCapturing;
};

};  // namespace android

#endif // CAMERA_HARDWARE_H
