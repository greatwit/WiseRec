
///////////////////////////////////////////////ndkmediacodec////////////////////////////////////////
#include <inttypes.h>

//#define LOG_NDEBUG 0
#define LOG_TAG "NdkMediaCodec"

#include "NdkMediaCodec.h"
#include "NdkMediaError.h"
#include "NdkMediaCryptoPriv.h"
#include "NdkMediaFormatPriv.h"


using namespace android;


static media_status_t translate_error(status_t err) {
    if (err == OK) {
        return AMEDIA_OK;
    } else if (err == -EAGAIN) {
        return (media_status_t) AMEDIACODEC_INFO_TRY_AGAIN_LATER;
    }
    ALOGE("sf error code: %d", err);
    return AMEDIA_ERROR_UNKNOWN;
}

enum {
    kWhatActivityNotify,
    kWhatRequestActivityNotifications,
    kWhatStopActivityNotifications,
};


class CodecHandler: public AHandler {
private:
    AMediaCodec* mCodec;
public:
    CodecHandler(AMediaCodec *codec);
    virtual void onMessageReceived(const sp<AMessage> &msg);
};

typedef void (*OnCodecEvent)(AMediaCodec *codec, void *userdata);

struct AMediaCodec {
    sp<android::MediaCodec> mCodec;
    sp<ALooper> mLooper;
    sp<CodecHandler> mHandler;
    sp<AMessage> mActivityNotification;
    int32_t mGeneration;
    bool mRequestedActivityNotification;
    OnCodecEvent mCallback;
    void *mCallbackUserData;
};

CodecHandler::CodecHandler(AMediaCodec *codec) {
    mCodec = codec;
}

void CodecHandler::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatRequestActivityNotifications:
        case kWhatActivityNotify:
        case kWhatStopActivityNotifications:
    }
}


static void requestActivityNotification(AMediaCodec *codec) {
    (new AMessage(kWhatRequestActivityNotifications, codec->mHandler))->post();
}

extern "C" {

static AMediaCodec * createAMediaCodec(const char *name, bool name_is_type, bool encoder);

EXPORT
AMediaCodec* AMediaCodec_createCodecByName(const char *name);

EXPORT
AMediaCodec* AMediaCodec_createDecoderByType(const char *mime_type);

EXPORT
AMediaCodec* AMediaCodec_createEncoderByType(const char *name);

EXPORT
media_status_t AMediaCodec_delete(AMediaCodec *mData);

EXPORT
media_status_t AMediaCodec_configure(
        AMediaCodec *mData,
        const AMediaFormat* format,
        ANativeWindow* window,
        AMediaCrypto *crypto,
        uint32_t flags);

EXPORT
media_status_t AMediaCodec_start(AMediaCodec *mData);

EXPORT
media_status_t AMediaCodec_stop(AMediaCodec *mData);

EXPORT
media_status_t AMediaCodec_flush(AMediaCodec *mData);

EXPORT
ssize_t AMediaCodec_dequeueInputBuffer(AMediaCodec *mData, int64_t timeoutUs);

EXPORT
uint8_t* AMediaCodec_getInputBuffer(AMediaCodec *mData, size_t idx, size_t *out_size);

EXPORT
media_status_t AMediaCodec_queueInputBuffer(AMediaCodec *mData,
        size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags);


EXPORT
uint8_t* AMediaCodec_getOutputBuffer(AMediaCodec *mData, size_t idx, size_t *out_size);

EXPORT
ssize_t AMediaCodec_dequeueOutputBuffer(AMediaCodec *mData,
        AMediaCodecBufferInfo *info, int64_t timeoutUs);

EXPORT
AMediaFormat* AMediaCodec_getOutputFormat(AMediaCodec *mData);

EXPORT
media_status_t AMediaCodec_releaseOutputBuffer(AMediaCodec *mData, size_t idx, bool render);

EXPORT
media_status_t AMediaCodec_releaseOutputBufferAtTime(
        AMediaCodec *mData, size_t idx, int64_t timestampNs);

//EXPORT
media_status_t AMediaCodec_setNotificationCallback(AMediaCodec *mData, OnCodecEvent callback,
        void *userdata);

typedef struct AMediaCodecCryptoInfo {
        int numsubsamples;
        uint8_t key[16];
        uint8_t iv[16];
        cryptoinfo_mode_t mode;
        size_t *clearbytes;
        size_t *encryptedbytes;
} AMediaCodecCryptoInfo;

EXPORT
media_status_t AMediaCodec_queueSecureInputBuffer(
        AMediaCodec* codec,
        size_t idx,
        off_t offset,
        AMediaCodecCryptoInfo* crypto,
        uint64_t time,
        uint32_t flags);

EXPORT
AMediaCodecCryptoInfo *AMediaCodecCryptoInfo_new(
        int numsubsamples,
        uint8_t key[16],
        uint8_t iv[16],
        cryptoinfo_mode_t mode,
        size_t *clearbytes,
        size_t *encryptedbytes);


EXPORT
media_status_t AMediaCodecCryptoInfo_delete(AMediaCodecCryptoInfo* info);

EXPORT
size_t AMediaCodecCryptoInfo_getNumSubSamples(AMediaCodecCryptoInfo* ci);

EXPORT
media_status_t AMediaCodecCryptoInfo_getKey(AMediaCodecCryptoInfo* ci, uint8_t *dst);

EXPORT
media_status_t AMediaCodecCryptoInfo_getIV(AMediaCodecCryptoInfo* ci, uint8_t *dst);

EXPORT
cryptoinfo_mode_t AMediaCodecCryptoInfo_getMode(AMediaCodecCryptoInfo* ci);

EXPORT
media_status_t AMediaCodecCryptoInfo_getClearBytes(AMediaCodecCryptoInfo* ci, size_t *dst);

EXPORT
media_status_t AMediaCodecCryptoInfo_getEncryptedBytes(AMediaCodecCryptoInfo* ci, size_t *dst);

} // extern "C"




