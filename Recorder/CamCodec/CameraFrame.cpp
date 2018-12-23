
#include <media/stagefright/foundation/AMessage.h>
#include <binder/IPCThreadState.h>
#include <camera/Camera.h>
#include <camera/ICamera.h>


#include "CameraFrame.h"
#include "ComDefine.h"

#include <media/stagefright/MediaCodec.h>

namespace android {


static const int64_t CAMERA_SOURCE_TIMEOUT_NS = 3000000000LL;



/////////////////////////////////cold camera listener//////////////////////////////////////////
struct CameraSourceListener : public CameraListener {
    CameraSourceListener(CameraFrame *pSource);

    virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2);
    virtual void postData(int32_t msgType, const sp<IMemory> &dataPtr,
                          camera_frame_metadata_t *metadata);

    virtual void postDataTimestamp(
            nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);

protected:
    virtual ~CameraSourceListener();

private:
    CameraFrame * mSource;

    CameraSourceListener(const CameraSourceListener &);
    CameraSourceListener &operator=(const CameraSourceListener &);
};

CameraSourceListener::CameraSourceListener(CameraFrame *pSource)
    : mSource(pSource) {
}

CameraSourceListener::~CameraSourceListener() {
}

void CameraSourceListener::notify(int32_t msgType, int32_t ext1, int32_t ext2) {
    GLOGW("notify(%d, %d, %d)", msgType, ext1, ext2);
}

void CameraSourceListener::postData(int32_t msgType, const sp<IMemory> &dataPtr,
                                    camera_frame_metadata_t *metadata) {
    GLOGW("postData(%d, ptr:%p, size:%d)",
         msgType, dataPtr->pointer(), dataPtr->size());

//    sp<CameraSource> source = mSource.promote();
//    if (source.get() != NULL) {
//        source->dataCallback(msgType, dataPtr);
//    }
}

void CameraSourceListener::postDataTimestamp(
        nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr) {

//    sp<CameraSource> source = mSource.promote();
//    if (source.get() != NULL) {
//        source->dataCallbackTimestamp(timestamp/1000, msgType, dataPtr);
//    }
}

////////////////////////////////////////////cameraFrame////////////////////////////////////////////

CameraFrame::CameraFrame() {
	InitExtratorSymbols(&mSymbols);
	mCodec    = mSymbols.AMediaCodec.createEncoderByType("video/avc");

	GLOGE("AMediaCodec.createEncoderByType result:%s", (mCodec!=NULL)?"true":"false");

}

CameraFrame::~CameraFrame() {
	if(mCodec) {
		mSymbols.AMediaCodec.stop(mCodec);
		mSymbols.AMediaCodec.deletemc(mCodec);
	}
}

