#pragma once
//lang:Cwc

#include "../utils/object.h"

//class for wrapping integers
class Integer: public Object {
    public:
        //value to be wrapped
        int val_;

        //default constructor
        Integer()
        {
            val_ = 0;
        }

        //pass in int
        Integer(int value)
        {
            val_ = value;
        }

        //compute hash
        size_t hash_me() {
            return val_ + (val_ << 2);
        }

        //does this Integer equal another one
        bool equals(Object* o) {
            Integer* other = dynamic_cast<Integer*>(o);

            return val_ == other->val_;
        }

        /** clones this integer */
        Integer* clone()
        {
            return new Integer(val_);
        }

        /** Return this integer as an int*/
        int asInt()
        {
            return val_;
        }

        /** Return this integer as a size_t. Make sure positive */
        size_t asUnsignedInt()
        {
            assert(val_ >= 0);
            return val_;
        }
};