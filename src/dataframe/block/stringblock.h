#pragma once

#include "simpleblock.h"
#include "../../utils/string.h"
#include "../../serial/serial.h"


/**
* StringBlock - to represent a block of Strings.
*
*/
class StringBlock : public SimpleBlock<String*>
{
public:

	~StringBlock() {
		clear(); //delete the Strings stored within this block, ~SimpleBlock does the rest
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

	/** Get a pointer to the String with the index in the block. Does not clone! */
//	String* get(size_t index)

	/** Adds the String to end of this block, does not clone. Now owns.
	 * If can't fit, return -1
	 */
//	int add(String* s)

	/**
	 * Set the element in the given index to the given String pointer.
	 * Now owns, returns the old value which the caller is responsible for deleting.
	 */
//	String* set(size_t index, String* s)

	/** Check if this String block equals another Object */
	bool equals(Object* other) override	{
		if (this == other)
		{
			return true;
		}

		StringBlock* b = dynamic_cast<StringBlock*>(other);
		if (!b || size_ != b->size_ || capacity_ != b->capacity_)
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

	/** Compute hash code of this string block */
	size_t hash_me() override {
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
	void clear() override {
		for (size_t i = 0; i < size_; i++) {
			delete vals_[i];
		}
		memset(vals_, 0, capacity_ * sizeof(String*));
		size_ = 0;
	}

	/** Returns a new StringBlock with all the elements cloned from this one */
	StringBlock* clone() override {
		StringBlock* out = new StringBlock();
		for (size_t i = 0; i < size_; i++) {
			out->add(get(i)->clone());
		}
		return out;
	}
};
