#pragma once

#include "../utils/object.h"
#include "../network/client.h"
#include "../network/pseudo/pseudonetwork.h"
#include "../serial/serial.h"
#include "../serial/message.h"
#include "../utils/map.h"
#include "key.h"
#include "value.h"
#include "../network/pseudo/messagequeue.h"

class DataFrame;

/** This class represents our key-value store. It will utilize a Map object to hold the
 * key-value pairings. It will also have networking capabilities to interact with other 
 * stores. 
 * 
 * @authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu 
 */
class KVStore : public Object
{
public:
    MapStrObj *kvMap;              //holds all key-value pairings
    //Client *client_;             //networking class used to talk to other stores
    PseudoNetwork* client_;        //fake network for demo
    size_t storeId;                //node id that this store belongs to
    MessageQueue* receivedMsgs_;   //for our ReceiverThread to pass msgs to this store


    KVStore(size_t id, PseudoNetwork* client)
    {
        kvMap = new MapStrObj();
        storeId = id;
        client_ = client;
        receivedMsgs_ = new MessageQueue();
    }

    ~KVStore()
    {
        delete kvMap;
    }

    /** Puts key-value pair into distributed KV store */
    void put(Key *k, Value *data)
    {
		PutMsg* msg = new PutMsg(k, data, storeId, k->getNode());
		client_->sendMsg(msg);
    }
	
	/** Puts key-value pair into local map. Will clone the data */
    void putLocal(Key *k, Value *data)
    {
        assert(k->getNode() == storeId);
		size_t tmp = kvMap->size();
        kvMap->put(k->getKeyStr()->clone(), data->clone());
		assert(kvMap->size() == tmp + 1);
    }

    DataFrame *get(Key *k);

    DataFrame* waitAndGet(Key* k);

    /** Get the actual Value that the given key maps to */
    Value *getValue(Key *k)
    {
        Value* val = nullptr;
        if (k->getNode() == storeId) {
            val = dynamic_cast<Value *>(kvMap->get(k->getKeyStr()));
        } else {
            val = getFromNetwork_(k);
        }

        return val;
    }

    /** Check if two distributed arrats equal */
    // bool equals(Object *other)
    // {
    //     if (this == other)
    //     {
    //         return true;
    //     }

    //     KVStore *kv = dynamic_cast<KVStore *>(other);

    //     if (kv == nullptr || storeId != kv->storeId || !(kvMap->equals(kv->kvMap)))
    //     {
    //         return false;
    //     }

    //     return true;
    // }

    // /** Compute hash code of this column */
    // size_t hash_me_()
    // {
    //     size_t hash_ = 0;
    //     hash_ += storeId;
    //     hash_ += reinterpret_cast<size_t>(kvMap);

    //     return hash_;
    // }

    Value* getFromNetwork_(Key* k) {
        GetDataMsg *dm = new GetDataMsg(k, storeId, k->getNode());
        client_->sendMsg(dm);
        ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(receivedMsgs_->pop());
		// ^^ Blocks until the message is ready for this store
        assert(dataMsg != nullptr);
        Value *val = dataMsg->getValue();
        return val;
    }
};