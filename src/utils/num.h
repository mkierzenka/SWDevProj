#pragma once

#include "object.h"

/** this class wraps an integer *
 * 
 * @authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
 */
class Num : public Object {
    public:
    int val_;

    Num() : Object()
    {
        val_ = 0;
    }

    Num(int val) : Object()
    {
        val_ = val;
    }

    /** Return this object as as int */
    int asInt()
    {
        return val_;
    }

    /** Returns a new Num with the same value */
    Num* clone() {
        return new Num(val_);
    }
}; 