status_t CameraFrame::CameraSetup(const sp<Camera> &pOriCamera, int32_t cameraId, const char* clientName, uid_t clientUid) {

	String16 strName(clientName, 1);

    sp<ICamera> camera = pOriCamera->remote();
    if (camera == 0) {
        mCamera = Camera::connect(cameraId, strName, clientUid);
        if (mCamera == 0) return -EBUSY;

        mCameraFlags &= ~FLAGS_HOT_CAMERA;

    } else {
        // We get the proxy from Camera, not ICamera. We need to get the proxy
        // to the remote Camera owned by the application. Here mCamera is a
        // local Camera object created by us. We cannot use the proxy from mCamera here.
        mCamera = Camera::create(camera);
        if (mCamera == 0) return -EBUSY;

        mCameraRecordingProxy = pOriCamera->getRecordingProxy();
        mCameraFlags |= FLAGS_HOT_CAMERA;

        mDeathNotifier = new DeathNotifier();
        // isBinderAlive needs linkToDeath to work.
        //mCameraRecordingProxy->asBinder()->linkToDeath(mDeathNotifier);
    }

    mCamera->lock();


	CameraParameters params(mCamera->getParameters());
    Vector<Size> sizes;
    params.getSupportedVideoSizes(sizes);
    const char* colorFormat = params.get( CameraParameters::KEY_VIDEO_FRAME_FORMAT);//yuv420sp
    int32_t frameRateActual = params.getPreviewFrameRate();
	int32_t previewWidth  = -1, previewHeight  = -1;
	int32_t videoWidth  = -1, videoHeight  = -1;
    if (sizes.size() > 0) {

		// video size is the same as preview size
		params.getPreviewSize(&previewWidth, &previewHeight);
		// video size may not be the same as preview
		params.getVideoSize(&videoWidth, &videoHeight);
		GLOGW("color format (%s) previewWidth:%d previewHeight:%d videoWidth:%d, videoHeight:%d",
			   colorFormat, previewWidth, previewHeight, videoWidth, videoHeight);
	}

    AMediaFormat*format = mSymbols.AMediaFormat.newfmt();
    mSymbols.AMediaFormat.setString(format, "mime", "video/avc");
    mSymbols.AMediaFormat.setInt32(format, "width", videoWidth);
    mSymbols.AMediaFormat.setInt32(format, "height", videoHeight);
    mSymbols.AMediaFormat.setInt32(format, "frame-rate", frameRateActual);
    mSymbols.AMediaFormat.setInt32(format, "bitrate", videoWidth*videoHeight*5);
    mSymbols.AMediaFormat.setInt32(format, "color-format", 21);
    mSymbols.AMediaFormat.setInt32(format, "i-frame-interval", 2);

    if (mSymbols.AMediaCodec.configure(mCodec, format, NULL, NULL, MediaCodec::CONFIGURE_FLAG_ENCODE) != AMEDIA_OK)
    {
        GLOGE("AMediaCodec.configure failed");
    }else
    	GLOGW("AMediaCodec.configure successful.");

    //set true,data size would be 8 byte
    if (OK == mCamera->storeMetaDataInBuffers(true)) {
    	GLOGE("camera storeMetaDataInBuffers true");
    }

    mSymbols.AMediaFormat.deletefmt(format);

    if (mSymbols.AMediaCodec.start(mCodec) != AMEDIA_OK)
        GLOGE("AMediaCodec.start failed");

    startCameraRecording();

    return OK;
}

status_t CameraFrame:: CameraSetdown() {
	stopCameraRecording();
	releaseCamera();
	return OK;
}

void CameraFrame::releaseCamera() {
    GLOGW("releaseCamera");
    if (mCamera != 0) {
        int64_t token = IPCThreadState::self()->clearCallingIdentity();
        if ((mCameraFlags & FLAGS_HOT_CAMERA) == 0) {
            GLOGW("Camera was cold when we started, stopping preview");
            mCamera->stopPreview();
            mCamera->disconnect();
        }
        mCamera->unlock();
        mCamera.clear();
        mCamera = 0;
        IPCThreadState::self()->restoreCallingIdentity(token);
    }
    if (mCameraRecordingProxy != 0) {
        //mCameraRecordingProxy->asBinder()->unlinkToDeath(mDeathNotifier);
        mCameraRecordingProxy.clear();
    }
    mCameraFlags = 0;
}

status_t CameraFrame::configureCamera(int width, int height,int frameRate) {
	// Set the camera to use the requested video frame size
	// and/or frame rate.
	status_t err;
	CameraParameters params(mCamera->getParameters());

    const char* colorFormat = params.get( CameraParameters::KEY_VIDEO_FRAME_FORMAT);//yuv420sp

    Vector<Size> sizes;
    params.getSupportedVideoSizes(sizes);
    if (sizes.size() > 0) {
		int32_t previewWidth  = -1, previewHeight  = -1;
		int32_t videoWidth  = -1, videoHeight  = -1;

		// video size is the same as preview size
		params.getPreviewSize(&previewWidth, &previewHeight);
		// video size may not be the same as preview
		params.getVideoSize(&videoWidth, &videoHeight);
		GLOGW("color format (%s) previewWidth:%d previewHeight:%d videoWidth:%d, videoHeight:%d",
			   colorFormat, previewWidth, previewHeight, videoWidth, videoHeight);

		params.setVideoSize(width, height);
	}

    const char* supportedFrameRates = params.get(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES);
    char buf[4];
    snprintf(buf, 4, "%d", frameRate);
    if (strstr(supportedFrameRates, buf) == NULL) {
        GLOGE("Requested frame rate (%d) is not supported: %s", frameRate, supportedFrameRates);
    }else
    {
    	GLOGE("Supported frame rates: %s", supportedFrameRates);
    	params.setPreviewFrameRate(frameRate);
    }

    // Either frame rate or frame size needs to be changed.
    String8 s = params.flatten();
    if (OK != mCamera->setParameters(s)) {
        GLOGE("Could not change settings." " Someone else is using camera %p?", mCamera.get());
        return -EBUSY;
    }

	return OK;
}

