#ifndef GH264_DECODEC_H_
#define GH264_DECODEC_H_

#include "gthread.h"

#include "mediaextrator.h"

class GH264Decodec : private GThread
{
	public:
	GH264Decodec();
		virtual ~GH264Decodec();

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
	    char 				mcharLength[4];
	    int 				mWidth,mHeight;
};

#endif
