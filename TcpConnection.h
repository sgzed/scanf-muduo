 ///
 /// @file    TcpConnection.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 20:11:11
 ///

#ifndef __WD_TCPCONNECTION_H__
#define __WD_TCPCONNECTION_H__

#include "Channel.h"
#include <memory>
#include <boost/function.hpp>
#include "EventLoop.h"
using std::shared_ptr;

#define MAX_LINE 512

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
typedef boost::function<void (shared_ptr<TcpConnection>)> ConnectionCallback;
typedef boost::function<void (shared_ptr<TcpConnection>)> MessageCallback;

public:
	TcpConnection(EventLoop* loop,int sockd);

	~TcpConnection()
	{}

	void setConnectionCallback(const ConnectionCallback& cb)
		{ _connectionCallback = cb; }

	  void setMessageCallback(const MessageCallback& cb)
		{ 
			_messageCallback = cb; 
		}

	void handleRead();

	shared_ptr<Channel> getChannel()
	{
		return _pChannel;
	}

	void send(const char*);

	char* getInputBuffer()
	{
		return inputbuffer;
	}

private:
	EventLoop* _loop;
	int _sockfd;
	shared_ptr<Channel> _pChannel;
	
	char inputbuffer[MAX_LINE];
	char outputbuffer[MAX_LINE];

	ConnectionCallback _connectionCallback;
	MessageCallback   _messageCallback;
};

#endif

