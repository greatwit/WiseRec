#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H


#include "IVideoCallback.h"


class BaseCamera
{
	public:	
		BaseCamera();
		virtual ~BaseCamera();
		virtual bool			InitCamera(int width, int height, int cameraId) = 0;
		virtual void			DeInitCamera() = 0;
		virtual int				StartCamera() = 0;
		virtual int				StopCamera() = 0;
		virtual void			SetPixelFormat(int format) = 0;
		virtual int				GetPixelFormat() = 0;

	public:
		void			SetFrameRate(int rate);
		int				GetFrameRate();
		void			RegisterCallback(IVideoCallback *base);

	public:
		int						mSleepTime;
		int						mPixelFomat;
		IVideoCallback			*mStreamBase;

	protected:
		bool					mCapturing;
		bool					mInited;
};


#endif // CAMERA_HARDWARE_H
