#ifndef _CAMERA_CONTEXT_H_
#define _CAMERA_CONTEXT_H_

#include <stdlib.h>

//#include <gui/Surface.h>
#include <jni.h>//#include "android_runtime/AndroidRuntime.h"

#include "IVideoCallback.h"

//using namespace android;

typedef int CameraSetup_t(IVideoCallback *callback, jint cameraId, jstring clientPackageName);
typedef int CameraRelease_t();
typedef void SetCameraParameter_t(jstring params);
typedef jstring GetCameraParameter_t();
//typedef void StartPreview_t(const sp<Surface> &surface);
typedef void StartPreview_t(const void* window);
typedef void StopPreview_t();
typedef void SetDisplayOrientation_t(int value);

//namespace android
//{

class CameraContext
{
	public:
		CameraContext();
		~CameraContext();
		static CameraContext*getInstance();
		bool LoadCameraContext(int version);
		bool ReleaseLib();
		
		CameraSetup_t* 				CameraSetup;
		CameraRelease_t*			CameraRelease;
		SetCameraParameter_t*		SetCameraParameter;
		GetCameraParameter_t* 		GetCameraParameter;
		StartPreview_t*				StartPreview;
		StopPreview_t*				StopPreview;
		SetDisplayOrientation_t*	SetDisplayOrientation;

		
	private:
		static CameraContext*mSelf;
		void *mLibHandle;
};

//}

#endif
