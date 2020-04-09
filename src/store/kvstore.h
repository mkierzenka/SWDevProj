#pragma once

#include "../utils/object.h"
#include "../network/inetwork.h"
#include "../serial/serial.h"
#include "../network/message.h"
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
    MapStrObj *kvMap;       // Owned, holds all key-value pairings
    INetwork* node_;        // Not owned, object used for Network communication
    size_t storeId;         // Node id that this store belongs to
    Map* msgCache_;         // WaitAndGet msgs we can't answer yet and ReplyData msgs we just got
    Lock msgCacheLock_;     // Owned, controls access to msgCache


	/**
	 * Sets up a new KVStore with given node number and Network object
	 * Network object is external, caller responsible for deleting it.
	 */
    KVStore(size_t id, INetwork* net)
    {
        kvMap = new MapStrObj();
        storeId = id;
        node_ = net;
        msgCache_ = new Map();
		
    }

    ~KVStore()
    {
        delete kvMap;
        delete msgCache_;
    }

    /**
     * Puts this key-value pair into the KV store.
     * Clones the args before working with them. Caller responsible
	 * for deleting k and data.
     */
    void put(Key *k, Value *data)
    {
		if (k->getNode() != storeId) {
			PutMsg* msg = new PutMsg(k, data, storeId, k->getNode());
			node_->sendMsg(msg);
			return;
		}
        
        Object* putRes = kvMap->put(k->getKeyStr()->clone(), data->clone());
        if (putRes) delete putRes;
		tryToHandleCache_(k);
    }

    DataFrame *get(Key *k);

    DataFrame* waitAndGet(Key* k);

    /**
     * Get the actual Value that the given key maps to (no clone).
     * Blocking - will call to network as needed.
     */
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

    /**
     * Adds a message another KVStore is waiting on us to reply to
	 * Does not clone, steals ownership.
     */
    void addMsgWaitingOn(WaitAndGetMsg* msg) {
        addToCache_(msg->getKey()->clone(), msg);
    }
	
    /**
     * Our ReceiverThread got a response for a key we requested.
	 * Does not clone, steals ownership.
     */
    void addReply(ReplyDataMsg* msg) {
        addToCache_(msg->getKey()->clone(), msg);
    }

    /** Check if two distributed arrats equal - do not use */
    bool equals(Object *other)
    {
        assert(false);
    }

    /** Compute hash code of this column - do not use */
    size_t hash_me_()
    {
        assert(false);
    }

    /* Waits on ReceiverThread (external to this store) to get the response. */
	// blocking
    Value* getFromNetwork_(Key* k) {
        WaitAndGetMsg *dm = new WaitAndGetMsg(k, storeId, k->getNode());
        node_->sendMsg(dm);
        msgCacheLock_.lock();
		while (!msgCache_->contains_key(k))
        {
            msgCacheLock_.wait();
        }
        // ... now msgCache has what we are looking for
        ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(msgCache_->remove(k));
        msgCacheLock_.unlock();
        assert(dataMsg);
        Value *val = dataMsg->getValue();
        return val;
    }
	
	/**
	 * Given that we just added the k-v pair to the store, see if we can deal
	 *   with any messages.
	 */
	void tryToHandleCache_(Key* k) {
		msgCacheLock_.lock();
		if (msgCache_->contains_key(k)) {
			Object* res = msgCache_->remove(k);
			WaitAndGetMsg* wagMsg = dynamic_cast<WaitAndGetMsg*>(res);
			assert(wagMsg);
			size_t sender = wagMsg->getSender();
			Value* val = getValue(k); //should be local, we just added it in kv.put()
			ReplyDataMsg *reply = new ReplyDataMsg(k, val, storeId, sender);
			node_->sendMsg(reply);
			delete wagMsg;
		}
		msgCacheLock_.unlock();
	}

	/** Adds a KV pair (Key, Message) to local cache */
	void addToCache_(Key* k, Message* msg) {
        msgCacheLock_.lock();
        msgCache_->put(k, msg);
        msgCacheLock_.notify_all();
        msgCacheLock_.unlock();
	}
};