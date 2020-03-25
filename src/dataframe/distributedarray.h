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
#include "floatblock.h"

//class KVStore;

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
	 * Get specific float from a value stored with key k
	 */
    float getFloat(Key *k, size_t itemIdx)
    {
		Value* val = nullptr;
		// if value isn't in cache yet, add it
        if (!(cache_->containsKey(k)))
        {
            //get data from store, and cache it
			val = store_->getValue(k);
			if (val != nullptr)
			{
				cache_->put(k->clone(), val->clone());
			}
        }
		if (val == nullptr)
		{
			val = cache_->getValue(k);
		}
		Serializer* s = new Serializer(val->getSize(), val->getData());
		FloatBlock* floatData = new FloatBlock();
		floatData->deserialize(s);
		float out = floatData->get(itemIdx);
		delete floatData;
		delete s;
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
};