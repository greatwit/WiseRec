/*

    This file is a part of the JThread package, which contains some object-
    oriented thread wrappers for different thread implementations.

    Copyright (c) 2000-2011  Jori Liesenborgs (jori.liesenborgs@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*/

#include "gthread.h"
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


GThread::GThread()
	:threadid(0)
{
	retval = NULL;
	mutexinit = false;
	running = false;
}

GThread::~GThread()
{
	Kill();
}

int GThread::Start()
{
	int status;

	if (!mutexinit)
	{
		if (!runningmutex.IsInitialized())
		{
			if (runningmutex.Init() < 0)
				return ERR_JTHREAD_CANTINITMUTEX;
		}
		if (!continuemutex.IsInitialized())
		{
			if (continuemutex.Init() < 0)
				return ERR_JTHREAD_CANTINITMUTEX;
		}
		if (!continuemutex2.IsInitialized())
		{
			if (continuemutex2.Init() < 0)
				return ERR_JTHREAD_CANTINITMUTEX;
		}
		mutexinit = true;
	}
	
	runningmutex.Lock();
	if (running)
	{
		runningmutex.Unlock();
		return ERR_JTHREAD_ALREADYRUNNING;
	}
	runningmutex.Unlock();
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	continuemutex.Lock();
	status = pthread_create(&threadid, &attr, TheThread, this);
	pthread_attr_destroy(&attr);
	if (status != 0)
	{
		continuemutex.Unlock();
		return ERR_JTHREAD_CANTSTARTTHREAD;
	}
	
	/* Wait until 'running' is set */
	
	runningmutex.Lock();			
	while (!running)
	{
		runningmutex.Unlock();
		
		struct timespec req,rem;

		req.tv_sec = 0;
		req.tv_nsec = 1000000;
		nanosleep(&req,&rem);

		runningmutex.Lock();
	}
	runningmutex.Unlock();
	
	continuemutex.Unlock();
	
	continuemutex2.Lock();
	continuemutex2.Unlock();
	return 0;
}

int GThread::Kill()
{
	runningmutex.Lock();			
	if (!running)
	{
		runningmutex.Unlock();
		return ERR_JTHREAD_NOTRUNNING;
	}
#ifdef __ANDROID__
	pthread_kill(threadid,SIGUSR1);
#elif __linux__
	pthread_cancel(threadid);
#endif
	running = false;
	runningmutex.Unlock();
	return 0;
}

bool GThread::IsRunning()
{
	bool r;
	
	runningmutex.Lock();			
	r = running;
	runningmutex.Unlock();
	return r;
}

void *GThread::GetReturnValue()
{
	void *val;
	
	runningmutex.Lock();
	if (running)
		val = NULL;
	else
		val = retval;
	runningmutex.Unlock();
	return val;
}

void *GThread::TheThread(void *param)
{
	GThread *jthread;
	void *ret;
	
	jthread = (GThread *)param;
	
	jthread->continuemutex2.Lock();
	jthread->runningmutex.Lock();
	jthread->running = true;
	jthread->runningmutex.Unlock();
	
	jthread->continuemutex.Lock();
	jthread->continuemutex.Unlock();
	
	ret = jthread->Thread();

	jthread->runningmutex.Lock();
	jthread->running = false;
	jthread->retval = ret;
	jthread->runningmutex.Unlock();

	return NULL;
}

void GThread::ThreadStarted()
{
	continuemutex2.Unlock();
}


