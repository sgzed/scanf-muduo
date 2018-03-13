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

	_pChannel->setWriteCallback(boost::bind(&TcpConnection::handleWrite,this));

	_pChannel->enableReading();
}

void TcpConnection::handleRead()
{

	size_t n = _inputBuffer.readFd(_pChannel->getFd());

	if(n>0)
	{
		_messageCallback(shared_from_this(),&_inputBuffer);
	}
	else if(n==0)
	{
		cout << "client fd:" << _pChannel->getFd() << " close !" << endl;
		close(_pChannel->getFd());
		(_loop->_poller->getChannels()).erase(_pChannel->getFd());
	}
	else
	{
		perror("read");
	}
}


void TcpConnection::send(Buffer* buffer)
{
//	size_t n = ::send(_pChannel->getFd(),buffer,buffer->readableBytes(),0);
//
//	buffer->retrieveAll();
//
//	if(n< buffer->readableBytes())
//	{
//		cout << "can't send one time" << endl;
//	}

	sendInLoop(buffer->peek(),buffer->readableBytes());
}

void TcpConnection::send(const string& buffer)
{
	sendInLoop(buffer.c_str(),buffer.size());
}

void TcpConnection::sendInLoop(const void* data,size_t len)
{
	ssize_t nwrote = 0;
	ssize_t remaining = len;

	if(_outputBuffer.readableBytes() ==0)
	{
		nwrote = ::send(_pChannel->getFd(),data,len,0);
		if(nwrote >=0)
		{
			remaining = len - nwrote;
			if(remaining==0 && _writeCompleteCallback)
				_writeCompleteCallback(shared_from_this());
		}
		else // nwrote <0
		{
			nwrote = 0;
			perror("sendInLoop");
		}
	}

	if(remaining > 0 )
	{
		size_t oldLen = _outputBuffer.readableBytes();
		
		_outputBuffer.append(static_cast<const char*>(data),remaining);
		
		if(!_pChannel->isWriting())
		{
			_pChannel->enableWriting();
		}
	}
}

void TcpConnection::handleWrite()
{
	if(_pChannel->isWriting())
	{
		ssize_t n = ::write(_pChannel->getFd(),_outputBuffer.peek(),_outputBuffer.readableBytes());
	
		if(n>0)
		{
			_outputBuffer.retrieve(n);
			if(_outputBuffer.readableBytes()==0)
			{
				_pChannel->disableWriting();
			}
		}
		else
		{
			perror("hanldeWrite");
		}
	}
	else
	{
		cout << "not in writing ... " << endl;
	}
}


