/*
 * Copyright (C) 2008 The Android Open Source Project
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
#define LOG_TAG "MediaRecorderJNI"
#include <utils/Log.h>

#include <gui/Surface.h>
#include <camera/ICameraService.h>
#include <camera/Camera.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <system/audio.h>
#include <android_runtime/android_view_Surface.h>

#include <media/mediarecorder.h>
#include "MediaRecorderClient.h"

// ----------------------------------------------------------------------------

#include "ComDefine.h"
#define TAG "myMediaRecorder"


using namespace android;

class MyMediaRecorderListen: public BpInterface<IMediaRecorderClient>
{
public:
    MyMediaRecorderListen(const sp<IBinder>& impl)
        : BpInterface<IMediaRecorderClient>(impl)
    {
    }

    virtual void notify(int msg, int ext1, int ext2)
    {
    }
};

// ----------------------------------------------------------------------------

// helper function to extract a native Camera object from a Camera Java object
extern sp<Camera> get_native_camera(JNIEnv *env, jobject thiz, struct JNICameraContext** context);

struct fields_t {
    jfieldID    context;
    jfieldID    surface;

    jmethodID   post_event;
};
static fields_t fields;

static Mutex sLock;

class RecorderListener: virtual public RefBase
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
};

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaRecorderListener: public RecorderListener
{
public:
    JNIMediaRecorderListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIMediaRecorderListener();
    void notify(int msg, int ext1, int ext2);
private:
    JNIMediaRecorderListener();
    jclass      mClass;     // Reference to MediaRecorder class
    jobject     mObject;    // Weak ref to MediaRecorder Java object to call on
};

JNIMediaRecorderListener::JNIMediaRecorderListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaRecorder class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find android/media/MediaRecorder");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaRecorder object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIMediaRecorderListener::~JNIMediaRecorderListener()
{
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIMediaRecorderListener::notify(int msg, int ext1, int ext2)
{
    ALOGV("JNIMediaRecorderListener::notify");

    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->CallStaticVoidMethod(mClass, fields.post_event, mObject, msg, ext1, ext2, 0);
}

// ----------------------------------------------------------------------------

static sp<Surface> get_surface(JNIEnv* env, jobject clazz)
{
    ALOGV("get_surface");
    return android_view_Surface_getSurface(env, clazz);
}

// Returns true if it throws an exception.
static bool process_media_recorder_call(JNIEnv *env, status_t opStatus, const char* exception, const char* message)
{
    ALOGV("process_media_recorder_call");
    if (opStatus == (status_t)INVALID_OPERATION) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return true;
    } else if (opStatus != (status_t)OK) {
        jniThrowException(env, exception, message);
        return true;
    }
    return false;
}

MediaRecorderClient *g_client = NULL;

static MediaRecorderClient* getMediaRecorder(JNIEnv* env, jobject thiz)
{
    Mutex::Autolock l(sLock);
    //MediaRecorderClient* const p = (MediaRecorderClient*)env->GetIntField(thiz, fields.context);
    return g_client;
}

static MediaRecorderClient* setMediaRecorder(JNIEnv* env, jobject thiz, MediaRecorderClient* recorder)
{
    Mutex::Autolock l(sLock);
    g_client = recorder;
    return g_client;
}


static void android_media_MediaRecorder_setCamera(JNIEnv* env, jobject thiz, jobject camera)
{
    // we should not pass a null camera to get_native_camera() call.
    if (camera == NULL) {
        jniThrowNullPointerException(env, "camera object is a NULL pointer");
        return;
    }
    sp<Camera> c = get_native_camera(env, camera, NULL);
    MediaRecorderClient*mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setCamera(c->remote(), c->getRecordingProxy()),
            "java/lang/RuntimeException", "setCamera failed.");
}

static void
android_media_MediaRecorder_setVideoSource(JNIEnv *env, jobject thiz, jint vs)
{
    ALOGV("setVideoSource(%d)", vs);
    if (vs < VIDEO_SOURCE_DEFAULT || vs >= VIDEO_SOURCE_LIST_END) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid video source");
        return;
    }
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setVideoSource(vs), "java/lang/RuntimeException", "setVideoSource failed.");
}

static void
android_media_MediaRecorder_setAudioSource(JNIEnv *env, jobject thiz, jint as)
{
    ALOGV("setAudioSource(%d)", as);
    if (as < AUDIO_SOURCE_DEFAULT || as >= AUDIO_SOURCE_CNT) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid audio source");
        return;
    }

    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setAudioSource(as), "java/lang/RuntimeException", "setAudioSource failed.");
}

static void
android_media_MediaRecorder_setOutputFormat(JNIEnv *env, jobject thiz, jint of)
{
    ALOGV("setOutputFormat(%d)", of);
    if (of < OUTPUT_FORMAT_DEFAULT || of >= OUTPUT_FORMAT_LIST_END) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid output format");
        return;
    }
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setOutputFormat(of), "java/lang/RuntimeException", "setOutputFormat failed.");
}

static void
android_media_MediaRecorder_setVideoEncoder(JNIEnv *env, jobject thiz, jint ve)
{
    ALOGV("setVideoEncoder(%d)", ve);
    if (ve < VIDEO_ENCODER_DEFAULT || ve >= VIDEO_ENCODER_LIST_END) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid video encoder");
        return;
    }
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setVideoEncoder(ve), "java/lang/RuntimeException", "setVideoEncoder failed.");
}

static void
android_media_MediaRecorder_setAudioEncoder(JNIEnv *env, jobject thiz, jint ae)
{
    ALOGV("setAudioEncoder(%d)", ae);
    if (ae < AUDIO_ENCODER_DEFAULT || ae >= AUDIO_ENCODER_LIST_END) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid audio encoder");
        return;
    }
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setAudioEncoder(ae), "java/lang/RuntimeException", "setAudioEncoder failed.");
}

static void
android_media_MediaRecorder_setParameter(JNIEnv *env, jobject thiz, jstring params)
{
    ALOGV("setParameter()");
    if (params == NULL)
    {
        ALOGE("Invalid or empty params string.  This parameter will be ignored.");
        return;
    }

    MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    const char* params8 = env->GetStringUTFChars(params, NULL);
    if (params8 == NULL)
    {
        ALOGE("Failed to covert jstring to String8.  This parameter will be ignored.");
        return;
    }

    process_media_recorder_call(env, mr->setParameters(String8(params8)), "java/lang/RuntimeException", "setParameter failed.");
    env->ReleaseStringUTFChars(params,params8);
}

static void
android_media_MediaRecorder_setOutputFileFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
    ALOGV("setOutputFile");
    if (fileDescriptor == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    status_t opStatus = mr->setOutputFile(fd, offset, length);
    process_media_recorder_call(env, opStatus, "java/io/IOException", "setOutputFile failed.");
}

static void
android_media_MediaRecorder_setVideoSize(JNIEnv *env, jobject thiz, jint width, jint height)
{
    ALOGV("setVideoSize(%d, %d)", width, height);
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    if (width <= 0 || height <= 0) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "invalid video size");
        return;
    }
    process_media_recorder_call(env, mr->setVideoSize(width, height), "java/lang/RuntimeException", "setVideoSize failed.");
}

static void
android_media_MediaRecorder_setVideoFrameRate(JNIEnv *env, jobject thiz, jint rate)
{
    ALOGV("setVideoFrameRate(%d)", rate);
    if (rate <= 0) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "invalid frame rate");
        return;
    }
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->setVideoFrameRate(rate), "java/lang/RuntimeException", "setVideoFrameRate failed.");
}

static void
android_media_MediaRecorder_setMaxDuration(JNIEnv *env, jobject thiz, jint max_duration_ms)
{
    ALOGV("setMaxDuration(%d)", max_duration_ms);
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    char params[64];
    sprintf(params, "max-duration=%d", max_duration_ms);

    process_media_recorder_call(env, mr->setParameters(String8(params)), "java/lang/RuntimeException", "setMaxDuration failed.");
}

static void
android_media_MediaRecorder_setMaxFileSize(
        JNIEnv *env, jobject thiz, jlong max_filesize_bytes)
{
    ALOGV("setMaxFileSize(%lld)", max_filesize_bytes);
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    char params[64];
    sprintf(params, "max-filesize=%lld", max_filesize_bytes);

    process_media_recorder_call(env, mr->setParameters(String8(params)), "java/lang/RuntimeException", "setMaxFileSize failed.");
}

static void
android_media_MediaRecorder_prepare(JNIEnv *env, jobject thiz)
{
    ALOGV("prepare");
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    jobject surface = env->GetObjectField(thiz, fields.surface);
    if (surface != NULL) {
        const sp<Surface> native_surface = get_surface(env, surface);

        // The application may misbehave and
        // the preview surface becomes unavailable
        if (native_surface.get() == 0) {
            ALOGE("Application lost the surface");
            jniThrowException(env, "java/io/IOException", "invalid preview surface");
            return;
        }

        ALOGI("prepare: surface=%p", native_surface.get());
        if (process_media_recorder_call(env, mr->setPreviewSurface(native_surface->getIGraphicBufferProducer()), "java/lang/RuntimeException", "setPreviewSurface failed.")) {
            return;
        }
    }
    process_media_recorder_call(env, mr->prepare(), "java/io/IOException", "prepare failed.");
}

static int
android_media_MediaRecorder_native_getMaxAmplitude(JNIEnv *env, jobject thiz)
{
    ALOGV("getMaxAmplitude");
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    int result = 0;
    process_media_recorder_call(env, mr->getMaxAmplitude(&result), "java/lang/RuntimeException", "getMaxAmplitude failed.");
    return result;
}

static void
android_media_MediaRecorder_start(JNIEnv *env, jobject thiz)
{
    ALOGV("start");
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->start(), "java/lang/RuntimeException", "start failed.");
}

static void
android_media_MediaRecorder_stop(JNIEnv *env, jobject thiz)
{
    ALOGV("stop");
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->stop(), "java/lang/RuntimeException", "stop failed.");
}

static void
android_media_MediaRecorder_native_reset(JNIEnv *env, jobject thiz)
{
    ALOGV("native_reset");
    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
    process_media_recorder_call(env, mr->reset(), "java/lang/RuntimeException", "native_reset failed.");
}

static void
android_media_MediaRecorder_release(JNIEnv *env, jobject thiz)
{
    ALOGV("release");
    MediaRecorderClient* mr = setMediaRecorder(env, thiz, 0);
    if (mr != NULL) {
        mr->setListener(NULL);
        mr->release();
    }
}

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in MediaRecorder, which won't run until the
// first time an instance of this class is used.
static void
android_media_MediaRecorder_native_init(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass("com/wise/mediarec/Recorder/WiseRecorder");
    if (clazz == NULL) {
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        return;
    }

    fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
    if (fields.surface == NULL) {
        return;
    }

    jclass surface = env->FindClass("android/view/Surface");
    if (surface == NULL) {
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        return;
    }
}


static void
android_media_MediaRecorder_native_setup(JNIEnv *env, jobject thiz, jobject weak_this,
                                         jstring packageName)
{
    ALOGV("setup");

	
    MediaRecorderClient* mr = new MediaRecorderClient();
    if (mr == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }
	mr->init();
    //if (mr->initCheck() != NO_ERROR) {
    //    jniThrowException(env, "java/lang/RuntimeException", "Unable to initialize media recorder");
    //    return;
    //}

    // create new listener and give it to MediaRecorder
    sp<MyMediaRecorderListen> listener = new MyMediaRecorderListen(NULL);
    mr->setListener(listener);

   // Convert client name jstring to String16
    const char16_t *rawClientName = env->GetStringChars(packageName, NULL);
    jsize rawClientNameLen = env->GetStringLength(packageName);
    String16 clientName(rawClientName, rawClientNameLen);
	GLOGE("clientName:%s", clientName.string());
    env->ReleaseStringChars(packageName, rawClientName);

    // pass client package name for permissions tracking
    mr->setClientName(clientName);

    setMediaRecorder(env, thiz, mr);
}

static void
android_media_MediaRecorder_native_finalize(JNIEnv *env, jobject thiz)
{
    ALOGV("finalize");
    android_media_MediaRecorder_release(env, thiz);
}

// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
    {"setCamera",            "(Landroid/hardware/Camera;)V",    (void *)android_media_MediaRecorder_setCamera},
    {"setVideoSource",       "(I)V",                            (void *)android_media_MediaRecorder_setVideoSource},
    {"setAudioSource",       "(I)V",                            (void *)android_media_MediaRecorder_setAudioSource},
    {"setOutputFormat",      "(I)V",                            (void *)android_media_MediaRecorder_setOutputFormat},
    {"setVideoEncoder",      "(I)V",                            (void *)android_media_MediaRecorder_setVideoEncoder},
    {"setAudioEncoder",      "(I)V",                            (void *)android_media_MediaRecorder_setAudioEncoder},
    {"setParameter",         "(Ljava/lang/String;)V",           (void *)android_media_MediaRecorder_setParameter},
    {"_setOutputFile",       "(Ljava/io/FileDescriptor;JJ)V",   (void *)android_media_MediaRecorder_setOutputFileFD},
    {"setVideoSize",         "(II)V",                           (void *)android_media_MediaRecorder_setVideoSize},
    {"setVideoFrameRate",    "(I)V",                            (void *)android_media_MediaRecorder_setVideoFrameRate},
    {"setMaxDuration",       "(I)V",                            (void *)android_media_MediaRecorder_setMaxDuration},
    {"setMaxFileSize",       "(J)V",                            (void *)android_media_MediaRecorder_setMaxFileSize},
    {"_prepare",             "()V",                             (void *)android_media_MediaRecorder_prepare},
    {"getMaxAmplitude",      "()I",                             (void *)android_media_MediaRecorder_native_getMaxAmplitude},
    {"start",                "()V",                             (void *)android_media_MediaRecorder_start},
    {"stop",                 "()V",                             (void *)android_media_MediaRecorder_stop},
    {"native_reset",         "()V",                             (void *)android_media_MediaRecorder_native_reset},
    {"release",              "()V",                             (void *)android_media_MediaRecorder_release},
    {"native_init",          "()V",                             (void *)android_media_MediaRecorder_native_init},
    {"native_setup",         "(Ljava/lang/Object;Ljava/lang/String;)V", (void *)android_media_MediaRecorder_native_setup},
    {"native_finalize",      "()V",                             (void *)android_media_MediaRecorder_native_finalize},
};

int myRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* methods, int numMethods)
{
	jclass clazz;

	GLOGW("Registering %s natives\n", className);
	clazz = env->FindClass(className);

	GLOGW("Registering %s natives 1\n", className);
	if (clazz == NULL) 
	{
		GLOGE("Native registration unable to find class '%s'\n", className);
		return -1;
	}

	GLOGW("Registering %s natives 2\n", className);
	if (env->RegisterNatives(clazz, methods, numMethods) < 0) 
	{
		GLOGE("RegisterNatives failed for '%s'\n", className);
		return -1;
	}
	GLOGW("Registering %s natives 3\n", className); 

	return 0;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) 
{
	JNIEnv* env = NULL;
	jint result = JNI_ERR;

	GLOGW("loading . . .1");
	
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) 
	{
		GLOGE("GetEnv failed!");
		return result;           
	}
	//com.wise.mediarec.Recorder
	if( myRegisterNativeMethods(env, "com/wise/mediarec/Recorder/WiseRecorder", gMethods, sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK) 
	{
		GLOGE("can't load ffmpeg");
	}
	
	
	GLOGW( "loading . . .2");
	
	result = JNI_VERSION_1_4;
	
	return result;
}

/*
static const char* const kClassPathName = "android/media/MediaRecorder";

// This function only registers the native methods, and is called from
// JNI_OnLoad in android_media_MediaPlayer.cpp
int register_android_media_MediaRecorder(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(env,
                "android/media/MediaRecorder", gMethods, NELEM(gMethods));
}
*/
