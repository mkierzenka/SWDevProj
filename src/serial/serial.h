//lang::CwC

#pragma once

#include <unistd.h>
#include "../utils/string.h"
#include "../network/msgkind.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Serializer
{
public:
	const size_t INIT_BUFFER_SIZE_ = 1024;
	char *buffer_; // owned
	size_t numBytesWritten_;
	size_t numBytesRead_;
	size_t capacity_;

	Serializer()
	{
		capacity_ = INIT_BUFFER_SIZE_;
		buffer_ = new char[capacity_];
		memset(buffer_, 0, capacity_);
		numBytesWritten_ = 0;
		numBytesRead_ = 0;
	}

	Serializer(const char *inp) : Serializer(strlen(inp), inp) {}

	Serializer(size_t size, const char *inp)
	{
		capacity_ = size;
		buffer_ = new char[capacity_];
		memset(buffer_, 0, capacity_);
		memcpy(buffer_, inp, capacity_);
		numBytesWritten_ = capacity_;
		numBytesRead_ = 0;
	}

	~Serializer()
	{
		delete[] buffer_;
	}

	void write(double d)
	{
		add_(&d, sizeof(double));
	}

	void write(MsgKind mk)
	{
		char out[2];
		switch (mk)
		{
		case Ack:
			out[0] = '0';
			break;
		case GetData:
			out[0] = '1';
			break;
		case Put:
			out[0] = '2';
			break;
		case ReplyData:
			out[0] = '3';
			break;
		case WaitAndGet:
			out[0] = '4';
			break;
		case Status:
			out[0] = '5';
			break;
		case Kill:
			out[0] = '6';
			break;
		case Register:
			out[0] = '7';
			break;
		case Dir:
			out[0] = '8';
			break;
		default:
			out[0] = 0;
		}
		out[1] = 0;
		add_(out, 2);
	}

	void write(size_t s)
	{
		add_(&s, sizeof(size_t));
	}

	void write(char c)
	{
		add_(&c, sizeof(char));
	}

	/** Calculates the length using strlen*/
	void write(const char *str)
	{
		size_t lenStr = strlen(str); //length of str (up to first null byte)
		add_(str, lenStr + 1);		 //add 1 to include null terminator
	}

	/** sz should include null terminator, if applicable */
	void write(size_t sz, const char *str)
	{
		add_(str, sz);
	}

	void write(short s)
	{
		add_(&s, sizeof(short));
	}

	void write(long l)
	{
		add_(&l, sizeof(long));
	}

	void write(int i)
	{
		add_(&i, sizeof(int));
	}

	void write(bool b)
	{
		add_(&b, sizeof(bool));
	}

	void add_(const void *d, size_t s)
	{
		if ((numBytesWritten_ + s) > capacity_)
		{
			grow_();
		}
		memcpy((buffer_ + numBytesWritten_), d, s);
		numBytesWritten_ += s;
	}

	void grow_()
	{
		capacity_ *= 2;
		char *newBuf = new char[capacity_];
		memmove(newBuf, buffer_, numBytesWritten_);
		delete[] buffer_;
		buffer_ = newBuf;
	}

	char *readString()
	{
		char *startPtr = buffer_ + numBytesRead_;
		size_t lenNextStr = strlen(startPtr); // get the next string (up to first \0)
		size_t bytesNeeded = lenNextStr + 1;  //extra for null terminator
		char *out = new char[bytesNeeded];
		memcpy(out, startPtr, bytesNeeded);
		numBytesRead_ += bytesNeeded;
		return out;
	}

	double readDouble()
	{
		double out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(double));
		numBytesRead_ += sizeof(double);
		return out;
	}

	short readShort()
	{
		short out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(short));
		numBytesRead_ += sizeof(short);
		return out;
	}

	long readLong()
	{
		long out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(long));
		numBytesRead_ += sizeof(long);
		return out;
	}

	int readInt()
	{
		int out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(int));
		numBytesRead_ += sizeof(int);
		return out;
	}

	bool readBool()
	{
		bool out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(bool));
		numBytesRead_ += sizeof(bool);
		return out;
	}

	char readChar()
	{
		char out;
		memcpy(&out, buffer_ + numBytesRead_, sizeof(char));
		numBytesRead_ += sizeof(char);
		return out;
	}

	MsgKind readMsgKind()
	{
		char sMsgKind[2];
		memcpy(sMsgKind, buffer_ + numBytesRead_, 2);
		numBytesRead_ += 2;
		if (strcmp(sMsgKind, "0") == 0)
		{
			return Ack;
		}
		else if (strcmp(sMsgKind, "1") == 0)
		{
			return GetData;
		}
		else if (strcmp(sMsgKind, "2") == 0)
		{
			return Put;
		}
		else if (strcmp(sMsgKind, "3") == 0)
		{
			return ReplyData;
		}
		else if (strcmp(sMsgKind, "4") == 0)
			{
				return WaitAndGet;
			}
			else if (strcmp(sMsgKind, "5") == 0)
			{
				return Status;
			}
			else if (strcmp(sMsgKind, "6") == 0)
			{
				return Kill;
			}
			else if (strcmp(sMsgKind, "7") == 0)
			{
				return Register;
			}
			else if (strcmp(sMsgKind, "8") == 0)
			{
				return Dir;
			}
			else
			{
				return GetData;
			}
		}

		size_t readSizeT()
		{
			size_t out;
			memcpy(&out, buffer_ + numBytesRead_, sizeof(size_t));
			numBytesRead_ += sizeof(size_t);
			return out;
		}

		char *getBuffer()
		{
			return buffer_;
		}

		size_t getNumBytesWritten()
		{
			return numBytesWritten_;
		}

		size_t getNumBytesRead()
		{
			return numBytesRead_;
		}

		char *clear()
		{
			delete[] buffer_;
			buffer_ = new char[capacity_];
			memset(buffer_, 0, capacity_);
			numBytesRead_ = 0;
			numBytesWritten_ = 0;
		}
	};