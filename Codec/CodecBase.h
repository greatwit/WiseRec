

#ifndef _ANDROID_CODEC_BASE_H_
#define _ANDROID_CODEC_BASE_H_


#include <gui/Surface.h>
#include "android_runtime/AndroidRuntime.h"
#include <utils/threads.h> 

struct CodecBuffer
{
	char*buf;
	int size;
	uint32_t flags;
};

class ICodecCallback
{
	public:
		virtual ~ICodecCallback(){}
		virtual void onCodecBuffer(struct CodecBuffer& buff){(void)buff;}
};

namespace android 
{

struct ABuffer;
struct ALooper;
struct AMessage;
struct AString;
struct ICrypto;
struct IGraphicBufferProducer;
struct PersistentSurface;
struct JMediaCodec;



struct CodecBase : public Thread
{
     public:  //api
    	CodecBase( const char *name, bool nameIsType, bool encoder);

    	status_t CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags);
	
		void  	 registerBufferCall(ICodecCallback *call);
		status_t startCodec();
		status_t stopCodec();

		status_t addBuffer(char*data, int len);
		

	
    protected:
    	virtual ~CodecBase();
		virtual bool threadLoop();
		
		status_t getCodecBuffer();
	

    private:
        sp<JMediaCodec> 	mCodec;
		ICodecCallback		*mpBufferCall;
		struct CodecBuffer	mBufferINfo;
		
		int*		mpInputBufferPoint;
		int*		mpOutputBufferPoint;
		int				miInputArrayLen;
		int				miOutputArrayLen;

	  
		JNIEnv 			*mpEnv;
		int 			mCount;
};

}  // namespace android

#endif  // _ANDROID_MEDIA_MEDIACODEC_H_


