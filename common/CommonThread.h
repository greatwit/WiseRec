#ifndef COMMNON_THREAD_H_
#define COMMNON_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>


#include "ComDefine.h"


class CommonThread
{
	public:
		CommonThread();
		virtual ~CommonThread();
		bool  StartThread();
		bool  StopThread();

		bool run(char*tag, int level);
		bool requestExit();
		bool getRunning();

		virtual bool threadLoop();

	protected:
		static void* ThreadWork(void* context);

	private:
		pthread_t				mPid;
		bool					mRunning;
		//CMutex 					mLock;
};

#endif 


