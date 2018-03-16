 ///
 /// @file    EventLoopThread.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 11:28:48
 ///

#ifndef __WD_EVENTLOOPTHTREAD_H__
#define __WD_EVENTLOOPTHTREAD_H__

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

class EventLoop;

class EventLoopThread 
{
public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
			const string& name = string());
	~EventLoopThread();

	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* _loop;
	bool _exiting;
	Thread _thread;
	MutexLock _mutex;
	Condition _cond;
	ThreadInitCallback _callback;
};

#endif
