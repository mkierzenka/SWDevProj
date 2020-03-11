//lang::CwC

#pragma once

#include "object.h"
#include "string.h"
#include <math.h>

/**
 * DoubleArray - to represent a list of doubles.
 *
 *
*/
class DoubleArray : public Object
{
public:
	double* dblList_; //list of doubles, owned
	size_t capacity_;  //how many elements allocated to size
	size_t len_;	   //length of array

	// constructor
	DoubleArray() : Object()
	{
		capacity_ = 1;
		dblList_ = new double[capacity_];
		len_ = 0;
	}

	// deconstructor
	~DoubleArray()
	{
		delete[] dblList_;
	}

	void serialize(Serializer* s) {
		s->write(len_);
		for (int i = 0; i < len_; i++) {
			s->write(dblList_[i]);
		}
	}
	
	void deserialize(Serializer* ser) {
		delete[] dblList_;
		capacity_ = ser->readSizeT();
		dblList_ = new double[capacity_];
		len_ = capacity_;
		for (int i=0; i < len_; i++) {
			dblList_[i] = ser->readDouble();
		}
	}

	// get the length of this array
	size_t length()
	{
		return len_;
	}

	// check if this array equals to other array
	bool equals(Object *other)
	{
		//same memory address: must be equal
		if (this == other)
		{
			return true;
		}

		DoubleArray *o = dynamic_cast<DoubleArray *>(other);
		if (!o)
		{
			return false;
		}

		//arrays cannot be equal if not the same length
		if (len_ != o->len_)
		{
			return false;
		}

		//return false if any of elements not same
		for (size_t i = 0; i < len_; i += 1)
		{
			if (get(i) == o->get(i))
			{
				return false;
			}
		}

		//arrays equal
		return true;
	}

	// hash_me override
	size_t hash_me()
	{
		size_t hash = 0;
		for (size_t i = 0; i < len_; i += 1)
		{
			size_t elementhash = floor(dblList_[i]);
			hash += elementhash + (hash << 3) - (hash << 1);
		}

		return hash;
	}

	// append two Arrays
	DoubleArray *append(DoubleArray *s)
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

	// get the double with the index in the array
	double get(size_t index)
	{
		// check for out-of-bounds
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
			exit(1);
		}

		return dblList_[index];
	}

	// add the double to the end of the array
	void add(double d)
	{
		if (!hasRoomForMoreElems_(1))
		{
			expandArray_();
		}

		//set value
		dblList_[len_] = d;
		//update list size
		len_ += 1;
	}

	// set the element in the given index to the given double
	void set(size_t index, double d)
	{
		// check for out-of-bounds
		if (index > len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
		}

		//if index to insert at is the length, treat as if you're adding to end of array
		if (index == len_)
		{
			add(d);
		}

		dblList_[index] = d;
	}

	// remove the element with the given index and return it
	double remove(size_t index)
	{
		// check for out-of-bounds
		if (index >= len_)
		{
			printf("Out-Of-Bounds Error: cannot get value from index %zu", index);
		}

		//get value to be removed
		double val = get(index);

		//decrement length
		len_ -= 1;

		//move elements over
		for (size_t i = index; i < len_; i += 1)
		{
			dblList_[i] = dblList_[i + 1];
		}

		return val;
	}

	// get the index of the given double
	int index_of(double d)
	{
		for (size_t i = 0; i < len_; i += 1)
		{
			if (get(i) == d)
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
		delete[] dblList_;
		capacity_ = 1;
		dblList_ = new double[capacity_];
		len_ = 0;
	}
	
	// prints all elements in this array
	void print() {
		printf("[");
		for (size_t i = 0; i < len_ - 1; i++) {
			printf("%lf ", dblList_[i]);
		}
		printf("%lf]", dblList_[len_ - 1]);
	}

	// determine if we have enough space allocated to fit the specified number of additional elements
	bool hasRoomForMoreElems_(int numElements)
	{
		return capacity_ >= (len_ + numElements);
	}

	//double size of allocated array memory
	void expandArray_()
	{
		//double list size
		capacity_ *= 2;
		double *tmp = new double[capacity_];
		for (size_t i = 0; i < len_; i += 1)
		{
			tmp[i] = dblList_[i];
		}

		//delete old reference and set updated pointer
		delete[] dblList_;
		dblList_ = tmp;
	}
};