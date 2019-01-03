
#include "CameraDL.h"

#include "android_runtime/AndroidRuntime.h"

#include "ComDefine.h"
#define TAG "CameraDL"

	CameraDL::CameraDL() {
		mCamera = CameraLib::getInstance();
	}

	CameraDL::~CameraDL() {
		CloseCamera();
	}

	void CameraDL::CreateCamera(int cameraId, jstring clientPackageName) {
		if(mCamera) {
			//JNIEnv *env = AndroidRuntime::getJNIEnv();
			//jstring clientPackageName = env->NewStringUTF(packName);
			int camSet = CameraLib::getInstance()->CameraSetup(this, cameraId, clientPackageName);
			if(camSet<0) {
				GLOGE("function %s,line:%d CameraSetup failed.", __FUNCTION__, __LINE__);
				return ;
			}
		}
		else
			GLOGE("function %s,line:%d LoadCameraLib failed.", __FUNCTION__, __LINE__);

	}

	void CameraDL::SetCameraParameter(jstring params) {
		mCamera->SetCameraParameter(params);
	}

	jstring CameraDL::GetCameraParameter() {
		return mCamera->GetCameraParameter();
	}

	void CameraDL::StartPreview(const sp<Surface> &surface) {
		mCamera->StartPreview(surface);
	}

	void CameraDL::StopPreview() {
		mCamera->StopPreview();
	}

	void CameraDL::CloseCamera() {
		if(mCamera) {
			mCamera->ReleaseLib();
			delete mCamera;
			mCamera = NULL;
		}
	}

	void CameraDL::VideoSource(VideoFrame *pBuf) {
		if(pBuf)
			GLOGI("video length:%d", pBuf->length);
	}

