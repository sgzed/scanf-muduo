 ///
 /// @file    Epoller.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 11:43:00
 ///

#ifndef __WD_EPOLL_H__
#define __WD_EPOLL_H__

#include "Channel.h"
#include <map>
#include <vector>
using std::vector;
using std::map;
using std::shared_ptr;

class Channel;
class EventLoop;

class Epoll
{
public:
	Epoll();

	~Epoll();

	void poll(vector<Channel*>* activeChannels);

	int getEpollFd()
	{
		return _epollfd;
	}

	map<int,shared_ptr<Channel>>& getChannels();

private:
	void fillActiveChannels(int numEvents,vector<Channel*>*);

	int _epollfd;
	vector<struct epoll_event> _events;
	map<int,shared_ptr<Channel>> _channels;
};


#endif

