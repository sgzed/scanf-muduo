 ///
 /// @file    main.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-09 23:34:01
 ///

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //for bzero
#include <assert.h>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/epoll.h>

#include <iostream>
using std::cout;
using std::endl;
using std::vector;

#define MAX_LINE 100
#define MAX_EVENTS 500
#define RESERVE_EVENTS 16 
#define MAX_LISTENFD 5

bool setNonblock(int fd)
{
	int flag = fcntl(fd,F_GETFL);
	flag |= O_NONBLOCK;
	return fcntl(fd,F_SETFL,flag)!=-1;
}

int createAndListen()
{
	int on=1;
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	assert(setNonblock(listenfd)==true);
	
	 setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(2018);
		
	if(-1 == bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))
	{
		cout << "bind err, errno" << errno << endl;
	}

	if(-1 == listen(listenfd,MAX_LISTENFD))
	{
		cout << "listen err, errno" << errno << endl;
		exit(-1);
	}
	return listenfd; 
}


int main(int agrc,char** argv)
{
	int listenfd,connfd,sockfd;
	
	char line[MAX_LINE];

	struct sockaddr_in peeraddr;
	socklen_t perrlen = sizeof perrlen;

	int epollfd = epoll_create1(EPOLL_CLOEXEC);

	if(epollfd<0)
	{
		cout << "epoll_create err,errno:" << errno << endl;
		exit(-1);
	}

	listenfd = createAndListen();
	
	struct epoll_event event;
	vector<struct epoll_event> evs(RESERVE_EVENTS);
	//evs.reserve(RESERVE_EVENTS);
	
	event.data.fd = listenfd;
	event.events = EPOLLIN;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event);

	int ret,nrecv;

	while(1)
	{
		ret = epoll_wait(epollfd,&*evs.begin(),static_cast<int>(evs.size()),-1);
	
		if(-1 == ret)
		{
			perror("epoll_wait");
			exit(-1);
		}
		
		for(int i=0;i<ret;++i)
		{
			if(evs[i].data.fd == listenfd)
			{
				connfd = accept(listenfd,(sockaddr*)&peeraddr,&perrlen);
				if(connfd>0)
				{
					cout << "new connection from [" 
						<< inet_ntoa(peeraddr.sin_addr)
						<< ":"  << ntohs(peeraddr.sin_port) << "] "
						<< "accept fd:" << connfd << endl;
					setNonblock(connfd);
					event.events = EPOLLIN;
					event.data.fd = connfd;
					epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event);
				}
				else
				{
					cout << "accept err, errno:" << errno << endl;
				}
			}
			else if(evs[i].events&EPOLLIN)
			{
				sockfd = evs[i].data.fd;
				bzero(line,MAX_LINE);
				
				if((nrecv=read(sockfd,line,sizeof line))<0)
				{
					if(errno ==ECONNRESET )
					{
						cout << "ECONNREST closed socket fd:" << sockfd << endl;  
						close(sockfd);
					}
					else
					cout << "sock fd" << sockfd <<  "read err ,errno:" << errno << endl; 
				}
				else if(nrecv ==0)
				{
						cout << "peer client closed socket fd:" << sockfd << endl; 
						close(sockfd);
				}
				else
				{
					cout << "receive " << nrecv << " bytes data from " << sockfd << endl;
					if(write(sockfd,line,nrecv)!=nrecv)
						cout << "err: not finished one time" << endl;
				}

			}
		}
	
	}
			return 0;
}






