#ifndef GH264_EXTRACTOR_H_
#define GH264_EXTRACTOR_H_

#include "gthread.h"

#include "mediaextrator.h"

class GH264Extractor : private GThread
{
	public:
		GH264Extractor();
		virtual ~GH264Extractor();

		int startPlayer(const char*filepath, void *surface, int w, int h);
		int stopPlayer();
		void setInt32(const char*key, int value);

	protected:
		void *Thread();

	private:
		struct symext 		mSymbols;
		AMediaCodec*	 	mCodec;
	    AMediaFormat* 		mFormat;
	    FILE 				*mrFile;
	    int mWidth,mHeight;
	    void *				mSurface;
};

#endif
