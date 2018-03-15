 ///
 /// @file    Timestamp.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-14 11:51:22
 ///

#ifndef __WD_TIMESTAMP_H__
#define __WD_TIMESTAMP_H__

#include <sys/types.h>
#include <string>
using std::string;

class Timestamp
{
public:	
	Timestamp(double microSecond=0.0);
	~Timestamp();

	bool valid();
	int64_t microSecondsSinceEpoch() const;

	string toString() const;

	static Timestamp now();
	static Timestamp nowAfter(double seconds);
	static double nowMicroSeconds();
	static const int kMicroSecondsPerSecond = 1000*1000;

private:
	int64_t _microSecondsSinceEpoch;
};

bool operator < (const Timestamp& lhs,const Timestamp& rhs);
bool operator == (const Timestamp & lhs,const Timestamp& rhs);

#endif
