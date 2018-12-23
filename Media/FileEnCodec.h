#ifndef FILE_EN_CODEC_H_
#define FILE_EN_CODEC_H_

#include <utils/threads.h> 
#include <utils/Log.h>

#include <string>
#include <map>


#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include <media/ICrypto.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/MediaErrors.h>

#include <gui/Surface.h> 

#include "IVideoCallback.h"
#include "CodecContext.h"

using namespace android;

class FileEnCodec : public ICodecCallback
{
	public:
		FileEnCodec();
		virtual ~FileEnCodec();
		
		bool CreateCodec(const sp<AMessage> &format, const sp<Surface> &surface, const sp<ICrypto> &crypto, int flags, char*filename);
		
		bool DeInit();
		
		bool StartVideo(int deivceid);
		bool StopVideo();

		void AddDecodecSource(char *data, int len);
		void onCodecBuffer(struct CodecBuffer& buff);
		
	protected:


	private:
		bool				mFirstFrame;
		bool				mbRunning;

		FILE 				*mFile;
		
		char mcharLength[4];
		char mData[1000000];
		char mSpsPps[21];
		int  mSpsPpsLen;
		
		//sp<CodecBase> 	mCodec;
};

#endif


