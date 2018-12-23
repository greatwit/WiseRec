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



#define MIME_H264		"h264"
#define MIME_G711		"g711"
#define TALK_REQ		"talkreq"
#define TALK_SETUP		"setup"
#define TALK_TEAR	"talktear"
#define AUDIO_REQ		"audioreq"
#define AUDIO_TEAR		"audiotear"
#define VIDEO_REQ		"videoreq"
#define VIDEO_TEAR		"videotear"

const int MIME_H264_LEN		= strlen(MIME_H264)+1;
const int MIME_G711_LEN		= strlen(MIME_G711)+1;
const int TALK_REQ_LEN		= strlen(TALK_REQ)+1;
const int TALK_SETUP_LEN	= strlen(TALK_SETUP)+1;
const int TALK_TEAR_LEN = strlen(TALK_TEAR)+1;
const int AUDIO_REQ_LEN		= strlen(AUDIO_REQ)+1;
const int AUDIO_TEAR_LEN	= strlen(AUDIO_TEAR)+1;
const int VIDEO_REQ_LEN		= strlen(VIDEO_REQ)+1;
const int VIDEO_TEAR_LEN	= strlen(VIDEO_TEAR)+1;


#define MAX_PACKET_SIZE	((64* 1024) - 1)


#ifndef SAFE_DELETE
#define SAFE_DELETE(p) 	do{delete(p); p = NULL;}while(false)
#endif

#define SAFE_DELETE_ELEMENT(ptr) if(ptr != NULL){ delete ptr; ptr = NULL;}

#ifndef SAFE_FREE
#define SAFE_FREE(p) 	do{free(p); p = NULL;}while(false)
#endif


#endif /* COMDEFINE_H_ */
