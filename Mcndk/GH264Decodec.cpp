
#include <unistd.h>
#include "ComDefine.h"
#include "mediacodec.h"
#include "GH264Decodec.h"

int charsToInt1(char* src, int offset)
{
	int value;
	value = (int) ((src[offset]	  & 0xFF)
				| ((src[offset+1] & 0xFF)<<8)
				| ((src[offset+2] & 0xFF)<<16)
				| ((src[offset+3] & 0xFF)<<24));
	return value;
}

GH264Decodec::GH264Decodec()
	:mFormat(NULL)
	,mCodec(NULL)
	,mrFile(NULL)
	,mWidth(0)
	,mHeight(0)
{
	//mcharLength[4] = {0};
	memset(mcharLength, 0 ,4);
	InitExtratorSymbols(&mSymbols);
	mFormat = mSymbols.AMediaFormat.newfmt();
}

GH264Decodec::~GH264Decodec()
{
	if(mrFile)
		fclose(mrFile);
}

void* GH264Decodec::Thread() {
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");

	int count = 0;
	int res = 0, dataLen = 0;
	int yuvLen = mWidth*mHeight*3/2;
	char *data = (char*)malloc(yuvLen);
	while(true) {
		res = fread(mcharLength, 4, 1, mrFile);
		if(res>0)
		{
			dataLen = charsToInt1(mcharLength,0);
			res 	= fread(data, dataLen, 1, mrFile);

			GLOGW("startCode res:%d dataLen:%d", res, dataLen);
			if(res>0) {
				media_status_t status;
				GLOGW("AMediaCodec.dequeueInputBuffer begin.");
				int index = mSymbols.AMediaCodec.dequeueInputBuffer(mCodec, 10000);//int64_t timeoutUs
				GLOGW("AMediaCodec.dequeueInputBuffer value:%d", index);
				if(index>=0) {
					uint8_t *p_mc_buf;
					size_t i_mc_size;
					p_mc_buf = mSymbols.AMediaCodec.getInputBuffer(mCodec, index, &i_mc_size);//size_t idx, size_t *out_size
					GLOGW("AMediaCodec.getInputBuffer mcSize:%d", i_mc_size);
					memcpy(p_mc_buf, data, res);
					status = mSymbols.AMediaCodec.queueInputBuffer(mCodec, index, 0, res,
							(++count)*50000, 0);
				} else {
					usleep(20*1000);
					continue;
				}

				AMediaCodecBufferInfo info;
				ssize_t out_index = mSymbols.AMediaCodec.dequeueOutputBuffer(mCodec, &info, 10000);//AMediaCodecBufferInfo *info, int64_t timeoutUs
				GLOGW("AMediaCodec.dequeueOutputBuffer out_index:%d", out_index);
				usleep(25*1000);
				status = mSymbols.AMediaCodec.releaseOutputBuffer(mCodec, out_index, true);
				GLOGW("AMediaCodec.releaseOutputBuffer status:%d", status);
				usleep(25*1000);
			}
			else
			{
				GLOGW("while loop break.");
				break;
			}
		}
	}

	free(data);

	return 0;
}

int GH264Decodec::startPlayer(const char*filepath, void *surface, int w, int h) {
	int rest = 1;

	if (GThread::IsRunning())
		return 0;

	mWidth = w;
	mHeight= h;
	mrFile =  fopen( filepath, "rb" );
	if(mrFile)
		GLOGI("open file:%s success.", filepath);

	mCodec    = mSymbols.AMediaCodec.createDecoderByType("video/avc");
	if(!mCodec)
		GLOGE("AMediaCodec.createDecoderByType for %s successful", "video/avc");

		mSymbols.AMediaFormat.setString(mFormat, "mime", "video/avc");
		GLOGW("format string:%s\n", mSymbols.AMediaFormat.toString(mFormat));

		if (mSymbols.AMediaCodec.configure(mCodec, mFormat, (ANativeWindow*)surface, NULL, 0) != AMEDIA_OK)
		{
			GLOGE("AMediaCodec.configure failed");
		}else
			GLOGW("AMediaCodec.configure successful.");

	    if (mSymbols.AMediaCodec.start(mCodec) != AMEDIA_OK)
	        GLOGE("AMediaCodec.start failed");

		if (GThread::Start() < 0)
			return -1;

	return rest;
}

int GH264Decodec::stopPlayer() {
	int rest = -1;

	if (GThread::IsRunning()) {
		GThread::Kill();
	}

	if(mCodec) {
		mSymbols.AMediaCodec.stop(mCodec);
		mSymbols.AMediaCodec.deletemc(mCodec);
	}

	return rest;
}


void GH264Decodec::setInt32(const char*key, int value) {
	mSymbols.AMediaFormat.setInt32(mFormat, key, value);
}

