#pragma once

#include "block.h"
#include "../serial/serial.h"

/**
* IntBlock - to represent a block of integers.
*
*/
class IntBlock : public Block
{
public:
	int* vals_; //list of ints, owned

	IntBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new int[capacity_];
		memset(vals_, 0, capacity_ * sizeof(int));
	}

	~IntBlock()
	{
		delete[] vals_;
	}
	
	/** Serialize this block of integers into s */
	void serialize(Serializer* s) {
		s->write(capacity_);
		s->write(size_);
		for (size_t i = 0; i < size_; i++) {
			s->write(vals_[i]);
		}
	}
	
	/** Deserialize a block of strings into this block (mutate) */
	void deserialize(Serializer* s) {
		delete[] vals_;
		capacity_ = s->readSizeT();
		vals_ = new int[capacity_];
		memset(vals_, 0, capacity_ * sizeof(int));
		size_ = s->readSizeT();
		for (size_t i = 0; i < size_; i++) {
			vals_[i] = s->readInt();
		}
	}

	/** Gets the int at the specified index of the array */
	int get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}
		return vals_[index];
	}

	/** Adds the int to end of this block. If can't fit, return -1 */
	int add(int n)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = n;
		size_++;
	}

	/** Sets the element at the given index */
	void set(size_t index, int v)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		
		vals_[index] = v;
	}

	/** Check if two blocks equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		IntBlock* b = dynamic_cast<IntBlock*>(other);
		
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
			hash_ += vals_[i];
		}

		return hash_;
	}
	
	/** Clears the memory in this IntBlock */
	void clear() {
		memset(vals_, 0, capacity_ * sizeof(int));
		size_ = 0;
	}
	
	/** Returns a new IntBlock with duplicate elements */
	IntBlock* clone() {
		IntBlock* out = new IntBlock();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i));
		}
		return out;
	}
};
