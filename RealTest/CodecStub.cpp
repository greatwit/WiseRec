
#include "CodecStub.h"

#include "ComDefine.h"
#define TAG "CodecStub"

#define WIDTH 1280
#define HEIGHT 720

		CodecStub::CodecStub()
				:mrFile(NULL)
				,mwFile(NULL)
		{
			mCodec = CodecContext::getInstance();

			mPool.initPara(2);
		}

		CodecStub::~CodecStub() {
			if(mrFile)
				fclose(mrFile);
			if(mwFile)
				fclose(mwFile);
		}

		bool CodecStub::CreateCodec(const sp<AMessage> &format, ANativeWindow *window, int flags, const char*readFile, const char*writeFile) {

			sp<ICrypto> crypto;
		    sp<Surface> surface = NULL;
		    if (window != NULL) {
		        surface = (Surface*) window;
		    }
			mCodec->CodecCreate(format, surface, crypto, flags, true);	//true is encodec
			mCodec->RegisterBufferCall(this);

			mrFile = fopen(readFile, "rb");
			mwFile = fopen(writeFile, "wb");

			StartVideo();

			return true;
		}

		bool CodecStub::CloseCodec() {
			StopVideo();
			return true;
		}

		bool CodecStub::StartVideo() {
			mCodec->StartCodec();
			mPool.dispatch( demuxFunc, this );
			return true;
		}

		bool CodecStub::StopVideo() {
			mCodec->StopCodec();
			return true;
		}

		int  CodecStub::ConvertKeyValueToMessage(JNIEnv *env, jobjectArray keys, jobjectArray values, sp<AMessage> *format) {
			mCodec->ConvertKeyValueToMessage(env, keys, values, format);
			return 0;
		}

		void CodecStub::demuxFunc( void *arg ) {
			CodecStub* context = (CodecStub*)arg;
			context->AddDecodecSource();
		}

		void CodecStub::AddDecodecSource() {
			int yuvLen = WIDTH*HEIGHT*3/2;
			char *data = new char[yuvLen];
			while(true) {
				int rest = fread(data, 1, yuvLen, mrFile);
				if(rest>0) {
					mCodec->AddBuffer(data, rest);
				}else
					break;
				usleep(50*1000);
			}
			delete[]data;
		}

		void CodecStub::AddDecodecSource(char *data, int len) {
		}

		void CodecStub::onCodecBuffer(struct CodecBuffer& buff) {
			int size = buff.size;
			fwrite(buff.buf, size, 1, mwFile);
		}



