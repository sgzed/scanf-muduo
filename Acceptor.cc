///
/// @file    Acceptor.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-11 17:29:22
///

#include "Acceptor.h"

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

#include "TcpConnection.h"
#include <boost/bind.hpp>
#include <iostream>
using std::cout;
using std::endl; 


int Acceptor::createAndListen()
{
	int on=1;
	_listenfd = socket(AF_INET,SOCK_STREAM,0);

	assert(setNonblock(_listenfd)==true);

	setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(_ip);
	servaddr.sin_port = htons(_port);

	if(-1 == bind(_listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))
	{
		perror("bind");
	}

	if(-1 == listen(_listenfd,MAX_LISTENFD))
	{
		perror("listen");
		exit(-1);
	}
	setNonblock(_listenfd);
	cout << "listenfd " << _listenfd << endl;
	_acceptor = std::shared_ptr<Channel> (new Channel(_loop,_listenfd));
	auto& iter =_loop->_poller->getChannels();
	iter[_listenfd] = _acceptor;
	return _listenfd; 
}

void Acceptor::handleRead()
{
	int connfd;

	struct sockaddr_in peeraddr;
	socklen_t perrlen = sizeof perrlen;
		connfd = accept(_listenfd,(sockaddr*)&peeraddr,&perrlen);
		if(connfd>0)
		{
			cout << "new connection from [" 
				<< inet_ntoa(peeraddr.sin_addr)
				<< ":"  << ntohs(peeraddr.sin_port) << "] "
				<< "accept fd:" << connfd << endl;
			_server->newConnection(connfd);
		}
		else
		{
			cout << "accept err, errno:" << errno << endl;
		}
}

void Acceptor::start()
{
	createAndListen();
	_acceptor->setReadCallback(boost::bind(&Acceptor::handleRead,this));
	_acceptor->enableReading();
}

