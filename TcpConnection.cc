 ///
 /// @file    TcpConnection.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 20:13:53
 ///

#include "TcpConnection.h"
#include <iostream>
#include <boost/bind.hpp>
#include <sys/types.h>
#include <sys/socket.h>

using std::endl;
using std::cout;

TcpConnection::TcpConnection(EventLoop* loop,int sockfd)
	:_loop(loop),_sockfd(sockfd)
{
	if(setNonblock(sockfd)==false)
		cout << "setNonblock false" << endl;
	
	_pChannel = shared_ptr<Channel>(new Channel(_loop,_sockfd));

	_pChannel->setReadCallback(boost::bind(&TcpConnection::handleRead,this));

	_pChannel->enableReading();
}

void TcpConnection::handleRead()
{

	size_t n = read(_pChannel->getFd(),inputbuffer,MAX_LINE);

	if(n>0)
	{
		_messageCallback(shared_from_this());
	}
	else if(n==0)
	{
		cout << "client fd:" << _pChannel->getFd() << " close !" << endl;
		close(_pChannel->getFd());
	}
	else
	{
		perror("read");
	}
}


void TcpConnection::send(const char* buffer)
{
	size_t n = ::send(_pChannel->getFd(),buffer,strlen(buffer),0);
	
	if(n<strlen(buffer))
	{
		cout << "can't send one time" << endl;
	}
}


