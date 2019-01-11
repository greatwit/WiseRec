#ifndef _CODEC_STUB_H_
#define _CODEC_STUB_H_

#include "IVideoCallback.h"
#include "CodecContext.h"

#include "gthreadpool.hpp"

#include <jni.h>

class CodecStub : public ICodecCallback
{
	public:
		CodecStub();
		virtual ~CodecStub();
		bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, int flags, char*filename);

		bool CloseCodec();

		bool StartVideo(int deivceid);
		bool StopVideo();

		int  ConvertKeyValueToMessage_t(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *msg);

		void AddDecodecSource(char *data, int len);
		void onCodecBuffer(struct CodecBuffer& buff);

	private:
		static void demuxFunc( void *arg );

	private:
		CodecContext *mCodec;
		GThreadPool 	mPool;
};

#endif
