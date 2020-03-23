#pragma once

#include "block.h"
#include "../serial/serial.h"

/**
* IntBlock - to represent a block of ints.
*
*/
class IntBlock : public Block
{
public:
	int* vals_; //list of ints, owned
	size_t size_;
	size_t capacity_;

	// constructor
	IntBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new int[capacity_];
		memset(vals_, 0, capacity_ * sizeof(int));
	}

	// deconstructor
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

	// get the int with the index in the array
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

	// add int to end of this block. if can't fit, return -1
	int add(int n)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = n;
		size_++;
	}

	// set the element in the given index to the given object
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
};
