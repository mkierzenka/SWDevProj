//lang::CwC

#pragma once

#include "../utils/object.h"
#include "../store/key.h"
#include "../serial/serial.h"


/**
 * todo
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class KeyArr : public Object
{
public:
	Array* keyList_; //list of columns, owned

	// constructor
	KeyArr() : Object()
	{
		keyList_ = new Array();
	}

	// destructor
	~KeyArr()
	{
		//deleteColList_();
		delete keyList_;
	}

	/** Serialize a KeyArr into char* representation */
	void serialize(Serializer* s)
	{
		//will elements be serialized as objects or columns?
		//colList_->serializeAsColumnArray(s);
		size_t len = keyList_->length();
		s->write(len);
		for (size_t i = 0; i < len; i++) {
			(dynamic_cast<Key*>(keyList_->get(i)))->serialize(s);
		}
	}

	/** Deserialize as a KeyArr, set values into this KeyArr*/
	void deserialize(Serializer* s)
	{
		size_t len = s->readSizeT();
		for (size_t i = 0; i < len; i++) {
			Key* k = new Key();
			k->deserialize(s);
			keyList_->add(k);
		}
	}
	
	// get the length of this array
	size_t length()
	{
		return keyList_->length();
	}

	// get the specified key (does not make a copy!)
	Key *get(size_t index)
	{
		Object* keyObj = keyList_->get(index);
		return dynamic_cast<Key*>(keyObj);
	}
	

	/**
	 * Appends Key to this KeyArr. Does not make copy
	 */
	void add(Key *k)
	{
		keyList_->add(k);
	}

	// get the index of the given Key
	int index_of(Key *k)
	{
		//casting needed?
		return keyList_->index_of(k);
	}
	
	/** Check if two distributed arrats equal */
    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        KeyArr *ka = dynamic_cast<KeyArr*>(other);

        if (ka == nullptr || keyList_->equals(ka->keyList_))
        {
            return false;
        }

        return true;
    }

    /** Compute hash code of this column */
    size_t hash_me_()
    {
        size_t hash_ = 0;
        hash_ += reinterpret_cast<size_t>(keyList_);

        return hash_;
    }
  
};