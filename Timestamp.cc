 ///
 /// @file    Timestamp.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 11:58:07
 ///

#include "Timestamp.h"

#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>

#include <iostream>
using std::cout;
using std::endl;

Timestamp::Timestamp(double microSecond)
	:_microSecondsSinceEpoch(microSecond)
{

}

Timestamp::~Timestamp()
{

}

bool Timestamp::valid()
{
	return _microSecondsSinceEpoch >0;
}

int64_t Timestamp::microSecondsSinceEpoch() const 
{
	return _microSecondsSinceEpoch;
}

string Timestamp::toString() const
{
	char buf[32]= {0};
	int64_t seconds = _microSecondsSinceEpoch/kMicroSecondsPerSecond;
	int64_t microseconds = _microSecondsSinceEpoch%kMicroSecondsPerSecond;

	snprintf(buf,sizeof(buf)-1,"%" PRId64 ".%06" PRId64 "",seconds,microseconds);

	return buf;
}

Timestamp Timestamp::now()
{
	return Timestamp(Timestamp::nowMicroSeconds());
}

Timestamp Timestamp::nowAfter(double seconds)
{
	return Timestamp(Timestamp::nowMicroSeconds()+kMicroSecondsPerSecond*seconds);
}

double Timestamp::nowMicroSeconds()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*kMicroSecondsPerSecond+tv.tv_usec;
}

bool operator < (const Timestamp& lhs,const Timestamp& rhs)
{
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}


bool operator == (const Timestamp & lhs,const Timestamp& rhs)
{
	cout << "operator == " << endl;
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}


