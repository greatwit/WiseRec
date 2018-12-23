
#ifndef I_VIDEO_CALLBACK
#define I_VIDEO_CALLBACK

#include "capture.h"



class IVideoCallback
{
	public:
		virtual ~IVideoCallback(){}
		virtual void VideoSource(V4L2BUF_t *pBuf)=0;
};

#endif
