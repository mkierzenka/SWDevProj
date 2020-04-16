//lang::CwC

#pragma once

#include "../../utils/object.h"
#include "../../utils/array.h"
#include "../../store/key.h"
#include "../../serial/serial.h"


/**
 * Represents an array of Keys.
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class KeyArr : public Object
{
public:
	Array* keyList_; //Owned

	KeyArr() : Object()
	{
		keyList_ = new Array();
	}

	~KeyArr()
	{
		delete keyList_;
	}

	/** Serialize this KeyArr */
	void serialize(Serializer* s)
	{
		size_t len = keyList_->length();
		s->write(len);
		for (size_t i = 0; i < len; i++) {
			(dynamic_cast<Key*>(keyList_->get(i)))->serialize(s);
		}
	}

	/** Deserialize as a KeyArr, set values into this KeyArr*/
	void deserialize(Serializer* s)
	{
		delete keyList_;
		keyList_ = new Array();
		size_t len = s->readSizeT();
		for (size_t i = 0; i < len; i++) {
			Key* k = new Key();
			k->deserialize(s);
			keyList_->add(k);
		}
	}
	
	/** Return the number of elements in this KeyArr */
	size_t length()
	{
		return keyList_->length();
	}

	/**
	 * Get the specified key (does not clone)
	 */
	Key *get(size_t index)
	{
		Object* keyObj = keyList_->get(index);
		return dynamic_cast<Key*>(keyObj);
	}
	

	/**
	 * Appends the given Key to this KeyArr. (does not clone, steals ownership)
	 */
	void add(Key *k)
	{
		keyList_->add(k);
	}

	/**
	 * Get the index of the specified Key. Return -1 if not found
	 */
	int index_of(Key *k)
	{
		return keyList_->index_of(k);
	}
	
	/** Check if this KeyArr is equal to another Object */
    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        KeyArr *ka = dynamic_cast<KeyArr*>(other);

        if (!ka || !keyList_->equals(ka->keyList_))
        {
            return false;
        }

        return true;
    }

    /** Compute hash code of this KeyArr */
    size_t hash_me_() { assert(false); }
  
};