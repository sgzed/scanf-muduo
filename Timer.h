 ///
 /// @file    Timer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 11:42:50
 ///


#ifndef __WD_TIMER_H__
#define __WD_TIMER_H__

#include <boost/function.hpp>
#include "Timestamp.h"

class Timer
{
typedef boost::function<void()> TimerCallback; 

public:
	Timer(const TimerCallback& cb,Timestamp when,double interval)
		:_callback(cb),
		_expiration(when),
		_interval(interval),
		_repeat(_interval > 0.0),
		_sequence(++_numCreated)
	{
	}

	bool repeat() const 
	{
		return _repeat;
	}

	void run() const
	{
		_callback();
	}

	Timestamp expiration() const
	{
		return _expiration;
	}

	int64_t sequence() const {return  _sequence;}

	void restart(Timestamp now);

	static int64_t numCreated() { return _numCreated;}


private:
	const TimerCallback _callback;

	Timestamp _expiration;

	const double _interval;
	const bool _repeat;
	const int64_t _sequence;

	static int _numCreated;
};

#endif

