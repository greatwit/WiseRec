

#include <stdio.h>
#include <android/native_window_jni.h>

#include "CameraListen.h"

#include "ComDefine.h"
#define TAG "CameraTest"

#define CLASS_PATH	"com/great/happyness/medialib/CameraTest7"


sp<JNICameraListen> gCamera = NULL;

static jboolean OpenCamera(JNIEnv *env, jobject, jint cameraId, jstring clientName)
{
	if(gCamera==NULL)
		gCamera = new JNICameraListen();

	gCamera->CameraSetup(cameraId, clientName);

//	const char *readfile = env->GetStringUTFChars(clientName, NULL);
//	gCamera->CreateCamera(cameraId, (char*)readfile);
//	GLOGW("OpenCamera clientName:%s\n", readfile);
//	env->ReleaseStringUTFChars(clientName, readfile);

	return true;
}

static jboolean CloseCamera(JNIEnv *env, jobject) {
	if(gCamera!=NULL) {
		gCamera->CameraRelease();
		gCamera = NULL;
	}

	return true;
}

static void SetCameraParameter(JNIEnv *env, jobject, jstring params) {
	gCamera->setParameters(params);
}

static jstring GetCameraParameter(JNIEnv *env, jobject) {
	return gCamera->getParameters();
}

static void StartPreview(JNIEnv *env, jobject, jobject surface) {
	if(surface!=NULL) {
		//sp<Surface> surface(android_view_Surface_getSurface(env, surface));
		ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);
		gCamera->startPreview_(pAnw);
	}
}

static void StopPreview(JNIEnv *env, jobject) {
	gCamera->stopPreview();
}

static JNINativeMethod gMethods[] =
{
		{ "OpenCamera", "(ILjava/lang/String;)Z", (void *)OpenCamera },
		{ "CloseCamera", "()Z", (void *)CloseCamera },
		{ "SetCameraParameter", "(Ljava/lang/String;)V", (void *)SetCameraParameter },
		{ "GetCameraParameter", "()Ljava/lang/String;", (void *)GetCameraParameter },
		{ "StartPreview", "(Landroid/view/Surface;)V", (void *)StartPreview },
		{ "StopPreview", "()V", (void *)StopPreview },
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

