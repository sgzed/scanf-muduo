 ///
 /// @file    EventLoopThreadPool.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-16 12:06:58
 ///

#ifndef __WD_EVENTLOOPTHREADPOOL_H__
#define __WD_EVENTLOOPTHREADPOOL_H__

#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
using std::string;

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThreadPool(EventLoop* baseloop,const string& name);
	~EventLoopThreadPool();

	void setThreadNum(int numThreads) {_numThreads =numThreads; }

	void start(const ThreadInitCallback& cb = ThreadInitCallback());

	EventLoop* getNextLoop();

	EventLoop* getLoopForHash(size_t hashCode);

	std::vector<EventLoop*> getAllLoops();

	bool started() const 
	{return _started;	}

	const string& name() const 
	{return _name;}

private:
	EventLoop* _baseLoop;
	string _name;
	bool _started;
	int _numThreads;
	int _next;
	boost::ptr_vector<EventLoopThread> _threads;
	std::vector<EventLoop*> _loops;
};
#endif
