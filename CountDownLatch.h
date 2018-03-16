 ///
 /// @file    CountDownLatch.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 22:28:34
 ///


#ifndef __WD_COUNTDOWNLATCH_H__
#define __WD_COUNTDOWNLATCH_H__

#include "Condition.h"
#include "Mutex.h"

class CountDownLatch
{
public:
	explicit CountDownLatch(int count);

	void wait();

	void countDown();

	int getCount() const;

private:
	mutable MutexLock _mutex;

	Condition _condition;
	
	int _count;
};

#endif
