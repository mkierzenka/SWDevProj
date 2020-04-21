#pragma once

#include "../../utils/object.h"
#include "../../utils/args.h"

/**
* Block - to represent a block of one type of data.
* It is expected that blocks will have a fixed capacity (BLOCK_SIZE items)
* They may have less elements, but cannot have more.
*
*/
class Block : public Object
{
public:
	size_t size_;
	size_t capacity_;

	virtual ~Block() {}

	/** Get the number of elements in this block */
	virtual size_t size()
	{
		return size_;
	}

	/** Get the number of elements this block can hold */
	virtual size_t capacity()
	{
		return capacity_;
	}

	/** Check if this array equals to another array */
	virtual bool equals(Object *other)
	{
		return this == other;
	}

	/** Return a hash of this Block */
	virtual size_t hash_me()
	{
		return 0;
	}
};
