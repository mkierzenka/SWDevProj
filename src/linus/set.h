#pragma once

#include "../dataframe/dataframe.h"

#include <stdlib.h>

/**
 * The input data is a processed extract from GitHub.
 *
 * projects:  I x S   --  The first field is a project id (or pid).
 *                    --  The second field is that project's name.
 *                    --  In a well-formed dataset the largest pid
 *                    --  is equal to the number of projects.
 *
 * users:    I x S    -- The first field is a user id, (or uid).
 *                    -- The second field is that user's name.
 *
 * commits: I x I x I -- The fields are pid, uid, uid', each row represent
 *                    -- a commit to project pid, written by user uid
 *                    -- and committed by user uid',
 **/

/**************************************************************************
 * A bit set contains size() booleans that are initialize to false and can
 * be set to true with the set() method. The test() method returns the
 * value. Does not grow.
 ************************************************************************/
class Set
{
public:
    bool *vals_;  // owned; data
    size_t size_; // number of elements
    size_t setSize_; //number of elements set

    /** Creates a set of the same size as the dataframe. */
    Set(DataFrame *df) : Set(df->nrows()) {}

    /** Creates a set of the given size. */
    Set(size_t sz) : vals_(new bool[sz]), size_(sz)
    {
        for (size_t i = 0; i < size_; i++)
            vals_[i] = false;
        
        setSize_ = 0;
    }

    ~Set() { delete[] vals_; }

    /** Add idx to the set. If idx is out of bound, ignore it.  Out of bound
   *  values can occur if there are references to pids or uids in commits
   *  that did not appear in projects or users.
   */
    void set(size_t idx)
    {
        if (idx >= size_)
            return; // ignoring out of bound writes
            
        vals_[idx] = true;
        setSize_++;
    }

    /** Is idx in the set?  See comment for set(). */
    bool test(size_t idx)
    {
        if (idx >= size_)
            return true; // ignoring out of bound reads
        return vals_[idx];
    }

    size_t size() { return size_; }

    /** How many elements in the size are set */
    size_t setSize() { return setSize_; }


    /** Performs set union in place. */
    void union_(Set &from)
    {
        for (size_t i = 0; i < from.size_; i++)
            if (from.test(i))
                set(i);
    }
};