
#include "CodecStub.h"

#include "ComDefine.h"
#define TAG "CodecStub"

		CodecStub::CodecStub() {
			mCodec = CodecContext::getInstance();

			mPool.initPara(2);
		}

		CodecStub::~CodecStub() {

		}

		bool CodecStub::CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, int flags, char*filename) {
			return true;
		}

		bool CodecStub::CloseCodec() {
			return true;
		}

		bool CodecStub::StartVideo(int deivceid) {
			mPool.dispatch( demuxFunc, this );
			return true;
		}

		bool CodecStub::StopVideo() {
			return true;
		}

		int  CodecStub::ConvertKeyValueToMessage_t(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *msg) {
			return 0;
		}

		void CodecStub::demuxFunc( void *arg ) {
			CodecStub* context = (CodecStub*)arg;
		}

		void CodecStub::AddDecodecSource(char *data, int len) {

		}

		void CodecStub::onCodecBuffer(struct CodecBuffer& buff) {

		}
