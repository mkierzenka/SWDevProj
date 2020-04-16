//lang: CPP

#pragma once

#include "cache.h"
#include "keyarr.h"

#include "../../store/key.h"
#include "../../store/kvstore.h"
#include "../../utils/object.h"
#include "../../serial/serial.h"
#include "../block/intblock.h"
#include "../block/boolblock.h"
#include "../block/stringblock.h"
#include "../block/doubleblock.h"

/**
 * This class represents a distributed array. It holds a list of keys
 * that describe the data it includes, along with a key-value store to
 * look up its data. It will also store some key-value mappings in its
 * cache.
 * 
 * @authors Chase Broder and Marcin Kierzenka
 */
class DistributedArray : public Object
{
public:
    KeyArr *keyList_; //owned, a list of keys that correspond to this DA
    KVStore *store_;  //external, store to look up values of keys
    Cache *cache_;    //owned, holds some values of its keys at any given time

    DistributedArray(KVStore *store)
    {
        store_ = store;
        keyList_ = new KeyArr();
        cache_ = new Cache();
    }

    ~DistributedArray()
    {
        delete keyList_;
        delete cache_;
    }

    void setStore(KVStore* store)
    {
        store_ = store;
    }
    
    void serialize(Serializer *s)
    {
        keyList_->serialize(s);
    }

    void deserialize(Serializer *s)
    {
        keyList_->deserialize(s);
    }

    /**
     * Check if this Distributed Array contains the given key.
     */
    bool containsKey(Key *k)
    {
        return keyList_->index_of(k) >= 0;
    }

    /**
     * Add a clone of the Key to this Distributed Array. Caller responsible for
	 * deleting k.
     */
    void addKey(Key *k)
    {
        keyList_->add(k->clone());
    }

	/**
	 * Returns the number of keys in this Distributed Array.
	 */
	size_t length() {
		return keyList_->length();
	}
	
	/**
	 * Get specific double from a value stored with key k. Caller responsible for deleting key.
	 * Blocks on network call
	 */
    double getDouble(Key *k, size_t itemIdx)
    {
		DoubleBlock* doubleData = nullptr;
		if (!(cache_->containsKey(k)))
        {
            //get data from store and cache
			DoubleBlock* val = getDoubleBlockFromStore_(k);
			if (val != nullptr) {
				cache_->put(k->clone(), val);
			}
        }
		doubleData = dynamic_cast<DoubleBlock*>(cache_->getBlock(k));
		if (doubleData == nullptr) {
			fprintf(stderr, "Unable to get double from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		return doubleData->get(itemIdx);
    }

	/**
	 * Get specific boolean from a value stored with key k. Caller responsible for deleting key.
	 * Blocks on network call
	 */
    bool getBool(Key *k, size_t itemIdx)
    {
		BoolBlock* boolData = nullptr;
		if (!(cache_->containsKey(k)))
        {
            //get data from store and cache
			BoolBlock* val = getBoolBlockFromStore_(k);
			if (val != nullptr) {
				cache_->put(k->clone(), val);
			}
        }
		boolData = dynamic_cast<BoolBlock*>(cache_->getBlock(k));
		if (boolData == nullptr) {
			fprintf(stderr, "Unable to get boolean from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		return boolData->get(itemIdx);
    }

	/**
	 * Get specific integer from a value stored with key k. Caller responsible for deleting key.
	 * Blocks on network call
	 */
    int getInt(Key *k, size_t itemIdx)
    {
		IntBlock* intData = nullptr;
		if (!(cache_->containsKey(k)))
        {
            //get data from store and cache
			IntBlock* val = getIntBlockFromStore_(k);
			if (val != nullptr) {
				cache_->put(k->clone(), val);
			}
        }
		intData = dynamic_cast<IntBlock*>(cache_->getBlock(k));
		if (intData == nullptr) {
			fprintf(stderr, "Unable to get integer from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		return intData->get(itemIdx);
    }

	/**
	 * Get specific string from a value stored with key k.
	 * Blocks on network call
	 * Caller is responsible for deleting the String returned and the key passed in.
	 */
    String* getString(Key *k, size_t itemIdx)
    {
		StringBlock* strData = nullptr;
		if (!(cache_->containsKey(k)))
        {
            //get data from store and cache
			StringBlock* val = getStrBlockFromStore_(k);
			if (val != nullptr) {
				cache_->put(k->clone(), val);
			}
        }
		strData = dynamic_cast<StringBlock*>(cache_->getBlock(k));
		if (strData == nullptr) {
			fprintf(stderr, "Unable to get string from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		return strData->get(itemIdx)->clone();
    }

    /** Check if two distributed arrats equal */
    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        DistributedArray *da = dynamic_cast<DistributedArray *>(other);

        // store_ is not included
        if (da == nullptr || !(da->keyList_->equals(keyList_)) || !(da->cache_->equals(cache_)))
        {
            return false;
        }

        return true;
    }

    /** Compute hash code of this column */
    size_t hash_me_()
    {
        size_t hash_ = 0;
        hash_ += reinterpret_cast<size_t>(keyList_);
        hash_ += reinterpret_cast<size_t>(cache_);

        return hash_;
    }
	
	/** Returns the Value mapped to this key, deserialized as a double block
	 *  Blocks on network call.
	 *  Caller is expected to delete the block
	 */
	DoubleBlock* getDoubleBlockFromStore_(Key* k) {
		Value* val = store_->getValue(k, true);
        assert(val != nullptr);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		DoubleBlock* out = new DoubleBlock();
		out->deserialize(s);
		delete s;
		return out;
	}

	/** Returns the Value mapped to this key, deserialized as a bool block.
	 *  Blocks on network call.
	 *  Caller is expected to delete the block
	 */
	BoolBlock* getBoolBlockFromStore_(Key* k) {
		Value* val = store_->getValue(k, true);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		BoolBlock* out = new BoolBlock();
		out->deserialize(s);
		delete s;
		return out;
	}

	/** Returns the Value mapped to this key, deserialized as a int block.
	 *  Blocks on network call.
	 *  Caller is expected to delete the block
	 */
	IntBlock* getIntBlockFromStore_(Key* k) {
		Value* val = store_->getValue(k, true);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		IntBlock* out = new IntBlock();
		out->deserialize(s);
		delete s;
		return out;
	}


	/** Returns the Value mapped to this key, deserialized as a string block.
	 *  Blocks on network call.
	 *  Caller is expected to delete the block
	 */
	StringBlock* getStrBlockFromStore_(Key* k) {
		Value* val = store_->getValue(k, true);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		StringBlock* out = new StringBlock();
		out->deserialize(s);
		delete s;
		return out;
	}
};