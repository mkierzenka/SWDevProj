#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/datatype.h"
#include "../serial/serial.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns, the number of rows, and the type of each column.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object
{
public:
	//char *types_;      //owned
	DataType* types_; //owned
	size_t numCols_;
	size_t capCols_;
	size_t numRows_;

	/** Create an empty schema **/
	Schema()
	{
		capCols_ = 2;
		types_ = new DataType[capCols_];
		numCols_ = 0;
		numRows_ = 0;
	}

	/** Copying constructor */
	Schema(Schema &from) : Schema(from, true)
	{
	}

	/** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr argument is
    * undefined. Caller is responsible for deleting types. **/
	Schema(const char *types)
	{
		size_t typesLen = strlen(types);
		capCols_ = typesLen;
		numCols_ = 0;
		numRows_ = 0;
		types_ = new DataType[capCols_];
		for (size_t i = 0; i < capCols_; i++)
		{
			add_column(types[i]);
		}
	}

	/** Constructor that copies over values from passed in schema. If passed in
    * boolean is true, copy over row information as well */
	Schema(Schema &from, bool copyRows)
	{
		capCols_ = (from.width() == 0 ? 2 : from.width()); // from.width != from.capacity
		numCols_ = 0;
		numRows_ = 0;
		types_ = new DataType[capCols_];
		if (from.width() != 0)
		{
			for (int i = 0; i < from.width(); i++)
			{
				add_column(from.col_type(i));
			}
			assert(numCols_ == capCols_);
		}
	}

	~Schema()
	{
		delete[] types_;
	}

	/** Serialize this schema */
	void serialize(Serializer *s)
	{
		s->write(capCols_);
		s->write(numCols_);
		s->write(numRows_);
		for (size_t i = 0; i < numCols_; i++)
		{
			s->write(typeToChar_(types_[i]));
		}
	}

	/** Deserialize a schema from s into this one (mutate) */
	void deserialize(Serializer *s)
	{
		capCols_ = s->readSizeT();
		numCols_ = s->readSizeT();
		numRows_ = s->readSizeT();
		delete[] types_;
		types_ = new DataType[capCols_];
		for (size_t i = 0; i < numCols_; i++)
		{
			types_[i] = charToType_(s->readChar());
		}
	}

	/** Add a column of the given type */
	void add_column(char typ)
	{
		if (numCols_ >= capCols_)
		{
			growColList_();
		}

		types_[numCols_] = charToType_(typ);
		numCols_++;
	}

	/** Add a column of the given type */
	void add_column(DataType typ)
	{
		if (numCols_ >= capCols_)
		{
			growColList_();
		}

		types_[numCols_] = typ;
		numCols_++;
	}

	/** Increment the number of rows tracked by this Schema by 1 */
	void add_row() {
	  add_rows(1);
  	}

	/** Increase the row count of this Schema */
	void add_rows(size_t numRows) {
	  numRows_ += numRows;
  	}

	/** Return char type of column at idx. An idx >= width is undefined. */
	char col_type_char(size_t idx)
	{
		if (idx >= numCols_)
		{
			fprintf(stderr, "Index out of bounds: Bad col index (%zu) in Schema", idx);
			exit(1);
		}
		return typeToChar_(types_[idx]);
	}

	/** Return type of column at idx. An idx >= width is undefined. */
	DataType col_type(size_t idx)
	{
		if (idx >= numCols_)
		{
			fprintf(stderr, "Index out of bounds: Bad col index (%zu) in Schema", idx);
			exit(1);
		}
		return types_[idx];
	}

	/** Set column at current index to given data type. An idx >= width is undefined */
	void set(size_t idx, DataType type)
	{
		if (idx >= numCols_)
		{
			fprintf(stderr, "Index out of bounds: Bad col index (%zu) in Schema", idx);
			exit(1);
		}

		types_[idx] = type;		
	}

	/** The number of columns */
	size_t width()
	{
		return numCols_;
	}

	/** The number of rows */
	size_t length()
	{
		return numRows_;
	}

	bool equals(Object* other)
	{
		Schema* s = dynamic_cast<Schema*>(other);
		if (numCols_ != s->numCols_ || capCols_ != s->capCols_
		|| numRows_ != s->numRows_)
		{
			return false;
		}

		for (size_t i = 0; i < numCols_; i++)
		{
			if (types_[i] != s->col_type(i))
			{
				return false;
			}
		}
	}

	/** Clear out the schema */
	void clear()
	{
		capCols_ = 2;
		delete[] types_;
		types_ = new DataType[capCols_];
		numCols_ = 0;
		numRows_ = 0;
	}

	/** Hash this schema - do not use! */
	size_t hash_me_()
	{
		assert(false);
	}

	void growColList_()
	{
		capCols_ *= 2;
		DataType* newTypesArr = new DataType[capCols_];
		for (size_t i = 0; i < numCols_; i++)
		{
			newTypesArr[i] = types_[i];
		}
		delete[] types_;
		types_ = newTypesArr;
	}

	/** Convert char to data type
	 * NOTE: this logic is duplicated in column; maybe make static util file
	 */
	DataType charToType_(char c)
    {
        switch (c)
        {
        case 'I':
        case 'i':
            return DataType::Integer;
        case 'S':
        case 's':
            return DataType::Str;
        case 'B':
        case 'b':
            return DataType::Boolean;
        case 'D':
        case 'd':
            return DataType::Double;
        default:
            fprintf(stderr, "Unknown char data type: %c\n", c);
            exit(-1);
        }
    }

	/** Return character for given type */
	char typeToChar_(DataType t)
    {
        switch (t)
        {
        case DataType::Integer:
            return 'I';
        case DataType::Str:
            return 'S';
        case DataType::Boolean:
            return 'B';
        case DataType::Double:
            return 'D';
        default:
            fprintf(stderr, "Unknown data type\n");
            exit(-1);
        }
    }
};