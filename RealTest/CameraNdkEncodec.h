#ifndef _CameraNdkEncodec_H_
#define _CameraNdkEncodec_H_

#include <jni.h>

#include "IVideoCallback.h"
#include "CameraStub.h"

#include "mediaextrator.h"



class CameraNdkEncodec : public IVideoCallback
{
	public:
		CameraNdkEncodec();
		virtual ~CameraNdkEncodec();
		bool openCamera( int cameraId, jstring clientName);
		bool closeCamera();
		void SetCameraParameter(jstring params);
		jstring GetCameraParameter();
		int startPlayer(const char*filepath, void *surface, int w, int h);
		int stopPlayer();
		void setInt32(const char*key, int value);

		void VideoSource(VideoFrame *pBuf);

	private:
		CameraStub 	*mCamera;
		struct symext 		mSymbols;
		AMediaCodec*	 	mCodec;
	    AMediaFormat* 		mFormat;
	    FILE 				*mpFile;
};

#endif
