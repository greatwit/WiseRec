
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

#include <android_runtime/android_view_Surface.h>

// ----------------------------------------------------------------------------

#include "CameraFrame.h"

#include "ComDefine.h"
#define TAG "myMediaRecorder"

#define CLASS_PATH	"com/wise/mediarec/Recorder/CamCodec"

using namespace android;

CameraFrame *gCamSource = NULL;

// ----------------------------------------------------------------------------

// helper function to extract a native Camera object from a Camera Java object
extern sp<Camera> get_native_camera(JNIEnv *env, jobject thiz, struct JNICameraContext** context);


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

static int StartCamCodec(JNIEnv* env, jobject thiz, jobject camera)
{
	int rest = -1;
	if(gCamSource==NULL) {
		gCamSource = new CameraFrame();
		// we should not pass a null camera to get_native_camera() call.
		if (camera == NULL) {
			jniThrowNullPointerException(env, "camera object is a NULL pointer");
			return rest;
		}
		sp<Camera> c = get_native_camera(env, camera, NULL);
		rest = gCamSource->CameraSetup(c, 0, "", 0);
	}
	return rest;
}

static int StopCamCodec(JNIEnv *env, jobject thiz)
{
    ALOGV("stop");
    int rest = -1;
    if(gCamSource!=NULL) {
    	gCamSource->CameraSetdown();
    	delete gCamSource;
    	gCamSource  = NULL;
    }

    return rest;
}

static void
setParameter(JNIEnv *env, jobject thiz, jstring params)
{
    ALOGV("setParameter()");
    if (params == NULL) {
        ALOGE("Invalid or empty params string.  This parameter will be ignored.");
        return;
    }

    //MediaRecorderClient* mr = getMediaRecorder(env, thiz);

    const char* params8 = env->GetStringUTFChars(params, NULL);
    if (params8 == NULL) {
        ALOGE("Failed to covert jstring to String8.  This parameter will be ignored.");
        return;
    }

    //process_media_recorder_call(env, mr->setParameters(String8(params8)), "java/lang/RuntimeException", "setParameter failed.");
    env->ReleaseStringUTFChars(params,params8);
}


static void
_prepare(JNIEnv *env, jobject thiz)
{
    ALOGV("prepare");
//    MediaRecorderClient* mr = getMediaRecorder(env, thiz);
//
//    jobject surface = env->GetObjectField(thiz, fields.surface);
//    if (surface != NULL) {
//        const sp<Surface> native_surface = get_surface(env, surface);
//
//        // The application may misbehave and
//        // the preview surface becomes unavailable
//        if (native_surface.get() == 0) {
//            ALOGE("Application lost the surface");
//            jniThrowException(env, "java/io/IOException", "invalid preview surface");
//            return;
//        }
//
//        ALOGI("prepare: surface=%p", native_surface.get());
//        if (process_media_recorder_call(env, mr->setPreviewSurface(native_surface->getIGraphicBufferProducer()), "java/lang/RuntimeException", "setPreviewSurface failed.")) {
//            return;
//        }
//    }
//    process_media_recorder_call(env, mr->prepare(), "java/io/IOException", "prepare failed.");
}

// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
    {"StartCamCodec",            "(Landroid/hardware/Camera;)I",    (void *)StartCamCodec},
    {"StopCamCodec",             "()I",                             (void *)StopCamCodec},
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
	if( myRegisterNativeMethods(env, CLASS_PATH, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK)
	{
		GLOGE("can't load ffmpeg");
	}
	
	
	GLOGW( "loading . . .2");
	
	result = JNI_VERSION_1_4;
	
	return result;
}

