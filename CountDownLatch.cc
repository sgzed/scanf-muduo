 ///
 /// @file    CountDownLatch.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 22:32:08
 ///

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
	:_mutex(),
	_condition(_mutex),
	_count(count)
{
	
}

void CountDownLatch::wait()
{
	MutexLockGuard lock(_mutex);

	while(_count>0)
		_condition.wait();
}

void CountDownLatch::countDown()
{
	MutexLockGuard lock(_mutex);
	--_count;
	if(_count==0)
		_condition.notifyAll();
}

int CountDownLatch::getCount() const
{
	MutexLockGuard lock(_mutex);
	return _count;
}










