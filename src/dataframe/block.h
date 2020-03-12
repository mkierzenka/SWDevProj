#pragma once

#include "../utils/object.h"

static const size_t BLOCK_SIZE = 2048;

/**
* Block - to represent a block.
*
*/
class Block : public Object
{
public:
	size_t size_;
	size_t capacity_;

	// get the number of elements in this block
	virtual size_t size()
	{
		return size_;
	}
	
	// get the number of elements this block can hold
	virtual size_t capacity()
	{
		return capacity_;
	}

	// check if this array equals to other array
	virtual bool equals(Object *other)
	{
		return this == other;
	}

	// hash_me override
	virtual size_t hash_me()
	{
		return 0;
	}

};
