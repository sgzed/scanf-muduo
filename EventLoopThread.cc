 ///
 /// @file    EventLoopThread.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 11:35:31
 ///

#include "EventLoopThread.h"
#include "EventLoop.h"
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
		const string& name)
	:_loop(NULL),
	_exiting(false),
	_thread(boost::bind(&EventLoopThread::threadFunc,this),name),
	_mutex(),
	_cond(_mutex),
	_callback(cb)
{
}

EventLoopThread::~EventLoopThread()
{
	_exiting = true;

	if(_loop!=NULL)
	{
		_loop->quit();
		_thread.join();
	}
}

EventLoop* EventLoopThread::startLoop()
{
	assert(!_thread.started());
	_thread.start();

	{
		MutexLockGuard lock(_mutex);
		while(_loop==NULL)
			_cond.wait();
	}
	return _loop;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;

	if(_callback)
	{
		_callback(&loop);
	}

	{
		MutexLockGuard lock(_mutex);
		_loop = &loop;
		_cond.notify();
	}

	loop.loop();

	// loop退出循环时	
	_loop = NULL;
}





