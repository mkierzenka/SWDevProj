// //lang:Cpp
// #pragma once

// #include "message.h"
// #include "../store/key.h"
// #include "../store/value.h"

// /**
//  * This is the message type that will be sent by a KVStore to put a KV pair in another
//  */
// class PutMsg : public Message {
//     public:
//         Key* key_;
//         Value* value_;

//         PutMsg(Key* k, Value* v, size_t sender, size_t target) : Message(MsgKind::Put, sender, target, 0)
//         {
//             key_ = k;
// 			value_ = v;
//         }

//         ~PutMsg() { }

//         void serialize(Serializer* s)
//         {
//             Message::serialize(s);
//             key_->serialize(s);
//             value_->serialize(s);
//         }

//         void deserialize(Serializer* s) {
//             Message::deserialize(s);
//             key_->deserialize(s);
//             value_->deserialize(s);
//         }

//         Value* getValue() {
//             return value_;
//         }
		
// 		Key* getKey() {
// 			return key_;
// 		}
// };
