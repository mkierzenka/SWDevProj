#pragma once

#include "set.h"
#include "../dataframe/row/writer.h"

/*****************************************************************************
 * A SetWriter copies all the values present in the set into a one-column
 * dataframe. The data contains all the values in the set. The dataframe has
 * at least one integer column.
 ****************************************************************************/
class SetWriter : public Writer
{
public:
    Set &set_;  // set to read from
    int i_ = 0; // position in set

    SetWriter(Set &set) : set_(set) {}

    /** Skip over false values and stop when the entire set has been seen */
    bool done()
    {
        while (i_ < (int)set_.size_ && set_.test(i_) == false)
            ++i_;
        return i_ == (int)set_.size_;
    }

    void visit(Row &row) { row.set(0, i_++); }
};