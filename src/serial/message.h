#pragma once
//lang::Cpp

#include "../utils/object.h"
#include "msgkind.h"
#include "serial.h"

/**
 * Stub class representing a TCP message. For Serializer demo.
 *
 *@author kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */
class Message : public Object {
public:
	MsgKind kind_;  // the message kind
    size_t sender_; // the index of the sender node
    size_t target_; // the index of the receiver node
    size_t id_;     // an id t unique within the node

	Message() : Object() {
		kind_ = Ack;
		sender_ = 0;
		target_ = 0;
		id_ = 0;
	}
	
	Message(MsgKind kind, size_t sender, size_t target, size_t id) : Object() {
		kind_ = kind;
		sender_ = sender;
		target_ = target;
		id_ = id;
	}
	
	virtual ~Message() {}
	
	virtual void serialize(Serializer* s) {
		s->write(kind_);
		s->write(sender_);
		s->write(target_);
		s->write(id_);
	}
	
	virtual void deserialize(Serializer* s) {
		kind_ = s->readMsgKind();
		sender_ = s->readSizeT();
		target_ = s->readSizeT();
		id_ = s->readSizeT();
	}

	size_t getSender()
	{
		return sender_;
	}

	size_t getTarget()
	{
		return target_;
	}

	size_t getSender() {
		return sender_;
	}

	MsgKind getKind() {
		return kind_;
	}
};