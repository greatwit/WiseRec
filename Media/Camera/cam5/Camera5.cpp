
#include "CameraListen.h"
#define TAG "Camera4"

using namespace android;

// connect to camera service
int JNICameraListen::CameraSetup(jint cameraId, jstring clientPackageName)//"com.greatmedia"
{
	if(mCamera == NULL)
	{
		JNIEnv *env = AndroidRuntime::getJNIEnv();
		// Convert jstring to String16
		const char16_t *rawClientName = reinterpret_cast<const char16_t*>(
										env->GetStringChars(clientPackageName, NULL));
		jsize rawClientNameLen = env->GetStringLength(clientPackageName);
	
		String16 clientName(rawClientName, rawClientNameLen);
		env->ReleaseStringChars(clientPackageName,
								reinterpret_cast<const jchar*>(rawClientName));
		mCamera = Camera::connect(cameraId, clientName, Camera::USE_CALLING_UID);
	}

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
	//if(gListenContext == NULL)
	//	gListenContext = new JNICameraListen(camera);

    mCamera->setListener(this);
	mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER);

	return 0;
}

void JNICameraListen::setParameters(jstring params)
{
	String8 params8;
	if (params) {
		JNIEnv *env = AndroidRuntime::getJNIEnv();
		const jchar* str = env->GetStringCritical(params, 0);
		params8 = String8(reinterpret_cast<const char16_t*>(str), env->GetStringLength(params)); //str is for android4,cast is for android6
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
		if (mCamera->setPreviewTarget(surface->getIGraphicBufferProducer()) != NO_ERROR)
			ALOGE("Camera setPreviewDisplay failed");
	
    if (mCamera->startPreview() != NO_ERROR)
        ALOGE( "startPreview failed");
}


