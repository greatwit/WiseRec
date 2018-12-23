
#include "BaseCamera.h"


 BaseCamera::BaseCamera()
			:mPixelFomat(-1)
			,mStreamBase(0)
			,mInited(false)
{

}

 BaseCamera::~BaseCamera()
{

}

 void BaseCamera::RegisterCallback(IVideoCallback *base) 
 {
    mStreamBase = base;
 }

 void BaseCamera::SetFrameRate(int rate)
 {
	 if(0<rate&&rate<50)
		 mSleepTime = 1000*1000/rate;
 }

 int BaseCamera::GetFrameRate()
 {
	 return 1000*1000/mSleepTime;
 }