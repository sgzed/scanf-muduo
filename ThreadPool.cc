 ///
 /// @file    ThreadPool.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 10:41:00
 ///
 
#include "ThreadPool.h"
#include <boost/bind.hpp>
#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;

ThreadPool::ThreadPool(const string& name)
	:_mutex(),
	_notEmpty(_mutex),
	_notFull(_mutex),
	_name(name),
	_maxQueueSize(0),
	_running(false)
{

}

ThreadPool::~ThreadPool()
{
	if(_running)
		stop();
}

void ThreadPool::start(int numThreads)
{
	assert(_threads.empty());
	_running = true;
	_threads.reserve(numThreads);

	for(int i=0;i<numThreads;++i)	
	{
		char id[32];
		snprintf(id,sizeof id,"%d",i+1);

		_threads.push_back(new Thread(boost::bind(&ThreadPool::runInThread,this),_name+id));
	
		_threads[i].start();
	}

	if(numThreads==0 && _threadInitCallback)
			_threadInitCallback();
}

void ThreadPool::stop()
{
	{
		MutexLockGuard lock(_mutex);
		_running = false;
		_notEmpty.notifyAll();
	}

	for_each(_threads.begin(),_threads.end(),
			boost::bind(&Thread::join,_1));
}

size_t ThreadPool::queueSize() const
{
	MutexLockGuard lock(_mutex);
	return _queue.size();
}

void ThreadPool::run(const Task& task)
{
	if(_threads.empty())
		task();
	else
	{
		MutexLockGuard lock(_mutex);
		while(isFull())
		{
			_notFull.wait();
		}
		_queue.push_back(task);
		_notEmpty.notify();
	}
}

ThreadPool::Task ThreadPool::take()
{
	MutexLockGuard lock(_mutex);

	while(_queue.empty() && _running)
	{
		_notEmpty.wait();
	}
	Task task;
	if(!_queue.empty())
	{
		task = _queue.front();
		_queue.pop_front();
		if(_maxQueueSize>0)
			_notFull.notify();
	}
	return task;
}

bool ThreadPool::isFull() const 
{
	_mutex.assertLocked();
	return _maxQueueSize>0 && _queue.size()>=_maxQueueSize;
}

void ThreadPool::runInThread()
{
	if(_threadInitCallback)
		_threadInitCallback();

	while(_running)
	{
	
		Task task(take());
		if(task)
			task();
	}
}







