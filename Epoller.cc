 ///
 /// @file    Epoller.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-12 12:05:40
 ///

#include "Epoller.h"
#include <sys/epoll.h>
#include <iostream>
using std::cout;
using std::endl;
using std::shared_ptr;

Epoll::Epoll():
	_epollfd(epoll_create1(EPOLL_CLOEXEC)),
	_events(16)
{

}

Epoll::~Epoll()
{
	close(_epollfd);
}

map<int,shared_ptr<Channel>>& Epoll::getChannels()
{
	return _channels;
}


void Epoll::poll(vector<Channel*>* activeChannels)
{
	cout << "fd total count " << _channels.size() << endl;

	int numEvents = ::epoll_wait(
				_epollfd,
				&*_events.begin(),
				static_cast<int>(_events.size()),
				-1);
	if(numEvents > 0)
	{
		cout << numEvents << " events happend" << endl;
		
		fillActiveChannels(numEvents,activeChannels);	
	}
	else if(numEvents ==0)
	{
		cout << "nothing happend " << endl;
	}
	else
	{
		if(errno!=EINTR)
		{
			perror("epoll_wait");
		}
	}
	
}

void Epoll::fillActiveChannels(int numEvents,vector<Channel*>* activeChannels)
{
	for(int i=0;i<numEvents;++i)
	{
		Channel* channel(static_cast<Channel*>(_events[i].data.ptr));
		channel->setRevent(_events[i].events);
		activeChannels->push_back(channel);
	}
}


