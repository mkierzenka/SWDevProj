#pragma once

#include "block.h"
#include "../../serial/serial.h"

#include <math.h>

/**
* DoubleBlock - to represent a block of doubles.
*
*/
class DoubleBlock : public Block
{
public:
	double* vals_; //list of doubles, owned

	DoubleBlock()
	{
		capacity_ = args.blockSize;
		size_ = 0;
		vals_ = new double[capacity_];
		memset(vals_, 0, capacity_ * sizeof(double));
	}

	~DoubleBlock()
	{
		delete[] vals_;
	}
	
	/** Serialize this block of doubles into s */
	void serialize(Serializer* s) {
		s->write(capacity_);
		s->write(size_);
		for (size_t i = 0; i < size_; i++) {
			s->write(vals_[i]);
		}
	}
	
	/** Deserialize a block of doubles into this block (mutate) */
	void deserialize(Serializer* s) {
		delete[] vals_;
		capacity_ = s->readSizeT();
		vals_ = new double[capacity_];
		memset(vals_, 0, capacity_ * sizeof(double));
		size_ = s->readSizeT();
		for (size_t i = 0; i < size_; i++) {
			vals_[i] = s->readDouble();
		}
	}

	/** Gets the double at the specified index of the array */
	double get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}
		return vals_[index];
	}

	/** Adds the double to end of this block. If can't fit, return -1. */
	int add(double s)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = s;
		size_++;
	}

	/** Set the double at the given index. Returns the old value at that index */
	double set(size_t index, double s)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		double oldVal = vals_[index];
		vals_[index] = s;
		return oldVal;
	}

	/** Check if two blocks equal */
	bool equals(Object* other)
	{
		if (this == other) {
			return true;
		}

		DoubleBlock* b = dynamic_cast<DoubleBlock*>(other);
		if (b == nullptr || size_ != b->size_ || capacity_ != b->capacity_)
		{
			return false;
		}

		for (size_t i = 0; i < size_; i++)
		{
			//set a tolerance
			double tolerance = 0.0001;
			if (abs(vals_[i] - b->vals_[i]) > tolerance)
			{
				return false;
			}
		}

		return true;
	}
	
	/** Compute hash code of this double block */
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
	
	/** Clears the memory in this DoubleBlock */
	void clear() {
		memset(vals_, 0, capacity_ * sizeof(double));
		size_ = 0;
	}
	
	/** Returns a new DoubleBlock with duplicate elements */
	DoubleBlock* clone() {
		DoubleBlock* out = new DoubleBlock();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i));
		}
		return out;
	}
};
