 ///
 /// @file    Buffer.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-13 16:59:27
 ///
 
#include "Buffer.h"
#include <iostream>
using std::cout;
using std::endl;

const char Buffer::kCRLF[] = "\r\n"; 

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

ssize_t Buffer::readFd(int fd)
{
	char extrabuf[65535];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	vec[0].iov_base = begin()+_writeIndex;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;

	const int iovcnt = (writable<sizeof extrabuf)?2:1;

	const ssize_t n = ::readv(fd,vec,iovcnt);
	if(n<0)
	{
		perror("readv");
	}
	else if(static_cast<size_t>(n) <= writable)
	{
		_writeIndex+=n;	
	}
	else
	{
		_writeIndex = _buffer.size();
		append(extrabuf,n-writable);
	}
	return n;
}



