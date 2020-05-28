#pragma once

#include "block.h"
#include "../../utils/object.h"
#include "../../utils/args.h"
#include "../../serial/serial.h"

/**
* SimpleBlock - to represent a block of one type of data.
* Provides a template for a straightforward block implementation.
* Suitable for direct usage with STD types (e.g. SimpleBlock<int>).
*
*/
template <class T>
class SimpleBlock : public Block
{
public:
	T* vals_; //list of items, owned

	SimpleBlock()
	{
		capacity_ = args.blockSize;
		size_ = 0;
		vals_ = new T[capacity_];
		memset(vals_, 0, capacity_ * sizeof(T));
	}

	~SimpleBlock()
	{
		delete[] vals_;
	}

	/** Serialize this block of items into s */
	virtual void serialize(Serializer* s) {
		s->write(capacity_);
		s->write(size_);
		for (size_t i = 0; i < size_; i++) {
			s->write(vals_[i]);
		}
	}

	/** Deserialize a block of items into this block (mutate) */
	virtual void deserialize(Serializer* s) {
		delete[] vals_;
		capacity_ = s->readSizeT();
		vals_ = new T[capacity_];
		memset(vals_, 0, capacity_ * sizeof(T));
		size_ = s->readSizeT();
		for (size_t i = 0; i < size_; i++) {
			vals_[i] = s->readA<T>();
		}
	}

	/** Gets the item at the specified index of the array */
	T get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get block entry from index %zu", index);
			exit(1);
		}
		return vals_[index];
	}

	/** Adds the item to end of this block. If can't fit, return -1 */
	int add(T n)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = n;
		size_++;
	}

	/** Sets the element at the given index. Returns the previous value */
	T set(size_t index, T v)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			printf("Out-Of-Bounds Error: cannot set value at index %zu", index);
			exit(1);
		}
		T oldVal = vals_[index];
		vals_[index] = v;
		return oldVal;
	}

	/** Check if two blocks equal */
	virtual bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		SimpleBlock<T>* b = dynamic_cast<SimpleBlock<T>*>(other);
		if (!b || size_ != b->size_ || capacity_ != b->capacity_)
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

	/** Compute hash code of this block */
	virtual size_t hash_me()
	{
		size_t hash_ = 0;
		hash_ += size_;
		hash_ += capacity_;

		for (size_t i = 0; i < size_; i++)
		{
			hash_ += std::hash<T>{}(vals_[i]);
		}

		return hash_;
	}

	/** Clears the memory in this IntBlock */
	virtual void clear() {
		memset(vals_, 0, capacity_ * sizeof(T));
		size_ = 0;
	}

	/** Returns a new IntBlock with duplicate elements */
	virtual SimpleBlock<T>* clone() {
		SimpleBlock<T>* out = new SimpleBlock<T>();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i));
		}
		return out;
	}
};
