//lang::CwC

#pragma once

#include "../utils/object.h"
#include "../serial/serial.h"
#include "column.h"


/**
 * This class represents a ColArray object, which manages an array of Column objects
 * within a Distributed KV store.
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class ColArray : public Object
{
public:
	Array* colList_; //list of columns, owned
	KVStore* store_; //kvstore; not positive how to handle it here

	// constructor
	ColArray() : Object()
	{
		colList_ = new Array();
	}

	// destructor
	~ColArray()
	{
		//deleteColList_();
		delete colList_;
	}

	/** Set KVStore */
	void setStore(KVStore* store)
	{
		store_ = store;
	}

	/** Serialize a ColArray into char* representation */
	void serialize(Serializer* s)
	{
		//will elements be serialized as objects or columns?
		//colList_->serializeAsColumnArray(s);
		size_t len = colList_->length();
		s->write(len);
		for (size_t i = 0; i < len; i++) {
			(dynamic_cast<Column*>(colList_->get(i)))->serialize(s);
		}
	}

	/** Deserialize as a ColArray, set values into this ColumnArray*/
	void deserialize(Serializer* s)
	{
		size_t len = s->readSizeT();
		for (size_t i = 0; i < len; i++) {
			Column* c = new Column();
			c->setStore(store_);
			c->deserialize(s);
			colList_->add(c);
		}
	}

	/** Check if two colarrays are equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		ColArray* ca = dynamic_cast<ColArray*>(other);
		
		// store_ is not included
		if (ca == nullptr || !(colList_->equals(ca->colList_)))
		{
			return false;
		}

		return true;
	}

	// hash_me override
	size_t hash_me()
	{
		size_t hash = 0;
		hash += colList_->hash();
		return hash;
	}
	
	// get the length of this array
	size_t length()
	{
		return colList_->length();
	}

	// get the specified column (does not make a copy!)
	Column *get(size_t index)
	{
		Object* colObj = colList_->get(index);
		return dynamic_cast<Column*>(colObj);
	}
	

	/**
	 * Appends column to this columnarray. Does not make copy
	 */
	void add_column(Column *c)
	{
		colList_->add(c);
	}

	// get the index of the given Column
	int index_of(Column *c)
	{
		//casting needed?
		return colList_->index_of(c);
	}

	// remove all elements in the array
/*	void clear()
	{
		deleteColList_();
		capacity_ = 2;
		colList_ = new Column *[capacity_];
		len_ = 0;
	}*/
	// ========== METHODS WORKING WITH ELEMENTS IN COLUMNS ==========

	
	/** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row)
  {
	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_int(row);
  }

  bool get_bool(size_t col, size_t row)
  {
	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_bool(row);
  }

  double get_double(size_t col, size_t row)
  {
	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_double(row);
  }

  // gets the actual String*, no copy
  String *get_string(size_t col, size_t row)
  {
	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_string(row);
  }
	
	/** Set the value at the given column and row to the given value.
    * If the column is not  of the right type or the indices are out of
    * bound, the result is undefined. */
//   void set(size_t col, size_t row, int val)
//   {
//     IntColumn *tmp = safeConvertIntCol_(col);
//     tmp->set(row, val);
//   }

//   void set(size_t col, size_t row, bool val)
//   {
//     BoolColumn *tmp = safeConvertBoolCol_(col);
//     tmp->set(row, val);
//   }

//   void set(size_t col, size_t row, double val)
//   {
// 	DoubleColumn *tmp = safeConvertDoubleCol_(col);
//     tmp->set(row, val);
//   }

//   void set(size_t col, size_t row, String *val)
//   {
// 	StringColumn *tmp = safeConvertStringCol_(col);
//     tmp->set(row, val);
//   }
	
	
	/** Type appropriate push_back methods. Appends the element to the end of the
	* specified column. Calling the wrong method is undefined behavior. **/
    void push_back(size_t col, int val) {
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, bool val) {
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, double val) {
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, String *val) {
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}
};