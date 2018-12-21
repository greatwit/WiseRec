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
#define LOG_TAG "MediaCodec-JNI"
#include <utils/Log.h> 

#include "media_MediaCodec.h"

#include "media_MediaCrypto.h"
#include "media_Utils.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include "jni.h"
#include "JNIHelp.h"

#include <cutils/compiler.h>

#include <gui/Surface.h>

#include <media/ICrypto.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/MediaErrors.h>

#include <nativehelper/ScopedLocalRef.h>

#include <system/window.h>

namespace android {

// Keep these in sync with their equivalents in MediaCodec.java !!!
enum {
    DEQUEUE_INFO_TRY_AGAIN_LATER            = -1,
    DEQUEUE_INFO_OUTPUT_FORMAT_CHANGED      = -2,
    DEQUEUE_INFO_OUTPUT_BUFFERS_CHANGED     = -3,
};

enum {
    EVENT_CALLBACK = 1,
    EVENT_SET_CALLBACK = 2,
};

static struct CryptoErrorCodes {
    jint cryptoErrorNoKey;
    jint cryptoErrorKeyExpired;
    jint cryptoErrorResourceBusy;
    jint cryptoErrorInsufficientOutputProtection;
} gCryptoErrorCodes;

static struct CodecActionCodes {
    jint codecActionTransient;
    jint codecActionRecoverable;
} gCodecActionCodes;

struct fields_t {
    jfieldID context;
    jmethodID postEventFromNativeID;
    jfieldID cryptoInfoNumSubSamplesID;
    jfieldID cryptoInfoNumBytesOfClearDataID;
    jfieldID cryptoInfoNumBytesOfEncryptedDataID;
    jfieldID cryptoInfoKeyID;
    jfieldID cryptoInfoIVID;
    jfieldID cryptoInfoModeID;
};

static fields_t gFields;

////////////////////////////////////////////////////////////////////////////////

JMediaCodec::JMediaCodec(
        JNIEnv *env, jobject thiz,
        const char *name, bool nameIsType, bool encoder)
    : mClass(NULL),
      mObject(NULL) {
    jclass clazz = env->GetObjectClass(thiz);
    CHECK(clazz != NULL);

    mClass = (jclass)env->NewGlobalRef(clazz);
    mObject = env->NewWeakGlobalRef(thiz);

    cacheJavaObjects(env);

    mLooper = new ALooper;
    mLooper->setName("MediaCodec_looper");

    mLooper->start(
            false,      // runOnCallingThread
            true,       // canCallJava
            PRIORITY_FOREGROUND);

    if (nameIsType) {
        mCodec = MediaCodec::CreateByType(mLooper, name, encoder, &mInitStatus);
    } else {
        mCodec = MediaCodec::CreateByComponentName(mLooper, name, &mInitStatus);
    }
    CHECK((mCodec != NULL) != (mInitStatus != OK));
}

void JMediaCodec::cacheJavaObjects(JNIEnv *env) {
    jclass clazz = (jclass)env->FindClass("java/nio/ByteBuffer");
    mByteBufferClass = (jclass)env->NewGlobalRef(clazz);
    CHECK(mByteBufferClass != NULL);

    ScopedLocalRef<jclass> byteOrderClass(
            env, env->FindClass("java/nio/ByteOrder"));
    CHECK(byteOrderClass.get() != NULL);

    jmethodID nativeOrderID = env->GetStaticMethodID(
            byteOrderClass.get(), "nativeOrder", "()Ljava/nio/ByteOrder;");
    CHECK(nativeOrderID != NULL);

    jobject nativeByteOrderObj =
        env->CallStaticObjectMethod(byteOrderClass.get(), nativeOrderID);
    mNativeByteOrderObj = env->NewGlobalRef(nativeByteOrderObj);
    CHECK(mNativeByteOrderObj != NULL);
    env->DeleteLocalRef(nativeByteOrderObj);
    nativeByteOrderObj = NULL;

    mByteBufferOrderMethodID = env->GetMethodID(
            mByteBufferClass,
            "order",
            "(Ljava/nio/ByteOrder;)Ljava/nio/ByteBuffer;");
    CHECK(mByteBufferOrderMethodID != NULL);

    mByteBufferAsReadOnlyBufferMethodID = env->GetMethodID(
            mByteBufferClass, "asReadOnlyBuffer", "()Ljava/nio/ByteBuffer;");
    CHECK(mByteBufferAsReadOnlyBufferMethodID != NULL);

    mByteBufferPositionMethodID = env->GetMethodID(
            mByteBufferClass, "position", "(I)Ljava/nio/Buffer;");
    CHECK(mByteBufferPositionMethodID != NULL);

    mByteBufferLimitMethodID = env->GetMethodID(
            mByteBufferClass, "limit", "(I)Ljava/nio/Buffer;");
    CHECK(mByteBufferLimitMethodID != NULL);
}

status_t JMediaCodec::initCheck() const {
    return mInitStatus;
}

void JMediaCodec::registerSelf() {
    mLooper->registerHandler(this);
}

void JMediaCodec::release() {
    if (mCodec != NULL) {
        mCodec->release();
        mCodec.clear();
        mInitStatus = NO_INIT;
    }

    if (mLooper != NULL) {
        mLooper->unregisterHandler(id());
        mLooper->stop();
        mLooper.clear();
    }
}

JMediaCodec::~JMediaCodec() {
    if (mCodec != NULL || mLooper != NULL) {
        /* MediaCodec and looper should have been released explicitly already
         * in setMediaCodec() (see comments in setMediaCodec()).
         *
         * Otherwise JMediaCodec::~JMediaCodec() might be called from within the
         * message handler, doing release() there risks deadlock as MediaCodec::
         * release() post synchronous message to the same looper.
         *
         * Print a warning and try to proceed with releasing.
         */
        ALOGW("try to release MediaCodec from JMediaCodec::~JMediaCodec()...");
        release();
        ALOGW("done releasing MediaCodec from JMediaCodec::~JMediaCodec().");
    }

    JNIEnv *env = AndroidRuntime::getJNIEnv();

    env->DeleteWeakGlobalRef(mObject);
    mObject = NULL;
    env->DeleteGlobalRef(mClass);
    mClass = NULL;
    deleteJavaObjects(env);
}

void JMediaCodec::deleteJavaObjects(JNIEnv *env) {
    env->DeleteGlobalRef(mByteBufferClass);
    mByteBufferClass = NULL;
    env->DeleteGlobalRef(mNativeByteOrderObj);
    mNativeByteOrderObj = NULL;

    mByteBufferOrderMethodID = NULL;
    mByteBufferAsReadOnlyBufferMethodID = NULL;
    mByteBufferPositionMethodID = NULL;
    mByteBufferLimitMethodID = NULL;
}

status_t JMediaCodec::setCallback(jobject cb) {
    if (cb != NULL) {
        if (mCallbackNotification == NULL) {
            mCallbackNotification = new AMessage(kWhatCallbackNotify, id());
        }
    } else {
        mCallbackNotification.clear();
    }

    return mCodec->setCallback(mCallbackNotification);
}

status_t JMediaCodec::configure(
        const sp<AMessage> &format,
        const sp<IGraphicBufferProducer> &bufferProducer,
        const sp<ICrypto> &crypto,
        int flags) {
    sp<Surface> client;
    if (bufferProducer != NULL) {
        mSurfaceTextureClient =
            new Surface(bufferProducer, true /* controlledByApp */);
    } else {
        mSurfaceTextureClient.clear();
    }

    return mCodec->configure(format, mSurfaceTextureClient, crypto, flags);
}

status_t JMediaCodec::createInputSurface(
        sp<IGraphicBufferProducer>* bufferProducer) {
    return mCodec->createInputSurface(bufferProducer);
}

status_t JMediaCodec::start() {
    return mCodec->start();
}

status_t JMediaCodec::stop() {
    mSurfaceTextureClient.clear();

    return mCodec->stop();
}

status_t JMediaCodec::flush() {
    return mCodec->flush();
}

status_t JMediaCodec::reset() {
    return mCodec->reset();
}

status_t JMediaCodec::queueInputBuffer(
        size_t index,
        size_t offset, size_t size, int64_t timeUs, uint32_t flags,
        AString *errorDetailMsg) {
    return mCodec->queueInputBuffer(
            index, offset, size, timeUs, flags, errorDetailMsg);
}

status_t JMediaCodec::queueSecureInputBuffer(
        size_t index,
        size_t offset,
        const CryptoPlugin::SubSample *subSamples,
        size_t numSubSamples,
        const uint8_t key[16],
        const uint8_t iv[16],
        CryptoPlugin::Mode mode,
        int64_t presentationTimeUs,
        uint32_t flags,
        AString *errorDetailMsg) {
    return mCodec->queueSecureInputBuffer(
            index, offset, subSamples, numSubSamples, key, iv, mode,
            presentationTimeUs, flags, errorDetailMsg);
}

status_t JMediaCodec::dequeueInputBuffer(size_t *index, int64_t timeoutUs) {
    return mCodec->dequeueInputBuffer(index, timeoutUs);
}

status_t JMediaCodec::dequeueOutputBuffer(
        JNIEnv *env, jobject bufferInfo, size_t *index, int64_t timeoutUs) {
    size_t size, offset;
    int64_t timeUs;
    uint32_t flags;
    status_t err = mCodec->dequeueOutputBuffer(
            index, &offset, &size, &timeUs, &flags, timeoutUs);

    if (err != OK) {
        return err;
    }

    ScopedLocalRef<jclass> clazz(
            env, env->FindClass("android/media/MediaCodec$BufferInfo"));

    jmethodID method = env->GetMethodID(clazz.get(), "set", "(IIJI)V");
    env->CallVoidMethod(bufferInfo, method, (jint)offset, (jint)size, timeUs, flags);

    return OK;
}

status_t JMediaCodec::dequeueOutputBuffer( JNIEnv *env, size_t *index, size_t &offset, size_t &size, int64_t &timeUs, uint32_t &flags, int64_t timeoutUs)
{
    status_t err = mCodec->dequeueOutputBuffer( index, &offset, &size, &timeUs, &flags, timeoutUs);

    if (err != OK) 
    {
        return err;
    }

    return OK;
}

status_t JMediaCodec::releaseOutputBuffer(
        size_t index, bool render, bool updatePTS, int64_t timestampNs) {
    if (updatePTS) {
        return mCodec->renderOutputBufferAndRelease(index, timestampNs);
    }
    return render
        ? mCodec->renderOutputBufferAndRelease(index)
        : mCodec->releaseOutputBuffer(index);
}

status_t JMediaCodec::signalEndOfInputStream() {
    return mCodec->signalEndOfInputStream();
}

status_t JMediaCodec::getFormat(JNIEnv *env, bool input, jobject *format) const {
    sp<AMessage> msg;
    status_t err;
    err = input ? mCodec->getInputFormat(&msg) : mCodec->getOutputFormat(&msg);
    if (err != OK) {
        return err;
    }

    return ConvertMessageToMap(env, msg, format);
}

status_t JMediaCodec::getOutputFormat(JNIEnv *env, size_t index, jobject *format) const {
    sp<AMessage> msg;
    status_t err;
    if ((err = mCodec->getOutputFormat(index, &msg)) != OK) {
        return err;
    }

    return ConvertMessageToMap(env, msg, format);
}

status_t JMediaCodec::getBuffers(
        JNIEnv *env, bool input, jobjectArray *bufArray) const {
    Vector<sp<ABuffer> > buffers;

    status_t err =
        input
            ? mCodec->getInputBuffers(&buffers)
            : mCodec->getOutputBuffers(&buffers);

    if (err != OK) {
        return err;
    }

    *bufArray = (jobjectArray)env->NewObjectArray(
            buffers.size(), mByteBufferClass, NULL);
    if (*bufArray == NULL) {
        return NO_MEMORY;
    }

    for (size_t i = 0; i < buffers.size(); ++i) {
        const sp<ABuffer> &buffer = buffers.itemAt(i);

        jobject byteBuffer = NULL;
        err = createByteBufferFromABuffer(
                env, !input /* readOnly */, true /* clearBuffer */, buffer, &byteBuffer);
        if (err != OK) {
            return err;
        }
        if (byteBuffer != NULL) {
            env->SetObjectArrayElement(
                    *bufArray, i, byteBuffer);

            env->DeleteLocalRef(byteBuffer);
            byteBuffer = NULL;
        }
    }

    return OK;
}

// static
status_t JMediaCodec::createByteBufferFromABuffer(
        JNIEnv *env, bool readOnly, bool clearBuffer, const sp<ABuffer> &buffer,
        jobject *buf) const {
    // if this is an ABuffer that doesn't actually hold any accessible memory,
    // use a null ByteBuffer
    *buf = NULL;
    if (buffer->base() == NULL) {
        return OK;
    }

    jobject byteBuffer =
        env->NewDirectByteBuffer(buffer->base(), buffer->capacity());
    if (readOnly && byteBuffer != NULL) {
        jobject readOnlyBuffer = env->CallObjectMethod(
                byteBuffer, mByteBufferAsReadOnlyBufferMethodID);
        env->DeleteLocalRef(byteBuffer);
        byteBuffer = readOnlyBuffer;
    }
    if (byteBuffer == NULL) {
        return NO_MEMORY;
    }
    jobject me = env->CallObjectMethod(
            byteBuffer, mByteBufferOrderMethodID, mNativeByteOrderObj);
    env->DeleteLocalRef(me);
    me = env->CallObjectMethod(
            byteBuffer, mByteBufferLimitMethodID,
            clearBuffer ? buffer->capacity() : (buffer->offset() + buffer->size()));
    env->DeleteLocalRef(me);
    me = env->CallObjectMethod(
            byteBuffer, mByteBufferPositionMethodID,
            clearBuffer ? 0 : buffer->offset());
    env->DeleteLocalRef(me);
    me = NULL;

    *buf = byteBuffer;
    return OK;
}

status_t JMediaCodec::getBuffer(
        JNIEnv *env, bool input, size_t index, jobject *buf) const {
    sp<ABuffer> buffer;

    status_t err =
        input
            ? mCodec->getInputBuffer(index, &buffer)
            : mCodec->getOutputBuffer(index, &buffer);

    if (err != OK) {
        return err;
    }

    return createByteBufferFromABuffer(
            env, !input /* readOnly */, input /* clearBuffer */, buffer, buf);
}

status_t JMediaCodec::getImage(
        JNIEnv *env, bool input, size_t index, jobject *buf) const {
    sp<ABuffer> buffer;

    status_t err =
        input
            ? mCodec->getInputBuffer(index, &buffer)
            : mCodec->getOutputBuffer(index, &buffer);

    if (err != OK) {
        return err;
    }

    // if this is an ABuffer that doesn't actually hold any accessible memory,
    // use a null ByteBuffer
    *buf = NULL;
    if (buffer->base() == NULL) {
        return OK;
    }

    // check if buffer is an image
    sp<ABuffer> imageData;
    if (!buffer->meta()->findBuffer("image-data", &imageData)) {
        return OK;
    }

    int64_t timestamp = 0;
    if (!input && buffer->meta()->findInt64("timeUs", &timestamp)) {
        timestamp *= 1000; // adjust to ns
    }

    jobject byteBuffer = NULL;
    err = createByteBufferFromABuffer(
            env, !input /* readOnly */, input /* clearBuffer */, buffer, &byteBuffer);
    if (err != OK) {
        return OK;
    }

    jobject infoBuffer = NULL;
    err = createByteBufferFromABuffer(
            env, true /* readOnly */, true /* clearBuffer */, imageData, &infoBuffer);
    if (err != OK) {
        env->DeleteLocalRef(byteBuffer);
        byteBuffer = NULL;
        return OK;
    }

    jobject cropRect = NULL;
    int32_t left, top, right, bottom;
    if (buffer->meta()->findRect("crop-rect", &left, &top, &right, &bottom)) {
        ScopedLocalRef<jclass> rectClazz(
                env, env->FindClass("android/graphics/Rect"));
        CHECK(rectClazz.get() != NULL);

        jmethodID rectConstructID = env->GetMethodID(
                rectClazz.get(), "<init>", "(IIII)V");

        cropRect = env->NewObject(
                rectClazz.get(), rectConstructID, left, top, right + 1, bottom + 1);
    }

    ScopedLocalRef<jclass> imageClazz(
            env, env->FindClass("android/media/MediaCodec$MediaImage"));
    CHECK(imageClazz.get() != NULL);

    jmethodID imageConstructID = env->GetMethodID(imageClazz.get(), "<init>",
            "(Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;ZJIILandroid/graphics/Rect;)V");

    *buf = env->NewObject(imageClazz.get(), imageConstructID,
            byteBuffer, infoBuffer,
            (jboolean)!input /* readOnly */,
            (jlong)timestamp,
            (jint)0 /* xOffset */, (jint)0 /* yOffset */, cropRect);

    // if MediaImage creation fails, return null
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        *buf = NULL;
    }

