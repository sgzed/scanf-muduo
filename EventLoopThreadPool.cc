 ///
 /// @file    EventLoopThreadPool.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 12:14:49
 ///

#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"
#include <boost/bind.hpp>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop,const string& name)
	:_baseLoop(baseloop),
	_name(name),
	_started(false),
	_numThreads(0),
	_next(0)
{}

EventLoopThreadPool::~EventLoopThreadPool()
{	
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
	_started = true;

	_baseLoop->assertInLoopThread();

	for(int i=0;i<_numThreads;++i)
	{
		char buf[_name.size()+32];
		snprintf(buf,sizeof buf,"%s%d",_name.c_str(),i);
		EventLoopThread* t = new EventLoopThread(cb,buf);
		_threads.push_back(t);
		_loops.push_back(t->startLoop());
	}

	if(_numThreads ==0 && cb)
	{
		cb(_baseLoop);
	}
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
	
	_baseLoop->assertInLoopThread();
	assert(_started);
	EventLoop * loop = _baseLoop;

	if(!_loops.empty())
	{
		loop = _loops[_next];
		++_next;
		if(static_cast<size_t>(_next) >= _loops.size())
			_next = 0;
	}
	return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
	_baseLoop->assertInLoopThread();
	EventLoop * loop = _baseLoop;

	if(!_loops.empty())
	{
		loop = _loops[hashCode%_loops.size()];
	}
	return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
	_baseLoop->assertInLoopThread();
	assert(_started);
	if(!_loops.empty())
	{
		return std::vector<EventLoop*>(1,_baseLoop);
	}
	else
		return _loops;
}






