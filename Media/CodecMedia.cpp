


#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include "jni.h"

#include "cutils/properties.h"
#include <gui/Surface.h>


#include <media/ICrypto.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/MediaErrors.h>

#include <sys/system_properties.h>


#include "FileEnCodec.h"
#include "CameraEncodec.h"

#include "ComDefine.h"
#include "CodecMedia.h"
#define TAG "CodecMedia"



using namespace android;

#ifdef __cplusplus
extern "C" 
{
#endif


static FileEnCodec    	*mpFileEncoder 	= NULL;
static CameraEncodec   	*mpCamEncodec   = NULL;
 
//#define CLASS_PATH	"com/wise/mediarec/Recorder/CameraSrc"
#define CLASS_PATH	"com/great/happyness/Codec/CodecMedia"
//////////////////////////////////////////////////////////////////////////////////////file codec//////////////////////////////////////////////////////////////////////////////////////////////




static jboolean StartFileEncoder(JNIEnv *env, jobject, 
											jobjectArray keys, jobjectArray values,
											jobject jsurface,
											jobject jcrypto,
											jint flags,
											jstring filepath)
{
	ALOGE("Enter:StartFileEncoder----------->1");
	
	//读取sdk版本
	char szSdkVer[32]={0};
	__system_property_get("ro.build.version.sdk", szSdkVer);
	GLOGI("sdk:%d",atoi(szSdkVer));
	CodecBaseLib::getInstance()->LoadBaseLib(atoi(szSdkVer));
	
	sp<AMessage> format;
	status_t err = CodecBaseLib::getInstance()->ConvertKeyValueToMessage(env, keys, values, &format);//ConvertKeyValueArraysToMessage(env, keys, values, &format);

	sp<ICrypto> crypto;
	if (jcrypto != NULL) {
	//crypto = JCrypto::GetCrypto(env, jcrypto);
	}
	
	const char *file = env->GetStringUTFChars(filepath, NULL);
	
	mpFileEncoder = new FileEnCodec();
	GLOGW("Enter:StartFileEncoder----------->2");

	if(jsurface!=NULL)
	{
		sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
		mpFileEncoder->CreateCodec(format, surface, crypto, flags, (char*)file);
		GLOGE("Enter:StartFileEncoder----------->31");
	}else
	{
		mpFileEncoder->CreateCodec(format, NULL, crypto, flags, (char*)file);
		GLOGE("Enter:StartFileEncoder----------->32");
	}

	env->ReleaseStringUTFChars(filepath, file);

	GLOGW("Enter:StartFileEncoder----------->4");
	mpFileEncoder->StartVideo(0);
	GLOGW("Enter:StartFileEncoder----------->5");
	
	return true;
}

static jboolean AddEncoderData(JNIEnv *env, jobject, jbyteArray byteData, jint len)
{
	jbyte* cameraFrame = env->GetByteArrayElements(byteData, NULL);
	mpFileEncoder->AddDecodecSource(reinterpret_cast<char*>(cameraFrame), len);
	env->ReleaseByteArrayElements(byteData, cameraFrame, JNI_ABORT);
	return true;
}

static jboolean StopFileEncoder(JNIEnv *env, jobject)
{
	mpFileEncoder->StopVideo();
	return mpFileEncoder->DeInit();
}


/////////////////////////////////////////////////////////////////////////////////////CameraEncodec///////////////////////////////////////////////////////////////////////////

static jboolean StartCameraEncodec(JNIEnv *env, jobject thiz,
					jobjectArray keys, jobjectArray values, jobject jsurface, jint flags, jstring clientName)
{
	GLOGW("Enter:StartCodecSender----------->1");
	bool bResult = false;
	if(mpCamEncodec == NULL)
	{
		//读取sdk版本
		char szSdkVer[32]={0};
		__system_property_get("ro.build.version.sdk", szSdkVer);
		GLOGI("sdk:%d",atoi(szSdkVer));
		CodecBaseLib::getInstance()->LoadBaseLib(atoi(szSdkVer));

		sp<AMessage> format;
		status_t err = CodecBaseLib::getInstance()->ConvertKeyValueToMessage(env, keys, values, &format);//ConvertKeyValueArraysToMessage(env, keys, values, &format);

		sp<ICrypto> crypto;

		mpCamEncodec = new CameraEncodec();
		GLOGD("Enter:StartCodecSender----------->2");

		if(jsurface!=NULL)
		{
			sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
			bResult = mpCamEncodec->CreateCodec(thiz, format, surface, crypto, flags, 0, clientName);
			GLOGD("Enter:StartCodecSender----------->31");
		}else
		{
			bResult = mpCamEncodec->CreateCodec(thiz, format, NULL, crypto, flags, 0, clientName);
			GLOGD("Enter:StartCodecSender----------->32");
		}
	}

    return true;
}

static jboolean StartCameraVideo(JNIEnv *env, jobject thiz, jobject jsurface)
{
	if(mpCamEncodec)
	{
		sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
		mpCamEncodec->StartVideo(surface);
		GLOGE("StartCameraVideo");
		return true;
	}
	return false;
}

static jboolean StopCameraVideo(JNIEnv *env, jobject)
{
	if(mpCamEncodec)
		mpCamEncodec->StopVideo();
	GLOGW("StopCameraVideo");
	return true;
}

static jstring GetCameraParameter(JNIEnv *env, jobject)
{
    GLOGW("GetCameraParameter");
	if(mpCamEncodec)
		return mpCamEncodec->GetCameraParameter();
	else
		return NULL;
}

static jboolean SetCameraParameter(JNIEnv *env, jobject, jstring params)
{
	if(mpCamEncodec)
		mpCamEncodec->SetCameraParameter(params);
	return true;
}

static jboolean SetDisplayOrientation(JNIEnv *env, jobject, jint value)
{
	if(mpCamEncodec)
		mpCamEncodec->SetDisplayOrientation(value);
	return true;
}

static jboolean StopCameraEncodec(JNIEnv *env, jobject) {
	if(mpCamEncodec)
	{
		mpCamEncodec->StopVideo();
		mpCamEncodec->DeInit();
		delete mpCamEncodec;
		mpCamEncodec = NULL;

		return true;
	}
	return false;
}


static jboolean LoadBaseLib(JNIEnv *env, jobject obj, int version)
{
	return CodecBaseLib::getInstance()->LoadBaseLib(version);
}

#ifdef __cplusplus
}
#endif




