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

#include <system/window.h>

struct BufferInfo
{
    size_t size;
    size_t offset;
    int64_t timeUs;
    uint32_t flags;
};

namespace android {

// Keep these in sync with their equivalents in MediaCodec.java !!!
enum {
    DEQUEUE_INFO_TRY_AGAIN_LATER            = -1,
    DEQUEUE_INFO_OUTPUT_FORMAT_CHANGED      = -2,
    DEQUEUE_INFO_OUTPUT_BUFFERS_CHANGED     = -3,
};

struct fields_t {
    jfieldID context;

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

    mLooper = new ALooper;
    mLooper->setName("MediaCodec_looper");

    mLooper->start(
            false,      // runOnCallingThread
            false,       // canCallJava
            PRIORITY_DEFAULT);

    if (nameIsType) {
        mCodec = MediaCodec::CreateByType(mLooper, name, encoder);
    } else {
        mCodec = MediaCodec::CreateByComponentName(mLooper, name);
    }
}

status_t JMediaCodec::initCheck() const {
    return mCodec != NULL ? OK : NO_INIT;
}

JMediaCodec::~JMediaCodec() {
    if (mCodec != NULL) {
        mCodec->release();
        mCodec.clear();
    }

    JNIEnv *env = AndroidRuntime::getJNIEnv();

    env->DeleteWeakGlobalRef(mObject);
    mObject = NULL;
    env->DeleteGlobalRef(mClass);
    mClass = NULL;
}

status_t JMediaCodec::configure(
        const sp<AMessage> &format,
        const sp<IGraphicBufferProducer> &bufferProducer,
        const sp<ICrypto> &crypto,
        int flags) {
    sp<Surface> client;
    if (bufferProducer != NULL) {
        mSurfaceTextureClient = new Surface(bufferProducer);
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
    status_t err;
    if ((err = mCodec->dequeueOutputBuffer(
                    index, &offset, &size, &timeUs, &flags, timeoutUs)) != OK) {
        return err;
    }

    jclass clazz = env->FindClass("android/media/MediaCodec$BufferInfo");

    jmethodID method = env->GetMethodID(clazz, "set", "(IIJI)V");
    env->CallVoidMethod(bufferInfo, method, offset, size, timeUs, flags);

    return OK;
}

status_t JMediaCodec::dequeueOutputBuffer(JNIEnv *env, size_t *index, size_t &offset, size_t &size, int64_t &timeUs, uint32_t &flags, int64_t timeoutUs) {

    status_t err = mCodec->dequeueOutputBuffer( index, &offset, &size, &timeUs, &flags, timeoutUs);
    if (err != OK)
        return err;

    return OK;
}

status_t JMediaCodec::releaseOutputBuffer(size_t index, bool render) {
    return render
        ? mCodec->renderOutputBufferAndRelease(index)
        : mCodec->releaseOutputBuffer(index);
}

status_t JMediaCodec::releaseOutputBuffer(size_t index, bool render, bool updatePTS, int64_t timestampNs) {
    if (updatePTS) {
    }
    return render
        ? mCodec->renderOutputBufferAndRelease(index)
        : mCodec->releaseOutputBuffer(index);
}

status_t JMediaCodec::signalEndOfInputStream() {
    return mCodec->signalEndOfInputStream();
}

status_t JMediaCodec::getOutputFormat(JNIEnv *env, jobject *format) const {
    sp<AMessage> msg;
    status_t err;
    if ((err = mCodec->getOutputFormat(&msg)) != OK) {
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

    jclass byteBufferClass = env->FindClass("java/nio/ByteBuffer");
    CHECK(byteBufferClass != NULL);

    jmethodID orderID = env->GetMethodID(
            byteBufferClass,
            "order",
            "(Ljava/nio/ByteOrder;)Ljava/nio/ByteBuffer;");

    CHECK(orderID != NULL);

    jclass byteOrderClass = env->FindClass("java/nio/ByteOrder");
    CHECK(byteOrderClass != NULL);

    jmethodID nativeOrderID = env->GetStaticMethodID(
            byteOrderClass, "nativeOrder", "()Ljava/nio/ByteOrder;");
    CHECK(nativeOrderID != NULL);

    jobject nativeByteOrderObj =
        env->CallStaticObjectMethod(byteOrderClass, nativeOrderID);
    CHECK(nativeByteOrderObj != NULL);

    *bufArray = (jobjectArray)env->NewObjectArray(
            buffers.size(), byteBufferClass, NULL);
    if (*bufArray == NULL) {
        env->DeleteLocalRef(nativeByteOrderObj);
        return NO_MEMORY;
    }

    for (size_t i = 0; i < buffers.size(); ++i) {
        const sp<ABuffer> &buffer = buffers.itemAt(i);

        // if this is an ABuffer that doesn't actually hold any accessible memory,
        // use a null ByteBuffer
        if (buffer->base() == NULL) {
            continue;
        }
        jobject byteBuffer =
            env->NewDirectByteBuffer(
                buffer->base(),
                buffer->capacity());
        if (byteBuffer == NULL) {
            env->DeleteLocalRef(nativeByteOrderObj);
            return NO_MEMORY;
        }
        jobject me = env->CallObjectMethod(
                byteBuffer, orderID, nativeByteOrderObj);
        env->DeleteLocalRef(me);
        me = NULL;

        env->SetObjectArrayElement(
                *bufArray, i, byteBuffer);

        env->DeleteLocalRef(byteBuffer);
        byteBuffer = NULL;
    }

    env->DeleteLocalRef(nativeByteOrderObj);
    nativeByteOrderObj = NULL;

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

void JMediaCodec::setVideoScalingMode(int mode) {
    if (mSurfaceTextureClient != NULL) {
        native_window_set_scaling_mode(mSurfaceTextureClient.get(), mode);
    }
}

}  // namespace android

////////////////////////////////////////////////////////////////////////////////

