 ///
 /// @file    Buffer.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-13 11:38:38
 ///


#ifndef __WD_BUFFER_H__
#define __WD_BUFFER_H__
#include <assert.h>
#include <arpa/inet.h>

#include <vector>
#include <string>
#include <algorithm>
using std::vector;
using std::string;

class Buffer
{
public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;

	explicit Buffer(size_t initialSize = kInitialSize)
		:_buffer(kCheapPrepend+initialSize),
		_readerIndex(kCheapPrepend),
		_writeIndex(kCheapPrepend)
	{
	}

	void swap(Buffer& rhs)
	{
		_buffer.swap(rhs._buffer);
		std::swap(_readerIndex,rhs._readerIndex);
		std::swap(_writeIndex,rhs._writeIndex);
	}

	size_t readableBytes() const
	{
		return _writeIndex-_readerIndex;
	}

	size_t writableBytes() const
	{
		return _buffer.size()-_writeIndex;
	}

	size_t prependableBytes() const
	{
		return _readerIndex;
	}

	const char* peek() const
	{
		return begin()+_readerIndex;
	}

	const char* beginWrite() const
	{
		return begin()+_writeIndex;
	}

	char* beginWrite()
	{
		return begin()+_writeIndex;
	}

	const char* findCRLF() const
	{
		const char* crlf=std::search(peek(),beginWrite(),kCRLF,kCRLF+2);
		return crlf==beginWrite()?NULL:crlf;
	}


	const char* findCRLF(const char* start) const
	{
		const char* crlf=std::search(start,beginWrite(),kCRLF,kCRLF+2);
		return crlf==beginWrite()?NULL:crlf;
	}

	void retrieve(size_t len)
	{
		assert(len<=readableBytes());
		if(len < readableBytes())
		{
			_readerIndex += len;
		}
		else
		{
			retrieveAll();
		}
	}

	void retrieveAll()
	{
		_readerIndex = kCheapPrepend;
		_writeIndex = kCheapPrepend;
	}

	string retrieveAsString(size_t len)
	{
		assert(len <= readableBytes());
		string result(peek(),len);
		retrieve(len);
		return result;
	}

	string retrieveAllAsString()
	{
		return retrieveAsString(readableBytes());
	}

	void append(const char* data,size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data,data+len,beginWrite());
		_writeIndex += len;
	}

	void append(const void * data,size_t len)
	{
		append(static_cast<const char*>(data),len);
	}

	void appendInt64(int64_t x)
	{
		int64_t be64 = htonl(x);
		append(&be64,sizeof be64);
	}
	
	void appendInt32(uint32_t x)
	{
		uint32_t be32 = htonl(x);
		append(&be32,sizeof be32);
	}
	
	void appendInt16(uint16_t x)
	{
		uint16_t be16 = htons(x);
		append(&be16,sizeof be16);
	}

	void appenInt8(int8_t x)
	{
		append(&x,sizeof x);
	}

	void prepend(const void* data,size_t len)
	{
		assert(len <= prependableBytes());
		_readerIndex -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d,d+len,begin()+_readerIndex);
	}

	void shrink()
	{
		// use shrink_to_fit for tesr
		_buffer.shrink_to_fit();
	}

	size_t internalCapacity()
	{
		return _buffer.capacity();
	}

	ssize_t readFd(int fd);

	void ensureWritableBytes(size_t len)
	{
		if(writableBytes() < len)
		{
			makeSpace(len);
		}
		assert(writableBytes()>=len);
	}

private:
	char* begin()
	{
		return &*_buffer.begin();
	}

	const char* begin() const 
	{
		return &*_buffer.begin();
	}

	void makeSpace(size_t len)
	{
		if(writableBytes()+prependableBytes() < len+kCheapPrepend)
		{
			_buffer.resize(_writeIndex+len);
		}
		else
		{
			assert(kCheapPrepend < _readerIndex);
			size_t readable = readableBytes();
			std::copy(begin()+_readerIndex,
					begin()+_writeIndex,
					begin()+kCheapPrepend);
			_readerIndex = kCheapPrepend;
			_writeIndex  = _readerIndex + readable;
		}
	}
	
private:
	vector<char> _buffer;
	size_t _readerIndex;
	size_t _writeIndex;

	static const char kCRLF[];
};

#endif
