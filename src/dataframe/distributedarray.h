//lang: CPP

#pragma once

#include "cache.h"
#include "keyarr.h"

#include "../store/key.h"
#include "../store/kvstore.h"
#include "../utils/object.h"
#include "../serial/serial.h"

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
            cache_->put(k, val);
        }

        return val;
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

        if (da == nullptr || !(da->keyList_->equals(keyList_)) || !(da->store_->equals(store_)) || !(da->cache_->equals(cache_)))
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
        hash_ += reinterpret_cast<size_t>(store_);
        hash_ += reinterpret_cast<size_t>(cache_);

        return hash_;
    }
};