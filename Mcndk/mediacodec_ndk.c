
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <jni.h>
#include <dlfcn.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <android/native_window.h>
//#include <vlc_common.h>
//
//#include <OMX_Core.h>
//#include <OMX_Component.h>
//#include "omxil_utils.h"
//
#include "mediacodec.h"

#define  TAG "mediacodec_ndk"
#include "ComDefine.h"


#define THREAD_NAME "mediacodec_ndk"

/* Not in NdkMedia API but we need it since we send config data via input
 * buffers and not via "csd-*" buffers from AMediaFormat */
#define AMEDIACODEC_FLAG_CODEC_CONFIG 2



/*****************************************************************************
 * NdkMediaCodec.h
 *****************************************************************************/

struct AMediaCodec;
typedef struct AMediaCodec AMediaCodec;

struct AMediaCodecBufferInfo {
    int32_t offset;
    int32_t size;
    int64_t presentationTimeUs;
    uint32_t flags;
};
typedef struct AMediaCodecBufferInfo AMediaCodecBufferInfo;

enum {
    AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM = 4,
    AMEDIACODEC_CONFIGURE_FLAG_ENCODE = 1,
    AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED = -3,
    AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED = -2,
    AMEDIACODEC_INFO_TRY_AGAIN_LATER = -1
};

struct AMediaFormat;
typedef struct AMediaFormat AMediaFormat;

struct AMediaCrypto;
typedef struct AMediaCrypto AMediaCrypto;

/*****************************************************************************
 * Ndk symbols
 *****************************************************************************/

typedef AMediaCodec* (*pf_AMediaCodec_createCodecByName)(const char *name);

typedef AMediaCodec* (*pf_AMediaCodec_createDecoderByType)(const char *mime_type);
typedef AMediaCodec* (*pf_AMediaCodec_createEncoderByType)(const char *mime_type);

typedef media_status_t (*pf_AMediaCodec_configure)(AMediaCodec*,
        const AMediaFormat* format,
        ANativeWindow* surface,
        AMediaCrypto *crypto,
        uint32_t flags);

typedef media_status_t (*pf_AMediaCodec_start)(AMediaCodec*);

typedef media_status_t (*pf_AMediaCodec_stop)(AMediaCodec*);

typedef media_status_t (*pf_AMediaCodec_flush)(AMediaCodec*);

typedef media_status_t (*pf_AMediaCodec_delete)(AMediaCodec*);

typedef AMediaFormat* (*pf_AMediaCodec_getOutputFormat)(AMediaCodec*);

typedef ssize_t (*pf_AMediaCodec_dequeueInputBuffer)(AMediaCodec*,
        int64_t timeoutUs);

typedef uint8_t* (*pf_AMediaCodec_getInputBuffer)(AMediaCodec*,
        size_t idx, size_t *out_size);

typedef media_status_t (*pf_AMediaCodec_queueInputBuffer)(AMediaCodec*,
        size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags);

typedef ssize_t (*pf_AMediaCodec_dequeueOutputBuffer)(AMediaCodec*,
        AMediaCodecBufferInfo *info, int64_t timeoutUs);

typedef uint8_t* (*pf_AMediaCodec_getOutputBuffer)(AMediaCodec*,
        size_t idx, size_t *out_size);

typedef media_status_t (*pf_AMediaCodec_releaseOutputBuffer)(AMediaCodec*,
        size_t idx, bool render);

typedef media_status_t (*pf_AMediaCodec_releaseOutputBufferAtTime)(AMediaCodec*,
        size_t idx, int64_t timestampNs);

typedef media_status_t (*pf_AMediaCodec_setOutputSurface)(AMediaCodec*,
        ANativeWindow *surface);



typedef AMediaFormat *(*pf_AMediaFormat_new)();
typedef media_status_t (*pf_AMediaFormat_delete)(AMediaFormat*);

typedef void (*pf_AMediaFormat_setString)(AMediaFormat*,
        const char* name, const char* value);

typedef const char* (*pf_AMediaFormat_toString)(AMediaFormat*);

