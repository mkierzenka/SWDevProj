#pragma once

#include "../utils/object.h"
#include "../network/client.h"
#include "../network/pseudo/pseudonetwork.h"
#include "../serial/serial.h"
#include "../serial/message.h"
#include "../serial/getdatamsg.h"
#include "../serial/replydatamsg.h"
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
    MapStrObj *kvMap; //holds all key-value pairings
    //Client *client_;     //networking class used to talk to other stores
    PseudoNetwork* client_; //fake network for demo
    size_t storeId; //node id that this store belongs to
    MessageQueue* receivedMsgs_;


    KVStore(size_t id, PseudoNetwork* client)
    {
        kvMap = new MapStrObj();
        storeId = id;
        client_ = client; //for demo, all need same object
        receivedMsgs_ = new MessageQueue();
    }

    ~KVStore()
    {
        delete kvMap;
    }

    /** Puts key-value pair into map. Since it is stored locally, don't need to include store id.
     * Will clone the data */
    void put(Key *k, Value *data)
    {
        //make sure adding key to right node
        if (k->getNode() != storeId)
        {
            fprintf(stderr, "Attempting to put Key with node %zu into store for node %zu", k->getNode(), storeId);
        }

        kvMap->put(k->getKeyStr()->clone(), data->clone());
    }

    DataFrame *get(Key *k);

    DataFrame* waitAndGet(Key* k);

    /** Get the actual Value that the given key maps to */
    Value *getValue(Key *k)
    {
        Value* val = dynamic_cast<Value *>(kvMap->get(k->getKeyStr()));
        if (val == nullptr) {
            // don't have the value locally
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
        while (receivedMsgs_->size() == 0) {
            //wait until ReceiverThread has something for me
        }
        //ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(client_->receiveMsg(storeId)); //blocks until received
        ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(receivedMsgs_->pop());
        assert(dataMsg != nullptr);
        Value *val = dataMsg->getValue();
        return val;
    }
};