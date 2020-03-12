//lang::CwC

#pragma once

#include "../utils/object.h"
#include "column.h"

/**
 * ColumnArray - to represent a list of column.
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class ColumnArray : public Object
{
public:
	Column **colList_; //list of columns, owned
	size_t capacity_;  //how many elements allocated to size
	size_t len_;	   //length of array

	// constructor
	ColumnArray()
	{
		capacity_ = 2;
		colList_ = new Column*[capacity_];
		len_ = 0;
	}

	// deconstructor
	~ColumnArray()
	{
		deleteColList_();
	}

	// get the length of this array
	size_t length()
	{
		return len_;
	}

	// hash_me override
	size_t hash_me()
	{
		size_t hash = 0;
		for (size_t i = 0; i < len_; i += 1)
		{
			size_t elementhash = get(i)->hash();
			hash += elementhash + (hash << 3) - (hash << 1);
		}

		return hash;
	}

	// get the column with the index in the array
	Column *get(size_t index)
	{
		// check for out-of-bounds
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		return colList_[index];
	}

	// add the column to the end of the array with the given string
	void add(Column *c)
	{
		if (!hasRoomForMoreElems_(1))
		{
			expandArray_();
		}

		//set value
		colList_[len_] = c;

		//update list size
		len_ += 1;
	}

	// set the element in the given index to the given column
	void set(size_t index, Column *c)
	{
		// check for out-of-bounds
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
		}

		colList_[index] = c;
	}

	// remove the element with the given index
	Column *remove(size_t index)
	{
		// check for out-of-bounds
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
		}

		//get value to be removed
		Column *val = get(index);

		//decrement length
		len_ -= 1;

		//move elements over
		for (size_t i = index; i < len_; i += 1)
		{
			colList_[i] = colList_[i + 1];
		}

		return val;
	}

	// get the index of the given Column
	int index_of(Column *o)
	{
		for (size_t i = 0; i < len_; i += 1)
		{
			if (get(i)->equals(o))
			{
				return i;
			}
		}

		//if element not in array, return -1
		return -1;
	}

	// remove all elements in the array
	void clear()
	{
		deleteColList_();
		capacity_ = 2;
		colList_ = new Column *[capacity_];
		len_ = 0;
	}

	// determine if we have enough space allocated to fit the specified number of additional elements
	bool hasRoomForMoreElems_(int numElements)
	{
		return capacity_ >= (len_ + numElements);
	}

	//double size of allocated array memory
	void expandArray_()
	{
		//double list size
		capacity_ *= 2;
		Column **tmp = new Column *[capacity_];
		for (size_t i = 0; i < len_; i += 1)
		{
			tmp[i] = colList_[i];
		}

		//delete old reference and set updated pointer
		delete[] colList_;
		colList_ = tmp;
	}
	
	void deleteColList_() {
		 for (int i = 0; i < len_; i++) {
		 	deleteCol_(colList_[i]);
		}
		
		delete[] colList_;
	}
	
	void deleteCol_(Column* col) {
		IntColumn* ic = col->as_int();
		FloatColumn* fc = col->as_float();
		BoolColumn* bc = col->as_bool();
		StringColumn* sc = col->as_string();
		if (ic != nullptr) {
			delete ic;
		} else if (fc != nullptr) {
			delete fc;
		} else if (bc != nullptr) {
			delete bc;
		} else if (sc != nullptr) {
			delete sc;
		}
	}
};