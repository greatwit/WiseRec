#include <jni.h>
#include "ComDefine.h"

#include "GMediaExtractor.h"
#include "GH264Extractor.h"
#include "GH264Decodec.h"


//#include <android/native_window.h>
#include <android/native_window_jni.h>

#define REG_PATH "com/great/happyness/medialib/NativeMcndk"

JavaVM*		g_javaVM		= NULL;

GMediaExtractor *mExtractor = NULL;
GH264Extractor  *mH264		= NULL;
GH264Decodec    *mDecodec 	= NULL;

/////////////////////////////////////////////////////extrator player///////////////////////////////////////////////////////////

static jboolean StartExtratorPlayer(JNIEnv *env, jobject, jstring filepath, jobject surface) {
	bool bRes = false;
	GLOGW("StartExtatorPlayer 1");

	ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);

	mExtractor = new GMediaExtractor();

	jboolean isOk = JNI_FALSE;
	const char *path = env->GetStringUTFChars(filepath, &isOk);
	mExtractor->startPlayer(path, pAnw);
	env->ReleaseStringUTFChars(filepath, path);

	GLOGW("StartExtatorPlayer 2");

	return bRes;
}

static jboolean StopExtratorPlayer(JNIEnv *env, jobject)
{
	 bool bRes = false;
	 if(mExtractor){
		 mExtractor->stopPlayer();
		 delete mExtractor;
		 mExtractor = NULL;
		 return true;
	 }
	 return bRes;
}

static jboolean StartH264Extrator(JNIEnv *env, jobject, jstring filepath, jobject surface, jint w, jint h) {
	bool bRes = false;
	ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);
	if(mH264==NULL)
		mH264 = new GH264Extractor();
	jboolean isOk = JNI_FALSE;
	const char *path = env->GetStringUTFChars(filepath, &isOk);
	mH264->startPlayer(path, pAnw, w, h);
	env->ReleaseStringUTFChars(filepath, path);

	GLOGW("StartH264Extrator ");
	return bRes;
}

static jboolean StopH264Extrator(JNIEnv *env, jobject)
{
	 bool bRes = false;
	 if(mH264){
		 mH264->stopPlayer();
		 delete mH264;
		 mH264 = NULL;
		 return true;
	 }
	 return bRes;
}

static void SetExtratorInt32(JNIEnv *env, jobject, jstring key, jint value) {
	jboolean isOk  = JNI_FALSE;
	if(mH264==NULL)
		mH264 = new GH264Extractor();
	const char *ck = env->GetStringUTFChars(key, &isOk);
	mH264->setInt32(ck, value);
	env->ReleaseStringUTFChars(key, ck);
}

static jboolean StartH264Decodec(JNIEnv *env, jobject, jstring filepath, jobject surface, jint w, jint h) {
	bool bRes = false;
	ANativeWindow *pAnw = ANativeWindow_fromSurface(env, surface);

	if(mDecodec==NULL)
		 mDecodec = new GH264Decodec();
	jboolean isOk = JNI_FALSE;
	const char *path = env->GetStringUTFChars(filepath, &isOk);
	mDecodec->startPlayer(path, pAnw, w, h);
	env->ReleaseStringUTFChars(filepath, path);

	GLOGW("StartH264Extrator ");
	return bRes;
}

static jboolean StopH264Decodec(JNIEnv *env, jobject)
{
	 bool bRes = false;
	 if(mDecodec){
		 mDecodec->stopPlayer();
		 delete mDecodec;
		 mDecodec = NULL;
		 return true;
	 }
	 return bRes;
}

static void SetDecodecInt32(JNIEnv *env, jobject, jstring key, jint value) {
	jboolean isOk  = JNI_FALSE;
	if(mDecodec==NULL)
		mDecodec = new GH264Decodec();
	const char *ck = env->GetStringUTFChars(key, &isOk);
	mDecodec->setInt32(ck, value);
	env->ReleaseStringUTFChars(key, ck);
}

static JNINativeMethod video_method_table[] = {
		{"StartExtratorPlayer", "(Ljava/lang/String;Landroid/view/Surface;)Z", (void*)StartExtratorPlayer },
		{"StopExtratorPlayer", "()Z", (void*)StopExtratorPlayer },

		{"StartH264Extrator", "(Ljava/lang/String;Landroid/view/Surface;II)Z", (void*)StartH264Extrator },
		{"StopH264Extrator", "()Z", (void*)StopH264Extrator },
		{"SetExtratorInt32", "(Ljava/lang/String;I)V", (void*)SetExtratorInt32 },

		{"StartH264Decodec", "(Ljava/lang/String;Landroid/view/Surface;II)Z", (void*)StartH264Decodec },
		{"StopH264Decodec", "()Z", (void*)StopH264Decodec },
		{"SetDecodecInt32", "(Ljava/lang/String;I)V", (void*)SetDecodecInt32 },
};

int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* methods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        GLOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        GLOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		GLOGE("GetEnv failed!");
		return result;
	}

	g_javaVM = vm;

	GLOGW("JNI_OnLoad......1");
	registerNativeMethods(env,
			REG_PATH, video_method_table,
			NELEM(video_method_table));
	GLOGW("JNI_OnLoad......2");
	
	return JNI_VERSION_1_4;
}


