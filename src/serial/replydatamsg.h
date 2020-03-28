//lang:Cpp
#include "message.h"
#include "../store/key.h"
#include "../store/value.h"

/**
 * This is the message type that will be sent by a KVStore in response to a GetData Message
 */
class ReplyDataMsg : public Message {
    public:
        //key to be sent in message
        Value* value_;

        ReplyDataMsg(Value* v, size_t sender, size_t target) : Message(MsgKind::ReplyData, sender, target, 0)
        {
            value_ = v;
        }

        ~ReplyDataMsg() { }

        void serialize(Serializer* s)
        {
            Message::serialize(s);
            value_->serialize(s);
        }

        void deserialize(Serializer* s) {
            Message::deserialize(s);
            value_->deserialize(s);
        }

        Value* getValue() {
            return value_;
        }
};
