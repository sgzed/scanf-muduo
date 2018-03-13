 ///
 /// @file    Channel.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 10:37:23
 ///

#ifndef __WD_CHANNEL_H__
#define __WD_CHANNEL_H__

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "EventLoop.h"

extern bool setNonblock(int fd);

class EventLoop;

class Channel : public std::enable_shared_from_this<Channel>
{
public: 
	typedef boost::function<void()> EventCallback;
	
	Channel(EventLoop *loop,int fd);

	~Channel();

	void setReadCallback(const EventCallback& cb)
	{
		_readCallback = cb;
	}

	void setWriteCallback(const EventCallback& cb)
	{
		_writeCallback = cb;
	}

	int getFd();

	void handleEvent();

	void enableReading();

	void enableWriting();

	void disableWriting();

	bool isWriting()
	{
		return _events&EPOLLOUT;
	}

	void setRevent(int);

private:
	EventLoop* _loop;
	int _fd;
	int _events;
	int _revents;

private:
	EventCallback _readCallback;
	EventCallback _writeCallback;
};


#endif
