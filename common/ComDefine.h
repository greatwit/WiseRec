/*

 */

#ifndef COMDEFINE_H_
#define COMDEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TAG
#define TAG "ComDefine"
#endif

#define  LOGTAG true

#if LOGTAG
	#define GLOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,	TAG,	__VA_ARGS__)
	#define GLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,		TAG,  	__VA_ARGS__)
	#define GLOGI(...)  __android_log_print(ANDROID_LOG_INFO,		TAG,  	__VA_ARGS__)
	#define GLOGW(...)  __android_log_print(ANDROID_LOG_WARN,		TAG,  	__VA_ARGS__)
	#define GLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,		TAG,  	__VA_ARGS__)
#else
	#define GLOGV(...)  
	#define GLOGE(...)
	#define GLOGD(...)  
	#define GLOGI(...)  
	#define GLOGW(...)  
#endif


#define MAX_PACKET_SIZE	((64* 1024) - 1)


#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) 	do{delete(p); p = NULL;}while(false)
#endif

#define SAFE_DELETE_ELEMENT(ptr) if(ptr != NULL){ delete ptr; ptr = NULL;}

#ifndef SAFE_FREE
#define SAFE_FREE(p) 	do{free(p); p = NULL;}while(false)
#endif


#endif /* COMDEFINE_H_ */
