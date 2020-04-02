// #pragma once
// //lang::Cpp

// #include "../utils/object.h"
// #include "msgkind.h"
// #include "serial.h"
// #include "message.h"

// class AckMsg : public Message {
// public:
// 	AckMsg() : Message() { }
	
// 	AckMsg(size_t sender, size_t target, size_t id) : Message(Ack, sender, target, id) { }
	
// 	~AckMsg() { }
	
// 	// Inherits from Message
// 	// virtual void serialize(Serializer* s);
	
// 	// Inherits from Message
// 	// virtual void deserialize(Serializer* s);
// };