/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#define	FILE_PATH	"cus1280x720.h264"

int charsToInt1(char* src, int offset) {
	int value;
	value = (int) ((src[offset]	  & 0xFF)
				| ((src[offset+1] & 0xFF)<<8)
				| ((src[offset+2] & 0xFF)<<16)
				| ((src[offset+3] & 0xFF)<<24));
	return value;
}

int main( int argc, char ** argv )
{
	if ((argc != 2)) {
		printf("Usage: exe h264_filename.\n");
		return 0;
	}

	FILE			*mpFile;

	mpFile = fopen(argv[1], "rb");
	if(mpFile==NULL) {
		printf("open file:%s failed.\n", argv[1]);
		return 0;
	}

	int res = 0, dataLen = 0, count = 0;
	unsigned char mData[1000000] = {0};
	do{
		char mcharLength[4] = {0};
		res = fread(mcharLength, 4, 1, mpFile);
		if(res>0) {
			count++;
			dataLen = charsToInt1(mcharLength,0);
			res 	= fread(mData, dataLen, 1, mpFile);
			printf("startCode type:0x%02X dataLen:%d count:%d\n", mData[4], dataLen, count);
			usleep(10*1000);
		}
	}while(res>0);

	if(mpFile != NULL)
		fclose(mpFile);

	return 0;
}



