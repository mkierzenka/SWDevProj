#pragma once

#include "block.h"
#include "../serial/serial.h"


/**
* BoolBlock - to represent a block of booleans.
*
*/
class BoolBlock : public Block
{
public:
	bool* vals_; //list of bools, owned

	BoolBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new bool[capacity_];
		memset(vals_, 0, capacity_ * sizeof(bool));
	}

	~BoolBlock()
	{
		delete[] vals_;
	}
	
	/** Serialize this block of booleans into s */
	void serialize(Serializer* s) {
		s->write(capacity_);
		s->write(size_);
		for (size_t i = 0; i < size_; i++) {
			s->write(vals_[i]);
		}
	}
	
	/** Deserialize a block of booleans into this block (mutate) */
	void deserialize(Serializer* s) {
		delete[] vals_;
		capacity_ = s->readSizeT();
		vals_ = new bool[capacity_];
		memset(vals_, 0, capacity_ * sizeof(bool));
		size_ = s->readSizeT();
		for (size_t i = 0; i < size_; i++) {
			vals_[i] = s->readBool();
		}
	}

	/** Get the boolean at the specified index of the array */
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

	/** Add a boolean to end of this block. If can't fit, return -1 */
	int add(bool n)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = n;
		size_++;
	}

	/** set the element in the given index to the given bool. Returns the previous value */
	bool set(size_t index, bool v)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		bool oldVal = vals_[index];
		vals_[index] = v;
		return oldVal;
	}

	/** Check if two blocks equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		BoolBlock* b = dynamic_cast<BoolBlock*>(other);
		
		if (b == nullptr || size_ != b->size_ || capacity_ != b->capacity_)
		{
			return false;
		}

		for (size_t i = 0; i < size_; i++)
		{
			if (vals_[i] != b->vals_[i])
			{
				return false;
			}
		}

		return true;
	}
	
	/** Compute hash code of this bool block */
	size_t hash_me_()
	{
		size_t hash_ = 0;
		hash_ += size_;
		hash_ += capacity_;

		for (size_t i = 0; i < size_; i++)
		{
			if (vals_[i])
			{
				hash_ += 1;
			}
		}

		return hash_;
	}
	
	/** Clears the memory in this BoolBlock */
	void clear() {
		memset(vals_, 0, capacity_ * sizeof(bool));
		size_ = 0;
	}
	
	/** Returns a new BoolBlock with duplicate elements */
	BoolBlock* clone() {
		BoolBlock* out = new BoolBlock();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i));
		}
		return out;
	}
};
