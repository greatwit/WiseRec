#include <jni.h>
#include "ComDefine.h"

#include "GMediaExtractor.h"


//#include <android/native_window.h>
#include <android/native_window_jni.h>

#define REG_PATH "com/great/happyness/medialib/NativeMcndk"

JavaVM*		g_javaVM		= NULL;

GMediaExtractor *mExtractor = NULL;



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

	GLOGW("StartExtatorPlayer 4");

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



static JNINativeMethod video_method_table[] = {
		{"StartExtratorPlayer", "(Ljava/lang/String;Landroid/view/Surface;)Z", (void*)StartExtratorPlayer },
		{"StopExtratorPlayer", "()Z", (void*)StopExtratorPlayer },
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


