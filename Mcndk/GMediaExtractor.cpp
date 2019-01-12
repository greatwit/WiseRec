



#include "ComDefine.h"
#include "mediacodec.h"
#include "GMediaExtractor.h"
#include <unistd.h>


GMediaExtractor::GMediaExtractor()
	:mExtrator(NULL)
	,mFormat(NULL)
	,mCodec(NULL)
	,mbRunning(false)
{
	InitExtratorSymbols(&mSymbols);
}

GMediaExtractor::~GMediaExtractor()
{

}

void* GMediaExtractor::Thread() {
	GThread::ThreadStarted();
	GLOGW("FileDeCodec::Thread");

	FILE *mFile = fopen("/sdcard/extra.h264", "wb");

	uint8_t buffer[100000] = {0};
	ssize_t size = 0;
	while(mbRunning&&size>=0) {
			media_status_t status;
			GLOGW("AMediaCodec.dequeueInputBuffer begin.");
			int index = mSymbols.AMediaCodec.dequeueInputBuffer(mCodec, 10000);//int64_t timeoutUs
			GLOGW("AMediaCodec.dequeueInputBuffer value:%d", index);
			if(index>=0) {
				uint8_t *p_mc_buf;
				size_t i_mc_size;
				p_mc_buf = mSymbols.AMediaCodec.getInputBuffer(mCodec, index, &i_mc_size);//size_t idx, size_t *out_size
				GLOGW("AMediaCodec.getInputBuffer mcSize:%d", i_mc_size);
				size = mSymbols.AMediaExtractor.readSampleData(mExtrator, p_mc_buf, i_mc_size);

				if(size<0) {
					status = mSymbols.AMediaCodec.queueInputBuffer(mCodec, index, 0, 0, 0, 4);//flag=4 is end; size_t idx, off_t offset, size_t size, uint64_t time, uint32_t flags
				}else
				{
					status = mSymbols.AMediaCodec.queueInputBuffer(mCodec, index, 0, size,
				    		 mSymbols.AMediaExtractor.getSampleTime(mExtrator), 0);
					fwrite(p_mc_buf, size, 1, mFile);
				}
				GLOGW("readSampleData size:%d timestamp:%lld status:%d", size, mSymbols.AMediaExtractor.getSampleTime(mExtrator), status);
				mSymbols.AMediaExtractor.advance(mExtrator);
			} else {
				//index = mSymbols.AMediaCodec.dequeueInputBuffer(mCodec, 10000);
				//			GLOGW("AMediaCodec.dequeueInputBuffer value99:%d", index);
				usleep(20*1000);
				continue;
			}

			AMediaCodecBufferInfo info;
		    ssize_t out_index = mSymbols.AMediaCodec.dequeueOutputBuffer(mCodec, &info, 10000);//AMediaCodecBufferInfo *info, int64_t timeoutUs
		    GLOGW("AMediaCodec.dequeueOutputBuffer out_index:%d", out_index);
		    usleep(10*1000);
		    status = mSymbols.AMediaCodec.releaseOutputBuffer(mCodec, out_index, true);
		    GLOGW("AMediaCodec.releaseOutputBuffer status:%d", status);
		    usleep(10*1000);
	}

	fclose(mFile);

	return 0;
}

int GMediaExtractor::startPlayer(const char*filepath, void *surface) {
	int rest = 1;

	if (GThread::IsRunning())
		return 0;

	mCodec    = mSymbols.AMediaCodec.createDecoderByType("video/avc");
	if(!mCodec)
		GLOGE("AMediaCodec.createDecoderByType for %s failed", "video/avc");

	mExtrator = mSymbols.AMediaExtractor.newext();
	if(mExtrator) {
		media_status_t stat = mSymbols.AMediaExtractor.setDataSource(mExtrator, filepath);
		size_t count = mSymbols.AMediaExtractor.getTrackCount(mExtrator);
		GLOGW("get track count:%d err:%d", count, stat);

		for(int i=0;i<count;i++) {
			int32_t iout = 0;
			if(i==0) {
				AMediaFormat*format = mSymbols.AMediaExtractor.getTrackFormat(mExtrator, i);
//				const char *csd = new char[100];
//				memset((void*)csd, 0, 100);
//				mSymbols.AMediaFormat.getString(format, "csd-0", &csd);
//				GLOGW("AMediaFormat.get csd-0:%s, len:%d", csd, strlen(csd));
//				delete[] csd;

				GLOGW("format string:%s\n", mSymbols.AMediaFormat.toString(format));
			    if (mSymbols.AMediaCodec.configure(mCodec, format, (ANativeWindow*)surface, NULL, 0) != AMEDIA_OK)
			    {
			        GLOGE("AMediaCodec.configure failed");
			    }else
			    	GLOGW("AMediaCodec.configure successful.");

			}
		}

		stat = mSymbols.AMediaExtractor.selectTrack(mExtrator, 0);
		GLOGW("selectTrack err:%d", stat);
		stat = mSymbols.AMediaExtractor.seekTo(mExtrator, 0, AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC);
		GLOGW("seekTo err:%d", stat);


	    if (mSymbols.AMediaCodec.start(mCodec) != AMEDIA_OK)
	        GLOGE("AMediaCodec.start failed");


		if (GThread::Start() < 0)
			return -1;

		mbRunning = true;
	}
	return rest;
}

int GMediaExtractor::stopPlayer() {
	int rest = -1;

	mbRunning = false;

	if (GThread::IsRunning()) {
		GThread::Kill();
	}

	if(mExtrator) {
		mSymbols.AMediaExtractor.deleteext(mExtrator);
	}

	if(mCodec) {
		mSymbols.AMediaCodec.stop(mCodec);
		mSymbols.AMediaCodec.deletemc(mCodec);
	}

	return rest;
}


