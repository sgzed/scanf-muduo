 ///
 /// @file    Condition.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 22:24:13
 ///

#include "Condition.h"
#include <errno.h>

bool Condition::waitForSeconds(double seconds)
{
	struct timespec abstime;

	clock_gettime(CLOCK_REALTIME, &abstime);

	const int64_t kNanoSecondsPerSecond = 1000000000;
	int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

	abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
	abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

	MutexLock::UnassignGuard ug(_mutex);
	return ETIMEDOUT == pthread_cond_timedwait(&_pcnd, _mutex.getPthreadMutex(), &abstime);
}
