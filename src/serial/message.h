#pragma once
//lang::Cpp

#include "../utils/object.h"
#include "../utils/string.h"
#include "serial.h"
#include "stringarray.h"
#include "../store/key.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Serializer;

/** Enum describing different message types available */
enum MsgKind
{
	Ack,
	GetData,
	Put,
	ReplyData,
	Get,
	WaitAndGet,
	Status,
	Kill,
	Register,
	Directory
};

/**
 * Stub class representing a TCP message. For Serializer demo.
 *
 *@author kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */
class Message : public Object
{
public:
	MsgKind kind_;	// the message kind
	size_t sender_; // the index of the sender node
	size_t target_; // the index of the receiver node
	size_t id_;		// an id t unique within the node

	Message() : Object()
	{
		kind_ = Ack;
		sender_ = 0;
		target_ = 0;
		id_ = 0;
	}

	Message(MsgKind kind, size_t sender, size_t target, size_t id) : Object()
	{
		kind_ = kind;
		sender_ = sender;
		target_ = target;
		id_ = id;
	}

	virtual ~Message() {}

	virtual void serialize(Serializer *s)
	{
		s->write(kind_);
		s->write(sender_);
		s->write(target_);
		s->write(id_);
	}

	virtual void deserialize(Serializer *s)
	{
		kind_ = s->readMsgKind();
		sender_ = s->readSizeT();
		target_ = s->readSizeT();
		id_ = s->readSizeT();
	}

	size_t getTarget()
	{
		return target_;
	}

	size_t getSender()
	{
		return sender_;
	}

	MsgKind getKind()
	{
		return kind_;
	}
};

/**
 * This is the message type that will be sent by a KVStore in response to a GetData Message
 */
class ReplyDataMsg : public Message
{
public:
	//key to be sent in message
	Value *value_;

	ReplyDataMsg(Value *v, size_t sender, size_t target) : Message(MsgKind::ReplyData, sender, target, 0)
	{
		value_ = v;
	}

	~ReplyDataMsg() {}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		value_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		value_->deserialize(s);
	}

	Value *getValue()
	{
		return value_;
	}
};

/**
 * This is the message type that will be sent by a KVStore to request data for
 * a specific key. The application thread is blocked, waiting for a reply.
 */
class WaitAndGetMsg : public Message
{
public:
	Key *key_;

	WaitAndGetMsg(Key *k, size_t sender, size_t target) : Message(MsgKind::WaitAndGet, sender, target, 0)
	{
		key_ = k;
	}

	~WaitAndGetMsg() {}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		key_->deserialize(s);
	}

	/** Return key from this message */
	Key *getKey()
	{
		return key_;
	}
};

// class StatusMsg : public Message
// {
// public:
// 	String *msg_; //owned

// 	StatusMsg() : Message()
// 	{
// 		msg_ = new String("");
// 	}

// 	StatusMsg(String *msg, size_t sender, size_t target, size_t id) : Message(Status, sender, target, id)
// 	{
// 		msg_ = new String(*msg);
// 	}

// 	~StatusMsg()
// 	{
// 		delete msg_;
// 	}

// 	// Inherits from Message
// 	virtual void serialize(Serializer *s)
// 	{
// 		Message::serialize(s);
// 		msg_->serialize(s);
// 	}

// 	// Inherits from Message
// 	virtual void deserialize(Serializer *s)
// 	{
// 		Message::deserialize(s);
// 		msg_->deserialize(s);
// 	}
// };

// /** This message sends a list of connected nodes */
// class DirectoryMsg : public Message
// {
// public:
// 	size_t client_;
// 	size_t port_;
// 	StringArray *ipList_;

// 	DirectoryMsg() : Message()
// 	{
// 		ipList_ = new StringArray();
// 	}

// 	DirectoryMsg(size_t client, size_t port, size_t sender, size_t target, size_t id) : Message(Directory, sender, target, id)
// 	{
// 		client_ = client;
// 		port_ = port;
// 		ipList_ = new StringArray();
// 	}

// 	~DirectoryMsg() {}

// 	virtual void serialize(Serializer *s)
// 	{
// 		Message::serialize(s);
// 		ipList_->serialize(s);
// 		s->write(client_);
// 		s->write(port_);
// 	}

// 	virtual void deserialize(Serializer *s)
// 	{
// 		Message::deserialize(s);
// 		ipList_->deserialize(s);
// 		client_ = s->readSizeT();
// 		port_ = s->readSizeT();
// 	}

// 	void addIp(String *ip)
// 	{
// 		ipList_->add(ip);
// 	}
// };

// /** Message to register a new node */
// class RegisterMsg : public Message
// {
// public:
// 	struct sockaddr_in client_;
// 	size_t port_;

// 	RegisterMsg() : Message()
// 	{
// 		client_.sin_family = AF_INET;
// 		inet_pton(AF_INET, "127.0.0.1", &client_.sin_addr);
// 		client_.sin_port = htons(8080);
// 		port_ = 8080;
// 	}

// 	RegisterMsg(sockaddr_in client, size_t port, size_t sender, size_t target, size_t id) : Message(Register, sender, target, id)
// 	{
// 		client_ = client;
// 		port_ = port;
// 	}

// 	~RegisterMsg()
// 	{
// 	}

// 	virtual void serialize(Serializer *s)
// 	{
// 		Message::serialize(s);
// 		s->write((short)client_.sin_family);
// 		s->write((long)client_.sin_addr.s_addr);
// 		s->write((short)client_.sin_port);
// 		s->write(port_);
// 	}

// 	virtual void deserialize(Serializer *s)
// 	{
// 		Message::deserialize(s);
// 		client_.sin_family = s->readShort();
// 		client_.sin_addr.s_addr = s->readLong();
// 		client_.sin_port = s->readShort();
// 		port_ = s->readSizeT();
// 	}
// };

/**
 * This is the message type that will be sent by a KVStore to put a KV pair in another
 */
class PutMsg : public Message
{
public:
	Key *key_;
	Value *value_;

	PutMsg(Key *k, Value *v, size_t sender, size_t target) : Message(MsgKind::Put, sender, target, 0)
	{
		key_ = k;
		value_ = v;
	}

	~PutMsg() {}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
		value_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		key_->deserialize(s);
		value_->deserialize(s);
	}

	Value *getValue()
	{
		return value_;
	}

	Key *getKey()
	{
		return key_;
	}
};

/**
 * This is the message type that will be sent by a KVStore to request data for a specific key
 */
class GetDataMsg : public Message
{
public:
	//key to be sent in message
	Key *key_;

	GetDataMsg(Key *k, size_t sender, size_t target) : Message(MsgKind::GetData, sender, target, 0)
	{
		key_ = k;
	}

	~GetDataMsg() {}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		key_->deserialize(s);
	}

	/** Return key from this message */
	Key *getKey()
	{
		return key_;
	}
};

// /** Message acknowledges that a message was received */
// class AckMsg : public Message
// {
// public:
// 	AckMsg() : Message() {}

// 	AckMsg(size_t sender, size_t target, size_t id) : Message(Ack, sender, target, id) {}

// 	~AckMsg() {}

// 	// Inherits from Message
// 	// virtual void serialize(Serializer* s);

// 	// Inherits from Message
// 	// virtual void deserialize(Serializer* s);
// };
