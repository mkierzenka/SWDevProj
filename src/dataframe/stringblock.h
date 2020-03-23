#pragma once

#include "block.h"
#include "../utils/string.h"
#include "../serial/serial.h"



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
		for (size_t i = 0; i < size_; i++) {
			delete vals_[i];
		}
		delete[] vals_;
	}
	
	/** Serialize this block of strings into s */
	void serialize(Serializer* s) {
		s->write(capacity_);
		s->write(size_);
		for (size_t i = 0; i < size_; i++) {
			vals_[i]->serialize(s);
		}
	}
	
	/** Deserialize a block of strings into this block (mutate) */
	void deserialize(Serializer* s) {
		for (size_t i = 0; i < size_; i++) {
			delete vals_[i];
		}
		delete[] vals_;

		capacity_ = s->readSizeT();
		vals_ = new String*[capacity_];
		memset(vals_, 0, capacity_ * sizeof(String*));
		size_ = s->readSizeT();
		for (size_t i = 0; i < size_; i++) {
			String* tmp = new String("");
			tmp->deserialize(s);
			vals_[i] = tmp;
		}
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

	// set the element in the given index to a clone of the given String
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

	/** Check if two blocks equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		StringBlock* b = dynamic_cast<StringBlock*>(other);
		
		if (b == nullptr || size_ != b->size_ || capacity_ != b->capacity_)
		{
			return false;
		}

		for (size_t i = 0; i < size_; i++)
		{
			if (!(vals_[i]->equals(b->vals_[i])))
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
			hash_ += vals_[i]->hash();
		}

		return hash_;
	}

};
