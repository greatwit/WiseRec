
#ifndef __VIDEO_FRAME__H__
#define __VIDEO_FRAME__H__


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VideoFrame_t
{
	unsigned int addrPhyY;
	unsigned int addrVirY;
	int index;
	int length;
	long long timeStamp;
}VideoFrame;

#ifdef __cplusplus
}
#endif

#endif // __CAPTURE__H__
