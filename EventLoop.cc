 ///
 /// @file    EventLoop.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 17:15:55
 ///
 
#include "EventLoop.h"
#include <iostream>
#include <boost/bind.hpp>
#include <sys/eventfd.h>
using std::cout;
using std::endl;


int createEventFd()
{
	int eventFd = ::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
	if(eventFd < 0)
	{
		perror("eventfd");
		abort();
	}
	return eventFd;
}

EventLoop::EventLoop():
	_looping(false),
	_quit(false),
	_callingPendingFunctors(false),
	_wakeupFd(createEventFd()),
	_wakeupChannel(new Channel(this,_wakeupFd)),
	_poller(new Epoll())
{
	cout << "EventLoop create " << endl;

	_wakeupChannel->setReadCallback(boost::bind(&EventLoop::handleRead,this));

	_wakeupChannel->enableReading();
}

void EventLoop::handleRead()
{
	uint64_t one =1;
	ssize_t n= read(_wakeupFd,&one,sizeof one);
	if(n!=sizeof one)
	{
		cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8" << endl; 
	}
}

EventLoop::~EventLoop()
{
}

int EventLoop::getEpollFd()	
{
	return _poller->getEpollFd();
}

void EventLoop::loop()
{
	_looping = true;
	_quit = false;

	while(!_quit)
	{
		_activeChannels.clear();	
		
		_poller->poll(&_activeChannels);

		for(auto iter : _activeChannels)
		{
			iter->handleEvent();	
		}
	}
	doPendingFunctors();	
}

void EventLoop::update(std::shared_ptr<Channel> channel)
{

}

void EventLoop::doPendingFunctors()
{
	vector<Functor>  functors;

	_callingPendingFunctors  = true;
	{
		MutexLockGuard lock(_mutex);

		functors.swap(_pendingFunctors);
	}

	for(size_t i=0;i<functors.size();++i)
	{
		functors[i]();
	}
	_callingPendingFunctors  = false;
}

void EventLoop::runInLoop(const Functor& cb)
{
	queueInLoop(cb);	
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lock(_mutex);
		_pendingFunctors.push_back(cb);
	}

	if(_callingPendingFunctors)
	  wakeup();
}

void EventLoop::wakeup()
{
	uint64_t one = 1;

	ssize_t n= write(_wakeupFd,&one,sizeof one);
	if(n !=sizeof one)
	{
		cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << endl;
	}
}









