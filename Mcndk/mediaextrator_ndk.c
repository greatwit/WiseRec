
#include <jni.h>
#include <dlfcn.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif


#include "mediacodec.h"
#include "mediaextrator.h"

#include "ComDefine.h"


struct members
{
    const char *name;
    int offset;
    bool critical;
};

static struct members members[] =
{
	#define OFF(x) offsetof(struct symext, AMediaExtractor.x)
		{ "AMediaExtractor_new", OFF(newext), true },
		{ "AMediaExtractor_delete", OFF(deleteext), true },
		{ "AMediaExtractor_setDataSource", OFF(setDataSource), true },
		{ "AMediaExtractor_getTrackCount", OFF(getTrackCount), true },
		{ "AMediaExtractor_getTrackFormat", OFF(getTrackFormat), true },
		{ "AMediaExtractor_selectTrack", OFF(selectTrack), true },
		{ "AMediaExtractor_unselectTrack", OFF(unselectTrack), true },
		{ "AMediaExtractor_advance", OFF(advance), true },
		{ "AMediaExtractor_readSampleData", OFF(readSampleData), true },
		{ "AMediaExtractor_getSampleTime", OFF(getSampleTime), true },
		{ "AMediaExtractor_seekTo", OFF(seekTo), true },//11
	#undef OFF

	#define OFF(x) offsetof(struct symext, AMediaFormat.x)
		{ "AMediaFormat_new", OFF(newfmt), true },
		{ "AMediaFormat_delete", OFF(deletefmt), true },
		{ "AMediaFormat_toString", OFF(toString), true },
		{ "AMediaFormat_setString", OFF(setString), true },
		{ "AMediaFormat_setInt32", OFF(setInt32), true },
		{ "AMediaFormat_setBuffer", OFF(setBuffer), true },
		{ "AMediaFormat_getInt32", OFF(getInt32), true },
		{ "AMediaFormat_getString", OFF(getString), true },//7
	#undef OFF

	#define OFF(x) offsetof(struct symext, AMediaCodec.x)
		{ "AMediaCodec_createCodecByName", OFF(createCodecByName), true },
		{ "AMediaCodec_createDecoderByType", OFF(createDecoderByType), true },
		{ "AMediaCodec_createEncoderByType", OFF(createEncoderByType), true },
		{ "AMediaCodec_configure", OFF(configure), true },
		{ "AMediaCodec_start", OFF(start), true },
		{ "AMediaCodec_stop", OFF(stop), true },
		{ "AMediaCodec_flush", OFF(flush), true },
		{ "AMediaCodec_delete", OFF(deletemc), true },
		{ "AMediaCodec_getOutputFormat", OFF(getOutputFormat), true },
		{ "AMediaCodec_dequeueInputBuffer", OFF(dequeueInputBuffer), true },
		{ "AMediaCodec_getInputBuffer", OFF(getInputBuffer), true },
		{ "AMediaCodec_queueInputBuffer", OFF(queueInputBuffer), true },
		{ "AMediaCodec_dequeueOutputBuffer", OFF(dequeueOutputBuffer), true },
		{ "AMediaCodec_getOutputBuffer", OFF(getOutputBuffer), true },
		{ "AMediaCodec_releaseOutputBuffer", OFF(releaseOutputBuffer), true },
		{ "AMediaCodec_releaseOutputBufferAtTime", OFF(releaseOutputBufferAtTime), true },
		{ "AMediaCodec_setOutputSurface", OFF(setOutputSurface), false },//17
	#undef OFF

		{ NULL, 0, false }
};
#undef OFF


/* Initialize all symbols.
 * Done only one time during the first initialisation */
bool InitExtratorSymbols(struct symext *symbols)//mc_api *api
{
    //static vlc_mutex_t lock = VLC_STATIC_MUTEX;
    static int i_init_state = -1;
    bool ret;

    //vlc_mutex_lock(&lock);

    if (i_init_state != -1) {
		GLOGE("i_init_state != -1.");
        goto end;
	}

    i_init_state = 0;

    void *ndk_handle = dlopen("libmediandk.so", RTLD_NOW);
    if (!ndk_handle) {
		GLOGE("dlopen failed.");
        goto end;
	}
	int i = 0;
    for (; members[i].name; i++)
    {
        void *sym = dlsym(ndk_handle, members[i].name);
        if (!sym && members[i].critical)
        {
            dlclose(ndk_handle);
			GLOGE("dlextsym total:%d", i);
            goto end;
        }
        *(void **)((uint8_t*)symbols + members[i].offset) = sym;
    }
    symbols->mHandle = ndk_handle;

	GLOGE("jump for i:%d", i);
    i_init_state = 1;
end:
    ret = i_init_state == 1;
    if (!ret)
    	GLOGE("MediaExtractor NDK init failed");

    //vlc_mutex_unlock(&lock);
    return ret;
}

bool ReleaseExtratorSymbols(struct symext *symbols) {
	bool ret = true;
	if(symbols && (symbols->mHandle)) {
        dlclose(symbols->mHandle);
        symbols->mHandle = NULL;
	}
	return ret;
}

/*****************************************************************************
 * DequeueInput
 *****************************************************************************/
//int DequeueInput(AMediaCodec*pCodec, mtime_t i_timeout)
//{
//    ssize_t i_index;
//
//    i_index = symext.AMediaCodec.dequeueInputBuffer(pCodec, i_timeout);
//    if (i_index >= 0)
//        return i_index;
//    else if (i_index == AMEDIACODEC_INFO_TRY_AGAIN_LATER)
//        return MC_API_INFO_TRYAGAIN;
//    else
//    {
//        //msg_Err(api->p_obj, "AMediaCodec.dequeueInputBuffer failed");
//        return MC_API_ERROR;
//    }
//}

