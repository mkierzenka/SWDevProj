#pragma once

#include "block.h"


/**
* BoolBlock - to represent a block of bools.
*
*/
class BoolBlock : public Block
{
public:
	bool* vals_; //list of bools, owned
	size_t size_;
	size_t capacity_;

	// constructor
	BoolBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new bool[capacity_];
		memset(vals_, 0, capacity_ * sizeof(bool));
	}

	// deconstructor
	~BoolBlock()
	{
		delete[] vals_;
	}

	// get the bool with the index in the array
	bool get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}
		return vals_[index];
	}

	// add bool to end of this block. if can't fit, return -1
	int add(bool n)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = n;
		size_++;
	}

	// set the element in the given index to the given bool
	void set(size_t index, bool v)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		
		vals_[index] = v;
	}

};