typedef void (*pf_AMediaFormat_setInt32)(AMediaFormat*,
        const char* name, int32_t value);

typedef bool (*pf_AMediaFormat_getInt32)(AMediaFormat*,
        const char *name, int32_t *out);

struct syms
{
    struct {
        pf_AMediaCodec_createCodecByName createCodecByName;
        pf_AMediaCodec_createDecoderByType createDecoderByType;//(const char *mime_type)
        pf_AMediaCodec_createEncoderByType createEncoderByType;//(const char *name)
        pf_AMediaCodec_configure configure;
        pf_AMediaCodec_start start;
        pf_AMediaCodec_stop stop;
        pf_AMediaCodec_flush flush;
        pf_AMediaCodec_delete delete;
        pf_AMediaCodec_getOutputFormat getOutputFormat;
        pf_AMediaCodec_dequeueInputBuffer dequeueInputBuffer;
        pf_AMediaCodec_getInputBuffer getInputBuffer;
        pf_AMediaCodec_queueInputBuffer queueInputBuffer;
        pf_AMediaCodec_dequeueOutputBuffer dequeueOutputBuffer;
        pf_AMediaCodec_getOutputBuffer getOutputBuffer;
        pf_AMediaCodec_releaseOutputBuffer releaseOutputBuffer;
        pf_AMediaCodec_releaseOutputBufferAtTime releaseOutputBufferAtTime;
        pf_AMediaCodec_setOutputSurface setOutputSurface;
    } AMediaCodec;
    struct {
        pf_AMediaFormat_new new;
        pf_AMediaFormat_delete delete;
        pf_AMediaFormat_setString setString;
        //pf_AMediaFormat_toString toString;
        pf_AMediaFormat_setInt32 setInt32;
        pf_AMediaFormat_getInt32 getInt32;
    } AMediaFormat;
};
static struct syms syms;

struct members
{
    const char *name;
    int offset;
    bool critical;
};
static struct members members[] =
{
#define OFF(x) offsetof(struct syms, AMediaCodec.x)
    { "AMediaCodec_createCodecByName", OFF(createCodecByName), true },
	{ "AMediaCodec_createDecoderByType", OFF(createDecoderByType), true },
	{ "AMediaCodec_createEncoderByType", OFF(createEncoderByType), true },
    { "AMediaCodec_configure", OFF(configure), true },
    { "AMediaCodec_start", OFF(start), true },
    { "AMediaCodec_stop", OFF(stop), true },
    { "AMediaCodec_flush", OFF(flush), true },
    { "AMediaCodec_delete", OFF(delete), true },
    { "AMediaCodec_getOutputFormat", OFF(getOutputFormat), true },
    { "AMediaCodec_dequeueInputBuffer", OFF(dequeueInputBuffer), true },
    { "AMediaCodec_getInputBuffer", OFF(getInputBuffer), true },
    { "AMediaCodec_queueInputBuffer", OFF(queueInputBuffer), true },
    { "AMediaCodec_dequeueOutputBuffer", OFF(dequeueOutputBuffer), true },
    { "AMediaCodec_getOutputBuffer", OFF(getOutputBuffer), true },
    { "AMediaCodec_releaseOutputBuffer", OFF(releaseOutputBuffer), true },
    { "AMediaCodec_releaseOutputBufferAtTime", OFF(releaseOutputBufferAtTime), true },
    { "AMediaCodec_setOutputSurface", OFF(setOutputSurface), false },
#undef OFF

#define OFF(x) offsetof(struct syms, AMediaFormat.x)
    { "AMediaFormat_new", OFF(new), true },
    { "AMediaFormat_delete", OFF(delete), true },
    { "AMediaFormat_setString", OFF(setString), true },
	//{ "AMediaFormat_toString", OFF(toString), true },
    { "AMediaFormat_setInt32", OFF(setInt32), true },
    { "AMediaFormat_getInt32", OFF(getInt32), true },
#undef OFF
    { NULL, 0, false }
};
#undef OFF


static void *mNdkHandle = NULL;

