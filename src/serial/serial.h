//lang::CwC

#pragma once

#include <unistd.h>
#include "../utils/object.h"
#include "../utils/string.h"
#include "msgkind.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Serializer: public Object {
public:
	const size_t BUFFER_SIZE_ = 1024;
	char* buffer_;			// owned
	char* curBuffPtrWrite_;	// just points to a place in buffer_
	char* curBuffPtrRead_;	// just points to a place in buffer_
	
	Serializer() : Object() {
		buffer_ = new char[BUFFER_SIZE_];
		memset(buffer_, 0, BUFFER_SIZE_);
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
	}
	
	Serializer(const char* inp) : Object() {
		buffer_ = new char[strlen(inp)];
		memset(buffer_, 0, strlen(inp));
		memcpy(buffer_, inp, strlen(inp));
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
	}
	
	~Serializer() {
		delete[] buffer_;
	}
	
	void write(double d) {
		memcpy(curBuffPtrWrite_, &d, sizeof(double));
		curBuffPtrWrite_ += sizeof(double);
	}

	void write(MsgKind mk) {
		char out[2];
		switch (mk) {
			case Ack:
				out[0]='0';
				break;
			case Nack:
				out[0]='1';
				break;
			case Put:
				out[0]='2';
				break;
			case Reply:
				out[0]='3';
				break;
			case Get:
				out[0]='4';
				break;
			case WaitAndGet:
				out[0]='5';
				break;
			case Status:
				out[0]='6';
				break;
			case Kill:
				out[0]='7';
				break;
			case Register:
				out[0]='8';
				break;
			case Directory:
				out[0]='9';
				break;
			default:
				out[0]=0;
		}
		out[1] = 0;
		memcpy(curBuffPtrWrite_, &out, 2);
		curBuffPtrWrite_ += 2;
	}
	
	void write(size_t s) {
		memcpy(curBuffPtrWrite_, &s, sizeof(size_t));
		curBuffPtrWrite_ += sizeof(size_t);
	}
	
	void write(const char* str) {
		size_t lenStr = strlen(str); //length of str
		memcpy(curBuffPtrWrite_, str, lenStr + 1); //add 1 to also copy null terminator
		curBuffPtrWrite_ += lenStr+1;
	}

	void write(short s) {
		memcpy(curBuffPtrWrite_, &s, sizeof(s));
		curBuffPtrWrite_ += sizeof(s);
	}

	void write(long l) {
		memcpy(curBuffPtrWrite_, &l, sizeof(l));
		curBuffPtrWrite_ += sizeof(l);
	}
	
	char* readString() {
		size_t lenNextStr = strlen(curBuffPtrRead_); // get the next string (up to first \0)
		char* out = new char[lenNextStr];
		memcpy(out, curBuffPtrRead_, lenNextStr);
		curBuffPtrRead_ += lenNextStr;
		curBuffPtrRead_++; //skip null terminator
		return out;
	}
	
	double readDouble() {
		double out;
		memcpy(&out, curBuffPtrRead_, sizeof(double));
		curBuffPtrRead_ += sizeof(double);
		return out;
	}

	short readShort() {
		short out;
		memcpy(&out, curBuffPtrRead_, sizeof(short));
		curBuffPtrRead_ += sizeof(short);
		return out;
	}

	long readLong() {
		long out;
		memcpy(&out, curBuffPtrRead_, sizeof(long));
		curBuffPtrRead_ += sizeof(long);
		return out;
	}
	
	MsgKind readMsgKind() {
		char sMsgKind[2];
		memcpy(sMsgKind, curBuffPtrRead_, 2);
		curBuffPtrRead_ += 2;
		if (strcmp(sMsgKind, "0") == 0) {
			return Ack;
		} else if (strcmp(sMsgKind, "1") == 0) {
			return Nack;
		} else if (strcmp(sMsgKind, "2") == 0) {
			return Put;
		} else if (strcmp(sMsgKind, "3") == 0) {
			return Reply;
		} else if (strcmp(sMsgKind, "4") == 0) {
			return Get;
		} else if (strcmp(sMsgKind, "5") == 0) {
			return WaitAndGet;
		} else if (strcmp(sMsgKind, "6") == 0) {
			return Status;
		} else if (strcmp(sMsgKind, "7") == 0) {
			return Kill;
		} else if (strcmp(sMsgKind, "8") == 0) {
			return Register;
		} else if (strcmp(sMsgKind, "9") == 0) {
			return Directory;
		} else {
			return Nack;
		}
	}
	
	size_t readSizeT() {
		size_t out;
		memcpy(&out, curBuffPtrRead_, sizeof(size_t));
		curBuffPtrRead_ += sizeof(size_t);
		return out;
	}
	
	char* getBuffer() {
		return buffer_;
	}
	
	char* clear() {
		delete[] buffer_;
		buffer_ = new char[BUFFER_SIZE_];
		memset(buffer_, 0, BUFFER_SIZE_);
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
	}

};