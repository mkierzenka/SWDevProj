//lang::CwC

#pragma once

#include "object.h"
#include <vector>

/**
* Array - to represent a list of objects. Array owns its elements
*
*Specification by Yiwen Ma & Viswajith Gopinathan
*Authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
*/
class Array : public Object
{
public:
	std::vector<Object*> inner_;
	
	Array(size_t initialCapacity)
	{
		inner_.reserve(initialCapacity);
	}
	
	Array() {}

	~Array() {
		deleteObjList_();
	}

	/** Return length of this Array */
	size_t length()
	{
		return inner_.size();
	}

	/** Return true if this Array equals the other Object */
	bool equals(Object *other)
	{
		if (this == other)
		{
			return true;
		}
		Array *o = dynamic_cast<Array *>(other);
		if (!o || length() != o->length())
		{
			return false;
		}
		auto myElems = inner_.begin();
		auto otherElems = o->inner_.begin();
		size_t len = length();
		for (size_t i = 0; i < len; i++)
		{
			Object* myE = *(myElems + i);
			Object* otherE = *(otherElems + i);
			if (!(myE->equals(otherE)))
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
		for (Object* item : inner_ )
		{
			size_t elementhash = item->hash();
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
		return inner_.at(index);
	}

	/** Add the object to the end of this Array. Array owns this object */
	void add(Object *o)
	{
		inner_.push_back(o);
	}

	/** Set the element at the given index to the specified object. Return the old value.
	 * Caller responsible for deleting returned value
	 */
	Object* set(size_t index, Object *o)
	{
		Object* old = inner_.at(index);
		inner_.at(index) = o;
		return old;
	}

	/** Remove the element at the given index from this Array. Return it.
	 * Caller responsible for deleting returned value
	 */
	Object *remove(size_t index)
	{
		Object* val = get(index);
		inner_.erase(inner_.begin() + index);
		return val;
	}

	/**
	 * Get the index of the given object within this Array (using equals method)
	 * Return -1 if not found.
	 */
	int index_of(Object *o)
	{
		int i = 0;
		auto myElems = inner_.begin();
		size_t len = length();
		for (size_t i = 0; i < len; i++)
		{
			Object* elem = *(myElems + i);
			if (elem->equals(o))
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
		inner_.clear();
	}

	/**
	 * Delete all elements in object list and then the list itself
	 */
	void deleteObjList_()
	{
		for (Object* item : inner_)
		{
			delete item;
		}
	}
};