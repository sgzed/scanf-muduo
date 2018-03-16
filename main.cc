///
/// @file    callback.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-11 11:51:16
///

#include "Timestamp.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp>
#include <errno.h>
#include <stdio.h>
using std::cout;
using std::endl;

void onMessage(shared_ptr<TcpConnection> pconn,Buffer* buffer);

void print(const char* msg)
{
	for(int i=0;i<5;++i)
		printf("msg : %s %s\n",Timestamp::now().toString().c_str(),msg);
}

int main()
{

	EventLoop loop;

	TcpServer echo(&loop,"echo");
	echo.setMessageCallback(boost::bind(&onMessage,_1,_2));

	loop.runEvery(2,boost::bind(&print,"once1"));
	echo.start();
	loop.loop();
}

void onMessage(shared_ptr<TcpConnection> pconn,Buffer* buffer)
{
	string msg(buffer->retrieveAllAsString());

	cout << "fd:" << pconn->getChannel()->getFd() << " echo " << msg.size() << " bytes" << endl;
	pconn->send(msg);
}
