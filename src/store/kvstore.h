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
#include "../serial/putmsg.h"
#include "../serial/waitandgetmsg.h"


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
        //receivedMsgs_ = new MessageQueue();
    }

    ~KVStore()
    {
        delete kvMap;
    }
	
	void setBackChannel(MessageQueue* mq) {
		receivedMsgs_ = mq;
	}

    /** Puts key-value pair into distributed KV store */
    void put(Key *k, Value *data)
    {
		if (k->getNode() != storeId) {
			PutMsg* msg = new PutMsg(k, data, storeId, k->getNode());
			client_->sendMsg(msg);
			return;
		}
		
		// Add to this store's map
        kvMap->put(k->getKeyStr()->clone(), data->clone());

		// Try to respond to messages in the receivedMsgs_ queue
		tryToHandleQueue_();
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
	
	/**
	 * Go through receivedMsgs_ Queue, respond to any WaitAndGet messages that we can.
	 */
	void tryToHandleQueue_() {
		MessageQueue* tmpQ = new MessageQueue();
		while (receivedMsgs_->size() > 0) {
			Message* m = receivedMsgs_->pop();
			WaitAndGetMsg* wagMsg = dynamic_cast<WaitAndGetMsg *>(m);
			if (wagMsg) {
				size_t sender = wagMsg->getSender();
				Key* k = wagMsg->getKey();
				Value* val = getValue(k);
				if(val) {
					ReplyDataMsg *reply = new ReplyDataMsg(val, storeId, sender);
					client_->sendMsg(reply);
					delete wagMsg;
				} else {
					tmpQ->push(wagMsg);
				}
			} else {
				// Not a WaitAndGetMsg
				tmpQ->push(m);
			}
		}
		while(tmpQ->size() > 0) {
			receivedMsgs_->push(tmpQ->pop());
		}
		delete tmpQ;		
	}
};