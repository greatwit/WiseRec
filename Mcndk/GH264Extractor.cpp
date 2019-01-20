
#include <unistd.h>
#include "ComDefine.h"
#include "mediacodec.h"
#include "GH264Extractor.h"
#include "NALDecoder.h"

GH264Extractor::GH264Extractor()
	:mFormat(NULL)
	,mCodec(NULL)
	,mrFile(NULL)
	,mWidth(0)
	,mHeight(0)
	,mSurface(NULL)
{
	InitExtratorSymbols(&mSymbols);
	mFormat = mSymbols.AMediaFormat.newfmt();
}

GH264Extractor::~GH264Extractor()
{
	if(mrFile)
		fclose(mrFile);
}

void* GH264Extractor::Thread() {
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");

	int yuvLen = mWidth*mHeight*3/2;
	NALU_t *data = AllocNALU(yuvLen);
	int count = 0;
	do{
		count++;
		int size=GetAnnexbNALU(mrFile, data);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
		GLOGE("GetAnnexbNALU type:0x%02X size:%d count:%d\n", data->buf[4], size, count);
		if(data->buf[4]==0x67) {
			GLOGE("mSymbols.AMediaFormat.setBuffer 1");
			mSymbols.AMediaFormat.setBuffer(mFormat, "csd-0", data->buf, size);
			GLOGE("mSymbols.AMediaFormat.setBuffer 2");
			continue;
		}
		if(data->buf[4]==0x68) {
			mSymbols.AMediaFormat.setBuffer(mFormat, "csd-1", data->buf, size);

			GLOGW("format string:%s\n", mSymbols.AMediaFormat.toString(mFormat));

			if (mSymbols.AMediaCodec.configure(mCodec, mFormat, (ANativeWindow*)mSurface, NULL, 0) != AMEDIA_OK)
			{
				GLOGE("AMediaCodec.configure failed");
			}else
				GLOGW("AMediaCodec.configure successful.");

		    if (mSymbols.AMediaCodec.start(mCodec) != AMEDIA_OK)
		        GLOGE("AMediaCodec.start failed");
		    continue;
		}
		if(size<4) {
			GLOGE("get nul error!\n");
			continue;
		}else if(size<=0)break;
			else
				{
					media_status_t status;
					GLOGW("AMediaCodec.dequeueInputBuffer begin.");
					int index = mSymbols.AMediaCodec.dequeueInputBuffer(mCodec, 10000);//int64_t timeoutUs
					GLOGW("AMediaCodec.dequeueInputBuffer value:%d", index);
					if(index>=0) {
						uint8_t *p_mc_buf;
						size_t i_mc_size;
						p_mc_buf = mSymbols.AMediaCodec.getInputBuffer(mCodec, index, &i_mc_size);//size_t idx, size_t *out_size
						GLOGW("AMediaCodec.getInputBuffer mcSize:%d", i_mc_size);
						memcpy(p_mc_buf, data->buf, size);
						status = mSymbols.AMediaCodec.queueInputBuffer(mCodec, index, 0, size,
								count*50000, 0);
						GLOGW("mSymbols.AMediaCodec.queueInputBuffer status:%d", status);
					} else {
						usleep(20*1000);
						continue;
					}

					usleep(25*1000);
					AMediaCodecBufferInfo info;
					ssize_t out_index = mSymbols.AMediaCodec.dequeueOutputBuffer(mCodec, &info, 10000);//AMediaCodecBufferInfo *info, int64_t timeoutUs
					GLOGW("AMediaCodec.dequeueOutputBuffer out_index:%d", out_index);
					usleep(25*1000);
					status = mSymbols.AMediaCodec.releaseOutputBuffer(mCodec, out_index, true);
					GLOGW("AMediaCodec.releaseOutputBuffer status:%d", status);
				}

	}while(!feof(mrFile));

	FreeNALU(data);

	return 0;
}

int GH264Extractor::startPlayer(const char*filepath, void *surface, int w, int h) {
	int rest = 1;

	if (GThread::IsRunning())
		return 0;

	mWidth = w;
	mHeight= h;
	mrFile =  OpenBitstreamFile( filepath );

	mCodec    = mSymbols.AMediaCodec.createDecoderByType("video/avc");
	if(!mCodec)
		GLOGE("AMediaCodec.createDecoderByType for %s successful", "video/avc");

	mSurface = surface;

		mSymbols.AMediaFormat.setString(mFormat, "mime", "video/avc");
		GLOGW("format string:%s\n", mSymbols.AMediaFormat.toString(mFormat));

//		if (mSymbols.AMediaCodec.configure(mCodec, mFormat, (ANativeWindow*)surface, NULL, 0) != AMEDIA_OK)
//		{
//			GLOGE("AMediaCodec.configure failed");
//		}else
//			GLOGW("AMediaCodec.configure successful.");
//
//	    if (mSymbols.AMediaCodec.start(mCodec) != AMEDIA_OK)
//	        GLOGE("AMediaCodec.start failed");

		if (GThread::Start() < 0)
			return -1;

	return rest;
}

int GH264Extractor::stopPlayer() {
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


void GH264Extractor::setInt32(const char*key, int value) {
	mSymbols.AMediaFormat.setInt32(mFormat, key, value);
}

