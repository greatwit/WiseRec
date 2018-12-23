

#include "CameraListen.h"

#include "CameraApi.h"
#define TAG "CameraApi"


/////////////////////////////////////////////////////////////////////////////////////api//////////////////////////////////////////////////////////////////////////////////

sp<JNICameraListen> gListenContext = NULL;


int CameraSetup(IVideoCallback *callback, jint cameraId, jstring clientPackageName)
{

	if(gListenContext == NULL)
		gListenContext = new JNICameraListen(callback);
	
	return gListenContext->CameraSetup(cameraId, clientPackageName);
}

int CameraRelease()
{
	if(gListenContext != NULL)
	{
		gListenContext->CameraRelease();
		gListenContext = NULL;
		return 0;
	}
		
	return -1;
}

void SetCameraParameter(jstring params)
{
	if(gListenContext != NULL)
		gListenContext->setParameters(params);
}

jstring GetCameraParameter()
{
	return gListenContext->getParameters();
}

void StartPreview(const sp<Surface> &surface)
{
	if(gListenContext != NULL)
		gListenContext->startPreview(surface);
}

void StopPreview()
{
	if(gListenContext != NULL)
		gListenContext->stopPreview();
}

void SetDisplayOrientation(int value)
{
	if(gListenContext != NULL)
		gListenContext->setDisplayOrientation(value);
}

