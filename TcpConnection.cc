 ///
 /// @file    TcpConnection.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 20:13:53
 ///

#include "TcpConnection.h"
#include <iostream>
#include <boost/bind.hpp>
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
//	char line[MAX_LINE];
//	size_t n = read(_pChannel->getFd(),line,MAX_LINE);
//
//	if(n>0)
//	{
//		_messageCallback(shared_from_this());
//	}
//	else if(n==0)
//	{
//		close(_pChannel->getFd());
//	}
//	else
//	{
//		cout << "read err " << endl ; 	
//	}
	_messageCallback(shared_from_this());
}




