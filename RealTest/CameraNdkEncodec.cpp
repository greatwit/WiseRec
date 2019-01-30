
#include "CameraNdkEncodec.h"

CameraNdkEncodec::CameraNdkEncodec() {
	mCamera = new CameraStub(this);
}

CameraNdkEncodec::~CameraNdkEncodec() {

}

void CameraNdkEncodec::VideoSource(VideoFrame *pBuf) {

}
