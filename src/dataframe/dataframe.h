#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/helper.h"
#include "../utils/array.h"
#include "../utils/args.h"
#include "../utils/datatypeutils.h"
#include "../store/kvstore.h"
#include "../store/key.h"
#include "../serial/serial.h"
#include "column/columnarray.h"
#include "schema.h"
#include "row/row.h"
#include "row/reader.h"
#include "row/writer.h"

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, D). A dataframe has a schema that
 * describes it. The actual data is not stored within this object, but can be
 * pulled on-demand from a KVStore.
 *
 * @author kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */
class DataFrame : public Object
{
public:
  ColumnArray *columns_; //owned, keeps track of columns in data frame
  KVStore *store_;       //external, used to communicate w/ rest of system
  Schema *schema_;       //owned, schema of dataframe
  Key *key_;             //owned, key for this dataframe in the KV store

  /**
   * This constructor is for the purpose of adding arrays.
   * Steals ownership of Key, not store.
   */
  DataFrame(Key *k, KVStore *kv)
  {
    schema_ = new Schema();
    store_ = kv;
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
  }

  /**
   * Create a data frame from a schema description and columns and a store.
   * All columns are created empty. Steals ownership of Key.
   * Caller responsible for deleting scm.
   */
  DataFrame(const char *scm, Key *k, KVStore *kv)
  {
    schema_ = new Schema(scm);
    store_ = kv;
    size_t numCols = schema_->width();
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
    for (size_t i = 0; i < numCols; i++)
    {
      columns_->add_column(new Column(store_, key_->clone(), scm[i]));
    }
  }

  ~DataFrame()
  {
    delete schema_;
    delete columns_;
    delete key_;
  }

