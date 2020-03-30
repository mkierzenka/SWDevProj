//lang::CwC

#pragma once

#include "../utils/object.h"
#include "../utils/array.h"
#include "../store/kvstore.h"
#include "../serial/serial.h"
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
	KVStore *store_; // NOT OWNED
	Array *colList_;
	Key *dfKey_; //key of dataframe this column array belongs to

	// constructor
	ColumnArray(KVStore *store, Key *k)
	{
		store_ = store;
		dfKey_ = k;
		colList_ = new Array();
	}

	// destructor
	~ColumnArray()
	{
		delete colList_;
	}

	/** Serialize a ColumnArray into char* representation */
	void serialize(Serializer *s)
	{
		//since using standard array, need to cast to column and serialize
		//each of those
		size_t len = colList_->length();
		s->write(len);
		for (size_t i = 0; i < len; i++)
		{
			(dynamic_cast<Column *>(colList_->get(i)))->serialize(s);
		}

		dfKey_->serialize(s);
	}

	/** Deserialize as a ColumnArray, set values into this ColumnArray*/
	void deserialize(Serializer *s)
	{
		//need to pass on KVStore to columns: assuming KVStore here is already set
		assert(store_ != nullptr);
		size_t len = s->readSizeT();
		for (size_t i = 0; i < len; i++)
		{
			Column *c = new Column();
			c->setStore(store_);
			c->deserialize(s);
			colList_->add(c);
		}

		dfKey_->deserialize(s);
	}

	/** get the length of this array */
	size_t length()
	{
		return colList_->length();
	}

	/**Set column array's store */
	void setStore(KVStore *store)
	{
		store_ = store;
	}

	/** Check if two column arrays are equal */
	bool equals(Object *other)
	{
		if (this == other)
		{
			return true;
		}

		ColumnArray *ca = dynamic_cast<ColumnArray *>(other);

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
		Object *colObj = colList_->get(index);
		return dynamic_cast<Column *>(colObj);
	}

	/**
	 * Appends column to this columnarray. Does not make copy
	 */
	void add_column(Column *c)
	{
		colList_->add(c);
	}

	/** Create a new column of the given types and add in the elements, chunks at a
	 * time */
	void add_column_fromarray(size_t numElements, double *elements)
	{
		//create column
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Double); //column steals ownership

		//add all data to the column
		c->add_all(numElements, elements);

		//add column to column array
		add_column(c);
	}

	/** Create a new column of the given types and add in the elements, chunks at a
	 * time */
	void add_column_fromarray(size_t numElements, int *elements)
	{
		//create column
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Integer); //column steals ownership

		//add all data to the column
		c->add_all(numElements, elements);

		//add column to column array
		add_column(c);
	}

	/** Create a new column of the given types and add in the elements, chunks at a
	 * time */
	void add_column_fromarray(size_t numElements, bool *elements)
	{
		//create column
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Boolean); //column steals ownership

		//add all data to the column
		c->add_all(numElements, elements);

		//add column to column array
		add_column(c);
	}

	/** Create a new column of the given types and add in the elements, chunks at a
	 * time */
	void add_column_fromarray(size_t numElements, String **elements)
	{
		//create column
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Str); //column steals ownership

		//add all data to the column
		c->add_all(numElements, elements);

		//add column to column array
		add_column(c);
	}

	// get the index of the given Column
	int index_of(Column *c)
	{
		//casting needed?
		return colList_->index_of(c);
	}

	// ========== METHODS WORKING WITH ELEMENTS IN COLUMNS ==========

	/** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
	int get_int(size_t col, size_t row)
	{
		//return colList_->get_int(col, row);
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->get_int(row);
	}

	bool get_bool(size_t col, size_t row)
	{
		//return colList_->get_bool(col, row);
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->get_bool(row);
	}

	double get_double(size_t col, size_t row)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		if (c == nullptr)
		{
			fprintf(stderr, "Cannot get double from null column\n");
		}
		
		//can check the type in the row
		return c->get_double(row);
	}

	// gets the actual String*, no copy
	String *get_string(size_t col, size_t row)
	{
		//return colList_->get_string(col, row);
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->get_string(row);
	}

	/** Type appropriate push_back methods. Appends the element to the end of the
	* specified column. Calling the wrong method is undefined behavior. **/
	void push_back(size_t col, int val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

	void push_back(size_t col, bool val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

	void push_back(size_t col, double val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}

	void push_back(size_t col, String *val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		//can check the type in the row
		return c->push_back(val);
	}
};