/* Initialize all symbols.
 * Done only one time during the first initialisation */
static bool
InitSymbols(mc_api *api)
{
    //static vlc_mutex_t lock = VLC_STATIC_MUTEX;
    int i_init_state = 0;
    bool ret;

    //vlc_mutex_lock(&lock);

    if(mNdkHandle==NULL) {
        void *mNdkHandle = dlopen("libmediandk.so", RTLD_NOW);
        if (!mNdkHandle) {
    		GLOGE("dlopen failed.");
            goto end;
    	}
    	int i = 0;
        for (; members[i].name; i++)
        {
            void *sym = dlsym(mNdkHandle, members[i].name);
            if (!sym && members[i].critical)
            {
                dlclose(mNdkHandle);
    			GLOGE("dlsym total:%d", i);
                goto end;
            }
            *(void **)((uint8_t*)&syms + members[i].offset) = sym;
        }

    	GLOGE("jump for i:%d", i);

        i_init_state = 1;
    }

	end:
		ret = i_init_state == 1;

    //vlc_mutex_unlock(&lock);

    return ret;
}

/****************************************************************************
 * Local prototypes
 ****************************************************************************/

struct mc_api_sys
{
    AMediaCodec* p_codec;
    AMediaFormat* p_format;
    AMediaCodecBufferInfo info;
};


/*****************************************************************************
 * Stop
 *****************************************************************************/
static int Stop(mc_api *api)
{
    mc_api_sys *p_sys = api->p_sys;

    api->b_direct_rendering = false;

    if (p_sys->p_codec)
    {
        if (api->b_started)
        {
            syms.AMediaCodec.stop(p_sys->p_codec);
            api->b_started = false;
        }
        syms.AMediaCodec.delete(p_sys->p_codec);
        p_sys->p_codec = NULL;
    }
    if (p_sys->p_format)
    {
        syms.AMediaFormat.delete(p_sys->p_format);
        p_sys->p_format = NULL;
    }

    //msg_Dbg(api->p_obj, "MediaCodec via NDK closed");
    return 0;
}


/*****************************************************************************
 * Start
 *****************************************************************************/
