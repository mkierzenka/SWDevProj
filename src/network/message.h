#pragma once
//lang::Cpp

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include "stringarray.h"
#include "../store/key.h"
#include "../store/value.h"
#include "../network/directory.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	Key* key_;

	ReplyDataMsg() : Message() {
		key_ = new Key();
		value_ = new Value();
	}

	ReplyDataMsg(Key* k, Value *v, size_t sender, size_t target) : Message(ReplyData, sender, target, 0)
	{
		value_ = v->clone();
		key_ = k->clone();
	}

	~ReplyDataMsg() {
		delete key_;
		delete value_;
	}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
		value_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		delete key_;
		key_ = new Key();
		key_->deserialize(s);
		delete value_;
		value_ = new Value();
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

	/**
	 * Check if this reply message equals the given one
	 */
	bool equals(ReplyDataMsg* other)
	{
		if (this == other)
		{
			return true;
		}

		return (value_->equals(other->getValue())) && (key_->equals(other->getKey()));
	}
};

/**
 * This is the message type that will be sent by a KVStore to request data when
 * an Application calls waitAndGet.
 */
class WaitAndGetMsg : public Message
{
public:
	Key *key_;

	WaitAndGetMsg() : Message()
	{
		key_ = new Key();
	}

	WaitAndGetMsg(Key *k, size_t sender, size_t target) : Message(WaitAndGet, sender, target, 0)
	{
		key_ = k->clone();
	}

	~WaitAndGetMsg() {
		delete key_;
	}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		delete key_;
		key_ = new Key();
		key_->deserialize(s);
	}

	/** Return key from this message */
	Key *getKey()
	{
		return key_;
	}
};

class StatusMsg : public Message
{
public:
	String *msg_; //owned

	StatusMsg() : Message()
	{
		msg_ = new String("");
	}

	StatusMsg(String *msg, size_t sender, size_t target, size_t id) : Message(Status, sender, target, id)
	{
		msg_ = new String(*msg);
	}

	~StatusMsg()
	{
		delete msg_;
	}

	// Inherits from Message
	virtual void serialize(Serializer *s)
	{
		Message::serialize(s);
		msg_->serialize(s);
	}

	// Inherits from Message
	virtual void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		msg_->deserialize(s);
	}
};

/** This message sends a list of connected nodes */
class DirectoryMsg : public Message
{
public:
	//size_t client_;
	size_t port_;
	//StringArray *ipList_;
	Directory* dir_; //keep track of clients

	DirectoryMsg() : Message()
	{
		//ipList_ = new StringArray();
		dir_ = new Directory();
	}

	DirectoryMsg(Directory* dir, size_t port, size_t sender, size_t target, size_t id) : Message(Dir, sender, target, id)
	{
		//client_ = client;
		dir_ = dir->clone();
		port_ = port;
		//ipList_ = new StringArray();
	}

	~DirectoryMsg() {
		delete dir_;
	}

	virtual void serialize(Serializer *s)
	{
		Message::serialize(s);
		//ipList_->serialize(s);
		//s->write(client_);
		dir_->serialize(s);
		s->write(port_);
	}

	virtual void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		//ipList_->deserialize(s);
		//client_ = s->readSizeT();
		delete dir_;
		dir_ = new Directory();
		dir_->deserialize(s);
		port_ = s->readSizeT();
	}

	void addIp(size_t node, String *ip)
	{
		//ipList_->add(ip);
		dir_->addIp(node, ip);
	}

	Directory* getDirectory()
	{
		return dir_;
	}
};

/** Message to register a new node */
class RegisterMsg : public Message
{
public:
	//struct sockaddr_in client_;
	String* client_; //ip address of new client; owned
	size_t port_;

	RegisterMsg() : Message()
	{
		// client_.sin_family = AF_INET;
		// inet_pton(AF_INET, "127.0.0.1", &client_.sin_addr);
		// client_.sin_port = htons(8080);
		port_ = 8080;
		client_ = new String("127.0.0.1");
	}

	RegisterMsg(String* client, size_t port, size_t sender, size_t target, size_t id) : Message(Register, sender, target, id)
	{
		client_ = client->clone();
		port_ = port;
	}

	RegisterMsg(char* client, size_t port, size_t sender, size_t target, size_t id) : Message(Register, sender, target, id)
	{
		client_ = new String(client);
		port_ = port;
	}

	~RegisterMsg()
	{
		delete client_;
	}

	virtual void serialize(Serializer *s)
	{
		Message::serialize(s);
		// s->write((short)client_.sin_family);
		// s->write((long)client_.sin_addr.s_addr);
		// s->write((short)client_.sin_port);
		client_->serialize(s);
		s->write(port_);
	}

	virtual void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		delete client_;
		client_ = new String("");
		client_->deserialize(s);
		// client_.sin_family = s->readShort();
		// client_.sin_addr.s_addr = s->readLong();
		// client_.sin_port = s->readShort();
		port_ = s->readSizeT();
	}

	String* getClient() {
		return client_;
	}

	size_t getPort() {
		return port_;
	}
};

/**
 * This is the message type that will be sent by a KVStore to put a KV pair into another
 */
class PutMsg : public Message
{
public:
	Key *key_;       // owned
	Value *value_;   // owned

	PutMsg() : Message() {
		key_ = new Key();
		value_ = new Value();
	}

	PutMsg(Key *k, Value *v, size_t sender, size_t target) : Message(Put, sender, target, 0)
	{
		key_ = k->clone();
		value_ = v->clone();
	}

	~PutMsg() {
		delete key_;
		delete value_;
	}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
		value_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		delete key_;
		key_ = new Key();
		key_->deserialize(s);
		delete value_;
		value_ = new Value();
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
	Key *key_;

	GetDataMsg() : Message()
	{
		key_ = new Key();
	}
	GetDataMsg(Key *k, size_t sender, size_t target) : Message(GetData, sender, target, 0)
	{
		key_ = k->clone();
	}

	~GetDataMsg() {
		delete key_;
	}

	void serialize(Serializer *s)
	{
		Message::serialize(s);
		key_->serialize(s);
	}

	void deserialize(Serializer *s)
	{
		Message::deserialize(s);
		delete key_;
		key_ = new Key();
		key_->deserialize(s);
	}

	/** Return key from this message */
	Key *getKey()
	{
		return key_;
	}
};

/** Message informing the receiver that the sender is ready for shutdown */
class DoneMsg : public Message
{
public:
	DoneMsg() : Message() {}

	DoneMsg(size_t sender, size_t target, size_t id) : Message(Done, sender, target, id) {}

	~DoneMsg() {}
};


/** Message acknowledges that a message was received */
class AckMsg : public Message
{
public:
	AckMsg() : Message() {}

	AckMsg(size_t sender, size_t target, size_t id) : Message(Ack, sender, target, id) {}

	~AckMsg() {}
};

/** Message sent by server to initiate teardown of nodes */
class TeardownMsg : public Message
{
	public:
	TeardownMsg() : Message() {}

	TeardownMsg(size_t sender, size_t target, size_t id) : Message(Teardown, sender, target, id) {}

	~TeardownMsg() {}
};
