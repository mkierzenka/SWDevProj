#pragma once

#include "../../utils/object.h"
#include "row.h"


/**
 * This class represents an Object for reading rows. Deduced from Milestone 4 code.
 * 
 * @authors broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class Reader : public Object {
public:

    /**
     * Visits the row of a DF, does not mutate it.
     * Returns true if a filter on the DF should keep the row.
     */
    virtual bool visit(Row& r) = 0;
};