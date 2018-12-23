
#ifndef RECORD_BASE_H_
#define RECORD_BASE_H_

#include <string>

#include "ComDefine.h"

#define TAG "RecordBase"



class RecordBase
{
	public:	
		RecordBase():mMediaRecording(false),mbAudioRecording(false){}
		virtual ~RecordBase(){}

		virtual bool Init() = 0;
		virtual bool DeInit() = 0;
		virtual bool IsInited()	= 0;


		virtual bool SetMediaParam(const char* pFilePath, int achannel, int arate, int abit, int vwidth, int vheight, double vfps) = 0;
		virtual bool StartMediaRecord() = 0;
		virtual bool StopMediaRecord() = 0;


		virtual bool SetAudioParam(const char* pFilePath, int achannel, int arate, int abit) = 0;
		virtual bool StartAudioRecord() = 0;
		virtual bool StopAudioRecord() = 0;

		virtual bool WriteAudioData(char*buffer, int size) = 0;
		virtual bool WriteVideoData(char*buffer, int size) = 0;

	public:
		bool mMediaRecording;
		bool mbAudioRecording;

};


#endif //
