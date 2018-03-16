 ///
 /// @file    Thread.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-15 22:44:16
 ///

#ifndef __WD_THREAD_H__
#define __WD_THREAD_H__

#include "Atomic.h"
#include "CountDownLatch.h"
#include <pthread.h>
#include <boost/function.hpp>
#include <memory>
#include <string>
using std::string;
using std::shared_ptr;

class Thread
{
public:
	typedef boost::function<void ()> ThreadFunc;

	explicit Thread(const ThreadFunc&,const string& name = string());

	~Thread();

	void start();

	int join();

	bool started() const {return _started;}

	pid_t tid() const { return _tid;}

	const string& name() const {return _name;}

	static int numCreated()
	{
		return _numCreated.get();
	}

	void runInThread();

private:
	void setDefaultName();		

	bool _started;
	bool _joined;
	pthread_t _pthreadId;
	pid_t  _tid;
	ThreadFunc _func;
	string _name;
	CountDownLatch _latch;

	static AtomicInt32 _numCreated; 
};

#endif
