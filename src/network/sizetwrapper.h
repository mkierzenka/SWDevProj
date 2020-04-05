#pragma once
//lang:Cwc

#include "../utils/object.h"

/**
 * Class for wrapping size t's.
 * 
 * @authors kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */
class SizeTWrapper: public Object {
    public:
        //value to be wrapped
        size_t val_;

        //default constructor
        SizeTWrapper()
        {
            val_ = 0;
        }

        //pass in int
        SizeTWrapper(size_t value)
        {
            val_ = value;
        }

        //compute hash
        size_t hash_me() {
            return val_ + (val_ << 2);
        }

        //does this Integer equal another one
        bool equals(Object* o) {
            SizeTWrapper* other = dynamic_cast<SizeTWrapper*>(o);

            return val_ == other->val_;
        }

        /** clones this integer */
        SizeTWrapper* clone()
        {
            return new SizeTWrapper(val_);
        }

        /** Return this integer as an int*/
        int asInt()
        {
            return val_;
        }

        /** Return this integer as a size_t. Make sure positive */
        size_t asSizeT()
        {
            return val_;
        }
};