////////////////////////////////////////////mediaformat type//////////////////////////////////////////


//#define LOG_NDEBUG 0
#define LOG_TAG "NdkMediaFormat"


#include "NdkMediaFormat.h"

using namespace android;

struct AMediaFormat {
    sp<AMessage> mFormat;
    String8 mDebug;
    KeyedVector<String8, String8> mStringCache;
};

extern "C" {

// private functions for conversion to/from AMessage
AMediaFormat* AMediaFormat_fromMsg(const void* data) {
    ALOGV("private ctor");
    AMediaFormat* mData = new AMediaFormat();
    mData->mFormat = *((sp<AMessage>*)data);
    return mData;
}

void AMediaFormat_getFormat(const AMediaFormat* mData, void* dest) {
    *((sp<AMessage>*)dest) = mData->mFormat;
}


/*
 * public function follow
 */
EXPORT
AMediaFormat *AMediaFormat_new();

EXPORT
media_status_t AMediaFormat_delete(AMediaFormat *mData);


EXPORT
const char* AMediaFormat_toString(AMediaFormat *mData);

EXPORT
bool AMediaFormat_getInt32(AMediaFormat* format, const char *name, int32_t *out);

EXPORT
bool AMediaFormat_getInt64(AMediaFormat* format, const char *name, int64_t *out) {
    return format->mFormat->findInt64(name, out);
}

EXPORT
bool AMediaFormat_getFloat(AMediaFormat* format, const char *name, float *out);

EXPORT
bool AMediaFormat_getSize(AMediaFormat* format, const char *name, size_t *out);

EXPORT
bool AMediaFormat_getBuffer(AMediaFormat* format, const char *name, void** data, size_t *outsize);

EXPORT
bool AMediaFormat_getString(AMediaFormat* mData, const char *name, const char **out);

EXPORT
void AMediaFormat_setInt32(AMediaFormat* format, const char *name, int32_t value);

EXPORT
void AMediaFormat_setInt64(AMediaFormat* format, const char *name, int64_t value);

EXPORT
void AMediaFormat_setFloat(AMediaFormat* format, const char* name, float value);

EXPORT
void AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value);

EXPORT
void AMediaFormat_setBuffer(AMediaFormat* format, const char* name, void* data, size_t size);


EXPORT const char* AMEDIAFORMAT_KEY_AAC_PROFILE = "aac-profile";
EXPORT const char* AMEDIAFORMAT_KEY_BIT_RATE = "bitrate";
EXPORT const char* AMEDIAFORMAT_KEY_CHANNEL_COUNT = "channel-count";
EXPORT const char* AMEDIAFORMAT_KEY_CHANNEL_MASK = "channel-mask";
EXPORT const char* AMEDIAFORMAT_KEY_COLOR_FORMAT = "color-format";
EXPORT const char* AMEDIAFORMAT_KEY_DURATION = "durationUs";
EXPORT const char* AMEDIAFORMAT_KEY_FLAC_COMPRESSION_LEVEL = "flac-compression-level";
EXPORT const char* AMEDIAFORMAT_KEY_FRAME_RATE = "frame-rate";
EXPORT const char* AMEDIAFORMAT_KEY_HEIGHT = "height";
EXPORT const char* AMEDIAFORMAT_KEY_IS_ADTS = "is-adts";
EXPORT const char* AMEDIAFORMAT_KEY_IS_AUTOSELECT = "is-autoselect";
EXPORT const char* AMEDIAFORMAT_KEY_IS_DEFAULT = "is-default";
EXPORT const char* AMEDIAFORMAT_KEY_IS_FORCED_SUBTITLE = "is-forced-subtitle";
EXPORT const char* AMEDIAFORMAT_KEY_I_FRAME_INTERVAL = "i-frame-interval";
EXPORT const char* AMEDIAFORMAT_KEY_LANGUAGE = "language";
EXPORT const char* AMEDIAFORMAT_KEY_MAX_HEIGHT = "max-height";
EXPORT const char* AMEDIAFORMAT_KEY_MAX_INPUT_SIZE = "max-input-size";
EXPORT const char* AMEDIAFORMAT_KEY_MAX_WIDTH = "max-width";
EXPORT const char* AMEDIAFORMAT_KEY_MIME = "mime";
EXPORT const char* AMEDIAFORMAT_KEY_PUSH_BLANK_BUFFERS_ON_STOP = "push-blank-buffers-on-shutdown";
EXPORT const char* AMEDIAFORMAT_KEY_REPEAT_PREVIOUS_FRAME_AFTER = "repeat-previous-frame-after";
EXPORT const char* AMEDIAFORMAT_KEY_SAMPLE_RATE = "sample-rate";
EXPORT const char* AMEDIAFORMAT_KEY_WIDTH = "width";
EXPORT const char* AMEDIAFORMAT_KEY_STRIDE = "stride";



mMeta->setCString(kKeyMIMEType,  MEDIA_MIMETYPE_VIDEO_RAW);
mMeta->setInt32(kKeyColorFormat, mColorFormat);
mMeta->setInt32(kKeyWidth,       mVideoSize.width);
mMeta->setInt32(kKeyHeight,      mVideoSize.height);
mMeta->setInt32(kKeyStride,      mVideoSize.width);
mMeta->setInt32(kKeySliceHeight, mVideoSize.height);
mMeta->setInt32(kKeyFrameRate,   mVideoFrameRate);
} // extern "C"


