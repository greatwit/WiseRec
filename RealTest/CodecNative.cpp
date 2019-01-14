

#include <stdio.h>
#include "CodecStub.h"
//#include "android_runtime/android_view_Surface.h"
#include <android/native_window_jni.h>

#include "ComDefine.h"
#define TAG "CodecNative"

#define CLASS_PATH	"com/great/happyness/medialib/NativeCodec"

CodecStub *gCodec = NULL;

static jboolean StartEncoderTest(JNIEnv *env, jobject,
										jobjectArray keys, jobjectArray values,
										jobject jsurface,
										jint flags,
										jstring readpath,
										jstring writepath)
{
	if(!gCodec) {
		gCodec = new CodecStub();

		sp<AMessage> format;
		GLOGW("StartEncoderTest 1\n");
		status_t err = CodecContext::getInstance()->ConvertKeyValueToMessage(env, keys, values, &format);
		GLOGW("StartEncoderTest 2\n");
		ANativeWindow *pAnw = ANativeWindow_fromSurface(env, jsurface);
		const char *readfile = env->GetStringUTFChars(readpath, NULL);
		const char *writefile = env->GetStringUTFChars(writepath, NULL);
		gCodec->CreateEncodec(format, pAnw, flags, readfile, writefile);
		GLOGW("StartEncoderTest readfile:%s\n", readfile);
		env->ReleaseStringUTFChars(readpath, readfile);
		env->ReleaseStringUTFChars(writepath, writefile);

	}
	return true;
}

static jboolean StopEncoderTest(JNIEnv *env, jobject) {
	if(gCodec) {
		gCodec->CloseCodec();
		delete gCodec;
		gCodec = NULL;
	}
	return true;
}

static jboolean StartDecoderTest(JNIEnv *env, jobject,
										jobjectArray keys, jobjectArray values,
										jobject jsurface,
										jint flags,
										jstring readpath)
{
	if(!gCodec) {
		gCodec = new CodecStub();

		sp<AMessage> format;
		GLOGW("StartDecoderTest 1\n");
		status_t err = CodecContext::getInstance()->ConvertKeyValueToMessage(env, keys, values, &format);
		GLOGW("StartDecoderTest 2\n");
		ANativeWindow *pAnw = ANativeWindow_fromSurface(env, jsurface);
		const char *readfile = env->GetStringUTFChars(readpath, NULL);
		gCodec->CreateDecodec(format, pAnw, flags, readfile);
		GLOGW("StartDecoderTest readfile:%s\n", readfile);
		env->ReleaseStringUTFChars(readpath, readfile);

	}
	return true;
}

static jboolean StopDecoderTest(JNIEnv *env, jobject) {
	if(gCodec) {
		gCodec->CloseCodec();
		delete gCodec;
		gCodec = NULL;
	}
	return true;
}

static JNINativeMethod gMethods[] =
{
		{ "StartEncoderTest", "([Ljava/lang/String;[Ljava/lang/Object;Landroid/view/Surface;"
						"ILjava/lang/String;Ljava/lang/String;)Z", (void *)StartEncoderTest },
		{ "StopEncoderTest", "()Z", (void *)StopEncoderTest },
		{ "StartDecoderTest", "([Ljava/lang/String;[Ljava/lang/Object;Landroid/view/Surface;"
						"ILjava/lang/String;)Z", (void *)StartDecoderTest },
		{ "StopDecoderTest", "()Z", (void *)StopDecoderTest },
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

