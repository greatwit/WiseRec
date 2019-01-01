
#include "CameraListen.h"
#define TAG "Camera4"

using namespace android;

#define UNUSER(val) (void*)val

// connect to camera service
int JNICameraListen::CameraSetup(jint cameraId, jstring clientPackageName)
{
	UNUSER(clientPackageName); 
	
	if(mCamera == NULL)
		mCamera = Camera::connect(cameraId);

    if (mCamera == NULL) {
        ALOGV("Fail to connect to camera service");
        return -1;
    }

    // make sure camera hardware is alive
    if (mCamera->getStatus() != NO_ERROR) {
        ALOGV("Camera initialization failed");
        return -2;
    }

	// We use a weak reference so the Camera object can be garbage collected.
    // The reference is only used as a proxy for callbacks.

    mCamera->setListener(this);
	mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_CAMERA );

	return 0;
}

void JNICameraListen::setParameters(jstring params)
{
	String8 params8;
	if (params) {
		JNIEnv *env = AndroidRuntime::getJNIEnv();
		const jchar* str = env->GetStringCritical(params, 0);
		params8 = String8(str, env->GetStringLength(params)); //str is for android4,cast is for android6
		env->ReleaseStringCritical(params, str);
		
		if (mCamera->setParameters(params8) != NO_ERROR) {
			ALOGE("Camera setParameters failed");
		}
	}
}

void JNICameraListen::startPreview(const sp<Surface> &surface)
{
    ALOGV("startPreview");

	if(surface!=NULL && mCamera != NULL)
	{
		if (mCamera->setPreviewDisplay(surface) != NO_ERROR)
			ALOGE("Camera setPreviewDisplay failed");
	
		if (mCamera->startPreview() != NO_ERROR)
			ALOGE( "startPreview failed");
	}
}


