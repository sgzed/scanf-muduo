 ///
 /// @file    EventLoop.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 17:15:55
 ///
 
#include "EventLoop.h"
#include <iostream>
using std::cout;
using std::endl;

EventLoop::EventLoop():
	_looping(false),
	_quit(false),
	_poller(new Epoll())
{
	cout << "EventLoop create " << endl;
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
	
}

void EventLoop::update(std::shared_ptr<Channel> channel)
{

}

