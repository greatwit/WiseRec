/*
 * Copyright 2012, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
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
    return (JCrypto *)env->GetLongField(thiz, gFields.context);
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

    if (crypto == NULL || (crypto->initCheck() != OK && crypto->initCheck() != NO_INIT)) {
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