static int Start(mc_api *api, union mc_api_args *p_args)
{
    mc_api_sys *p_sys = api->p_sys;
    int i_ret = MC_API_ERROR;
    ANativeWindow *p_anw = NULL;

    //assert(api->psz_mime && api->psz_name);
	
	GLOGE("createDecoderByType 1");
    p_sys->p_codec = syms.AMediaCodec.createDecoderByType("video/avc");//syms.AMediaCodec.createCodecByName("video_decoder.avc");//api->psz_name OMX.google.h264.decoder
    if (!p_sys->p_codec)
    {
		GLOGE("AMediaCodec.createDecoderByType for %s failed", "video/avc");
        goto error;
    }
	GLOGE("createDecoderByType 2");
	
    p_sys->p_format = syms.AMediaFormat.new();
    if (!p_sys->p_format)
    {
        GLOGE("AMediaFormat.new failed");
        goto error;
    }
    GLOGW("Configure begin.");
    //syms.AMediaFormat.setInt32(p_sys->p_format, "decoder", 1);
    syms.AMediaFormat.setString(p_sys->p_format, "mime", "video/avc");//api->psz_mime

    /* No limits for input size */
    //syms.AMediaFormat.setInt32(p_sys->p_format, "max-input-size", 0);
    if (api->i_cat == VIDEO_ES)
    {
    	GLOGW("VIDEO_ES------ width:%d height:%d.", p_args->video.i_width, p_args->video.i_height);
        syms.AMediaFormat.setInt32(p_sys->p_format, "width", p_args->video.i_width);
        syms.AMediaFormat.setInt32(p_sys->p_format, "height", p_args->video.i_height);
        syms.AMediaFormat.setInt32(p_sys->p_format, "frame-rate", 30);
        //syms.AMediaFormat.setInt32(p_sys->p_format, "rotation-degrees", p_args->video.i_angle);
        if (p_args->video.p_surface)
        {
            p_anw = p_args->video.p_surface;
//            if (p_args->video.b_tunneled_playback)
//                syms.AMediaFormat.setInt32(p_sys->p_format,
//                                           "feature-tunneled-playback", 1);
//            if (p_args->video.b_adaptive_playback)
//                syms.AMediaFormat.setInt32(p_sys->p_format,
//                                           "feature-adaptive-playback", 1);
            GLOGW("p_args->video.p_surface.");
        }
    }
    else
    {
        syms.AMediaFormat.setInt32(p_sys->p_format, "sample-rate", p_args->audio.i_sample_rate);
        syms.AMediaFormat.setInt32(p_sys->p_format, "channel-count", p_args->audio.i_channel_count);
    }

    if (syms.AMediaCodec.configure(p_sys->p_codec, p_sys->p_format,
                                   p_anw, NULL, 0) != AMEDIA_OK)
    {
        GLOGE("AMediaCodec.configure failed");
        goto error;
    }
    GLOGW("syms.AMediaCodec.configure.");
    if (syms.AMediaCodec.start(p_sys->p_codec) != AMEDIA_OK)
    {
        GLOGE("AMediaCodec.start failed");
        goto error;
    }
    GLOGW("syms.AMediaCodec.start.");
    api->b_started = true;
    api->b_direct_rendering = !!p_anw;
    i_ret = 0;

    GLOGW("MediaCodec via NDK opened");

    uint8_t *p_mc_buf;
    size_t i_mc_size;
    p_mc_buf = syms.AMediaCodec.getInputBuffer(p_sys->p_codec,
                                               0, &i_mc_size);
    GLOGW("getInputBuffer size:%d", i_mc_size);

    //p_mc_buf = syms.AMediaCodec.getOutputBuffer(p_sys->p_codec, 0, &i_mc_size);
    //GLOGW("getOutputBuffer size:%d", i_mc_size);
error:
    if (i_ret != 0)
        Stop(api);
    return i_ret;
}

/*****************************************************************************
 * Flush
 *****************************************************************************/
static int Flush(mc_api *api)
{
    mc_api_sys *p_sys = api->p_sys;

    if (syms.AMediaCodec.flush(p_sys->p_codec) == AMEDIA_OK)
        return 0;
    else
        return MC_API_ERROR;
}

/*****************************************************************************
 * DequeueInput
 *****************************************************************************/
static int DequeueInput(mc_api *api, mtime_t i_timeout)
{
    mc_api_sys *p_sys = api->p_sys;
    ssize_t i_index;

    i_index = syms.AMediaCodec.dequeueInputBuffer(p_sys->p_codec, i_timeout);
    if (i_index >= 0)
        return i_index;
    else if (i_index == AMEDIACODEC_INFO_TRY_AGAIN_LATER)
        return MC_API_INFO_TRYAGAIN;
    else
    {
        //msg_Err(api->p_obj, "AMediaCodec.dequeueInputBuffer failed");
        return MC_API_ERROR;
    }
}

/*****************************************************************************
 * QueueInput
 *****************************************************************************/
static int QueueInput(mc_api *api, int i_index, const void *p_buf,
                      size_t i_size, mtime_t i_ts, bool b_config)
{
    mc_api_sys *p_sys = api->p_sys;
    uint8_t *p_mc_buf;
    size_t i_mc_size;
    int i_flags = (b_config ? AMEDIACODEC_FLAG_CODEC_CONFIG : 0)
                | (p_buf == NULL ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM : 0);

    assert(i_index >= 0);

    p_mc_buf = syms.AMediaCodec.getInputBuffer(p_sys->p_codec,
                                               i_index, &i_mc_size);
    GLOGE("getInputBuffer memsize:%d datasize:%d", i_mc_size, i_size);
    if (!p_mc_buf)
        return MC_API_ERROR;

    if (i_mc_size > i_size)
        i_mc_size = i_size;
    memcpy(p_mc_buf, p_buf, i_mc_size);

    if (syms.AMediaCodec.queueInputBuffer(p_sys->p_codec, i_index, 0, i_mc_size,
                                          i_ts, i_flags) == AMEDIA_OK)
        return 0;
    else
    {
        //msg_Err(api->p_obj, "AMediaCodec.queueInputBuffer failed");
        return MC_API_ERROR;
    }
}

