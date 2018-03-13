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
	_acceptor = shared_ptr<Acceptor>(new Acceptor(this,_loop));

	_acceptor->start();

	//_loop->loop();
}


void TcpServer::newConnection(int connfd)
{
	shared_ptr<TcpConnection> conn(new TcpConnection(_loop,connfd));

	auto& iter = _loop->_poller->getChannels();
	iter[connfd] = conn->getChannel();

	conn->setMessageCallback(boost::bind(_messageCallback,_1,_2));
		
	_connections[connfd] = conn;

}











 
