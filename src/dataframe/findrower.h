#pragma once

#include "row.h"
#include "../utils/object.h"
#include "dataframe.h"

/*******************************************************************************
 *  Rower::
 *  This rower will take in a dataframe and then determine how many elements from that row
 * exist in the dataframe. After that it will set another element to the end of the index indicating
 * how many elements were found in the dataframe. the given dataframe will only contain ints
 */
class FindRower : public Rower
{
public:
    DataFrame *searchFrame_; //data file
    DataFrame *rowFrame_; //haystack

    FindRower(DataFrame *sf, DataFrame *rf) : searchFrame_(sf), rowFrame_(rf) {}

    /** Go through each element in the row (except for last). If element
     * found in the dataframe, increment found. found becomes last element
     * in row. */
    bool accept(Row &r)
    {
        int found = 0;
        for (size_t i = 0; i < r.width() - 1; i++)
        {
            //row data should only be ints
            if (intElementInDf_(r, i) || boolElementInDf_(r, i) || stringElementInDf_(r, i) || floatElementInDf_(r, i))
            {
                found++;
            }
        }

        rowFrame_->set(r.width() - 1, r.get_idx(), found);
    }

    /** Once traversal of the data frame is complete the rowers that were
      split off will be joined.  There will be one join per split. The
      original object will be the last to be called join on. The join method
      is reponsible for cleaning up memory. */
    void join_delete(Rower *other) {}

    //helper to determine whether int in the dataframe
    bool intElementInDf_(Row &r, size_t idx)
    {
        //if type not an int, return false
        if (r.col_type(idx) != 'I')
        {
            return false;
        }

        int val = r.get_int(idx);
        for (size_t c = 0; c < searchFrame_->ncols(); c++)
        {
            //don't check column if types don't match
            if (searchFrame_->get_schema().col_type(c) != 'I') {
                continue;
            }

            for (int r = 0; r < searchFrame_->nrows(); r++)
            {
                //found match
                if (searchFrame_->get_int(c, r) == val)
                {
                    return true;
                }
            }
        }

        return false;
    }

    //helper to determine whether bool in the dataframe
    bool boolElementInDf_(Row &r, size_t idx)
    {
        //if type not an int, return false
        if (r.col_type(idx) != 'B')
        {
            return false;
        }

        bool val = r.get_bool(idx);
        for (int c = 0; c < searchFrame_->ncols(); c++)
        {
            //don't check column if types don't match
            if (searchFrame_->get_schema().col_type(c) != 'B') {
                continue;
            }

            for (int r = 0; r < searchFrame_->nrows(); r++)
            {
                //found match
                if (searchFrame_->get_bool(c, r) == val)
                {
                    return true;
                }
            }
        }

        return false;
    }

    //helper to determine whether string in the dataframe
    bool stringElementInDf_(Row &r, size_t idx)
    {
        //if type not an int, return false
        if (r.col_type(idx) != 'S')
        {
            return false;
        }

        String *val = r.get_string(idx);
        for (int c = 0; c < searchFrame_->ncols(); c++)
        {
            //don't check column if types don't match
            if (searchFrame_->get_schema().col_type(c) != 'S') {
                continue;
            }

            for (int r = 0; r < searchFrame_->nrows(); r++)
            {
                //found match
                if (searchFrame_->get_string(c, r)->equals(val))
                {
                    return true;
                }
            }
        }

        return false;
    }

    //helper to determine whether float in the dataframe
    bool floatElementInDf_(Row &r, size_t idx)
    {
        //if type not an int, return false
        if (r.col_type(idx) != 'F')
        {
            return false;
        }

        float val = r.get_float(idx);
        for (int c = 0; c < searchFrame_->ncols(); c++)
        {
            //don't check column if types don't match
            if (searchFrame_->get_schema().col_type(c) != 'F') {
                continue;
            }

            for (int r = 0; r < searchFrame_->nrows(); r++)
            {
                //found match
                if (searchFrame_->get_float(c, r) == val)
                {
                    return true;
                }
            }
        }

        return false;
    }
};