#ifndef _UpperNdkEncodec_H_
#define _UpperNdkEncodec_H_

#include <jni.h>


#include "mediaextrator.h"



class UpperNdkEncodec
{
	public:
		UpperNdkEncodec();
		~UpperNdkEncodec();

		int startPlayer(const char*filepath, int w, int h);
		int stopPlayer();
		void setInt32(const char*key, int value);

		void ProvideNV21Data(uint8_t *pBuf, int len);

	private:
		struct symext 		mSymbols;
		AMediaCodec*	 	mCodec;
	    AMediaFormat* 		mFormat;
	    FILE 				*mpFile;

	    int					mUvlen;
	    int					mYlen;
};

#endif
