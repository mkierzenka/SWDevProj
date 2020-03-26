//lang: CPP

#pragma once

#include "cache.h"
#include "keyarr.h"

#include "../store/key.h"
#include "../store/kvstore.h"
#include "../utils/object.h"
#include "../serial/serial.h"
#include "intblock.h"
#include "boolblock.h"
#include "stringblock.h"
#include "doubleblock.h"

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
    KeyArr *keyList_; //holds a list of keys that correspond to this DA
    KVStore *store_;  //store to look up values of keys
    Cache *cache_;    //holds some values of its keys at any given time

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
         * Check if the distributed array contains the given key
         */
    bool containsKey(Key *k)
    {
        ///TODO: might need to do casting or create a KeyArray
        return keyList_->index_of(k) != -1;
    }

    /**
          * Add key to the array
          */
    void addKey(Key *k)
    {
        keyList_->add(k);
    }

    /**
         * Get the data for the specified key. Adds nullptr if value
         * cannot be found.
         */
    Value *get(Key *k)
    {
        //check the cache first: return if exists
        if (cache_->containsKey(k))
        {
            return cache_->getValue(k);
        }

        //get data from store, and cache and return it
        Value *val = store_->getValue(k);
        if (val != nullptr)
        {
            cache_->put(k->clone(), val->clone());
        }

        return val;
    }
	
	/**
	 * Get specific double from a value stored with key k
	 */
    double getDouble(Key *k, size_t itemIdx)
    {
		DoubleBlock* doubleData = getDoubleBlock_(k);
		if (doubleData == nullptr) {
			fprintf(stderr, "Unable to get double from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		double out = doubleData->get(itemIdx);
		delete doubleData;
		return out;
    }

	/**
	 * Get specific boolean from a value stored with key k
	 */
    bool getBool(Key *k, size_t itemIdx)
    {
		BoolBlock* boolData = getBoolBlock_(k);
		if (boolData == nullptr) {
			fprintf(stderr, "Unable to get boolean from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		bool out = boolData->get(itemIdx);
		delete boolData;
		return out;
    }

	/**
	 * Get specific integer from a value stored with key k
	 */
    int getInt(Key *k, size_t itemIdx)
    {
		IntBlock* intData = getIntBlock_(k);
		if (intData == nullptr) {
			fprintf(stderr, "Unable to get integer from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		int out = intData->get(itemIdx);
		delete intData;
		return out;
    }

	/**
	 * Get specific string from a value stored with key k.
	 * Caller is responsible for deleting the String returned
	 */
    String* getString(Key *k, size_t itemIdx)
    {
		StringBlock* strData = getStrBlock_(k);
		if (strData == nullptr) {
			fprintf(stderr, "Unable to get string from key [%s, %zu]\n", k->getKeyStr()->c_str(), k->getNode());
			exit(1);
		}
		String* out = strData->get(itemIdx); //get should have cloned it
		delete strData;
		return out;
    }

    /**
     * Gets key with specified index in array, then get data for that key
     */
    Value *get(size_t idx)
    {
        return get(getKeyAtIndex(idx));
    }

    /**
         * Get key at specified index, return error if out-of-bounds
         */
    Key *getKeyAtIndex(size_t idx)
    {
        return keyList_->get(idx);
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
	 *  Caller is expected to delete the block
	 */
	DoubleBlock* getDoubleBlock_(Key* k) {
		Value* val = get(k);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		DoubleBlock* out = new DoubleBlock();
		out->deserialize(s);
		delete s;
		return out;
	}

	/** Returns the Value mapped to this key, deserialized as a bool block.
	 *  Caller is expected to delete the block
	 */
	BoolBlock* getBoolBlock_(Key* k) {
		Value* val = get(k);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		BoolBlock* out = new BoolBlock();
		out->deserialize(s);
		delete s;
		return out;
	}

	/** Returns the Value mapped to this key, deserialized as a int block.
	 *  Caller is expected to delete the block
	 */
	IntBlock* getIntBlock_(Key* k) {
		Value* val = get(k);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		IntBlock* out = new IntBlock();
		out->deserialize(s);
		delete s;
		return out;
	}


	/** Returns the Value mapped to this key, deserialized as a string block.
	 *  Caller is expected to delete the block
	 */
	StringBlock* getStrBlock_(Key* k) {
		Value* val = get(k);
		Serializer* s = new Serializer(val->getSize(), val->getData());
		StringBlock* out = new StringBlock();
		out->deserialize(s);
		delete s;
		return out;
	}	

	
};