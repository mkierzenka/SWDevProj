#pragma once
//lang::Cpp

#include "object.h"
#include "stringarray.h"
#include "msgkind.h"
#include "serial.h"
#include "message.h"

class DirectoryMsg : public Message {
public:
	size_t client_;
	size_t port_;
	StringArray* ipList_;
	
	
	DirectoryMsg() : Message() {
		ipList_ = new StringArray();
	}
	
	DirectoryMsg(size_t client, size_t port, size_t sender, size_t target, size_t id) : Message(Directory, sender, target, id) {
		client_ = client;
		port_ = port;
		ipList_ = new StringArray();
	}
	
	~DirectoryMsg() { }
	
	virtual void serialize(Serializer* s) {
		Message::serialize(s);
		ipList_->serialize(s);
		s->write(client_);
		s->write(port_);
	}
	
	virtual void deserialize(Serializer* s) {
		Message::deserialize(s);
		ipList_->deserialize(s);
		client_ = s->readSizeT();
		port_ = s->readSizeT();
	}
	
	void addIp(String* ip) {
		ipList_->add(ip);
	}
};