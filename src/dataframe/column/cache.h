//lang: Cpp

#pragma once

#include "../../utils/object.h"
#include "../../utils/map.h"
#include "../../utils/queue.h"
#include "../../store/key.h"
#include "../../store/value.h"
#include "../block/block.h"

/**
 * This class represents a cache in our distributed array. It contains
 * a maximum amount of blocks at a time and will update the map on a FIFO
 * basis (key-value that was added first will be overwritten by new value)
 *
 * Steals ownership of the keys and blocks added to it
 * 
 * Authors: Marcin Kierzenka and Chase Broder
 */
class Cache : public Object
{
public:
    size_t maxSize_;     //maximum size of the cache
    Map *data_;          //owned, data being stored in cache
    Queue *keyOrder_;    //owned, keeps track of order in which keys were added to cache
	                     //       can be safely delete (has clones of keys)

    Cache() : Cache(15) {}

    /**
     * Constructor to specify max size of cache
     */
    Cache(size_t max) : Object()
    {
        if (max == 0)
        {
            fprintf(stderr, "Can't have cache with max size 0\n");
            exit(-1);
        }

        maxSize_ = max;
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
        return data_->contains_key(k);
    }

    /**
     * Get mapped data for given key or nullptr if it doesn't exist.
	 * Returns a pointer to the actual data - No clone.
     */
    Block *getBlock(Key *k)
    {
        return dynamic_cast<Block *>(data_->get(k));
    }

    /**
     * Put key-block pair into cache; if cache full, remove pair that was
     * added longest ago (deletes.
	 *
	 * Adds this exact key and block to the cache - steals ownership.
     */
    void put(Key *k, Block *val)
    {
        if (isFull_())
        {
            delete removeFirstAddedElement_();
        }

        data_->put(k, val); //used to clone
        keyOrder_->push(k->clone());
        assert(data_->size() == keyOrder_->size());
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
     * popping from the queue. Returns nullptr if empty queue.
	 *
	 * Caller is responsible for deleting the returned key.
     */
    Key *removeFirstAddedElement_()
    {
        Key *k = dynamic_cast<Key*>(keyOrder_->pop());
        if (k == nullptr)
        {
            //queue is empty or it's not a Key (error)
			return k;
        }

		delete data_->remove(k);
        assert(data_->size() == keyOrder_->size());
        return k;
    }
};