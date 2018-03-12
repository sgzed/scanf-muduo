///
/// @file    callback.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-11 11:51:16
///

#include "TcpServer.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp>
#include <errno.h>
using std::cout;
using std::endl;

void onMessage(shared_ptr<TcpConnection> );

int main()
{
	TcpServer server;

	server.setMessageCallback(boost::bind(onMessage,_1));

	server.start();

}

void onMessage(shared_ptr<TcpConnection> pconn)
{
	int nrecv;

	char line[512];
	int sockfd = pconn->getChannel()->getFd();
	cout << "sockfd = " << sockfd << endl;
	if((nrecv=read(sockfd,line,sizeof line))<0)
	{
		if(errno ==ECONNRESET )
		{
			cout << "ECONNREST closed socket fd:" << sockfd << endl;  
			close(sockfd);
		}
		else
			perror("read");
	}
	else if(nrecv ==0)
	{
		cout << "peer client closed socket fd:" << sockfd << endl; 
		close(sockfd);
	}
	else
	{	
		cout << "receive " << nrecv << " bytes data from sock fd " << sockfd << endl;
		if(write(sockfd,line,nrecv)!=nrecv)
			cout << "can't send back one time" << endl;
	}
}
