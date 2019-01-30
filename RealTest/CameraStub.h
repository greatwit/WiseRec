#ifndef _CAMERA_STUB_H_
#define _CAMERA_STUB_H_

#include "IVideoCallback.h"
#include "CameraContext.h"

#include <jni.h>

class CameraStub : public IVideoCallback
{
	public:
		CameraStub();
		CameraStub(IVideoCallback*callback);
		virtual ~CameraStub();
		void CreateCamera(int cameraId, jstring clientPackageName);
		void SetCameraParameter(jstring params);
		jstring GetCameraParameter();
		//void StartPreview(const sp<Surface> &surface);
		void StartPreview(void* window);
		void StopPreview();
		void CloseCamera();

		void VideoSource(VideoFrame *pBuf);

	private:
		CameraContext 	*mCamera;
		IVideoCallback	*mVideoCall;
};

#endif
