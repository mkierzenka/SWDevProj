#pragma once
//lang::Cpp

#include "object.h"
#include "msgkind.h"
#include "serial.h"
#include "message.h"

class StatusMsg : public Message {
public:
	String* msg_; //owned

	StatusMsg() : Message() {
		msg_ = new String("");
	 }
	
	StatusMsg(String* msg, size_t sender, size_t target, size_t id) : Message(Status, sender, target, id) {
		msg_ = new String(*msg);
	 }
	
	~StatusMsg() {
		delete msg_;
	 }
	
	// Inherits from Message
	virtual void serialize(Serializer* s) {
		Message::serialize(s);
		msg_->serialize(s);
	}
	
	// Inherits from Message
	virtual void deserialize(Serializer* s) {
		Message::deserialize(s);
		msg_->deserialize(s);
	}
};