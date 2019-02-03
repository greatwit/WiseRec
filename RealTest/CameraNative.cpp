

#include <stdio.h>
#include "CameraStub.h"
#include "CameraNdkEncodec.h"
#include <android/native_window_jni.h>

#include "ComDefine.h"
#define TAG "CameraNative"

#define CLASS_PATH	"com/great/happyness/medialib/NativeCamera"

CameraStub *gCamera = NULL;
CameraNdkEncodec *gNdkenCam = NULL;

static jboolean OpenCamera(JNIEnv *env, jobject, jint cameraId, jstring clientName)
{
	if(!gCamera)
		gCamera = new CameraStub();

	gCamera->CreateCamera(cameraId, clientName);

//	const char *readfile = env->GetStringUTFChars(clientName, NULL);
//	gCamera->CreateCamera(cameraId, (char*)readfile);
//	GLOGW("OpenCamera clientName:%s\n", readfile);
//	env->ReleaseStringUTFChars(clientName, readfile);

	return true;
}

static jboolean CloseCamera(JNIEnv *env, jobject) {
	if(gCamera) {
		gCamera->CloseCamera();
		delete gCamera;
		gCamera = NULL;
	}

	return true;
}

static void SetCameraParameter(JNIEnv *env, jobject, jstring params) {
	gCamera->SetCameraParameter(params);
}

static jstring GetCameraParameter(JNIEnv *env, jobject) {
	return gCamera->GetCameraParameter();
}

static void StartPreview(JNIEnv *env, jobject, jobject surface) {
	if(surface!=NULL) {
		//sp<Surface> surface(android_view_Surface_getSurface(env, surface));
		ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);
		gCamera->StartPreview(pAnw);
	}
}

static void StopPreview(JNIEnv *env, jobject) {
	gCamera->StopPreview();
}
////////////////////////////////////////////////////////////////CAMERA NDK ENCODEC///////////////////////////////////////////////////////////////

static jboolean StartCamndkEncodec(JNIEnv *env, jobject, jstring readpath, jobject surface) {
	bool ret = true;
	if(gNdkenCam==NULL)
		gNdkenCam = new CameraNdkEncodec();

	ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);
	const char *readfile = env->GetStringUTFChars(readpath, NULL);

	gNdkenCam->startPlayer(readfile, pAnw, 0, 0);

	GLOGW("StartCamndkEncodec readfile:%s\n", readfile);
	env->ReleaseStringUTFChars(readpath, readfile);

	return ret;
}

static jboolean StopCamndkEncodec(JNIEnv *env, jobject) {
	bool ret = true;
	if(gNdkenCam) {
		gNdkenCam->stopPlayer();
		delete gNdkenCam;
		gNdkenCam = NULL;
	}
	return ret;
}

static void McndkSetCameraParam(JNIEnv *env, jobject, jstring params) {
	gNdkenCam->SetCameraParameter(params);
}

static jstring McndkGetCameraParam(JNIEnv *env, jobject) {
	return gNdkenCam->GetCameraParameter();
}

static jboolean McndkSetInt32(JNIEnv *env, jobject, jstring key, jint value) {
	jboolean isOk  = JNI_FALSE;

	const char *ck = env->GetStringUTFChars(key, &isOk);
	gNdkenCam->setInt32(ck, value);
	env->ReleaseStringUTFChars(key, ck);
	return true;
}

static jboolean McndkOpenCamera(JNIEnv *env, jobject, jint cameraId, jstring clientName)
{
	if(gNdkenCam==NULL)
		gNdkenCam = new CameraNdkEncodec();

	gNdkenCam->openCamera(cameraId, clientName);

	return true;
}

static jboolean McndkCloseCamera(JNIEnv *env, jobject) {
	if(gNdkenCam) {
		gNdkenCam->closeCamera();
	}

	return true;
}

static JNINativeMethod gMethods[] =
{
		{ "OpenCamera", "(ILjava/lang/String;)Z", (void *)OpenCamera },
		{ "CloseCamera", "()Z", (void *)CloseCamera },
		{ "SetCameraParameter", "(Ljava/lang/String;)V", (void *)SetCameraParameter },
		{ "GetCameraParameter", "()Ljava/lang/String;", (void *)GetCameraParameter },
		{ "StartPreview", "(Landroid/view/Surface;)V", (void *)StartPreview },
		{ "StopPreview", "()V", (void *)StopPreview },

		{ "StartCamndkEncodec", "(Ljava/lang/String;Landroid/view/Surface;)Z", (void *)StartCamndkEncodec },
		{ "StopCamndkEncodec", "()Z", (void *)StopCamndkEncodec },
		{ "McndkSetCameraParam", "(Ljava/lang/String;)V", (void *)McndkSetCameraParam },
		{ "McndkGetCameraParam", "()Ljava/lang/String;", (void *)McndkGetCameraParam },
		{ "McndkSetInt32", "(Ljava/lang/String;I)Z", (void*)McndkSetInt32 },
		{ "McndkOpenCamera", "(ILjava/lang/String;)Z", (void *)McndkOpenCamera },
		{ "McndkCloseCamera", "()Z", (void *)McndkCloseCamera },
};

int jniRegisterNativeMethods1(JNIEnv* env, const char* className,
							  const JNINativeMethod* methods, int numMethods)
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

int registerNatives(JNIEnv *env)
{
	return jniRegisterNativeMethods1(env, CLASS_PATH, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = JNI_ERR;

	GLOGW("loading . . .1");

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		GLOGE("GetEnv failed!");
		return result;
	}

	if( registerNatives(env) != JNI_OK) {
		GLOGE("can't registerNatives");
	}

	GLOGW( "loading . . .2");

	return JNI_VERSION_1_6;
}

