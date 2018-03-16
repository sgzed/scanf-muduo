 ///
 /// @file    TcpServer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-10 23:52:37
 ///
 //

#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__

#include "EventLoopThreadPool.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include <sys/epoll.h>
#include <map>
#include <vector>
#include <boost/function.hpp>
using std::map;
using std::vector;

#define MAX_EVENTS 500
#define RESERVE_EVENTS 16 

class Acceptor;
class TcpServer
{
typedef boost::function<void(Acceptor*)> ConnectionCallback;
typedef boost::function<void(shared_ptr<TcpConnection>,Buffer*)> MessageCallback;
typedef boost::function<void (shared_ptr<TcpConnection>)> WriteCompleteCallback;

public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;

	TcpServer(EventLoop* loop,const string& name)
		:_loop(loop),
		_name(name),
		_threadPool(new EventLoopThreadPool(loop,_name)),
		_messageCallback(NULL),
		 _writeCompleteCallback(NULL)
	{
	}

	~TcpServer()
	{}

	void setThreadNum(int numThreads);

	void setThreadInitCallback(const ThreadInitCallback& cb)
	{	_threadInitCallback = cb;}


	std::shared_ptr<EventLoopThreadPool> threadPool()
	{ return _threadPool;} 

	void setConnectionCallback (const ConnectionCallback& cb);

	void setMessageCallback(const MessageCallback& cb);

	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		_writeCompleteCallback = cb;
	}

	void newConnection(int sockfd);

//	void removeConnectionInLoop(const TcpConnection& coon);

	void start();

private:
//	Channel _listen;
	EventLoop* _loop;	
	
	string _name;

	map<int,shared_ptr<TcpConnection>> _connections;

	shared_ptr<Acceptor> _acceptor;
	std::shared_ptr<EventLoopThreadPool> _threadPool;

	ThreadInitCallback _threadInitCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
};


#endif

