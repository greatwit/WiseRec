#include "CodecContext.h"

#include<dlfcn.h>

#include <utils/Log.h>



#define TAG "CodecBaseLib"

namespace android 
{

	CodecBaseLib* CodecBaseLib::mSelf = NULL;

	CodecBaseLib::CodecBaseLib():mLibHandle(NULL)
	{
		
	}

	CodecBaseLib::~CodecBaseLib()
	{
		if(mLibHandle)
		{
			dlclose(mLibHandle); // 关闭so句柄
			mLibHandle = NULL;
		}
		
	}

	CodecBaseLib*CodecBaseLib::getInstance()
	{
		if(mSelf == NULL)
			mSelf = new CodecBaseLib();
		return mSelf;
	}

	bool CodecBaseLib::LoadBaseLib(int version)
	{
		if(mLibHandle)
			return false;
		
			switch(version)
			{
				case 17: 		//17 4.2, 4.2.2
				mLibHandle = dlopen("libCodecBase4.so", RTLD_NOW); // 载入.so文件 RTLD_LAZY
					break;
				case 18: 		//18 4.3
				case 19:		//19 4.4
				case 20:		//20 4.4W
					break;
					
				case 21: 		//21 5.0
				case 22:		//22 5.1
				mLibHandle = dlopen("libCodecBase5.so", RTLD_NOW); // 载入.so文件 RTLD_LAZY
					break;
					
				case 23: 		//6.0
				mLibHandle = dlopen("libCodecBase6.so", RTLD_NOW); // 载入.so文件 RTLD_LAZY
					break;
					
				case 24:		//7.0
				case 25:		//7.1.1/7.1
					break;
					
				case 26:		//8.0 O
					break;
				
				default:
					break;
			}
			
			if (!mLibHandle) 
			{
				ALOGE("Error: load so  failed.\n");
			}
			dlerror(); // 清空错误信息
		
		
		testAdd 					= (test_t*)dlsym(mLibHandle, "test"); // 载入函数
		
		CodecCreate 				= (CodecCreate_t*)dlsym(mLibHandle, "CodecCreate");
		RegisterBufferCall			= (RegisterBufferCall_t*)dlsym(mLibHandle, "RegisterBufferCall");
		StartCodec 					= (StartCodec_t*)dlsym(mLibHandle, "StartCodec");
		StopCodec 					= (StopCodec_t*)dlsym(mLibHandle, "StopCodec");
		AddBuffer 					= (AddBuffer_t*)dlsym(mLibHandle, "AddBuffer");
		CodecDestroy 				= (CodecDestroy_t*)dlsym(mLibHandle, "CodecDestroy");
		ConvertKeyValueToMessage 	= (ConvertKeyValueToMessage_t*)dlsym(mLibHandle, "ConvertKeyValueToMessage");
		
		
		const char *err = dlerror();
		if (NULL != err) 
		{
			ALOGE("dlsym stderr:%s\n", err);
			return false;
		}

		ALOGE("testAdd 57 + 3 = %d\n", testAdd(57, 3)); // 调用函数
			
		return mLibHandle!=NULL;
	}

}


