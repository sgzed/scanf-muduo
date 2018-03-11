 ///
 /// @file    Channel.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-11 10:37:23
 ///

#ifndef __WD_CHANNEL_H__
#define __WD_CHANNEL_H__

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class Channel 
{
public: 
	typedef boost::function<void()> EventCallback;
	
	Channel(int epollfd,int fd);

	~Channel();

	void setReadCallback(const EventCallback& cb)
	{
		_readCallback = cb;
	}
	int getFd();

	void handleEvent();

	void enableReading();

	void setRevent(int);

private:
	int _epollfd;
	int _fd;
	int _events;
	int _revents;

private:
	EventCallback _readCallback;
};


#endif
