 ///
 /// @file    Condition.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 22:05:43
 ///

#ifndef __WD_CONDITION_H__
#define __WD_CONDITION_H__
	
#include "Mutex.h"
#include <pthread.h>

class Condition
{
public:
	explicit Condition(MutexLock& mutex)
		:_mutex(mutex)
	{
		pthread_cond_init(&_pcnd,NULL);
	}

	~Condition()
	{
		pthread_cond_destroy(&_pcnd);
	}

	void wait()
	{
		MutexLock::UnassignGuard ug(_mutex);
		pthread_cond_wait(&_pcnd,_mutex.getPthreadMutex());
	}

	bool waitForSeconds(double seconds);

	void notify()
	{
		pthread_cond_signal(&_pcnd);
	}

	void notifyAll()
	{
		pthread_cond_broadcast(&_pcnd);
	}

private:
	MutexLock& _mutex;
	pthread_cond_t _pcnd;
};

#endif 
