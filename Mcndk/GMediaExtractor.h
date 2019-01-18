#ifndef GMEDIA_EXTRACTOR_H
#define GMEDIA_EXTRACTOR_H


#include "gthread.h"

#include "mediaextrator.h"

class GMediaExtractor : private GThread
{
	public:
		GMediaExtractor();
		virtual ~GMediaExtractor();
		int startPlayer(const char*filepath, void *surface);
		int stopPlayer();

	protected:
		void *Thread();

	private:
		struct symext 		mSymbols;
		AMediaCodec*	 	mCodec;
		AMediaExtractor* 	mExtrator;
	    AMediaFormat* 		mFormat;

	    bool		mbRunning;
};

#endif
