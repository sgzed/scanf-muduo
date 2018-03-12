 ///
 /// @file    TcpServer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-10 23:52:37
 ///
 //

#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__

#include "TcpConnection.h"
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
typedef boost::function<void(shared_ptr<TcpConnection>)> MessageCallback;

public:
	TcpServer()
		:_epollfd(-1),
		_messageCallback(NULL)
	{
		_events.resize(RESERVE_EVENTS);
	}

	~TcpServer()
	{}

	void setConnectionCallback (const ConnectionCallback& cb);

	void setMessageCallback(const MessageCallback& cb);
				
	void newConnection(int sockfd);

	void start();

private:
	int _epollfd;
//	Channel _listen;

	vector<struct epoll_event> _events;

	map<int,shared_ptr<TcpConnection>> _connections;

	shared_ptr<Acceptor> _acceptor;

	MessageCallback _messageCallback;
};


#endif

