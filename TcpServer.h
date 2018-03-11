 ///
 /// @file    TcpServer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-10 23:52:37
 ///
 //

#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__

class TcpServer
{
public:
	TcpServer()
	{}

	~TcpServer()
	{}
	
	void start();

private:
	int createAndListen();
};


#endif

