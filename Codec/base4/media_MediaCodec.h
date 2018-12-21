

#ifndef _CODEC_MEDIA_MEDIACODEC_H_
#define _CODEC_MEDIA_MEDIACODEC_H_

#include "jni.h"

#include <media/hardware/CryptoAPI.h>
#include <media/stagefright/foundation/ABase.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>

#include <media/stagefright/foundation/AString.h>

namespace android {

struct ALooper;
struct AMessage;
struct AString;
struct ICrypto;
struct ISurfaceTexture;
struct MediaCodec;
struct SurfaceTextureClient;

struct JMediaCodec : public RefBase {
    JMediaCodec(
            JNIEnv *env, jobject thiz,
            const char *name, bool nameIsType, bool encoder);

    status_t initCheck() const;

    status_t configure(
            const sp<AMessage> &format,
            const sp<ISurfaceTexture> &surfaceTexture,
            const sp<ICrypto> &crypto,
            int flags);

    status_t start();
    status_t stop();

    status_t flush();

    status_t queueInputBuffer(
            size_t index,
            size_t offset, size_t size, int64_t timeUs, uint32_t flags,
            AString *errorDetailMsg);

    status_t dequeueInputBuffer(size_t *index, int64_t timeoutUs);

    status_t dequeueOutputBuffer(
            JNIEnv *env, jobject bufferInfo, size_t *index, int64_t timeoutUs);

    status_t dequeueOutputBuffer(JNIEnv *env, size_t *index, size_t &offset, size_t &size, int64_t &timeUs, uint32_t &flags, int64_t timeoutUs);

    status_t releaseOutputBuffer(size_t index, bool render);
	status_t releaseOutputBuffer(size_t index, bool render, bool updatePTS, int64_t timestampNs);

    status_t getOutputFormat(JNIEnv *env, jobject *format) const;

    status_t getBuffers(
            JNIEnv *env, bool input, jobjectArray *bufArray) const;

    void setVideoScalingMode(int mode);

protected:
    virtual ~JMediaCodec();

private:
    jclass mClass;
    jweak mObject;
    sp<SurfaceTextureClient> mSurfaceTextureClient;

    sp<ALooper> mLooper;
    sp<MediaCodec> mCodec;

    DISALLOW_EVIL_CONSTRUCTORS(JMediaCodec);
};

}  // namespace android

#endif  // _ANDROID_MEDIA_MEDIACODEC_H_
