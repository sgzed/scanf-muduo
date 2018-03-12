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

void TcpServer::setConnectionCallback(const ConnectionCallback& cb)
{
	//_acceptor.setReadCallback(boost::bind(cb,_acceptor));
}

void TcpServer::setMessageCallback(const MessageCallback& cb)
{
	_messageCallback = cb;	
}

void TcpServer::start()
{
	_epollfd = epoll_create1(EPOLL_CLOEXEC);

	if(_epollfd<0)
	{
		cout << "epoll_create err,errno:" << errno << endl;
		exit(-1);
	}

	_acceptor = shared_ptr<Acceptor>(new Acceptor(this,_epollfd));

	_acceptor->start();

	int ret;
	while(1)
	{
		ret = epoll_wait(_epollfd,&*_events.begin(),static_cast<int>(_events.size()),-1);

		if(-1 == ret)
		{
			perror("epoll_wait");
			exit(-1);
		}
		
		for(int i=0;i<ret;++i)
		{
			int sockfd = ((Channel*)_events[i].data.ptr)->getFd();
			if(_acceptor->fd() == sockfd)
			{
				_acceptor->handleRead();	
			}
			else 
			{
				if(_connections.find(sockfd)== _connections.end())
				{
					cout << "no sock fd found"  ;
					exit(EXIT_FAILURE);
				}
				cout << "sockfd " << sockfd << " receive message" << endl;
				_connections[sockfd]->getChannel()->setRevent(_events[i].events);
				_connections[sockfd]->getChannel()->handleEvent();
			}
		}
	
	}
}


void TcpServer::newConnection(int connfd)
{
	shared_ptr<TcpConnection> conn(new TcpConnection(_epollfd,connfd));
		
	conn->setMessageCallback(boost::bind(_messageCallback,_1));
		
	_connections[connfd] = conn;

}











 
