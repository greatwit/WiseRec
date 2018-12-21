#ifndef CODEC_API_H
#define CODEC_API_H




using namespace android;

#ifdef __cplusplus
extern "C"
{
#endif

	status_t ConvertKeyValueToMessage(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *msg);

	int   CodecCreate(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, bool isEncode);
	void  RegisterBufferCall(ICodecCallback *call);
	int   StartCodec();
	int   StopCodec();
	int   AddBuffer(char*data, int len);
	
	void  CodecDestroy();
	
	
	int test(int a, int b);

#ifdef __cplusplus
}
#endif


#endif