static int32_t GetFormatInteger(AMediaFormat *p_format, const char *psz_name)
{
    int32_t i_out = 0;
    syms.AMediaFormat.getInt32(p_format, psz_name, &i_out);
    return i_out;
}

/*****************************************************************************
 * DequeueOutput
 *****************************************************************************/
static int DequeueOutput(mc_api *api, mtime_t i_timeout)
{
    mc_api_sys *p_sys = api->p_sys;
    ssize_t i_index;

    i_index = syms.AMediaCodec.dequeueOutputBuffer(p_sys->p_codec, &p_sys->info, i_timeout);
    GLOGE("dequeueOutputBuffer size:%d time:%lld", p_sys->info.size, p_sys->info.presentationTimeUs);
    if (i_index >= 0)
        return i_index;
    else if (i_index == AMEDIACODEC_INFO_TRY_AGAIN_LATER)
        return MC_API_INFO_TRYAGAIN;
    else if (i_index == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED)
        return MC_API_INFO_OUTPUT_BUFFERS_CHANGED;
    else if (i_index == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
    	//AMediaFormat* format = syms.AMediaCodec.getOutputFormat(p_sys->p_codec);
        return MC_API_INFO_OUTPUT_FORMAT_CHANGED;
    }
    else
    {
        //msg_Warn(api->p_obj, "AMediaCodec.dequeueOutputBuffer failed");
        return MC_API_ERROR;
    }
}

/*****************************************************************************
 * GetOutput
 *****************************************************************************/
static int GetOutput(mc_api *api, int i_index, mc_api_out *p_out)
{
    mc_api_sys *p_sys = api->p_sys;

    if (i_index >= 0)
    {
        p_out->type = MC_OUT_TYPE_BUF;
        p_out->buf.i_index = i_index;

        p_out->buf.i_ts = p_sys->info.presentationTimeUs;
        p_out->b_eos = p_sys->info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM;

        if (api->b_direct_rendering)
        {
            p_out->buf.p_ptr = NULL;
            p_out->buf.i_size = 0;
        }
        else
        {
            size_t i_mc_size;
            uint8_t *p_mc_buf = syms.AMediaCodec.getOutputBuffer(p_sys->p_codec,
                                                                 i_index,
                                                                 &i_mc_size);
            /* p_mc_buf can be NULL in case of EOS */
            if (!p_mc_buf && !p_out->b_eos)
            {
                //msg_Err(api->p_obj, "AMediaCodec.getOutputBuffer failed");
                return MC_API_ERROR;
            }
            p_out->buf.p_ptr = p_mc_buf + p_sys->info.offset;
            p_out->buf.i_size = p_sys->info.size;
        }
        return 1;
    }
    else if (i_index == MC_API_INFO_OUTPUT_FORMAT_CHANGED)
    {
        AMediaFormat *format = syms.AMediaCodec.getOutputFormat(p_sys->p_codec);

        p_out->type = MC_OUT_TYPE_CONF;
        p_out->b_eos = false;
        if (api->i_cat == VIDEO_ES)
        {
            p_out->conf.video.width         = GetFormatInteger(format, "width");
            p_out->conf.video.height        = GetFormatInteger(format, "height");
            p_out->conf.video.stride        = GetFormatInteger(format, "stride");
            p_out->conf.video.slice_height  = GetFormatInteger(format, "slice-height");
            p_out->conf.video.pixel_format  = GetFormatInteger(format, "color-format");
            p_out->conf.video.crop_left     = GetFormatInteger(format, "crop-left");
            p_out->conf.video.crop_top      = GetFormatInteger(format, "crop-top");
            p_out->conf.video.crop_right    = GetFormatInteger(format, "crop-right");
            p_out->conf.video.crop_bottom   = GetFormatInteger(format, "crop-bottom");
            GLOGE("OutputFormat change to w:%d h:%d format:%d", p_out->conf.video.width, p_out->conf.video.height, p_out->conf.video.pixel_format);
        }
        else
        {
            p_out->conf.audio.channel_count = GetFormatInteger(format, "channel-count");
            p_out->conf.audio.channel_mask  = GetFormatInteger(format, "channel-mask");
            p_out->conf.audio.sample_rate   = GetFormatInteger(format, "sample-rate");
        }
        return 1;
    }
    return 0;
}

