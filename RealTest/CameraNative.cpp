

#include <stdio.h>

#include <sys/system_properties.h>

#include "CameraContext.h"

#include "android_runtime/AndroidRuntime.h"

#include "ComDefine.h"
#define TAG "CameraNative"


#define CLASS_PATH	"com/great/happyness/Codec/CodecMedia"


void func() {
    //读取sdk版本
    char szSdkVer[32]={0};
    __system_property_get("ro.build.version.sdk", szSdkVer);
    GLOGW("sdk:%d",atoi(szSdkVer));

	JNIEnv *env = AndroidRuntime::getJNIEnv();
	jstring clientPackageName = env->NewStringUTF("com.greatmedia");
	bool bResult = CameraLib::getInstance()->LoadCameraLib(atoi(szSdkVer));
}


static JNINativeMethod gMethods[] =
{

};

int jniRegisterNativeMethods1(JNIEnv* env,
							 const char* className,
							 const JNINativeMethod* methods,
							 int numMethods)
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

	return JNI_OK;
}

