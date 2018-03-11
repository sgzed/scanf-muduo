 ///
 /// @file    TcpServer.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-10 23:52:28
 ///

#include "TcpServer.h"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <assert.h>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/epoll.h>

#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl;
using std::vector;

bool setNonblock(int fd)
{
	int flag = fcntl(fd,F_GETFL);
	flag |= O_NONBLOCK;
	return fcntl(fd,F_SETFL,flag)!=-1;
}


int TcpServer::createAndListen()
{
	int on=1;
	int _listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	assert(setNonblock(_listenfd)==true);
	
	 setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.70.128");
	servaddr.sin_port = htons(2018);
		
	if(-1 == bind(_listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))
	{
		cout << "bind err, errno" << errno << endl;
	}

	if(-1 == listen(_listenfd,MAX_LISTENFD))
	{
		cout << "listen err, errno" << errno << endl;
		exit(-1);
	}

	_listen = Channel(_epollfd,_listenfd);
	_listen.enableReading();
	return _listenfd; 
}


void TcpServer::setConnectionCallback(const ConnectionCallback& cb)
{
	_listen.setReadCallback(boost::bind(cb,&_listen));
}

void TcpServer::setMessageCallback(const MessageCallback& cb)
{
	_messageCallback = cb;	
}

void TcpServer::start()
{
	int connfd,sockfd;
	
	struct sockaddr_in peeraddr;
	socklen_t perrlen = sizeof perrlen;

	_epollfd = epoll_create1(EPOLL_CLOEXEC);

	if(_epollfd<0)
	{
		cout << "epoll_create err,errno:" << errno << endl;
		exit(-1);
	}

	cout << "listen fd : "<< createAndListen() << endl;
	
	int ret;

	while(1)
	{
		ret = epoll_wait(_epollfd,&*_events.begin(),static_cast<int>(_events.size()),-1);

		if(-1 == ret)
		{
			perror("epoll_wait");
			exit(-1);
		}
		
		int _listenfd = _listen.getFd();
		for(int i=0;i<ret;++i)
		{
			sockfd =((Channel*)_events[i].data.ptr)->getFd();
			if(sockfd == _listenfd)
			{
				connfd = accept(_listenfd,(sockaddr*)&peeraddr,&perrlen);
				if(connfd>0)
				{
					cout << "new connection from [" 
						<< inet_ntoa(peeraddr.sin_addr)
						<< ":"  << ntohs(peeraddr.sin_port) << "] "
						<< "accept fd:" << connfd << endl;
					Channel* chl = new Channel(_epollfd,connfd);
					chl->enableReading();
					chl->setReadCallback(boost::bind(_messageCallback,chl));
					_channels[connfd]=chl;
				}
				else
				{
					cout << "accept err, errno:" << errno << endl;
				}
			}
			else 
			{
				if(_channels.find(sockfd)== _channels.end())
				{
					cout << "no sock fd found" << endl;
					exit(EXIT_FAILURE);
				}	
				_channels[sockfd]->setRevent(_events[i].events);
				_channels[sockfd]->handleEvent();
			}
		}
	
	}
}






 
