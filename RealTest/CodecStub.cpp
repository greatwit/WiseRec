
#include "CodecStub.h"
#include "NALDecoder.h"
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

		bool CodecStub::CreateEncodec(const sp<AMessage> &format, ANativeWindow *window, int flags, const char*readFile, const char*writeFile) {

			sp<ICrypto> crypto;
		    sp<Surface> surface = NULL;
		    if (window != NULL) {
		        surface = (Surface*) window;
		    }
			mCodec->CodecCreate(format, surface, crypto, flags, true);	//true is encodec
			mCodec->RegisterBufferCall(this);

			mrFile = fopen(readFile, "rb");
			mwFile = fopen(writeFile, "wb");

			StartVideo(true);

			return true;
		}

		bool CodecStub::CreateDecodec(const sp<AMessage> &format, ANativeWindow *window, int flags,  const char*readFile) {
			sp<ICrypto> crypto;
		    sp<Surface> surface = NULL;
		    if (window != NULL) {
		        surface = (Surface*) window;
		    }
			mCodec->CodecCreate(format, surface, crypto, flags, false);	//true is decodec
			mCodec->RegisterBufferCall(this);

			mrFile =  OpenBitstreamFile( readFile );

			StartVideo(false);

			return true;
		}

		bool CodecStub::CloseCodec() {
			StopVideo();
			return true;
		}

		bool CodecStub::StartVideo(bool encodec) {
			mCodec->StartCodec();
			(encodec==true)?mPool.dispatch( encodecFunc, this ):mPool.dispatch( decodecFunc, this );
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

		void CodecStub::encodecFunc( void *arg ) {
			CodecStub* context = (CodecStub*)arg;
			context->AddEncodecSource();
		}

		void CodecStub::decodecFunc( void *arg ) {
			CodecStub* context = (CodecStub*)arg;
			context->AddDecodecSource();
		}

		void CodecStub::AddEncodecSource() {
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

		void CodecStub::AddDecodecSource() {
			int yuvLen = WIDTH*HEIGHT*3/2;
			NALU_t *data = AllocNALU(yuvLen);
			int count = 0;
			do{
				count++;
				int size=GetAnnexbNALU(mrFile, data);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
				GLOGE("GetAnnexbNALU type:0x%02X size:%d count:%d\n", data->buf[0], size, count);
				if(size<4) {
					GLOGE("get nul error!\n");
					continue;
				}else if(size<=0)break;
					else
						mCodec->AddBuffer((char*)data->buf, size);

				usleep(500*1000);
			}while(!feof(mrFile));

			FreeNALU(data);
		}

		void CodecStub::onCodecBuffer(struct CodecBuffer& buff) {
			int size = buff.size;
			if(mwFile)
				fwrite(buff.buf, size, 1, mwFile);
			GLOGI("onCodecBuffer size:%d", size);
		}



