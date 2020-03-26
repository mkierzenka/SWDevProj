#pragma once

#include "row.h"
#include "../utils/object.h"
#include "dataframe.h"

/*******************************************************************************
 *  FindRower::
 *  This Rower takes in 2 dataframe (Search Frame and Row Frame).
 *  It should be mapped over the "Row Frame"
 *  It determines how many elements from a row in "Row Frame" are found in "Search Frame"
 *  This Rower keeps track of the total number of elements it has found so far 
 *  (sum over all rows it accepts).
 *
 */
class FindRower : public Rower
{
public:
    DataFrame *searchFrame_;
    DataFrame *rowFrame_;
	size_t totalFound_; //total number of elements found

    FindRower(DataFrame *sf, DataFrame *rf, size_t numFoundSoFar) :
		searchFrame_(sf), rowFrame_(rf), totalFound_(numFoundSoFar) {}

    FindRower(DataFrame *sf, DataFrame *rf) : FindRower(sf, rf, 0) {}
	
	~FindRower() {}
	
	/**
	 * Returns the running total number of "Search Frame" elements this rower
	 * has found to be in "Row Frame" so far.
	 */
	size_t getCount() {
		return totalFound_;
	}

    /** Go through each element in the row. If element
     * found in the dataframe, increment found. found becomes last element
     * in row. Should be a row from "Row Frame"*/
    bool accept(Row &r)
    {
        size_t ncols = r.width();
		for (size_t i = 0; i < ncols; i++)
        {
            if (intElementInDf_(r, i) || boolElementInDf_(r, i) || stringElementInDf_(r, i) || doubleElementInDf_(r, i))
            {
                totalFound_++;
            }
        }
    }
	
	/** Override. Clones this rower for parallel execution through pmap */
	Rower* clone() {
		return new FindRower(searchFrame_, rowFrame_, totalFound_);
	}

    /**
	 * Adds the count from the other Rower into this Rower's count
	 */
    void join_delete(Rower *other) {
		FindRower* otherFR = dynamic_cast<FindRower*>(other);
		if (otherFR == nullptr) {
			perror("Trying to join non FindRower with this FindRower\n");
		}

		totalFound_ += otherFR->getCount();
	}

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
        //if type not an bool, return false
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
        //if type not a String, return false
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

    //helper to determine whether double in the dataframe
    bool doubleElementInDf_(Row &r, size_t idx)
    {
        //if type not an double, return false
        if (r.col_type(idx) != 'D')
        {
            return false;
        }

        double val = r.get_double(idx);
        for (int c = 0; c < searchFrame_->ncols(); c++)
        {
            //don't check column if types don't match
            if (searchFrame_->get_schema().col_type(c) != 'D') {
                continue;
            }

            for (int r = 0; r < searchFrame_->nrows(); r++)
            {
                //found match
                if (searchFrame_->get_double(c, r) == val)
                {
                    return true;
                }
            }
        }

        return false;
    }
	
};