    if (cropRect != NULL) {
        env->DeleteLocalRef(cropRect);
        cropRect = NULL;
    }

    env->DeleteLocalRef(byteBuffer);
    byteBuffer = NULL;

    env->DeleteLocalRef(infoBuffer);
    infoBuffer = NULL;

    return OK;
}

status_t JMediaCodec::getName(JNIEnv *env, jstring *nameStr) const {
    AString name;

    status_t err = mCodec->getName(&name);

    if (err != OK) {
        return err;
    }

    *nameStr = env->NewStringUTF(name.c_str());

    return OK;
}

status_t JMediaCodec::setParameters(const sp<AMessage> &msg) {
    return mCodec->setParameters(msg);
}

void JMediaCodec::setVideoScalingMode(int mode) {
    if (mSurfaceTextureClient != NULL) {
        native_window_set_scaling_mode(mSurfaceTextureClient.get(), mode);
    }
}

static jthrowable createCodecException(
        JNIEnv *env, status_t err, int32_t actionCode, const char *msg = NULL) {
    ScopedLocalRef<jclass> clazz(
            env, env->FindClass("android/media/MediaCodec$CodecException"));
    CHECK(clazz.get() != NULL);

    const jmethodID ctor = env->GetMethodID(clazz.get(), "<init>", "(IILjava/lang/String;)V");
    CHECK(ctor != NULL);

    ScopedLocalRef<jstring> msgObj(
            env, env->NewStringUTF(msg != NULL ? msg : String8::format("Error %#x", err)));

    // translate action code to Java equivalent
    switch (actionCode) {
    case ACTION_CODE_TRANSIENT:
        actionCode = gCodecActionCodes.codecActionTransient;
        break;
    case ACTION_CODE_RECOVERABLE:
        actionCode = gCodecActionCodes.codecActionRecoverable;
        break;
    default:
        actionCode = 0;  // everything else is fatal
        break;
    }

    return (jthrowable)env->NewObject(clazz.get(), ctor, err, actionCode, msgObj.get());
}

