 ///
 /// @file    Channel.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 10:49:15
 ///

#include "Channel.h"
#include <sys/epoll.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>
using std::cout;
using std::endl;


bool setNonblock(int fd)
{
	int flag = fcntl(fd,F_GETFL);
	flag |= O_NONBLOCK;
	return fcntl(fd,F_SETFL,flag)!=-1;
}

Channel::Channel(int epollfd,int fd)
	:_epollfd(epollfd),
	 _fd(fd),
	 _events(0),
	 _revents(0),
	 _readCallback(NULL)
{

}

Channel::~Channel()
{
	//cout << "~Channel()" <<  endl;	
}

void Channel::enableReading()
{
	_events |= EPOLLIN;

	struct epoll_event event;
	event.data.ptr = this;
	event.events = _events;
	int ret = epoll_ctl(_epollfd,EPOLL_CTL_ADD,_fd,&event);
	if(ret < 0)
	{
		cout << "fd " << _fd << endl;
		perror("epoll_ctl");
	}
}

void Channel::handleEvent()
{
	if(_revents&EPOLLIN)
	{
		_readCallback();
	}
}

int Channel::getFd()
{
	return _fd;
}

void Channel::setRevent(int revent)
{
	_revents = revent;
}

