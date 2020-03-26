//lang: Cpp

#pragma once

#include "../utils/object.h"
#include "../utils/map.h"
#include "../utils/queue.h"
#include "../store/key.h"
#include "../store/value.h"

/**
 * This class represents a cache in our distributed array. It contains
 * a maximum amount of values at a time and will update the map on a FIFO
 * basis (key-value that was added first will be overwritten by new value)
 * 
 * Authors: Marcin Kierzenka and Chase Broder
 */
class Cache : public Object
{
public:
    size_t maxSize_ = 15; //maximum size of the cache; this value can be altered
    Map *data_;          //data being stored in cache
    Queue *keyOrder_;    //keeps track of order in which keys added to cache

    Cache()
    {
        data_ = new Map();
        keyOrder_ = new Queue();
    }

    ~Cache()
    {
        delete data_;
        delete keyOrder_;
    }

    /**
     * Check if the cache contains the given key
     */
    bool containsKey(Key *k)
    {
        //Possibly same issue as before: k will be treated as an Object
        return data_->contains_key(k);
    }

    /**
     * Get mapped data for given key or nullptr if it doesn't exist
     */
    Value *getValue(Key *k)
    {
        return dynamic_cast<Value *>(data_->get(k));
    }

    /**
     * Put key-value pair into cache; if cache full, remove key that was
     * added longest ago
     */
    void put(Key *k, Value *val)
    {
        //remove element from queue if it's full
        if (isFull_())
        {
            removeFirstAddedElement_();
        }

        data_->put(k, val);
        keyOrder_->push(k);
    }

    /**
     * Clears the cache
     */
    void clear()
    {
        data_->clear();
        keyOrder_->clear();
    }

    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        Cache* c = dynamic_cast<Cache *>(other);

        if (c == nullptr || !(data_->equals(c->data_)) || !(keyOrder_->equals(c->keyOrder_)))
        {
            return false;
        }

        return true;
    }

    /** Compute hash code of this column */
    size_t hash_me_()
    {
        size_t hash_ = 0;
        hash_ += reinterpret_cast<size_t>(data_);
        hash_ += reinterpret_cast<size_t>(keyOrder_);

        return hash_;
    }

    /**
     * Return if cache is up to capacity
     */
    bool isFull_()
    {
        return maxSize_ <= data_->size();
    }

    /**
     * Remove element from map that was first added. Figure out key by
     * popping from the queue
     */
    Key *removeFirstAddedElement_()
    {
        Key *k = dynamic_cast<Key*>(keyOrder_->pop());
        //queue is empty: don't need to return anything or it's not a Key (error)
        if (k == nullptr)
        {
            return k;
        }

        data_->remove(k);
        return k;
    }
};