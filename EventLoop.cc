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
	_threadId(CurrentThread::tid()),
	_poller(new Epoll()),
	_wakeupFd(createEventFd()),
	_wakeupChannel(new Channel(this,_wakeupFd)),
	_timerQueue(new TimerQueue(this))
{
	cout << "EventLoop create " << endl;

	cout << "wakeup fd:" << _wakeupFd << endl;

	cout << "timerfd:" << _timerQueue->getTimerfd() << endl;

	_wakeupChannel->setReadCallback(boost::bind(&EventLoop::handleRead,this));

	auto& iter=_poller->getChannels();
	iter[_wakeupFd] = _wakeupChannel;

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
	else
	{
		cout << "tiggered wakeupfd" << endl;
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
		doPendingFunctors();	
	}
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
	if(isInLoopThread())
		cb();
	else
		queueInLoop(cb);	
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lock(_mutex);
		_pendingFunctors.push_back(cb);
	}

	if(!isInLoopThread() || _callingPendingFunctors)
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

void EventLoop::quit()
{
	_quit = true;
	wakeup();
}

Timer* EventLoop::runAt(const Timestamp& time,const TimerCallback& cb)
{
	return _timerQueue->addTimer(cb,time,0.0);
}

Timer* EventLoop::runAfter(double delay,const TimerCallback& cb)
{
	Timestamp time(Timestamp::nowAfter(delay));

	return runAt(time,cb);
}

Timer* EventLoop::runEvery(double interval,const TimerCallback& cb)
{
	return _timerQueue->addTimer(cb,Timestamp::now(),interval);	
}

void EventLoop::abortNotInLoopThread()
{
	cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
		    << " was created in threadId_ = " << _threadId
		<< ", current thread id = " <<  CurrentThread::tid();
}
