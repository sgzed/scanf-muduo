 ///
 /// @file    EventLoop.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 11:37:59
 ///

#ifndef _WD_EVENTLOOP_H__
#define _WD_EVENTLOOP_H__
#include "Channel.h"
#include "Epoller.h"
#include "Mutex.h"
#include "Timestamp.h"
#include "TimerQueue.h"
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
using std::vector;

class Channel;
class Epoll;
class Timer;
class TimerQueue;

class EventLoop
{
typedef boost::function<void()> Functor;
typedef boost::function<void()> TimerCallback;

public:
	EventLoop();
	~EventLoop();
	void loop();
	void update(std::shared_ptr<Channel> channel);

	int getEpollFd();

	void doPendingFunctors();

	void runInLoop(const Functor& cb);

	void queueInLoop(const Functor& cb);

	void wakeup();

	void quit();

	Timer* runAt(const Timestamp& time,const TimerCallback& cb);
	
	Timer* runAfter(double delay,const TimerCallback& cb);

	Timer* runEvery(double interval,const TimerCallback& cb);

	void cancel(Timer* timer);
private:
	void handleRead();

private:
	bool _looping;
	bool _quit;
	bool _callingPendingFunctors;

	//map<int,Channel*> _channels;
	vector<Channel*> _activeChannels; 

	mutable MutexLock _mutex;	
	vector<Functor> _pendingFunctors;	

public:
	boost::scoped_ptr<Epoll> _poller;
	int _wakeupFd;
	std::shared_ptr<Channel> _wakeupChannel;
	boost::scoped_ptr<TimerQueue> _timerQueue;
};

#endif

