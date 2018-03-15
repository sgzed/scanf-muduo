 ///
 /// @file    Timer.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 15:12:55
 ///


#include "Timer.h"

int Timer::_numCreated = 0;

void Timer::restart(Timestamp now)
{
	if(_repeat)
	{
		_expiration = now.nowAfter(_interval);
	}
		
	else 
	{
		_expiration = Timestamp();
	}

}
