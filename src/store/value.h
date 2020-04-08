#pragma once

#include "../utils/object.h"
#include "../serial/serial.h"

/** This class represents the serialized data that's stored in our key-value store. 
 * It can be used to store either a dataframe or a chunk of data. Values are immutable,
 * once they are set.
 * 
 * Authors: Marcin Kierzenka and Chase Broder */
class Value : public Object
{
public:
    char *val_;        //owned, serialized data stored in char pointer buffer
    size_t capacity_;  //max size of data, in bytes

    Value() {
        capacity_ = 2;
        val_ = new char[capacity_];
        memset(val_, 0, capacity_);
    }
    
    /**
     * Creates a new Value from the first cap bytes of a given array of chars.
     * Copies memory, caller is responsible for deleting data.
     */
    Value(const char *data, size_t cap)
    {
        capacity_ = cap;
        val_ = new char[capacity_];
        memcpy(val_, data, capacity_);
    }

    ~Value()
    {
        delete[] val_;
    }

    /** Returns a pointer to the data stored in this Value (no clone) */
    char *getData()
    {
        return val_;
    }

    /**Return size of the data stored in this Value */
    size_t getSize()
    {
        return capacity_;
    }

    /** Serializes this Value into s */
    void serialize(Serializer* s) {
        s->write(capacity_);
        s->write(capacity_, val_);
    }

    /** Deserializes s into this Value */
    void deserialize(Serializer* s) {
        if (val_) {
            delete[] val_;
        }
        capacity_ = s->readSizeT();
        val_ = new char[capacity_];
        memcpy(val_, s->readCharPtr(capacity_), capacity_);
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