void CameraFrame::startCameraRecording() {
    GLOGW("startCameraRecording");
    // Reset the identity to the current thread because media server owns the
    // camera and recording is started by the applications. The applications
    // will connect to the camera in ICameraRecordingProxy::startRecording.
    int64_t token = IPCThreadState::self()->clearCallingIdentity();
    int mNumInputBuffers = 5;
    if (mNumInputBuffers > 0) {
        status_t err = mCamera->sendCommand(
            CAMERA_CMD_SET_VIDEO_BUFFER_COUNT, mNumInputBuffers, 0);

        // This could happen for CameraHAL1 clients; thus the failure is
        // not a fatal error
        if (err != OK) {
            GLOGW("Failed to set video buffer count to %d due to %d", mNumInputBuffers, err);
        }
    }

    if (mCameraFlags & FLAGS_HOT_CAMERA) {
        mCamera->unlock();
        mCamera.clear();
        //CHECK_EQ((status_t)OK,
        mCameraRecordingProxy->startRecording(new ProxyListener(this));
    } else {
        mCamera->setListener(new CameraSourceListener(this));
        mCamera->startRecording();
        mCamera->recordingEnabled();
        //CHECK();
    }
    IPCThreadState::self()->restoreCallingIdentity(token);
}

void CameraFrame::stopCameraRecording() {
    GLOGW("stopCameraRecording");
    if (mCameraFlags & FLAGS_HOT_CAMERA) {
        mCameraRecordingProxy->stopRecording();
    } else {
        mCamera->setListener(NULL);
        mCamera->stopRecording();
    }
}

void CameraFrame::dataCallbackTimestamp(int64_t timestampUs, int32_t msgType, const sp<IMemory> &data) {
	GLOGW("CameraFrame::dataCallbackTimestamp: timestamp %lld us datasize:%d", timestampUs, data->size());

	int index = mSymbols.AMediaCodec.dequeueInputBuffer(mCodec, 0);//int64_t timeoutUs
	GLOGE("AMediaCodec.dequeueInputBuffer value:%d", index);
	if(index>=0) {
		uint8_t *p_mc_buf;
		size_t i_mc_size;
		p_mc_buf = mSymbols.AMediaCodec.getInputBuffer(mCodec, index, &i_mc_size);
		GLOGE("AMediaCodec.getInputBuffer mcSize:%d", i_mc_size);
	}

	releaseRecordingFrame(data);
}

void CameraFrame::releaseRecordingFrame(const sp<IMemory>& frame) {
    //GLOGW("releaseRecordingFrame");
    if (mCameraRecordingProxy != NULL) {
        mCameraRecordingProxy->releaseRecordingFrame(frame);
    } else if (mCamera != NULL) {
        int64_t token = IPCThreadState::self()->clearCallingIdentity();
        mCamera->releaseRecordingFrame(frame);
        IPCThreadState::self()->restoreCallingIdentity(token);
    }
}

////////////////////////////////////////proxyListener for hot camera/////////////////////////////////////////////////////////

CameraFrame::ProxyListener::ProxyListener(CameraFrame* pSource) {
    mSource = pSource;
}

void CameraFrame::ProxyListener::dataCallbackTimestamp(
        nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr) {
    mSource->dataCallbackTimestamp(timestamp / 1000, msgType, dataPtr);
}

void CameraFrame::DeathNotifier::binderDied(const wp<IBinder>& who) {
    GLOGI("Camera recording proxy died");
}

}
