//lang::CwC

#pragma once

#include "object.h"

#include <assert.h>

/**
* Array - to represent a list of objects. Array owns its elements
*
*Specification by Yiwen Ma & Viswajith Gopinathan
*Authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
*/
class Array : public Object
{
public:
	Object **objList_; //list of objects, owned
	size_t capacity_;  //how many elements allocated to size
	size_t len_;	   //length of array

	
	Array(size_t initialCapacity)
	{
		capacity_ = initialCapacity;
		objList_ = new Object *[capacity_];
		len_ = 0;
	}
	
	Array() : Array(2) {}

	~Array()
	{
        deleteObjList_();
	}

	/** Return length of this Array */
	size_t length()
	{
		return len_;
	}

	/** Return true if this Array equals the other Object */
	bool equals(Object *other)
	{
		if (this == other)
		{
			return true;
		}

		Array *o = dynamic_cast<Array *>(other);
		if (!o || this->length() != o->length())
		{
			return false;
		}

		for (size_t i = 0; i < len_; i += 1)
		{
			if (!get(i)->equals(o->get(i)))
			{
				return false;
			}
		}

		return true;
	}

	/** Return hash of this Array */
	size_t hash_me()
	{
		size_t hash = 0;
		for (size_t i = 0; i < len_; i += 1)
		{
			size_t elementhash = get(i)->hash();
			hash += elementhash + (hash << 3) - (hash << 1);
		}

		return hash;
	}

	/** Return the Object located at a given index */
	Object *get(size_t index)
	{
		if (index >= length())
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		return objList_[index];
	}

	/** Add the object to the end of this Array. Array owns this object */
	void add(Object *o)
	{
		if (!hasRoomForMoreElems_(1))
		{
			expandArray_();
		}

		objList_[len_] = o;
		len_ += 1;
	}

	/** Set the element at the given index to the specified object. Return the old value.
	 * Caller responsible for deleting returned value
	 */
	Object* set(size_t index, Object *o)
	{
		if (index > len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		//if index to insert at is the length, treat as if you're adding to end of array
		if (index == len_)
		{
			assert(false);
		}

		Object* rep = objList_[index];
		objList_[index] = o;

		return rep;
	}

	/** Remove the element at the given index from this Array. Return it */
	Object *remove(size_t index)
	{
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		Object *val = get(index);
		len_ -= 1;

		//move elements over
		for (size_t i = index; i < len_; i += 1)
		{
			objList_[i] = objList_[i + 1];
		}

		return val;
	}

	/**
	 * Get the index of the given object within this Array (using equals method)
	 * Return -1 if not found.
	 */
	int index_of(Object *o)
	{
		for (size_t i = 0; i < length(); i += 1)
		{
			if (get(i)->equals(o))
			{
				return i;
			}
		}

		return -1;
	}

	/** Delete all entries in this Array */
	void clear()
	{
		deleteObjList_();
		capacity_ = 2;
		objList_ = new Object *[capacity_];
		len_ = 0;
	}

	/**
	 * Determine if we have enough space allocated to fit the specified number
	 * of additional elements
	 */
	bool hasRoomForMoreElems_(int numElements)
	{
		return capacity_ >= (len_ + numElements);
	}

	/** Double the capacity of this Array. Entries persist */
	void expandArray_()
	{
		capacity_ *= 2;
		Object **tmp = new Object *[capacity_];
		for (size_t i = 0; i < len_; i += 1)
		{
			tmp[i] = objList_[i];
		}

		delete[] objList_;
		objList_ = tmp;
	}

	/**
	 * Delete all elements in object list and then the list itself
	 */ 
	void deleteObjList_()
	{
		for (int i = 0; i < len_; i++) {
            delete objList_[i];
        }
		delete[] objList_;
	}
};