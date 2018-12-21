

#include "CommonThread.h"

#define SLEEP_TIME 1000

CommonThread::CommonThread()
			:mRunning(false)
{
}

CommonThread::~CommonThread()
{
}

bool CommonThread::StartThread()
{
	int iResult;

	iResult = pthread_create(&mPid, NULL, ThreadWork, this);

	if (0 != iResult)
	{
		GLOGV("create thread1 failed:----!%s\n", strerror(iResult));
		return false;
	}
	return true;
}

bool CommonThread::StopThread()
{
	void *status;
	
	mRunning = false;
	
	pthread_join(mPid, &status);

	return true;
}

bool CommonThread::run(char*tag, int level)
{
	int iResult;

	mRunning = true;
	
	pthread_attr_t attr;
	iResult = pthread_attr_init(&attr);

	iResult = pthread_attr_setschedpolicy (&attr, SCHED_RR );//SCHED_RR SCHED_FIFO SCHED_OTHER
	if (0 != iResult)
	{
		GLOGE("pthread_attr_setschedpolicy failed:----!%s\n", strerror(iResult));
	}
	iResult = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (0 != iResult)
	{
		GLOGE("pthread_attr_setdetachstate failed:----!%s\n", strerror(iResult));
	}
	//pthread_attr_setschedparam ( &attr, &sched );

	iResult = pthread_create(&mPid, &attr, ThreadWork, this);
	pthread_attr_destroy (&attr);

	if (0 != iResult)
	{
		GLOGE("create thread1 failed:----!%s\n", strerror(iResult));
		return false;
	}
	
	pthread_detach(mPid);

	return true;
}

bool CommonThread::requestExit()
{
	//CMutex::Autolock _l(mLock);
	mRunning = false;
	return true;
}

bool CommonThread::getRunning()
{
	//CMutex::Autolock _l(mLock);
	return mRunning;
}

void* CommonThread::ThreadWork(void* context)
{
	if(NULL == context)
		return 0;

	CommonThread *pContext = (CommonThread*)context;
	
	//while(pContext->getRunning())
		pContext->threadLoop();

	return ((void*)1); 
}

 bool CommonThread::threadLoop()
 {
	 return true;
 }


