
#define LOG_TAG "MediaCrypto-JNI"
#include <utils/Log.h>

#include "media_MediaCrypto.h"

#include "android_runtime/AndroidRuntime.h"
#include "jni.h"
#include "JNIHelp.h"

#include <binder/IServiceManager.h>
#include <media/ICrypto.h>
#include <media/IMediaPlayerService.h>
#include <media/stagefright/foundation/ADebug.h>

namespace android {

struct fields_t {
    jfieldID context;
};

static fields_t gFields;

static sp<JCrypto> getCrypto(JNIEnv *env, jobject thiz) {  
    return (JCrypto *)env->GetIntField(thiz, gFields.context);
}

JCrypto::JCrypto(
        JNIEnv *env, jobject thiz,
        const uint8_t uuid[16], const void *initData, size_t initSize) {
    mObject = env->NewWeakGlobalRef(thiz);

    mCrypto = MakeCrypto(uuid, initData, initSize);
}

JCrypto::~JCrypto() {
    mCrypto.clear();

    JNIEnv *env = AndroidRuntime::getJNIEnv();

    env->DeleteWeakGlobalRef(mObject);
    mObject = NULL;
}

// static
sp<ICrypto> JCrypto::MakeCrypto() {
    sp<IServiceManager> sm = defaultServiceManager();    

    sp<IBinder> binder =
        sm->getService(String16("media.player"));

    sp<IMediaPlayerService> service =
        interface_cast<IMediaPlayerService>(binder);

    if (service == NULL) {
        return NULL;
    }

    sp<ICrypto> crypto = service->makeCrypto();

    if (crypto == NULL || crypto->initCheck() != OK) {
        return NULL;
    }

    return crypto;
}

// static
sp<ICrypto> JCrypto::MakeCrypto(
        const uint8_t uuid[16], const void *initData, size_t initSize) {
    sp<ICrypto> crypto = MakeCrypto();

    if (crypto == NULL) {
        return NULL;
    }

    status_t err = crypto->createPlugin(uuid, initData, initSize);

    if (err != OK) {
        return NULL;
    }

    return crypto;
}

bool JCrypto::requiresSecureDecoderComponent(const char *mime) const {
    if (mCrypto == NULL) {
        return false;
    }

    return mCrypto->requiresSecureDecoderComponent(mime);
}

// static
bool JCrypto::IsCryptoSchemeSupported(const uint8_t uuid[16]) {
    sp<ICrypto> crypto = MakeCrypto();

    if (crypto == NULL) {
        return false;
    }

    return crypto->isCryptoSchemeSupported(uuid);
}

status_t JCrypto::initCheck() const {
    return mCrypto == NULL ? NO_INIT : OK;
}

// static
sp<ICrypto> JCrypto::GetCrypto(JNIEnv *env, jobject obj) {
    jclass clazz = env->FindClass("android/media/MediaCrypto");
    CHECK(clazz != NULL);

    if (!env->IsInstanceOf(obj, clazz)) {
        return NULL;
    }

    sp<JCrypto> jcrypto = getCrypto(env, obj);

    if (jcrypto == NULL) {
        return NULL;
    }

    return jcrypto->mCrypto;
}

}  // namespace android


