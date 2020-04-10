#pragma once

#include "../utils/object.h"
#include "../dataframe/row/row.h"


/**
 * This class represents an Object for reading rows. Deduced from Milestone 4 code.
 * 
 * @authors broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class Reader : public Object {
public:
    Reader() { }

    ~Reader() { }
    
    virtual bool visit(Row& r) { }
};