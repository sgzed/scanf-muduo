 ///
 /// @file    Acceptor.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 17:24:03
 ///

#ifndef __WD_ACCEPTOR_H__
#define __WD_ACCEPTOR_H__

#include "TcpServer.h"
#include "Channel.h"
#include <memory>

#define MAX_LISTENFD 5

class TcpServer;

class Acceptor
{
	typedef boost::function<void(shared_ptr<Acceptor>)> ConnectionCallback;
public:
	Acceptor(TcpServer* server,int epollfd,const char* ip="127.0.0.1",short port=2018):
		_server(server),
		_epollfd(epollfd),
		_ip(ip),
		_port(port),
		_listenfd(-1),
		_acceptor(NULL)
	{ }

	~Acceptor()
	{
	}

	void start();

	void handleRead();

	int fd()
	{
		return _listenfd;
	}
  
private:
	int createAndListen();

	TcpServer* _server;
	int _epollfd;
	const char * _ip;
	short _port;
	int _listenfd;
	std::shared_ptr<Channel> _acceptor;

	//ConnectionCallback _connectionCallback;
};

#endif

