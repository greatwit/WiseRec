

#ifndef _CODEC_MEDIA_CRYPTO_H_
#define _CODEC_MEDIA_CRYPTO_H_

#include "jni.h"

#include <media/stagefright/foundation/ABase.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>

namespace android {

struct ICrypto;

struct JCrypto : public RefBase {
    static bool IsCryptoSchemeSupported(const uint8_t uuid[16]);

    JCrypto(JNIEnv *env, jobject thiz,
            const uint8_t uuid[16], const void *initData, size_t initSize);

    status_t initCheck() const;

    bool requiresSecureDecoderComponent(const char *mime) const;

    static sp<ICrypto> GetCrypto(JNIEnv *env, jobject obj);

protected:
    virtual ~JCrypto();

private:
    jweak mObject;
    sp<ICrypto> mCrypto;

    static sp<ICrypto> MakeCrypto();

    static sp<ICrypto> MakeCrypto(
            const uint8_t uuid[16], const void *initData, size_t initSize);

    DISALLOW_EVIL_CONSTRUCTORS(JCrypto);
};

}  // namespace android

#endif  // _ANDROID_MEDIA_CRYPTO_H_
