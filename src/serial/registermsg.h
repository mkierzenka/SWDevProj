#pragma once
//lang::Cpp

#include "object.h"
#include "string.h"
#include "msgkind.h"
#include "serial.h"
#include "message.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class RegisterMsg : public Message {
public:
	struct sockaddr_in client_;
	size_t port_;
	
	RegisterMsg() : Message() {
		client_.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &client_.sin_addr);
		client_.sin_port = htons(8080);
		port_ = 8080;
	}
	
	RegisterMsg(sockaddr_in client, size_t port, size_t sender, size_t target, size_t id) : Message(Register, sender, target, id) {
		client_ = client;
		port_ = port;
	}
	
	~RegisterMsg() {
	
	}
	
	virtual void serialize(Serializer* s) {
		Message::serialize(s);
		s->write((short)client_.sin_family);
		s->write((long)client_.sin_addr.s_addr);
		s->write((short)client_.sin_port);
		s->write(port_);
	}
	
	virtual void deserialize(Serializer* s) {
		Message::deserialize(s);
		client_.sin_family = s->readShort();
		client_.sin_addr.s_addr = s->readLong();
		client_.sin_port = s->readShort();
		port_ = s->readSizeT();
	}
};