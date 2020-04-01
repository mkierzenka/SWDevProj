//lang::CwC

#pragma once

#include "object.h"
//#include "../serial/serial.h"

/**
* Array - to represent a list of objects.
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

	// get the length of this array
	size_t length()
	{
		return len_;
	}

	// check if this array equals to other array
	bool equals(Object *other)
	{
		if (this == other)
		{
			return true;
		}

		Array *o = dynamic_cast<Array *>(other);
		if (o == nullptr || this->length() != o->length())
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

	// hash_me override
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

	// append two Arrays
	Array *append(Array *s)
	{
		//check if we need to expand array
		while (!hasRoomForMoreElems_(s->length()))
		{
			expandArray_();
		}

		//copy in elements from new string
		for (size_t i = 0; i < s->length(); i += 1)
		{
			add(s->get(i));
		}

		//return updated Array
		return this;
	}

	// get the object with the index in the array
	Object *get(size_t index)
	{
		// check for out-of-bounds
		if (index >= length())
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		return objList_[index];
	}

	// add the object to the end of the array
	void add(Object *o)
	{
		if (!hasRoomForMoreElems_(1))
		{
			expandArray_();
		}

		//set value
		objList_[len_] = o;
		//update list size
		len_ += 1;
	}

	// set the element in the given index to the given object
	void set(size_t index, Object *o)
	{
		// check for out-of-bounds
		if (index > len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		//if index to insert at is the length, treat as if you're adding to end of array
		if (index == len_)
		{
			add(o);
		}

		objList_[index] = o;
	}

	// remove the element with the given index
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

	// get the index of the given object
	int index_of(Object *o)
	{
		for (size_t i = 0; i < length(); i += 1)
		{
			if (get(i)->equals(o))
			{
				return i;
			}
		}

		//if element not in array, return -1
		return -1;
	}

	// remove all elements in the array
	void clear()
	{
		deleteObjList_();
		capacity_ = 2;
		objList_ = new Object *[capacity_];
		len_ = 0;
	}

	// determine if we have enough space allocated to fit the specified number of additional elements
	bool hasRoomForMoreElems_(int numElements)
	{
		return capacity_ >= (len_ + numElements);
	}

	//double size of allocated array memory
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