static JNINativeMethod gMethods[] = 
{
	//FileEncoder
//	{ "StartFileEncoder",
//		"([Ljava/lang/String;[Ljava/lang/Object;Landroid/view/Surface;"
//		"Landroid/media/MediaCrypto;ILjava/lang/String;)Z",
//		(void *)StartFileEncoder },
//	{ "StopFileEncoder", "()Z", (void *)StopFileEncoder },
//	{ "AddEncoderData", "([BI)Z", (void *)AddEncoderData },

	//sender
	{ "StartCameraEncodec",
		"([Ljava/lang/String;[Ljava/lang/Object;Landroid/view/Surface;"
		"ILjava/lang/String;)Z",
		(void *)StartCameraEncodec },
	{ "StartCameraVideo", "(Landroid/view/Surface;)Z", (void *)StartCameraVideo },
	{ "StopCameraEncodec", "()Z", (void *)StopCameraEncodec },
	{ "GetCameraParameter", "()Ljava/lang/String;", (void *)GetCameraParameter },
	{ "SetCameraParameter", "(Ljava/lang/String;)Z", (void *)SetCameraParameter },
	{ "SetDisplayOrientation", "(I)Z", (void *)SetDisplayOrientation },
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
		GLOGE("can't load ffmpeg");
	}
	
	
	GLOGW( "loading . . .2");
	
	result = JNI_VERSION_1_4;
	char prop[PROPERTY_VALUE_MAX] = "000";
	if(property_get("prop_name", prop, NULL) != 0) {
	}
	
	return result;
}


