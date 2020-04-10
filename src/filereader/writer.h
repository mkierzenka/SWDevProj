#pragma once

#include "../utils/object.h"
#include "../dataframe/row/row.h"


/**
 * This class represents an Object for filling rows with values. Deduced from Milestone 4 code.
 * Usually, visit is called on rows until done() is true
 * 
 * @authors broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class Writer : public Object {
public:
    Writer() { }

    ~Writer() { }
    
    virtual void visit(Row& r) {

    }

    virtual bool done() {
        
    }
};