void JMediaCodec::handleCallback(const sp<AMessage> &msg) {
    int32_t arg1, arg2 = 0;
    jobject obj = NULL;
    CHECK(msg->findInt32("callbackID", &arg1));
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    switch (arg1) {
        case MediaCodec::CB_INPUT_AVAILABLE:
        {
            CHECK(msg->findInt32("index", &arg2));
            break;
        }

        case MediaCodec::CB_OUTPUT_AVAILABLE:
        {
            CHECK(msg->findInt32("index", &arg2));

            size_t size, offset;
            int64_t timeUs;
            uint32_t flags;
            CHECK(msg->findSize("size", &size));
            CHECK(msg->findSize("offset", &offset));
            CHECK(msg->findInt64("timeUs", &timeUs));
            CHECK(msg->findInt32("flags", (int32_t *)&flags));

            ScopedLocalRef<jclass> clazz(
                    env, env->FindClass("android/media/MediaCodec$BufferInfo"));
            jmethodID ctor = env->GetMethodID(clazz.get(), "<init>", "()V");
            jmethodID method = env->GetMethodID(clazz.get(), "set", "(IIJI)V");

            obj = env->NewObject(clazz.get(), ctor);

            if (obj == NULL) {
                if (env->ExceptionCheck()) {
                    ALOGE("Could not create MediaCodec.BufferInfo.");
                    env->ExceptionClear();
                }
                jniThrowException(env, "java/lang/IllegalStateException", NULL);
                return;
            }

            env->CallVoidMethod(obj, method, (jint)offset, (jint)size, timeUs, flags);
            break;
        }

        case MediaCodec::CB_ERROR:
        {
            int32_t err, actionCode;
            CHECK(msg->findInt32("err", &err));
            CHECK(msg->findInt32("actionCode", &actionCode));

            // note that DRM errors could conceivably alias into a CodecException
            obj = (jobject)createCodecException(env, err, actionCode);

            if (obj == NULL) {
                if (env->ExceptionCheck()) {
                    ALOGE("Could not create CodecException object.");
                    env->ExceptionClear();
                }
                jniThrowException(env, "java/lang/IllegalStateException", NULL);
                return;
            }

            break;
        }

        case MediaCodec::CB_OUTPUT_FORMAT_CHANGED:
        {
            sp<AMessage> format;
            CHECK(msg->findMessage("format", &format));

            if (OK != ConvertMessageToMap(env, format, &obj)) {
                jniThrowException(env, "java/lang/IllegalStateException", NULL);
                return;
            }

            break;
        }

        default:
            TRESPASS();
    }

    env->CallVoidMethod(
            mObject,
            gFields.postEventFromNativeID,
            EVENT_CALLBACK,
            arg1,
            arg2,
            obj);

    env->DeleteLocalRef(obj);
}

void JMediaCodec::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatCallbackNotify:
        {
            handleCallback(msg);
            break;
        }
        default:
            TRESPASS();
    }
}

}  // namespace android

////////////////////////////////////////////////////////////////////////////////
