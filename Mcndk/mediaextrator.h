#ifndef MEDIA_EXTRATOR_H
#define MEDIA_EXTRATOR_H

#include <jni.h>
#include <android/native_window.h>
#include "mediacodec.h"

typedef enum {
    AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC,
    AMEDIAEXTRACTOR_SEEK_NEXT_SYNC,
    AMEDIAEXTRACTOR_SEEK_CLOSEST_SYNC
} SeekMode;


struct AMediaCodecBufferInfo {
    int32_t offset;
    int32_t size;
    int64_t presentationTimeUs;
    uint32_t flags;
};
typedef struct AMediaCodecBufferInfo AMediaCodecBufferInfo;

struct AMediaExtractor;
typedef struct AMediaExtractor AMediaExtractor;
struct AMediaFormat;
typedef struct AMediaFormat AMediaFormat;
struct AMediaCodec;
typedef struct AMediaCodec AMediaCodec;
struct AMediaCrypto;
typedef struct AMediaCrypto AMediaCrypto;

/////////////////////AMediaExtractor
typedef AMediaExtractor* (*pf_AMediaExtractor_new)();
typedef media_status_t   (*pf_AMediaExtractor_delete)(AMediaExtractor*);
typedef media_status_t   (*pf_AMediaExtractor_setDataSource)(AMediaExtractor*, const char *location);
typedef size_t   		 (*pf_AMediaExtractor_getTrackCount)(AMediaExtractor*);
typedef AMediaFormat*    (*pf_AMediaExtractor_getTrackFormat)(AMediaExtractor*, size_t idx);
typedef media_status_t   (*pf_AMediaExtractor_selectTrack)(AMediaExtractor*, size_t idx);
typedef media_status_t   (*pf_AMediaExtractor_unselectTrack)(AMediaExtractor*, size_t idx);
typedef bool   			 (*pf_AMediaExtractor_advance)(AMediaExtractor*);
typedef ssize_t   		 (*pf_AMediaExtractor_readSampleData)(AMediaExtractor*, uint8_t *buffer, size_t capacity);
typedef int64_t   		 (*pf_AMediaExtractor_getSampleTime)(AMediaExtractor*);
typedef media_status_t   (*pf_AMediaExtractor_seekTo)(AMediaExtractor*, int64_t seekPosUs, SeekMode mode);


////////////////AMediaFormat
typedef AMediaFormat *(*pf_AMediaFormat_new)();
typedef media_status_t (*pf_AMediaFormat_delete)(AMediaFormat*);
typedef const char* (*pf_AMediaFormat_toString)(AMediaFormat *);
typedef void (*pf_AMediaFormat_setString)(AMediaFormat*, const char* name, const char* value);
typedef void (*pf_AMediaFormat_setInt32)(AMediaFormat*, const char* name, int32_t value);
typedef void (*pf_AMediaFormat_setBuffer)(AMediaFormat*, const char* name, void* data, size_t size);
typedef bool (*pf_AMediaFormat_getInt32)(AMediaFormat*, const char *name, int32_t *out);
typedef bool (*pf_AMediaFormat_getString)(AMediaFormat*, const char *name, const char **out);


////////////////AMediaCodec
typedef AMediaCodec* (*pf_AMediaCodec_createCodecByName)(const char *name);
typedef AMediaCodec* (*pf_AMediaCodec_createDecoderByType)(const char *mime_type);
typedef AMediaCodec* (*pf_AMediaCodec_createEncoderByType)(const char *mime_type);
typedef media_status_t (*pf_AMediaCodec_configure)(AMediaCodec*, const AMediaFormat* format, ANativeWindow* surface,
        										   AMediaCrypto *crypto, uint32_t flags);//flag: encode is 1, decode is 0;
typedef media_status_t (*pf_AMediaCodec_start)(AMediaCodec*);
typedef media_status_t (*pf_AMediaCodec_stop)(AMediaCodec*);
typedef media_status_t (*pf_AMediaCodec_flush)(AMediaCodec*);
typedef media_status_t (*pf_AMediaCodec_delete)(AMediaCodec*);
typedef AMediaFormat* (*pf_AMediaCodec_getOutputFormat)(AMediaCodec*);
typedef ssize_t (*pf_AMediaCodec_dequeueInputBuffer)(AMediaCodec*, int64_t timeoutUs);
typedef uint8_t* (*pf_AMediaCodec_getInputBuffer)(AMediaCodec*, size_t idx, size_t *out_size);
typedef media_status_t (*pf_AMediaCodec_queueInputBuffer)(AMediaCodec*, size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags);
typedef ssize_t (*pf_AMediaCodec_dequeueOutputBuffer)(AMediaCodec*, AMediaCodecBufferInfo *info, int64_t timeoutUs);
typedef uint8_t* (*pf_AMediaCodec_getOutputBuffer)(AMediaCodec*, size_t idx, size_t *out_size);
typedef media_status_t (*pf_AMediaCodec_releaseOutputBuffer)(AMediaCodec*, size_t idx, bool render);
typedef media_status_t (*pf_AMediaCodec_releaseOutputBufferAtTime)(AMediaCodec*, size_t idx, int64_t timestampNs);
typedef media_status_t (*pf_AMediaCodec_setOutputSurface)(AMediaCodec*, ANativeWindow *surface);



struct symext
{
    struct {
    	pf_AMediaExtractor_new newext;
    	pf_AMediaExtractor_delete deleteext;//(const char *mime_type)
    	pf_AMediaExtractor_setDataSource setDataSource;//(const char *name)
    	pf_AMediaExtractor_getTrackCount getTrackCount;
    	pf_AMediaExtractor_getTrackFormat getTrackFormat;
    	pf_AMediaExtractor_selectTrack selectTrack;
    	pf_AMediaExtractor_unselectTrack unselectTrack;
    	pf_AMediaExtractor_advance advance;
    	pf_AMediaExtractor_readSampleData readSampleData;
    	pf_AMediaExtractor_getSampleTime getSampleTime;
    	pf_AMediaExtractor_seekTo seekTo;
    	pf_AMediaFormat_getString getString;
    } AMediaExtractor;

    struct {
        pf_AMediaFormat_new newfmt;
        pf_AMediaFormat_delete deletefmt;
        pf_AMediaFormat_toString toString;
        pf_AMediaFormat_setString setString;
        pf_AMediaFormat_setInt32 setInt32;
        pf_AMediaFormat_setBuffer setBuffer;
        pf_AMediaFormat_getInt32 getInt32;
        pf_AMediaFormat_getString getString;
    } AMediaFormat;

    struct {
        pf_AMediaCodec_createCodecByName createCodecByName;
        pf_AMediaCodec_createDecoderByType createDecoderByType;//(const char *mime_type)
        pf_AMediaCodec_createEncoderByType createEncoderByType;//(const char *name)
        pf_AMediaCodec_configure configure;
        pf_AMediaCodec_start start;
        pf_AMediaCodec_stop stop;
        pf_AMediaCodec_flush flush;
        pf_AMediaCodec_delete deletemc;
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

    void *mHandle;

};


#ifdef __cplusplus
extern "C"
{
#endif

//extern struct symext;
bool InitExtratorSymbols(struct symext *symbols);
bool ReleaseExtratorSymbols(struct symext *symbols);

#ifdef __cplusplus
};
#endif

#endif
