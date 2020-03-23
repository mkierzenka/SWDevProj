//lang::CwC

#pragma once

#include "../utils/object.h"
#include "../store/kvstore.h"
#include "../serial/serial.h"
#include "colarr.h"
#include "column.h"
#include <assert.h>

/**
 * ColumnArray - to represent a list of column.
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class ColumnArray : public Object
{
public:
	KVStore* store_; // NOT OWNED
	//Column **colList_; //list of columns, owned
	ColArray* colList_; //list of columns, owned
	Key* dfKey_; //key of dataframe this column array belongs to
	//DistributedArray* colList_; //owned

	// constructor
	ColumnArray(KVStore* store, Key* k)
	{
		store_ = store;
		dfKey_ = k;
		//capacity_ = 2;
		//colList_ = new Column*[capacity_];
		colList_ = new ColArray();
		//colList_ = new DistributedArray(store_);
		//len_ = 0;
	}

	// destructor
	~ColumnArray()
	{
		//deleteColList_();
		delete colList_;
	}

	/** Serialize a ColumnArray into char* representation */
	void serialize(Serializer* s)
	{
		//will elements be serialized as objects or columns?
		//colList_->serializeAsColumnArray(s);
		colList_->serialize(s);
		dfKey_->serialize(s);
	}

	/** Deserialize as a ColumnArray, set values into this ColumnArray*/
	void deserialize(Serializer* s)
	{
		//need to pass on KVStore to columns: assuming KVStore here is already set
		assert(store_ != nullptr);
		colList_->setStore(store_);
		colList_->deserialize(s);
		dfKey_->deserialize(s);
	}
	
	/** get the length of this array */
	size_t length()
	{
		return colList_->length();
	}

	/**Set column array's store */
	void setStore(KVStore* store)
	{
		store_ = store;
	}

	/** Check if two column arrays are equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		ColumnArray* ca = dynamic_cast<ColumnArray*>(other);
		
		// store_ is not included
		if (ca == nullptr || !(colList_->equals(ca->colList_)) || !(dfKey_->equals(ca->dfKey_)))
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
		hash += dfKey_->hash();
		return hash;
	}

	// get the specified column (does not make a copy!)
	Column *get(size_t index)
	{
		Object* colObj = colList_->get(index);
		return dynamic_cast<Column*>(colObj);
	}
	
	/**
	 * Get the type of the specified column. Exits if invalid index
	 */
	/*char getType(size_t index) {
		Column* col = get(index); // <-- will error if invalid index
		return col->get_type();
		/*IntColumn *ic = col->as_int();
		FloatColumn *fc = col->as_float();
		BoolColumn *bc = col->as_bool();
		StringColumn *sc = col->as_string();
		if (ic != nullptr)
		{
		  return ic->get_type();
		}
		else if (fc != nullptr)
		{
		  return fc->get_type();
		}
		else if (bc != nullptr)
		{
		  return bc->get_type();
		}
		else if (sc != nullptr)
		{
		  return sc->get_type();
		}
		//doesn't match any types, so delegate to Column
		return col->get_type();*/
	//}

	/**
	 * Appends column to this columnarray. Does not make copy
	 */
	void add_column(Column *c)
	{
		colList_->add_column(c);
	}

	/** Create a new column of the given types and add in the elements, chunks at a
	 * time */
	void add_column_fromarray(size_t numElements, float* elements)
	{	
		//create column
		Column* c = new Column(store_, dfKey_->addIndex(length()), ColType::Float); //column steals ownership

		//add column to column array
		add_column(c);

		//add all data to the column
		c->add_all(numElements, elements);
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

  float get_float(size_t col, size_t row)
  {
    Column* c = colList_->get(col);
	//can check the type in the row
	return c->get_float(row);
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

//   void set(size_t col, size_t row, float val)
//   {
// 	FloatColumn *tmp = safeConvertFloatCol_(col);
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
		//IntColumn *tmp = safeConvertIntCol_(col);
		//tmp->push_back(val);

		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, bool val) {
		//BoolColumn *tmp = safeConvertBoolCol_(col);
		//tmp->push_back(val);
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, float val) {
		//FloatColumn *tmp = safeConvertFloatCol_(col);
		//tmp->push_back(val);
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

    void push_back(size_t col, String *val) {
		//StringColumn *tmp = safeConvertStringCol_(col);
		//tmp->push_back(val);
		Column* c = dynamic_cast<Column*>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}
	
	/*void errorIfOutOfBounds_(size_t colIdx) {
		if (colIdx >= len_) {
		  fprintf(stderr, "Out-Of-Bounds Error: cannot get column from index %zu", colIdx);
		  exit(1);
		}
	}*/
  
};