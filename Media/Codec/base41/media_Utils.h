

#ifndef _CODEC_MEDIA_UTILS_H_
#define _CODEC_MEDIA_UTILS_H_

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/KeyedVector.h>
#include <utils/String8.h>

namespace android {

/**
 * Returns true if the conversion is successful; otherwise, false.
 */
bool ConvertKeyValueArraysToKeyedVector(
    JNIEnv *env, jobjectArray keys, jobjectArray values,
    KeyedVector<String8, String8>* vector);

struct AMessage;
status_t ConvertMessageToMap(
        JNIEnv *env, const sp<AMessage> &msg, jobject *map);

status_t ConvertKeyValueArraysToMessage(
        JNIEnv *env, jobjectArray keys, jobjectArray values,
        sp<AMessage> *msg);

};  // namespace android

#endif //  _ANDROID_MEDIA_UTILS_H_
