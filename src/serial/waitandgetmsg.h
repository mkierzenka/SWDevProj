//lang:Cpp

#pragma once
#include "message.h"
#include "../store/key.h"

/**
 * This is the message type that will be sent by a KVStore to request data for
 * a specific key. The application thread is blocked, waiting for a reply.
 */
class WaitAndGetMsg : public Message {
    public:
        Key* key_;

        WaitAndGetMsg(Key* k, size_t sender, size_t target) : Message(MsgKind::WaitAndGet, sender, target, 0)
        {
            key_ = k;
        }

        ~WaitAndGetMsg() { }

        void serialize(Serializer* s)
        {
            Message::serialize(s);
            key_->serialize(s);
        }

        void deserialize(Serializer* s) {
            Message::deserialize(s);
            key_->deserialize(s);
        }

        /** Return key from this message */
        Key* getKey()
        {
            return key_;
        }
};

