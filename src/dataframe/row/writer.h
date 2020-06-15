#pragma once

#include "../../utils/object.h"
#include "row.h"


/**
 * This class represents an Object for filling rows with values. Deduced from Milestone 4 code.
 * Usually, visit is called on rows until done() is true
 * 
 * @authors broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class Writer : public Object {
public:

    /**
     * Called on rows of a dataframe (mutates them) until done() = true.
     */
    virtual void visit(Row& r) = 0;

    /**
     * Returns true when the writer has finished writing rows, false otherwise.
     */
    virtual bool done() = 0;
};