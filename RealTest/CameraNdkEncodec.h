#ifndef _CameraNdkEncodec_H_
#define _CameraNdkEncodec_H_

#include "IVideoCallback.h"
#include "CameraStub.h"

#include <jni.h>

class CameraNdkEncodec : public IVideoCallback
{
	public:
		CameraNdkEncodec();
		virtual ~CameraNdkEncodec();


		void VideoSource(VideoFrame *pBuf);

	private:
		CameraStub 	*mCamera;
};

#endif
