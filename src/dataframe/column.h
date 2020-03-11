#pragma once

#include "object.h"
#include "string.h"
#include "block.h"

#include <stddef.h>
#include <math.h>
#include <stdarg.h>

class IntColumn;
class BoolColumn;
class StringColumn;
class FloatColumn;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
class Column : public Object
{
public:
    size_t size_;     //how many elements are in the column
    size_t capacity_; //how many elements the column can store
	size_t numBlocks_;
	size_t capacityBlocks_;

    /** Default constructor: start column to fit two elements */
    Column() : Column(2) {}

    /** Constructor allows you to set the starting capacity of
     * the column */
    Column(size_t capacity)
    {
        size_ = 0;
        capacity_ = capacity;
    }

    /** Type converters: Return same column under its actual type, or
    *  nullptr if of the wrong type.  */
    virtual IntColumn *as_int()
    {
        return nullptr;
    }

    virtual BoolColumn *as_bool()
    {
        return nullptr;
    }

    virtual FloatColumn *as_float()
    {
        return nullptr;
    }

    virtual StringColumn *as_string()
    {
        return nullptr;
    }

    /** Type appropriate push_back methods. Calling the wrong method is
    * undefined behavior. **/
    virtual void push_back(int val) {}

    virtual void push_back(bool val) {}

    virtual void push_back(float val) {}

    virtual void push_back(String *val) {}

    /** Returns the number of elements in the column. */
    virtual size_t size()
    {
        return size_;
    }

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'. */
    char get_type()
    {
        return '\0'; //no type for default column
    }

    /** Print the element in sorer format, with brackets */
    virtual void printElement(size_t rowIdx)
    {
    }

    /** Helper to determine if given index is out-of-bounds */
    bool isIndexOutOfBounds_(size_t idx)
    {
        return idx >= size_;
    }

    /** Helper that exits program if index out-of-bounds */
    void exitIfIndexOutOfBounds_(size_t idx)
    {
        if (isIndexOutOfBounds_(idx))
        {
            //error message and exit of bounds
            fprintf(stderr, "Index %zu out of bounds", idx);
            exit(1);
        }
    }
};