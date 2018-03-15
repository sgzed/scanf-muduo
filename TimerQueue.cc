 ///
 /// @file    TimeQueue.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 15:49:07
 ///

#include "TimerQueue.h"

#include <unistd.h>
#include <sys/timerfd.h>
#include "boost/bind.hpp"

#include <iterator>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
			                    TFD_NONBLOCK | TFD_CLOEXEC);
	if(timerfd < 0)
	{
		perror("timerfd_create");
		abort();
	}
	return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64_t microseconds = when.microSecondsSinceEpoch()-Timestamp::nowMicroSeconds();
	if(microseconds < 100)
	{
		microseconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds/Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>(
		(microseconds%Timestamp::kMicroSecondsPerSecond)*1000);
	return ts;
}

void readTimerfd(int timerfd,Timestamp now)
{
	uint64_t howmany;
	ssize_t n= ::read(timerfd,&howmany,sizeof(howmany));
	cout << "TimeQueu::handleRead() " << howmany << " at " << now.toString();

	if(n!=sizeof howmany)
	{
		perror("Timer::read()");
		abort();
	}
}

void resetTimerfd(int timerfd,Timestamp expiration)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;

	bzero(&newValue,sizeof newValue);
	bzero(&oldValue,sizeof oldValue);

	newValue.it_value = howMuchTimeFromNow(expiration);

	int ret = ::timerfd_settime(timerfd,0,&newValue,&oldValue);

	if(ret)
	{
		perror("timerfd_settime");
	}
}


TimerQueue::TimerQueue(EventLoop* loop)
	:_loop(loop),
	_timerfd(createTimerfd()),
	_timerfdChannel(new Channel(_loop,_timerfd)),
	_timers()	
{
	_timerfdChannel->setReadCallback(boost::bind(&TimerQueue::handleRead,this));
	_timerfdChannel->enableReading();
}

TimerQueue::~TimerQueue()
{
	for(TimerList::iterator it=_timers.begin();it!=_timers.end();++it)
	{
		delete it->second;
	}
}

void TimerQueue::handleRead()
{
	Timestamp now(Timestamp::now());
	readTimerfd(_timerfd,now);

	vector<Entry> expired = getExpired(now);

	for(vector<Entry>::iterator it=expired.begin();it!=expired.end();++it)
	{
		it->second->run();
	}

	reset(expired,now);
}

vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	vector<Entry> expired;
	Entry sentry(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = _timers.lower_bound(sentry);

	std::copy(_timers.begin(),end,std::back_inserter(expired));
	_timers.erase(_timers.begin(),end);

	return expired;
}

Timer* TimerQueue::addTimer(const TimerCallback& cb,Timestamp when,double interval)
{
	Timer* timer = new Timer(cb,when,interval);

	_loop->runInLoop(boost::bind(&TimerQueue::addTimerInLoop,this,timer));

	return timer;
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	bool earliestChanged = insert(timer);

	if(earliestChanged)
	{
		resetTimerfd(_timerfd,timer->expiration());
	}
}

void TimerQueue::reset(const vector<Entry>& expired,Timestamp now)
{
	Timestamp nextExpire;

	for(vector<Entry>::const_iterator it=expired.cbegin();it!=expired.cend();++it)
	{
		if(it->second->repeat())
		{
				it->second->restart(now);
				insert(it->second);
		}	
		else
		{
			delete it->second;
		}
	}

	if(!_timers.empty())
	{
		nextExpire=_timers.begin()->second->expiration();
	}

	if(nextExpire.valid())
	{
		resetTimerfd(_timerfd,nextExpire);
	}
}

bool TimerQueue::insert(Timer* timer)
{
	bool earliestChanged = false;
	
	Timestamp when  = timer->expiration();

	TimerList::iterator it = _timers.begin();

	if(it==_timers.end() || when<it->first)
	{
		earliestChanged = true;
	}

	_timers.insert(Entry(when,timer));

	return earliestChanged;
}

void TimerQueue::cancel(Timer* timer)
{
	_loop->runInLoop(boost::bind(&TimerQueue::cancelInLoop,this,timer));
}

void TimerQueue::cancelInLoop(Timer* timer)
{
	Entry activeEntry = Entry(timer->expiration(),timer);	

	TimerList::iterator it=_timers.find(activeEntry);

	if( it!=_timers.end())
	{
		_timers.erase(activeEntry);
		delete it->second;
	}
}














