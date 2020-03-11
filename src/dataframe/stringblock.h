#pragma once

#include "block.h"
#include "string.h"


/**
* StringBlock - to represent a block of String*.
*
*/
class StringBlock : public Block
{
public:
	String** vals_; //list of String*, owned
	size_t size_;
	size_t capacity_;

	// constructor
	StringBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new String*[capacity_];
		memset(vals_, 0, capacity_ * sizeof(String*));
	}

	// deconstructor
	~StringBlock()
	{
		delete[] vals_;
	}

	// get the String with the index in the array
	String* get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}
		return vals_[index];
	}

	// add String to end of this block, does not clone. Now owns s. if can't fit, return -1
	int add(String* s)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = s;
		size_++;
	}

	// set the element in the given index to the given object
	void set(size_t index, String* s)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		
		vals_[index] = s->clone();
	}

};
