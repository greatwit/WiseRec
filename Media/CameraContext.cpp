#include "CameraContext.h"

#include<dlfcn.h>

#include <utils/Log.h>


#include "ComDefine.h"
#define TAG "CameraLib"

namespace android 
{

	CameraLib* CameraLib::mSelf = NULL;

	CameraLib::CameraLib():mLibHandle(NULL)
	{
		
	}

	CameraLib::~CameraLib()
	{
		if(mLibHandle)
		{
			dlclose(mLibHandle); // 关闭so句柄
			mLibHandle = NULL;
		}
	}

	CameraLib*CameraLib::getInstance()
	{
		if(mSelf == NULL)
			mSelf = new CameraLib();
		return mSelf;
	}

	bool CameraLib::LoadCameraLib(int version)
	{
		if(mLibHandle)
			return false;
		
			switch(version)
			{
				case 17: 		//17 4.2, 4.2.2
				mLibHandle = dlopen("libCamera4.so", RTLD_NOW); // 载入.so文件 RTLD_LAZY
					break;
				case 18: 		//18 4.3
				case 19:		//19 4.4
				case 20:		//20 4.4W
					break;
					
				case 21: 		//21 5.0
				case 22:		//22 5.1
				mLibHandle = dlopen("libCamera5.so", RTLD_NOW); // 载入.so文件 RTLD_LAZY
					break;
					
				case 23: 		//6.0
				mLibHandle = dlopen("libCamera6.so", RTLD_LAZY); // 载入.so文件 RTLD_LAZY
					break;
					
				case 24:		//7.0
				case 25:		//7.1.1/7.1
					break;
					
				case 26:		//8.0 O
					break;
				
				default:
					break;
			}
			
			const char *err = dlerror(); 			//清空错误信息
			
			if (mLibHandle == NULL)
				GLOGE("function %s,line:%d load camera failed stderr:%s", __FUNCTION__, __LINE__, err);
			else
				GLOGE("function %s,line:%d dlopen camera succ", __FUNCTION__, __LINE__);
			
		
		CameraSetup 					= (CameraSetup_t*)dlsym(mLibHandle, "CameraSetup");
		CameraRelease 					= (CameraRelease_t*)dlsym(mLibHandle, "CameraRelease");
		SetCameraParameter 				= (SetCameraParameter_t*)dlsym(mLibHandle, "SetCameraParameter");
		GetCameraParameter 				= (GetCameraParameter_t*)dlsym(mLibHandle, "GetCameraParameter");
		StartPreview 					= (StartPreview_t*)dlsym(mLibHandle, "StartPreview");
		StopPreview 					= (StopPreview_t*)dlsym(mLibHandle, "StopPreview");
		SetDisplayOrientation			= (SetDisplayOrientation_t*)dlsym(mLibHandle, "SetDisplayOrientation");
		
		
		err = dlerror();
		if (NULL != err) 
		{
			GLOGE("dlsym stderr:%s\n", err);
			return false;
		}
			
		return true;
	}
	
	bool CameraLib::ReleaseLib()
	{
		if(mLibHandle)
		{
			dlclose(mLibHandle); // 关闭so句柄
			mLibHandle = NULL;
		}
		return mLibHandle==NULL;
	}

}


