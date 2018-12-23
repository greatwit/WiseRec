
#ifndef __CAPTURE__H__
#define __CAPTURE__H__


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct V4L2BUF_t
{
	unsigned int addrPhyY;
	unsigned int addrVirY;
	int index;
	int length;
	long long timeStamp;
}V4L2BUF_t;


#ifdef __cplusplus
}
#endif

#endif // __CAPTURE__H__
