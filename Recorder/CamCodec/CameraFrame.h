#ifndef CAMERA_FRAME_H_
#define CAMERA_FRAME_H_

#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaSource.h>

#include <camera/ICameraRecordingProxyListener.h>
#include <camera/CameraParameters.h>
#include <utils/List.h>
#include <utils/RefBase.h>
#include <utils/String16.h>



//using namespace android;
namespace android {

#include "mediacodec.h"
#include "mediaextrator.h"

class IMemory;
class Camera;
class Surface;

struct AMediaFormat;
typedef struct AMediaFormat AMediaFormat;

class CameraFrame {
public:
	CameraFrame();
	virtual ~CameraFrame();
	status_t CameraSetup(const sp<Camera> &pOriCamera, int32_t cameraId, const char* clientName, uid_t clientUid);
	status_t CameraSetdown();

	status_t configureCamera(int width, int height,int frameRate);


protected:

    class ProxyListener: public BnCameraRecordingProxyListener {
		public:
			ProxyListener(CameraFrame* pSource);
			virtual void dataCallbackTimestamp(int64_t timestampUs, int32_t msgType,
					const sp<IMemory> &data);

		private:
			CameraFrame* mSource;
    };

    // isBinderAlive needs linkToDeath to work.
    class DeathNotifier: public IBinder::DeathRecipient {
		public:
			DeathNotifier() {}
			virtual void binderDied(const wp<IBinder>& who);
    };

    enum CameraFlags {
        FLAGS_SET_CAMERA = 1L << 0,
        FLAGS_HOT_CAMERA = 1L << 1,
    };

    void startCameraRecording();
    void stopCameraRecording();
    virtual void dataCallbackTimestamp(int64_t timestampUs, int32_t msgType, const sp<IMemory> &data);
    void releaseRecordingFrame(const sp<IMemory>& frame);

private:
    void releaseCamera();
    int32_t  mCameraFlags;
//    Size     mVideoSize;
//    int32_t  mNumInputBuffers;
//    int32_t  mVideoFrameRate;
    int32_t  mColorFormat;

    sp<Camera>   mCamera;
    sp<ICameraRecordingProxy>   mCameraRecordingProxy;
    sp<DeathNotifier> mDeathNotifier;

    struct symext 		mSymbols;
    AMediaCodec*	 	mCodec;
};




}

#endif
