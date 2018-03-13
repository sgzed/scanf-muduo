 ///
 /// @file    EventLoop.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 11:37:59
 ///

#ifndef _WD_EVENTLOOP_H__
#define _WD_EVENTLOOP_H__
#include "Channel.h"
#include "Epoller.h"
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

private:
	bool _looping;
	bool _quit;
	//map<int,Channel*> _channels;
	vector<Channel*> _activeChannels; 

public:
	boost::scoped_ptr<Epoll> _poller;
};

#endif

