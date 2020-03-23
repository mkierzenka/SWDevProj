#pragma once

#include "../utils/object.h"

/** This class represents the serialized data that's stored in our key-value store. 
 * It can be used to store either a dataframe or a chunk of data. Values are immutable,
 * once they are set
 * 
 * Authors: Marcin Kierzenka and Chase Broder */
class Value : public Object
{
public:
    char *val_;       //serialized data stored in character pointer buffer
    size_t capacity_; //max size of data, in bytes

    Value(const char *data, size_t cap)
    {
        capacity_ = cap;
        val_ = (char*)malloc(cap);
        memcpy(val_, data, capacity_);
    }

    ~Value()
    {
        free(val_);
    }

    /** Returns serialized data */
    char *getData()
    {
        return val_;
    }

    /**Return size of the data */
    size_t getSize()
    {
        return capacity_;
    }

    /** Check if this Value object equals the given one */
    bool equals(Object *other)
    {
        Value *o = dynamic_cast<Value *>(other);
        return (capacity_ == o->getSize()) && (memcmp(val_, o->getData(), capacity_) == 0);
    }

    /** Compute hash for this value */
    size_t hash_me_()
    {
        return reinterpret_cast<size_t>(val_) + capacity_;
    }

    /** Clone the Value object and return it */
    Value* clone()
    {
        return new Value(val_, capacity_);
    }
};