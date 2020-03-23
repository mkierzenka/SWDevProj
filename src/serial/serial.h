//lang::CwC

#pragma once

#include <unistd.h>
#include "../utils/string.h"
#include "msgkind.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Serializer {
public:
	const size_t BUFFER_SIZE_ = 1024;
	char* buffer_;			// owned
	char* curBuffPtrWrite_;	// just points to a place in buffer_
	char* curBuffPtrRead_;	// just points to a place in buffer_
	size_t numBytesWritten_;
	size_t numBytesRead_;
	
	Serializer() {
		buffer_ = new char[BUFFER_SIZE_];
		memset(buffer_, 0, BUFFER_SIZE_);
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
		numBytesWritten_ = 0;
		numBytesRead_ = 0;
	}
	
	Serializer(const char* inp) {
		buffer_ = new char[strlen(inp)];
		memset(buffer_, 0, strlen(inp));
		memcpy(buffer_, inp, strlen(inp));
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
		numBytesWritten_ = strlen(inp);
		numBytesRead_ = 0;
	}

	Serializer(size_t size, const char* inp) {
		buffer_ = new char[size];
		memset(buffer_, 0, size);
		memcpy(buffer_, inp, size);
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
		numBytesWritten_ = size;
		numBytesRead_ = 0;
	}
	
	~Serializer() {
		// if (numBytesWritten_ != numBytesRead_)
		// {
		// 	printf("Num bytes written does not equal num bytes read\n");
		// 	printf("Num bytes written: %zu\n", numBytesWritten_);
		// 	printf("Num bytes read: %zu\n", numBytesRead_);
		// }

		delete[] buffer_;
	}
	
	void write(double d) {
		memcpy(curBuffPtrWrite_, &d, sizeof(double));
		curBuffPtrWrite_ += sizeof(double);
		numBytesWritten_ += sizeof(double);
	}

	void write(float f) {
		memcpy(curBuffPtrWrite_, &f, sizeof(float));
		curBuffPtrWrite_ += sizeof(float);
		numBytesWritten_ += sizeof(float);
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
		numBytesWritten_ += 2;
	}
	
	void write(size_t s) {
		memcpy(curBuffPtrWrite_, &s, sizeof(size_t));
		curBuffPtrWrite_ += sizeof(size_t);
		numBytesWritten_ += sizeof(size_t);
	}
	
	void write(char c) {
		memcpy(curBuffPtrWrite_, &c, sizeof(char));
		curBuffPtrWrite_ += sizeof(char);
		numBytesWritten_ += sizeof(char);
	}

	/** Calculates the length using strlen*/
	void write(const char* str) {
		//issue with strlen if null byte in middle of string?
		size_t lenStr = strlen(str); //length of str
		memcpy(curBuffPtrWrite_, str, lenStr + 1); //add 1 to also copy null terminator
		curBuffPtrWrite_ += lenStr+1;
		numBytesWritten_ += lenStr+1;
	}

	void write(short s) {
		memcpy(curBuffPtrWrite_, &s, sizeof(s));
		curBuffPtrWrite_ += sizeof(s);
		numBytesWritten_ += sizeof(s);
	}

	void write(long l) {
		memcpy(curBuffPtrWrite_, &l, sizeof(l));
		curBuffPtrWrite_ += sizeof(l);
		numBytesWritten_ += sizeof(l);
	}

	void write(int i) {
		memcpy(curBuffPtrWrite_, &i, sizeof(i));
		curBuffPtrWrite_ += sizeof(i);
		numBytesWritten_ += sizeof(i);
	}
	
	void write(bool b) {
		memcpy(curBuffPtrWrite_, &b, sizeof(b));
		curBuffPtrWrite_ += sizeof(b);
		numBytesWritten_ += sizeof(b);
	}

	char* readString() {
		size_t lenNextStr = strlen(curBuffPtrRead_); // get the next string (up to first \0)
		char* out = new char[lenNextStr];
		memcpy(out, curBuffPtrRead_, lenNextStr+1);
		curBuffPtrRead_ += lenNextStr+1; //skip null terminator
		numBytesRead_ += lenNextStr+1;
		return out;
	}
	
	double readDouble() {
		double out;
		memcpy(&out, curBuffPtrRead_, sizeof(double));
		curBuffPtrRead_ += sizeof(double);
		numBytesRead_ += sizeof(double);
		return out;
	}

	float readFloat() {
		float out;
		memcpy(&out, curBuffPtrRead_, sizeof(float));
		curBuffPtrRead_ += sizeof(float);
		numBytesRead_ += sizeof(float);
		return out;
	}

	short readShort() {
		short out;
		memcpy(&out, curBuffPtrRead_, sizeof(short));
		curBuffPtrRead_ += sizeof(short);
		numBytesRead_ += sizeof(short);
		return out;
	}

	long readLong() {
		long out;
		memcpy(&out, curBuffPtrRead_, sizeof(long));
		curBuffPtrRead_ += sizeof(long);
		numBytesRead_ += sizeof(long);
		return out;
	}

	int readInt() {
		int out;
		memcpy(&out, curBuffPtrRead_, sizeof(int));
		curBuffPtrRead_ += sizeof(int);
		numBytesRead_ += sizeof(int);
		return out;
	}

	bool readBool() {
		bool out;
		memcpy(&out, curBuffPtrRead_, sizeof(bool));
		curBuffPtrRead_ += sizeof(bool);
		numBytesRead_ += sizeof(bool);
		return out;
	}

	char readChar() {
		char out;
		memcpy(&out, curBuffPtrRead_, sizeof(char));
		curBuffPtrRead_ += sizeof(char);
		numBytesRead_ += sizeof(char);
		return out;
	}
	
	MsgKind readMsgKind() {
		char sMsgKind[2];
		memcpy(sMsgKind, curBuffPtrRead_, 2);
		curBuffPtrRead_ += 2;
		numBytesRead_ += 2;
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
		numBytesRead_ += sizeof(size_t);
		return out;
	}
	
	char* getBuffer() {
		return buffer_;
	}
	
	size_t getNumBytesWritten() {
		return numBytesWritten_;
	}

	size_t getNumBytesRead() {
		return numBytesRead_;
	}
	
	char* clear() {
		delete[] buffer_;
		buffer_ = new char[BUFFER_SIZE_];
		memset(buffer_, 0, BUFFER_SIZE_);
		curBuffPtrWrite_ = buffer_;
		curBuffPtrRead_ = buffer_;
	}

};