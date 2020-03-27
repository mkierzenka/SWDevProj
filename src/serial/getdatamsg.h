//lang:Cpp
#include "message.h"
#include "../store/key.h"

/**
 * This is the message type that will be sent by a KVStore to request data for a specific key
 */
class GetDataMsg : public Message {
    public:
        //key to be sent in message
        Key* key_;

        GetDataMsg(Key* k, size_t sender, size_t target) : Message(MsgKind::GetData, sender, target, 0)
        {
            key_ = k;
        }

        ~GetDataMsg() { }

        void serialize(Serializer* s)
        {
            Message::serialize(s);
            key_->serialize(s);
        }

        void deserialize(Serializer* s) {
            Message::deserialize(s);
            key_->deserialize(s);
        }
};
