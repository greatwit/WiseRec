#ifndef _CAMERA_DL_H_
#define _CAMERA_DL_H_

#include "IVideoCallback.h"
#include "CameraContext.h"

#include <jni.h>
#include <gui/Surface.h>

class CameraDL : public IVideoCallback
{
	public:
		CameraDL();
		virtual ~CameraDL();
		void CreateCamera(int cameraId, jstring clientPackageName);
		void SetCameraParameter(jstring params);
		jstring GetCameraParameter();
		void StartPreview(const sp<Surface> &surface);
		void StopPreview();
		void CloseCamera();


		void VideoSource(VideoFrame *pBuf);

	private:
		CameraLib *mCamera;
};

#endif
