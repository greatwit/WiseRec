
#include<dlfcn.h>
#include <utils/Log.h>
#include <sys/system_properties.h>
#include "CodecContext.h"

#include "ComDefine.h"
#define TAG "CodecContext"

namespace android 
{
	CodecContext* CodecContext::mSelf = NULL;
	CodecContext::CodecContext():mLibHandle(NULL)
	{
	    //读取sdk版本
	    char szSdkVer[32]={0};
	    __system_property_get("ro.build.version.sdk", szSdkVer);
	    GLOGW("sdk:%d",atoi(szSdkVer));

	    LoadBaseLib(atoi(szSdkVer));
	}

	CodecContext::~CodecContext()
	{
		if(mLibHandle)
		{
			dlclose(mLibHandle); // �ر�so���
			mLibHandle = NULL;
		}
	}

	CodecContext*CodecContext::getInstance()
	{
		if(mSelf == NULL)
			mSelf = new CodecContext();
		return mSelf;
	}

	bool CodecContext::LoadBaseLib(int version)
	{
		if(mLibHandle)
			return false;
		
			switch(version)
			{
				case 17: 		//17 4.2, 4.2.2
				mLibHandle = dlopen("libCodecBase4.so", RTLD_NOW); // ����.so�ļ� RTLD_LAZY
					break;

				case 18: 		//18 4.3
				mLibHandle = dlopen("libCodecBase4.so", RTLD_NOW); // ����.so�ļ� RTLD_LAZY
					break;

				case 19:		//19 4.4
				case 20:		//20 4.4W
					break;
					
				case 21: 		//21 5.0
				case 22:		//22 5.1
				mLibHandle = dlopen("libCodecBase5.so", RTLD_NOW); // ����.so�ļ� RTLD_LAZY
					break;
					
				case 23: 		//6.0
				mLibHandle = dlopen("libCodecBase6.so", RTLD_NOW); // ����.so�ļ� RTLD_LAZY
					break;
					
				case 24:		//7.0
				case 25:		//7.1.1/7.1
					break;
					
				case 26:		//8.0 O
					break;
				
				default:
					break;
			}
			
			if (!mLibHandle) {
				GLOGE("Error: load so  failed.\n");
			}
			dlerror(); // ��մ�����Ϣ
		
		
		testAdd 					= (test_t*)dlsym(mLibHandle, "test"); // ���뺯��
		
		CodecCreate 				= (CodecCreate_t*)dlsym(mLibHandle, "CodecCreate");
		RegisterBufferCall			= (RegisterBufferCall_t*)dlsym(mLibHandle, "RegisterBufferCall");
		StartCodec 					= (StartCodec_t*)dlsym(mLibHandle, "StartCodec");
		StopCodec 					= (StopCodec_t*)dlsym(mLibHandle, "StopCodec");
		AddBuffer 					= (AddBuffer_t*)dlsym(mLibHandle, "AddBuffer");
		CodecDestroy 				= (CodecDestroy_t*)dlsym(mLibHandle, "CodecDestroy");
		ConvertKeyValueToMessage 	= (ConvertKeyValueToMessage_t*)dlsym(mLibHandle, "ConvertKeyValueToMessage");
		
		const char *err = dlerror();
		if (NULL != err) {
			GLOGE("dlsym stderr:%s\n", err);
			return false;
		}

		GLOGE("testAdd 57 + 3 = %d\n", testAdd(57, 3)); // ���ú���
			
		return mLibHandle!=NULL;
	}

}

