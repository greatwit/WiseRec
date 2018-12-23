

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <cutils/properties.h>
#include <utils/Vector.h>

#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>


#include "CameraApi.h"
#define TAG "CameraApi"

using namespace android;

// provides persistent context for calls from native code to Java
class JNICameraListen: public CameraListener
{
	public:
		JNICameraListen(IVideoCallback *callback); 
		~JNICameraListen() { }
		sp<Camera> getCamera();
		
		virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2);
		virtual void postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata);
		virtual void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);
		
		
		// connect to camera service
		int CameraSetup(jint cameraId, jstring clientPackageName);
		// disconnect from camera service
		// It's okay to call this when the native camera context is already null.
		// This handles the case where the user has called release() and the
		// finalizer is invoked later.
		int CameraRelease();

		void SetCameraParameter(jstring params);
		jstring GetCameraParameter();
		void StartPreview(const sp<Surface> &surface);
		void StopPreview();

	private:
		sp<Camera>  mCamera;                // strong reference to native object
		IVideoCallback *mCallback;

};

JNICameraListen::JNICameraListen(IVideoCallback *callback):mCamera(NULL)
{
	mCallback = callback;
}

sp<Camera> JNICameraListen::getCamera()
{
	return mCamera;
}

void JNICameraListen::notify(int32_t msgType, int32_t ext1, int32_t ext2)
{
    ALOGV("notify");
}

void JNICameraListen::postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata)
{
	/*
    // VM pointer will be NULL if object is released
    JNIEnv *env = AndroidRuntime::getJNIEnv();


    int32_t dataMsgType = msgType & ~CAMERA_MSG_PREVIEW_METADATA;

    // return data based on callback type
    switch (dataMsgType) {
        case CAMERA_MSG_VIDEO_FRAME:
            // should never happen
            break;

        // For backward-compatibility purpose, if there is no callback
        // buffer for raw image, the callback returns null.
        case CAMERA_MSG_RAW_IMAGE:
            break;

        // There is no data.
        case 0:
            break;

        default:
            ALOGV("dataCallback(%d, %p)", dataMsgType, dataPtr.get());
            break;
    }

    // post frame metadata to Java
    if (metadata && (msgType & CAMERA_MSG_PREVIEW_METADATA)) {
        //postMetadata(env, CAMERA_MSG_PREVIEW_METADATA, metadata);
    }
	*/
	if (dataPtr != NULL) {
		ssize_t offset;
		size_t size;
		sp<IMemoryHeap> heap = dataPtr->getMemory(&offset, &size);
		
		uint8_t *heapBase = (uint8_t*)heap->base();
		if (heapBase != NULL)
		{
			//const char* data = reinterpret_cast<const char*>(heapBase + offset);
			if(mCallback)
			{
				V4L2BUF_t buff;
				buff.addrVirY = (unsigned int)heap->base();
				buff.length   = size;
				mCallback->VideoSource(&buff);
			}
			
			//CodecBaseLib::getInstance()->AddBuffer((char*)data, size);
		}
		
		//ALOGE("postData: off=%ld, size=%d", offset, size);
		mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER);
	}
}

void JNICameraListen::postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr)
{
    // TODO: plumb up to Java. For now, just drop the timestamp
}



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

// disconnect from camera service
// It's okay to call this when the native camera context is already null.
// This handles the case where the user has called release() and the
// finalizer is invoked later.
int JNICameraListen::CameraRelease()
{
    // TODO: Change to ALOGV
    ALOGV("release camera");

	// clear callbacks
	if (mCamera != NULL) {
		mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_NOOP);
		mCamera->disconnect();
		mCamera.clear();
		mCamera = NULL;
	}

	// remove context to prevent further Java access
	//context->decStrong(thiz);
	
	return 0;
}

void JNICameraListen::SetCameraParameter(jstring params)
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

jstring JNICameraListen::GetCameraParameter()
{
	String8 params8 = mCamera->getParameters();
	if (params8.isEmpty()) {
        ALOGE("getParameters failed (empty parameters)");
		return NULL;
    }
	JNIEnv *env = AndroidRuntime::getJNIEnv();
    return env->NewStringUTF(params8.string());
}

void JNICameraListen::StartPreview(const sp<Surface> &surface)
{
    ALOGV("startPreview");

	if(surface!=NULL && mCamera != NULL)
		if (mCamera->setPreviewTarget(surface->getIGraphicBufferProducer()) != NO_ERROR)
			ALOGE("Camera setPreviewDisplay failed");
	
    if (mCamera->startPreview() != NO_ERROR)
        ALOGE( "startPreview failed");
}

void JNICameraListen::StopPreview()
{
    ALOGV("stopPreview");

    mCamera->stopPreview();
}

////////////////////////////////////////////////////////////////////////////api for opendl//////////////////////////////////////////////////////////////////////////
sp<JNICameraListen> gListenContext = NULL;

int CameraSetup(IVideoCallback *callback, jint cameraId, jstring clientPackageName)
{
	if(gListenContext == NULL)
		gListenContext = new JNICameraListen(callback);
	
	return gListenContext->CameraSetup(cameraId, clientPackageName);
}

int CameraRelease()
{
	if(gListenContext != NULL)
	{
		gListenContext->CameraRelease();
		gListenContext = NULL;
		return 0;
	}
		
	return -1;
}

void SetCameraParameter(jstring params)
{
	if(gListenContext != NULL)
		gListenContext->SetCameraParameter(params);
}

jstring GetCameraParameter()
{
	if(gListenContext != NULL)
		return gListenContext->GetCameraParameter();
	else
		return NULL;
}

void StartPreview(const sp<Surface> &surface)
{
	if(gListenContext != NULL)
		gListenContext->StartPreview(surface);
}

void StopPreview()
{
	if(gListenContext != NULL)
		gListenContext->StopPreview();
}



