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
		int  ConvertKeyValueToMessage(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *format);

		bool CreateEncodec(const sp<AMessage> &format, ANativeWindow *window, int flags,  const char*readFile, const char*writeFile);
		bool CreateDecodec(const sp<AMessage> &format, ANativeWindow *window, int flags,  const char*readFile);
		bool CloseCodec();

	private:
		bool StartVideo(bool encodec);
		bool StopVideo();

		void AddDecodecSource();
		void AddEncodecSource();
		void onCodecBuffer(struct CodecBuffer& buff);

		static void encodecFunc( void *arg );
		static void decodecFunc( void *arg );

	private:
		CodecContext 	*mCodec;
		GThreadPool 	mPool;

		FILE 			*mrFile;
		FILE 			*mwFile;
};

#endif
