
#ifndef I_VIDEO_CALLBACK
#define I_VIDEO_CALLBACK

#include "VideoFrame.h"



class IVideoCallback
{
	public:
		virtual ~IVideoCallback(){}
		virtual void VideoSource(VideoFrame *pBuf)=0;
};

#endif
