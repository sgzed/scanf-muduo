 ///
 /// @file    TimerQueue.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 15:28:48
 ///

#ifndef __TIMERQUEUE_H__
#define __TIMERQUEUE_H__

#include <set>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include "EventLoop.h"
#include "Timer.h"
#include "Mutex.h"
#include "Timestamp.h"
#include <utility>
using std::set;
using std::vector;
using std::pair;

class EventLoop;
class Channel;

class TimerQueue
{
typedef boost::function<void()> TimerCallback;

	typedef pair<Timestamp,Timer*> Entry;
	typedef set<Entry> TimerList;
public:
	explicit TimerQueue(EventLoop* loop);
	~TimerQueue();
	
	Timer* addTimer(const TimerCallback& cb,Timestamp when,double interval);

	void handleRead();

	vector<TimerQueue::Entry> getExpired(Timestamp);

	void cancel(Timer* timer);

	int getTimerfd()
	{
		return _timerfd;
	}

private:
	void addTimerInLoop(Timer* timer);

	void cancelInLoop(Timer* timer);

	void reset(const vector<Entry>& expired,Timestamp now);

	bool insert(Timer* timer);

private:
	EventLoop * _loop;
	const int _timerfd;
	std::shared_ptr<Channel> _timerfdChannel;
	TimerList _timers;
};

#endif

