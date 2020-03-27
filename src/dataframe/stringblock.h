#pragma once

#include "block.h"
#include "../utils/string.h"
#include "../serial/serial.h"



/**
* StringBlock - to represent a block of Strings.
*
*/
class StringBlock : public Block
{
public:
	String** vals_; //list of String*, owned
	size_t size_;
	size_t capacity_;

	StringBlock()
	{
		capacity_ = BLOCK_SIZE;
		size_ = 0;
		vals_ = new String*[capacity_];
		memset(vals_, 0, capacity_ * sizeof(String*));
	}

	~StringBlock()
	{
		clear(); //expected that this calls delete on each String
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

	/** Get a clone of the String with the index in the block */
	String* get(size_t index)
	{
		// check for out-of-bounds
		if (index >= size_)
		{
			fprintf(stderr, "Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}
		return vals_[index]->clone();
	}

	/** Adds the String to end of this block, does not clone. Now owns.
	 * If can't fit, return -1
	 */
	int add(String* s)
	{
		if (size_ >= capacity_)
		{
			return -1;
		}
        vals_[size_] = s;
		size_++;
	}

	/** Set the element in the given index to a clone of the given String */
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
	
	/** Clears the memory in this StringBlock, deleting each String ptr */
	void clear() {
		for (size_t i = 0; i < size_; i++) {
			delete vals_[i];
		}
		memset(vals_, 0, capacity_ * sizeof(int));
		size_ = 0;
	}
	
	/** Returns a new StringBlock with all the elements cloned from this one */
	StringBlock* clone() {
		StringBlock* out = new StringBlock();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i)->clone());
		}
		return out;
	}
};