/*****************************************************************************
 * ReleaseOutput
 *****************************************************************************/
static int ReleaseOutput(mc_api *api, int i_index, bool b_render)
{
    mc_api_sys *p_sys = api->p_sys;

    assert(i_index >= 0);
    if (syms.AMediaCodec.releaseOutputBuffer(p_sys->p_codec, i_index, b_render)
                                             == AMEDIA_OK)
        return 0;
    else
        return MC_API_ERROR;
}

/*****************************************************************************
 * ReleaseOutputAtTime
 *****************************************************************************/
static int ReleaseOutputAtTime(mc_api *api, int i_index, int64_t i_ts_ns)
{
    mc_api_sys *p_sys = api->p_sys;

    assert(i_index >= 0);
    if (syms.AMediaCodec.releaseOutputBufferAtTime(p_sys->p_codec, i_index, i_ts_ns)
                                                   == AMEDIA_OK)
        return 0;
    else
        return MC_API_ERROR;
}

/*****************************************************************************
 * SetOutputSurface
 *****************************************************************************/
static int SetOutputSurface(mc_api *api, void *p_surface, void *p_jsurface)
{
    (void) p_jsurface;
    assert(p_surface != NULL);
    mc_api_sys *p_sys = api->p_sys;

    return syms.AMediaCodec.setOutputSurface != NULL
        && syms.AMediaCodec.setOutputSurface(p_sys->p_codec, p_surface)
        == AMEDIA_OK ? 0 : MC_API_ERROR;
}

/*****************************************************************************
 * Clean
 *****************************************************************************/
static void Clean(mc_api *api)
{
    free(api->psz_name);
    free(api->p_sys);
}

/*****************************************************************************
 * Configure
 *****************************************************************************/
static int Configure(mc_api * api, int i_profile)
{
    //free(api->psz_name);
    bool b_adaptive;
    //api->psz_name = MediaCodec_GetName(api->p_obj, api->psz_mime, i_profile, &b_adaptive);
    //api->psz_name = "video/avc";
    //api->psz_mime = "video/avc";
    api->i_cat = VIDEO_ES;
    //if (!api->psz_name)
     //   return MC_API_ERROR;
    //api->i_quirks = OMXCodec_GetQuirks(api->i_cat, api->i_codec, api->psz_name, strlen(api->psz_name));

    /* Allow interlaced picture after API 21 */
    api->i_quirks |= MC_API_VIDEO_QUIRKS_SUPPORT_INTERLACED;
    if (b_adaptive)
        api->i_quirks |= MC_API_VIDEO_QUIRKS_ADAPTIVE;
    return 0;
}

/*****************************************************************************
 * MediaCodecNdk_Init
 *****************************************************************************/
int MediaCodecNdk_Init(mc_api *api)
{
    if (!InitSymbols(api))
        return MC_API_ERROR;

    api->p_sys = calloc(1, sizeof(mc_api_sys));
    if (!api->p_sys)
        return MC_API_ERROR;

    api->clean 				= Clean;
    api->configure 			= Configure;
    api->start 				= Start;
    api->stop 				= Stop;
    api->flush 				= Flush;
    api->dequeue_in 		= DequeueInput;
    api->queue_in 			= QueueInput;
    api->dequeue_out 		= DequeueOutput;
    api->get_out 			= GetOutput;
    api->release_out 		= ReleaseOutput;
    api->release_out_ts 	= ReleaseOutputAtTime;
    api->set_output_surface = SetOutputSurface;

    api->b_support_rotation = true;
    return 0;
}

