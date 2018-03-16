///
/// @file    Thread.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-15 23:05:42
///

#include "Thread.h"
#include "CurrentThread.h"

#include "Timestamp.h"
#include <sys/syscall.h>

namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread const char* t_threadName = "unkown";
	//const bool sameType = boost::is_same<int,pid_t>::value;
}


pid_t gettid()
{
	return static_cast<pid_t>(syscall(SYS_gettid));
}

void afterFork()
{
	CurrentThread::t_cachedTid = 0;
	CurrentThread::t_threadName = "main";
	CurrentThread::tid();
}

class ThreadNameInitializer
{
	public:
		ThreadNameInitializer()
		{
			CurrentThread::t_threadName = "main";
			CurrentThread::tid();
			pthread_atfork(NULL, NULL, &afterFork);
		}
};

ThreadNameInitializer init;

void CurrentThread::cacheTid()
{
	if(t_cachedTid == 0)
	{
		t_cachedTid = gettid();
	}
}

bool CurrentThread::isMainThread()
{
	return tid()==::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
	struct timespec ts = { 0, 0 };
	ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
	::nanosleep(&ts, NULL);
}


void * startThread(void* obj)
{
	Thread* data = static_cast<Thread*>(obj);
	data->runInThread();
	//delete data;
	return NULL;
}


AtomicInt32 Thread::_numCreated;

Thread::Thread(const ThreadFunc& func,const string& n)
	:_started(false),
	_joined(false),
	_pthreadId(0),
	_tid(0),
	_func(func),
	_name(n),
	_latch(1)
{
	setDefaultName();
}

void Thread::runInThread()
{
	_tid = CurrentThread::tid();
	_latch.countDown();

	CurrentThread::t_threadName=_name.empty()?"muduoThread":_name.c_str();

	_func();

}

Thread::~Thread()
{
	if(_started && !_joined)
		pthread_detach(_pthreadId);
}

void Thread::start()
{
	_started  = true;

	if(pthread_create(&_pthreadId,NULL,startThread,this))
	{
		_started  = false;
	}
	else
	{
		_latch.wait();
	}	
}

int Thread::join()
{
	_joined = true;
	return pthread_join(_pthreadId,NULL);
}

void Thread::setDefaultName()
{
	int num = _numCreated.incrementAndGet();

	if(_name.empty())
	{
		char buf[32];
		snprintf(buf,sizeof buf,"Thread%d",num);
		_name  =buf;
	}
}


