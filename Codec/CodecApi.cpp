
#include "CodecBase.h"

#include "CodecApi.h"

#include "media_Utils.h"


	status_t   ConvertKeyValueToMessage(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *msg)
	{
		status_t err;
		err = ConvertKeyValueArraysToMessage(env, keys, values, msg);
		return err;
	}  

	sp<CodecBase> gCodec = NULL;

	int CodecCreate(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, bool isEncode)
	{
		if(gCodec == NULL)
		{
			gCodec = new CodecBase("video/avc", true, isEncode);
			gCodec->CreateCodec(format, surface, crypto, flags);
			return gCodec!=NULL;
		}
		return 0;
	}

	void  RegisterBufferCall(ICodecCallback *call)
	{
		if(gCodec!=NULL)
			gCodec->registerBufferCall(call);
	}
	
	int   StartCodec()
	{
		if(gCodec!=NULL)
			gCodec->startCodec();

		return 0;
	}
	
	int   StopCodec()
	{
		if(gCodec!=NULL)
		{
			gCodec->stopCodec();
			gCodec = NULL;
		}
		
		return 0;
	}
	
	int   AddBuffer(char*data, int len)
	{
		if(gCodec!=NULL)
			gCodec->addBuffer(data, len);
		
		return 0;
	}
	
	void  CodecDestroy()
	{

	}
	
int test(int a, int  b)
{
    return a + b;
}


