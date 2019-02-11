
#include "CameraStub.h"

#include "ComDefine.h"
#define TAG "CameraStub"

	CameraStub::CameraStub()
		:mVideoCall(NULL)
	{
		mCamera = CameraContext::getInstance();
	}

	CameraStub::CameraStub(IVideoCallback*callback)
		:mVideoCall(callback)
	{
		mCamera = CameraContext::getInstance();
	}

	CameraStub::~CameraStub() {
		CloseCamera();
	}

	void CameraStub::CreateCamera(int cameraId, jstring clientPackageName) {
		if(mCamera) {
			//JNIEnv *env = AndroidRuntime::getJNIEnv();
			//jstring clientPackageName = env->NewStringUTF(packName);

			int camSet = 0;
			if(mVideoCall)
				camSet = mCamera->CameraSetup(mVideoCall, cameraId, clientPackageName);
			else
				camSet = mCamera->CameraSetup(this, cameraId, clientPackageName);

			if(camSet<0) {
				GLOGE("function %s,line:%d CameraSetup failed.", __FUNCTION__, __LINE__);
				return ;
			}
		}
		else
			GLOGE("function %s,line:%d LoadCameraContext failed.", __FUNCTION__, __LINE__);
	}

	void CameraStub::SetCameraParameter(jstring params) {
		mCamera->SetCameraParameter(params);
	}

	jstring CameraStub::GetCameraParameter() {
		return mCamera->GetCameraParameter();
	}

//	void CameraStub::StartPreview(const sp<Surface> &surface) {
//		mCamera->StartPreview(surface);
//	}

	void CameraStub::StartPreview(void* window) {
		mCamera->StartPreview(window);
	}

	void CameraStub::StopPreview() {
		mCamera->StopPreview();
	}

	void CameraStub::CloseCamera() {
		if(mCamera) {
			mCamera->ReleaseLib();
			delete mCamera;
			mCamera = NULL;
		}
	}

	void CameraStub::VideoSource(VideoFrame *pBuf) {
		if(pBuf)
			GLOGI("video length:%d", pBuf->length);
	}

