


#ifndef __gthreadpool_hpp__
#define __gthreadpool_hpp__

#include <pthread.h>

typedef struct tagGThread GThread_t;

class GThreadPool {
public:
	typedef void ( * DispatchFunc_t )( void * );

	GThreadPool();
	GThreadPool( int maxThreads, const char * tag = 0 );
	~GThreadPool();

	void initPara(int maxThreads, const char * tag = 0);
	/// @return 0 : OK, -1 : cannot create thread
	int dispatch( DispatchFunc_t dispatchFunc, void *arg );

	int getMaxThreads();

private:
	char * mTag;

	int mMaxThreads;
	int mIndex;
	int mTotal;
	int mIsShutdown;

	pthread_mutex_t mMainMutex;
	pthread_cond_t mIdleCond;
	pthread_cond_t mFullCond;
	pthread_cond_t mEmptyCond;

	GThread_t ** mThreadList;

	static void * wrapperFunc( void * );
	int saveThread( GThread_t * thread );
};

#endif

