 ///
 /// @file    TcpConnection.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 20:11:11
 ///

#ifndef __WD_TCPCONNECTION_H__
#define __WD_TCPCONNECTION_H__

#include "Channel.h"
#include "Buffer.h"
#include <memory>
#include <boost/function.hpp>
#include "EventLoop.h"
using std::shared_ptr;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
typedef boost::function<void (shared_ptr<TcpConnection>)> ConnectionCallback;
typedef boost::function<void (shared_ptr<TcpConnection>,Buffer*)> MessageCallback;
typedef boost::function<void (shared_ptr<TcpConnection>)> WriteCompleteCallback;

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

	  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	  {
			_writeCompleteCallback = cb;
	  }

	void handleRead();
	
	void handleWrite();

	shared_ptr<Channel> getChannel()
	{
		return _pChannel;
	}

	void send(Buffer*);

	void send(const string&);

	void sendInLoop(const void*,size_t len);

private:
	EventLoop* _loop;
	int _sockfd;
	shared_ptr<Channel> _pChannel;
	
	Buffer _inputBuffer;
	Buffer _outputBuffer;

	ConnectionCallback _connectionCallback;
	MessageCallback   _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
};

#endif

