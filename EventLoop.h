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
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
using std::vector;

class Channel;
class Epoll;

class EventLoop
{
typedef boost::function<void()> Functor;

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

private:
	void handleRead();

private:
	bool _looping;
	bool _quit;
	bool _callingPendingFunctors;

	int _wakeupFd;
	boost::scoped_ptr<Channel> _wakeupChannel;

	//map<int,Channel*> _channels;
	vector<Channel*> _activeChannels; 

	mutable MutexLock _mutex;	
	vector<Functor> _pendingFunctors;	

public:
	boost::scoped_ptr<Epoll> _poller;
};

#endif

