#ifndef CAMERA_ENCODEC_H_
#define CAMERA_ENCODEC_H_

#include <utils/threads.h> 
#include <utils/Log.h>


#include <string>
#include <map>



#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include <media/ICrypto.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/MediaErrors.h>

#include <gui/Surface.h>

#include <camera/Camera.h>

#include "IVideoCallback.h"
#include "CodecContext.h"
#include "CameraContext.h"

using namespace android;

class CameraEncodec : public ICodecCallback, public IVideoCallback
{
	public:
		CameraEncodec();
		virtual ~CameraEncodec();
		
		bool CreateCodec(JNIEnv *env, jobject thiz, const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, short sendPort);
		bool CreateCodec(jobject thiz, const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, int cameraId, jstring clientPackageName);//using
		
		bool DeInit();
		
		void SetCameraParameter(jstring params);
		jstring GetCameraParameter();
		void SetDisplayOrientation(int value);
		
		bool StartVideo(const sp<Surface> &cameraSurface);
		bool StopVideo();

		void AddDecodecSource(char *data, int len);
		virtual void onCodecBuffer(struct CodecBuffer& buff);
		virtual void VideoSource(VideoFrame *pBuf);
		
		
	protected:

	private:
		bool				mFirstFrame;
		bool				mbRunning;

};

#endif


