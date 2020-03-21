//lang::CwC

#pragma once

#include "../utils/object.h"
#include "../serial/serial.h"
#include "column.h"


/**
 * todo
 *
 * @author broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class ColArray : public Object
{
public:
	Array* colList_; //list of columns, owned

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
			c->deserialize(s);
			colList_->add(c);
		}
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
    //IntColumn *tmp = safeConvertIntCol_(col);
    //return tmp->get(row);

	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_int(row);
  }

  bool get_bool(size_t col, size_t row)
  {
    // BoolColumn *tmp = safeConvertBoolCol_(col);
    // return tmp->get(row);

	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_bool(row);
  }

  float get_float(size_t col, size_t row)
  {
    // FloatColumn *tmp = safeConvertFloatCol_(col);
    // return tmp->get(row);

	Column* c = dynamic_cast<Column*>(colList_->get(col));
	//can check the type in the row
	return c->get_float(row);
  }

  // gets the actual String*, no copy
  String *get_string(size_t col, size_t row)
  {
    // StringColumn *tmp = safeConvertStringCol_(col);
    // return tmp->get(row);

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
	
	/** Return pointer to column at given index as IntColumn
	* Errors and exits if no column at index or of improper type*/
//   IntColumn *safeConvertIntCol_(size_t colIdx)
//   {
//     errorIfOutOfBounds_(colIdx);
//     IntColumn *ic = colList_[colIdx]->as_int();
//     if (ic == nullptr)
//     {
//       fprintf(stderr, "Illegal Column Conversion: column %zu is not an int column", colIdx);
//       exit(1);
//     }
//     return ic;
//   }

//   /** Return pointer to column at given index as BoolColumn
// 	* Errors and exits if no column at index or of improper type*/
//   BoolColumn *safeConvertBoolCol_(size_t colIdx)
//   {
//     errorIfOutOfBounds_(colIdx);
//     BoolColumn *ic = colList_[colIdx]->as_bool();
//     if (ic == nullptr)
//     {
//       fprintf(stderr, "Illegal Column Conversion: column %zu is not an bool column", colIdx);
//       exit(1);
//     }
//     return ic;
//   }

//   /** Return pointer to column at given index as FloatColumn
// 	* Errors and exits if no column at index or of improper type*/
//   FloatColumn *safeConvertFloatCol_(size_t colIdx)
//   {
//     errorIfOutOfBounds_(colIdx);
//     FloatColumn *ic = colList_[colIdx]->as_float();
//     if (ic == nullptr)
//     {
//       fprintf(stderr, "Illegal Column Conversion: column %zu is not a float column", colIdx);
//       exit(1);
//     }
//     return ic;
//   }

//   /** Return pointer to column at given index as StringColumn
// 	* Errors and exits if no column at index or of improper type*/
//   StringColumn *safeConvertStringCol_(size_t colIdx)
//   {
//     errorIfOutOfBounds_(colIdx);
//     StringColumn *ic = colList_[colIdx]->as_string();
//     if (ic == nullptr)
//     {
//       fprintf(stderr, "Illegal Column Conversion: column %zu is not a string column", colIdx);
//       exit(1);
//     }
//     return ic;
//   }
  
  
};