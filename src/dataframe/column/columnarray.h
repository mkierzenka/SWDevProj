//lang::CwC

#pragma once

#include "../../utils/object.h"
#include "../../utils/array.h"
#include "../../store/kvstore.h"
#include "../../serial/serial.h"
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
	KVStore *store_;  //external
	Array *colList_;  //owned, the columns in this ColumnArray
	Key *dfKey_;      //external, key of dataframe this column array belongs to

    /**
     * Constructs a new ColumnArray from external arguments (not cloned).
     * Do not delete store and k until done with this ColumnArray.
     */
	ColumnArray(KVStore *store, Key *k)
	{
		store_ = store;
		dfKey_ = k;
		colList_ = new Array();
	}

	~ColumnArray()
	{
		delete colList_;
	}

	/** Serialize a ColumnArray */
	void serialize(Serializer *s)
	{
		size_t len = colList_->length();
		s->write(len);
		for (size_t i = 0; i < len; i++)
		{
			(dynamic_cast<Column *>(colList_->get(i)))->serialize(s);
		}
		dfKey_->serialize(s);
	}

	/** Deserialize as a ColumnArray, set values into this ColumnArray (mutates) */
	void deserialize(Serializer *s)
	{
		assert(store_); //need to pass KVStore into columns: assume KVStore is already set
		size_t len = s->readSizeT();
		colList_->clear();
		for (size_t i = 0; i < len; i++)
		{
			Column *c = new Column();
			c->setStore(store_);
			c->deserialize(s);
			colList_->add(c);
		}
		dfKey_->deserialize(s);
	}

	/** Get the number of columns in this array */
	size_t length()
	{
		return colList_->length();
	}

	/** Set this column array's store (no clone) */
	void setStore(KVStore *store)
	{
		store_ = store;
	}

	/** Check if this ColumnArray equals another Object */
	bool equals(Object *other) override
	{
		if (this == other)
		{
			return true;
		}
		ColumnArray *ca = dynamic_cast<ColumnArray *>(other);

		// store_ is not included
		if (!ca || !(colList_->equals(ca->colList_)) || !(dfKey_->equals(ca->dfKey_)))
		{
			return false;
		}
		return true;
	}

	size_t hash_me() override
	{
		size_t hash = 0;
		hash += colList_->hash();
		hash += dfKey_->hash();
		return hash;
	}

	/** Return a pointer to the specified column (does not clone!) */
	Column *get(size_t index)
	{
		Object *colObj = colList_->get(index);
		return dynamic_cast<Column *>(colObj);
	}

	/**
	 * Appends a column to this ColumnArray. Does not make copy.
	 */
	void add_column(Column *c)
	{
		colList_->add(c);
	}

	/**
	 * Appends new column of specified type to this columnarray.
	 */
	void add_column(const char colType)
	{
		add_column(new Column(store_, dfKey_->addIndex(length()), colType));
	}

	/**
	 * Create a new column of the given type with specified elements.
	 * Caller responsible for deleting elements argument.
	 */
	void add_column_fromarray(size_t numElements, double *elements)
	{
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Double);
		c->add_all(numElements, elements, colList_->length());
		add_column(c); //add column to this columnarray
	}

	/**
	 * Create a new column of the given type with specified elements.
	 * Caller responsible for deleting elements argument.
	 */
	void add_column_fromarray(size_t numElements, int *elements)
	{
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Integer);
		c->add_all(numElements, elements, colList_->length());
		add_column(c);
	}

	/**
	 * Create a new column of the given type with specified elements.
	 * Caller responsible for deleting elements argument.
	 */
	void add_column_fromarray(size_t numElements, bool *elements)
	{
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Boolean);
		c->add_all(numElements, elements, colList_->length());
		add_column(c);
	}

	/**
	 * Create a new column of the given type with specified elements.
	 * Caller responsible for deleting elements argument.
	 */
	void add_column_fromarray(size_t numElements, String **elements)
	{
		Column *c = new Column(store_, dfKey_->addIndex(length()), ColType::Str);
		c->add_all(numElements, elements, colList_->length());
		add_column(c);
	}

	// ========== METHODS WORKING WITH ELEMENTS IN COLUMNS ==========

	/** Return the value at the given column and row. Accessing rows or
      *  columns out of bounds, or request the wrong type is undefined.*/
	int get_int(size_t col, size_t row)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		if (c == nullptr)
		{
			fprintf(stderr, "Cannot get int from null column\n");
		}

		return c->get_int(row, col);
	}

	bool get_bool(size_t col, size_t row)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		if (c == nullptr)
		{
			fprintf(stderr, "Cannot get bool from null column\n");
		}
		
		return c->get_bool(row, col);
	}

	double get_double(size_t col, size_t row)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		if (c == nullptr)
		{
			fprintf(stderr, "Cannot get double from null column\n");
		}
		
		return c->get_double(row, col);
	}

	// gets the actual String*, no copy
	String *get_string(size_t col, size_t row)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		if (c == nullptr)
		{
			fprintf(stderr, "Cannot get string from null column\n");
		}

		return c->get_string(row, col);
	}

	/** Type appropriate push_back methods. Appends the element to the end of the
	  * specified column. Calling the wrong method is undefined behavior. **/
/*	void push_back(size_t col, int val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		return c->push_back(val);
	}

	void push_back(size_t col, bool val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		return c->push_back(val);
	}

	void push_back(size_t col, double val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		return c->push_back(val);
	}

	void push_back(size_t col, String *val)
	{
		Column *c = dynamic_cast<Column *>(colList_->get(col));
		return c->push_back(val);
	}*/
};