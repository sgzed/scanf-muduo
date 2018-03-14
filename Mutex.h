 ///
 /// @file    Mutex.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-01 17:30:50
 ///

#ifndef __WD_MUTEX_H__
#define __WD_MUTEX_H__

#include <pthread.h>
#include <assert.h>


class MutexLock 
{
public:
	MutexLock():_holder(0)
	{
		pthread_mutex_init(&_mutex,NULL);
	}

	~MutexLock()
	{
		assert(_holder == 0);
		pthread_mutex_destroy(&_mutex);
	}

	void assertLocked()
	{
	}

	void lock()
	{
		pthread_mutex_lock(&_mutex);
		assignHolder();
	}

	pthread_mutex_t* getPthreadMutex() /* non-const */
	{
		return &_mutex;
	}


	void unlock()
	{
		unassignHolder();
		pthread_mutex_unlock(&_mutex);
	}

	MutexLock(const MutexLock&) = delete;
	MutexLock& operator=(const MutexLock&) = delete;

private:
	friend class Condition;

	class UnassignGuard 
	{
		public:
			UnassignGuard(MutexLock& owner)
				:_owner(owner)
			{
				_owner.unassignHolder();
			}

			~UnassignGuard()
			{
				_owner.assignHolder();	
			}

		private:
			MutexLock& _owner;
	};
	 void unassignHolder()
	 {
		_holder = 0;
	}

	void assignHolder()
	{
	}

private:
	pthread_mutex_t _mutex;
	pid_t  _holder;
};

class MutexLockGuard
{
public:
	MutexLockGuard(MutexLock& mutex)
		:_mutex(mutex)
	{
		_mutex.lock();
	}

	~MutexLockGuard()
	{
		_mutex.unlock();
	}

private:
	MutexLock& _mutex;
};

#define MutexLockGuard(x) error "Missing guard object name"

#endif
