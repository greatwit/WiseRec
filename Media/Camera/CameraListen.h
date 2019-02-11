
#ifndef CAMERA_LISTEN_H
#define CAMERA_LISTEN_H

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <cutils/properties.h>
#include <utils/Vector.h>


#include <gui/Surface.h>
#include <camera/Camera.h>
#include <binder/IMemory.h>

#include "IVideoCallback.h"

using namespace android;

// provides persistent context for calls from native code to Java
class JNICameraListen: public CameraListener
{
	public:
		JNICameraListen();
		JNICameraListen(IVideoCallback *callback);
		~JNICameraListen() { }
		sp<Camera> getCamera();
		
		virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2);
		virtual void postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata);
		virtual void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);
		virtual void postRecordingFrameHandleTimestamp(nsecs_t timestamp, native_handle_t* handle);
		
		
		// connect to camera service
		int CameraSetup(jint cameraId, jstring clientPackageName);
		// disconnect from camera service
		// It's okay to call this when the native camera context is already null.
		// This handles the case where the user has called release() and the
		// finalizer is invoked later.
		int CameraRelease();

		void setParameters(jstring params);
		jstring getParameters();
		void startPreview(const sp<Surface> &surface);
		void startPreview_(const ANativeWindow* window);
		void stopPreview();
		void setDisplayOrientation(jint value);

	private:
		sp<Camera>  mCamera;                // strong reference to native object
		IVideoCallback *mCallback;

};


#endif


