 ///
 /// @file    TcpServer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-10 23:52:37
 ///
 //

#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__

#include "Channel.h"
#include <sys/epoll.h>
#include <map>
#include <vector>
#include <boost/function.hpp>
using std::map;
using std::vector;

#define MAX_LINE 100
#define MAX_EVENTS 500
#define RESERVE_EVENTS 16 
#define MAX_LISTENFD 5

class TcpServer
{
typedef boost::function<void(Channel*)> ConnectionCallback;
typedef boost::function<void(Channel*)> MessageCallback;

public:
	TcpServer()
		:_epollfd(-1),
		_listen(_epollfd,-1),
		_messageCallback(NULL)
	{
		_events.resize(RESERVE_EVENTS);
	}

	~TcpServer()
	{}

	void setConnectionCallback (const ConnectionCallback& cb);

	void setMessageCallback(const MessageCallback& cb);
				
	
	void start();

private:
	int createAndListen();

private:
	int _epollfd;
	Channel _listen;

	vector<struct epoll_event> _events;

	map<int,Channel*> _channels;

	MessageCallback _messageCallback;
};


#endif

