 ///
 /// @file    CurrentThread.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 21:49:00
 ///

#ifndef __WD_CURRENTTHREAD_H__
#define __WD_CURRENTTHREAD_H__

#include <stdint.h>

namespace CurrentThread
{
	extern __thread int t_cachedTid;
	extern __thread char t_tidString[32];
	extern __thread int t_tidStringLength;
	extern __thread const char* t_threadName;

	void cacheTid();

	inline int tid()
	{
		if(t_cachedTid == 0)
			cacheTid();
		return t_cachedTid;
	}

	inline const char* name() 
	{
		return t_threadName;
	}

	bool isMainThread();

	void sleepUsec(int64_t usec);
}


#endif
