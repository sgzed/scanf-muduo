 ///
 /// @file    ThreadPool.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 10:34:17
 ///

#ifndef __WD_THREADPOOL_H__
#define __WD_THREADPOOL_H__

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>
using std::deque;

class ThreadPool
{
public:
	typedef boost::function<void()> Task;

	explicit ThreadPool(const string& nameArg=string("ThreadPool"));

	~ThreadPool();

	void setMaxQueueSize(int maxSize) {_maxQueueSize = maxSize;}

	void setThreadInitCallback(const Task& cb)
	{
		_threadInitCallback = cb;
	}

	void start(int numThreads);

	void stop();

	const string& name() const { return _name; }

	size_t queueSize() const;

	void run(const Task& f);

private:
	bool isFull() const;
	void runInThread();
	Task take();

	mutable MutexLock _mutex;
	Condition _notEmpty;
	Condition _notFull;
	string _name;
	Task _threadInitCallback;
	boost::ptr_vector<Thread> _threads;
	deque<Task> _queue;
	size_t _maxQueueSize;
	bool _running;
};



#endif