  /**
   * Add array of doubles to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Caller responsible for deleting elements.
   */
  void add_array(size_t numElements, double *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('D');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of ints to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Caller responsible for deleting elements.
   */
  void add_array(size_t numElements, int *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('I');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of booleans to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Caller responsible for deleting elements.
   */
  void add_array(size_t numElements, bool *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('B');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of Strings to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Caller responsible for deleting elements.
   */
  void add_array(size_t numElements, String **elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('S');
    schema_->add_rows(numElements);
  }

  /** Check if two dataframes equal */
  bool equals(Object *o)
  {
    if (this == o)
      return true;

    DataFrame *other = dynamic_cast<DataFrame *>(o);

    return columns_->equals(other->columns_) && schema_->equals(other->schema_) && key_->equals(other->key_);
  }

  /** Returns hash of this Dataframe - deprecated */
  size_t hash()
  {
    assert(false);
  }

  /**
   * Returns the dataframe's schema. Modifying the schema after a dataframe
   * has been created in undefined.
   */
  Schema &get_schema()
  {
    return *schema_;
  }

  /**
   * Returns a pointer to this DataFrame's key. No clone.
   */
  Key *get_key()
  {
    return key_;
  }

  /** Serialize this DataFrame into s*/
  void serialize(Serializer *s)
  {
    columns_->serialize(s);
    schema_->serialize(s);
    key_->serialize(s);
  }

  /** Deserialize as a dataframe and set the values in this dataframe */
  void deserialize(Serializer *s)
  {
    //deserialized column array needs store
    columns_->setStore(store_);
    columns_->deserialize(s);
    schema_->deserialize(s);
    key_->deserialize(s);
  }

  /** Adds the column this dataframe, updates the schema, the new column
   * is external, and appears as the last column of the dataframe, the
   * name is optional and external. A nullptr colum is undefined. */
  void add_column(Column *col)
  {
    if (col == nullptr)
    {
      fprintf(stderr, "Cannot add null column");
      exit(1);
    }
    else if (col->size() != schema_->length())
    {
      fprintf(stderr, "Cannot add column whose length (%zu) != number of rows in dataframe (%zu)\n", col->size(), schema_->length());
      exit(1);
    }

    columns_->add_column(col); //Append to column array
    char type = DataTypeUtils::typeToChar(col->getType());
    schema_->add_column(type);
  }

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row)
  {
    return columns_->get_int(col, row);
  }

  bool get_bool(size_t col, size_t row)
  {
    return columns_->get_bool(col, row);
  }

  double get_double(size_t col, size_t row)
  {
    return columns_->get_double(col, row);
  }

  // String already cloned in Distributed Array. Caller responsible for deleting it.
  String *get_string(size_t col, size_t row)
  {
    return columns_->get_string(col, row);
  }

  /** The number of rows in the dataframe. */
  size_t nrows()
  {
    return schema_->length();
  }

  /** The number of columns in the dataframe.*/
  size_t ncols()
  {
    return schema_->width();
  }

  /**
   * Add rows to this DataFrame built by Writer. Until wr->done().
   * Caller responsible for deleting wr.
   */
  void visit(Writer *wr)
  {
    size_t block_size = args.blockSize;
    Array *rowChunk = new Array(block_size);
    for (size_t i = 0; i < block_size; i++)
    {
      rowChunk->add(new Row(*schema_));
    }

    size_t idxInCurBlock = 0;
    size_t rowIdx = 0;
    while (!wr->done())
    {
      if (idxInCurBlock == block_size)
      {
        addRowChunk_(rowChunk, idxInCurBlock);
        for (size_t i = 0; i < block_size; i++)
        {
          (dynamic_cast<Row *>(rowChunk->get(i)))->clear();
        }
        idxInCurBlock = 0;
      }
      (dynamic_cast<Row *>(rowChunk->get(idxInCurBlock)))->set_idx(rowIdx);
      wr->visit(*(dynamic_cast<Row *>(rowChunk->get(idxInCurBlock))));
      rowIdx++;
      idxInCurBlock++;
    }
    if (idxInCurBlock > 0)
    {
      addRowChunk_(rowChunk, idxInCurBlock);
    }
    delete rowChunk;
  }

  /** Visit rows in order */
  void map(Reader &r)
  {
    map(r, 0, schema_->length());
  }

  /** Visit subset of rows in order */
  void map(Reader &r, size_t startIdx, size_t endIdx)
  {
    Row *row = new Row(*schema_);

    for (size_t rowIdx = startIdx; rowIdx < endIdx; rowIdx++)
    {
      row->set_idx(rowIdx);
      //iterate through each column to get value
      for (size_t colIdx = 0; colIdx < row->width(); colIdx++)
      {
        setRowValByColType_(*row, colIdx, rowIdx, schema_->col_type_char(colIdx));
      }

      r.visit(*row);
      row->clear();
    }

    delete row;
  }

  /** Visit subset of rows in order */
  void local_map(Reader &r)
  {
    Row *row = new Row(*schema_);

    for (size_t rowIdx = 0; rowIdx < schema_->length(); rowIdx++)
    {
      if ((rowIdx / args.blockSize) % args.numNodes == store_->storeId)
      {
        row->set_idx(rowIdx);
        //iterate through each column to get value
        for (size_t colIdx = 0; colIdx < row->width(); colIdx++)
        {
          setRowValByColType_(*row, colIdx, rowIdx, schema_->col_type_char(colIdx));
        }

        r.visit(*row);
        row->clear();
      }
      else
      {
        // skip this block
        rowIdx += (args.blockSize - 1); //forloop will increment rowIdx an extra time before continuing
      }
    }

    delete row;
  }

  /** Helper that allows program to error and exit if index-out-of-bounds */
  void errorIfOutOfBounds_(size_t colIdx)
  {
    if (colIdx >= schema_->width())
    {
      fprintf(stderr, "Out-Of-Bounds Error: cannot get column from index %zu", colIdx);
      exit(1);
    }
  }

  /** Error and exit if number of entries in row does not equal number of columns
   * in schema */
  void checkRowLen_(size_t rowWidth)
  {
    if (rowWidth != schema_->width())
    {
      fprintf(stderr, "Cannot have row of %zu entries in schema of %zu columns",
              rowWidth, schema_->width());
      exit(1);
    }
  }

  /** Checks for matching col types. Error and exit if not the same */
  void checkColTypes_(char colTypeFromRow, size_t schemaColIdx)
  {
    char colTypeFromSchema = schema_->col_type_char(schemaColIdx);
    if (colTypeFromRow != colTypeFromSchema)
    {
      fprintf(stderr, "Row's column type \"%c\" at index %zu does not match dataframe's column type \"%c\"", colTypeFromRow, schemaColIdx, colTypeFromSchema);
      exit(1);
    }
  }

  /** Create Row object from the DataFrame info */
  Row *createRow_(size_t rowIdx)
  {
    Row *r = new Row(*schema_);
    r->set_idx(rowIdx);

    //iterate through each column to get value
    for (size_t i = 0; i < r->width(); i++)
    {
      setRowValByColType_(*r, i, rowIdx, schema_->col_type_char(i));
    }

    return r;
  }

  /** Set value in Row, given the column type */
  void setRowValByColType_(Row &r, size_t colIdx, size_t rowIdx, char colType)
  {
    switch (colType)
    {
    case 'I':
    {
      r.set(colIdx, columns_->get_int(colIdx, rowIdx));
      break;
    }
    case 'B':
    {
      r.set(colIdx, columns_->get_bool(colIdx, rowIdx));
      break;
    }
    case 'D':
    {
      r.set(colIdx, columns_->get_double(colIdx, rowIdx));
      break;
    }
    case 'S':
    {
      String *str = columns_->get_string(colIdx, rowIdx);
      //row clones string
      r.set(colIdx, str);
      delete str;
      break;
    }
    default:
      fprintf(stderr, "Invalid col type: %c", colType);
    }
  }

  /**
   * Appends a set of rows to this DataFrame, in order. Does it by making temporary arrays
   *   and adding them to specific columns in one shot. The length of rowChunk nor numRowsInChunk
   *   should not exceed args.blockSize
   * 
   * Approach: For each column in the schema, get the type and make a temporary (c-style) array of those values
   *            Iterate through each of the rows in the array, pulling a copy of the value into the c-style arr
   *            Add them in one shot directly into the necessary column using add_all
   * 
   * rowChunk - an Array of Row* that should be used to add data to this DataFrame
   * numRowsInChunk - how many rows to take from this chunk (array may contain more)
   */
  void addRowChunk_(Array *rowChunk, size_t numRowsInChunk)
  {
    for (size_t i = 0; i < schema_->width(); i++)
    {
      char colType = schema_->col_type_char(i);
      switch (colType)
      {
      case 'I':
      {
        int *ints = new int[numRowsInChunk];
        for (size_t j = 0; j < numRowsInChunk; j++)
        {
          ints[j] = (dynamic_cast<Row *>(rowChunk->get(j)))->get_int(i);
        }
        columns_->get(i)->add_all(numRowsInChunk, ints, i);
        delete[] ints;
        break;
      }
      case 'B':
      {
        bool *bools = new bool[numRowsInChunk];
        for (size_t j = 0; j < numRowsInChunk; j++)
        {
          bools[j] = (dynamic_cast<Row *>(rowChunk->get(j)))->get_bool(i);
        }
        columns_->get(i)->add_all(numRowsInChunk, bools, i);
        delete[] bools;
        break;
      }
      case 'S':
      {
        String **strs = new String *[numRowsInChunk];
        size_t strsLen = 0;
        for (size_t j = 0; j < numRowsInChunk; j++)
        {
          strs[j] = (dynamic_cast<Row *>(rowChunk->get(j)))->get_string(i)->clone();
          strsLen++;
        }
        columns_->get(i)->add_all(numRowsInChunk, strs, i);
        for (size_t k = 0; k < strsLen; k++)
        {
          delete strs[k];
        }

        delete[] strs;
        break;
      }
      case 'D':
      {
        double *dbls = new double[numRowsInChunk];
        for (size_t j = 0; j < numRowsInChunk; j++)
        {
          dbls[j] = (dynamic_cast<Row *>(rowChunk->get(j)))->get_double(i);
        }
        columns_->get(i)->add_all(numRowsInChunk, dbls, i);
        delete[] dbls;
        break;
      }
      default:
      {
        fprintf(stderr, "Bad schema column type (%c) in addRowChunk_\n", colType);
        exit(1);
      }
      }
    }
    schema_->add_rows(numRowsInChunk);
  }
};