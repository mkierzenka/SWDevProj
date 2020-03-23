#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/helper.h"
#include "../store/kvstore.h"
#include "../store/key.h"
#include "../serial/serial.h"

#include "columnarray.h"

#include "schema.h"
#include "rower.h"
#include "row.h"
#include "thread.h"

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 *
 * @author kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */
class DataFrame : public Object
{
public:
  ColumnArray *columns_; //keeps track of columns in data frame
  KVStore* store_;
  Schema* schema_;        //owned, schema of dataframe
  Key* key_; //key for this dataframe in the KV store

  /** This constructor is for the purpose of adding arrays */
  DataFrame(Key* k, KVStore* kv)
  {
    schema_ = new Schema();
    store_ = kv;
    size_t numCols = schema_->width();
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
  }

  /** This constructor is for the purpose of deserializing */
  DataFrame(Key* k)
  {
    schema_ = new Schema();
    key_ = k;
    store_ = nullptr;
    columns_ = new ColumnArray(nullptr, key_);
  }

  /** Create a data frame with the same columns as the give df but no rows */
  DataFrame(DataFrame &df, Key* k) : DataFrame(df.get_schema(), k)
  {
  }

  /** Create a data frame from a schema and columns. All columns are created
    * empty. */
  DataFrame(Schema &schema, Key* k)
  {
    //don't copy rows
    schema_ = new Schema(schema, false);
    size_t numCols = schema_->width();
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
    /*for (int i = 0; i < numCols; i++)
    {
	    columns_->addNew(schema_->col_type(i));
    }*/
  }

  ~DataFrame()
  {
    delete schema_;
	  delete columns_;
  }

  /** Converts an array into a dataframe object. Returns the df result */
  static DataFrame* fromArray(Key* k, KVStore* kv, size_t numElems, float* elems) {
    //initialize dataframe
    DataFrame* df = new DataFrame(k, kv);
    df->add_array(numElems, elems);
    Serializer* s = new Serializer();
    df->serialize(s);
    Value* v = new Value(s->getBuffer(), s->getNumBytesWritten());
    kv->put(k, v);

    delete s;
    return df;
  }

  /** Add array to dataframe as a column. Add the data into chunks at a time, and generate
   * keys for them. Column needs to get dataframe's key and key-value store */
  void add_array(size_t numElements, float* elements)
  {
    columns_->add_column_fromarray(numElements, elements);
  }


  /** Returns the dataframe's schema. Modifying the schema after a dataframe
    * has been created in undefined. */
  Schema &get_schema()
  {
    return *schema_;
  }

  // Serialize store?
  /** Serialize this DataFrame into s*/
  void serialize(Serializer* s) {
    columns_->serialize(s);
    schema_->serialize(s);
    key_->serialize(s);
  }

  
  //Deserialize store?
  /** Deserialize as a dataframe and set the values in this dataframe */
  void deserialize(Serializer* s) {
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

	columns_->add_column(col); // Must add to end of column array
    char type = col->getCharType();//columns_->getType(columns_->length() - 1);
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

  float get_float(size_t col, size_t row)
  {
    return columns_->get_float(col, row);
  }

  String *get_string(size_t col, size_t row)
  {
    return columns_->get_string(col, row);
  }

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undedined.  */
  void add_row(Row &row)
  {
    size_t rowWidth = row.width();
    checkRowLen_(rowWidth);
    for (int i = 0; i < rowWidth; i++)
    {
      checkColTypes_(row.col_type(i), i);
      addToEndOfColByType_(i, row);
    }

    //Row does not have a name
    schema_->add_row();
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

  /** Visit rows in order */
  void map(Rower &r)
  {
    map(r, 0, schema_->length());
  }

  /** Visit subset of rows in order */
  void map(Rower &r, size_t startIdx, size_t endIdx)
  {
    Row *row = new Row(*schema_);

    for (size_t rowIdx = startIdx; rowIdx < endIdx; rowIdx++)
    {
      row->set_idx(rowIdx);
      //iterate through each column to get value
      for (int colIdx = 0; colIdx < row->width(); colIdx++)
      {
        setRowValByColType_(*row, colIdx, rowIdx, schema_->col_type(colIdx));
      }

      r.accept(*row);
      row->clear();
    }

    delete row;
  }

  /** An inner class of DataFrame representing a Thread for doing work on a contiguous
	 *    subset of rows of a DataFrame.
	 *  
	 *  author: kierzenka.m@husky.neu.edu */
  class DataFrameThread : public Thread
  {
  public:
    size_t startRowIdx_; // inclusive
    size_t endRowIdx_;   // exclusive
    DataFrame *df_;
    Rower &r_;

    DataFrameThread(DataFrame *df, Rower &r, size_t startRowIdx, size_t endRowIdx) :
		df_(df), r_(r), startRowIdx_(startRowIdx), endRowIdx_(endRowIdx) {}

    void run()
    {
      df_->map(r_, startRowIdx_, endRowIdx_);
    }
	
	Rower& getRower() {
		return r_;
	}
  };

  /**
   * This method clones the Rower and executes the map in parallel over
   * the specified number of threads.
   */
  void pmap(Rower &r, size_t numThreads)
  {
    if (numThreads == 0)
    {
      fprintf(stderr, "Number of threads to execute in parallel (%zu) must be > 0", numThreads);
      exit(1);
    }
    DataFrameThread **threads = new DataFrameThread *[numThreads];
    size_t startIdx = 0, endIdx = 0, step = schema_->length() / numThreads;
    size_t t;
    for (t = 0; t < numThreads - 1; t++)
    {
      startIdx = t * step;
      endIdx = (t + 1) * step;
      threads[t] = new DataFrameThread(this, *(r.clone()), startIdx, endIdx);
      threads[t]->start();
    }

    //handle remaining rows in last thread
    startIdx = t * step;
    endIdx = schema_->length();
    threads[t] = new DataFrameThread(this, *(r.clone()), startIdx, endIdx);
    threads[t]->start();

    for (int i = numThreads - 1; i >= 0; i--)
    {
      threads[i]->join();
	  r.join_delete(&(threads[i]->getRower()));
	  delete threads[i];
    }

    delete[] threads;
  }

  /** This method clones the Rower and executes the map in parallel. Join is
	* used at the end to merge the results. Creates (number of threads / 2) + 1
	* parallel worker threads. This means that for Intel CPUs with hyperthreading,
	* this will run 1 thread per physical CPU core + 1 extra
	*/
  void pmap(Rower &r)
  {
    size_t numThreads = (std::thread::hardware_concurrency() / 2) + 1;
    p("Number of threads in pmap: ").pln(numThreads);
    pmap(r, numThreads);
  }

  /** Create a new dataframe, constructed from rows for which the given Rower
    * returned true from its accept method. */
/*  DataFrame *filter(Rower &r)
  {
    DataFrame *newFrame = new DataFrame(*schema_);
    for (int i = 0; i < schema_->length(); i++)
    {
      Row *row = createRow_(i);
      if (r.accept(*row))
      {
        newFrame->add_row(*row);
      }
      delete row;
    }

    return newFrame;
  }*/

  /** Print the dataframe in SoR format to standard output. */
  void print()
  {
    for (int rowIdx = 0; rowIdx < schema_->length(); rowIdx++)
    {
      for (int colIdx = 0; colIdx < schema_->width(); colIdx++)
      {
        columns_->get(colIdx)->printElement(rowIdx);
      }

      printf("\n");
    }
  }

  /** Add to end of column depending on the type */
  void addToEndOfColByType_(size_t colIdx, Row &row)
  {
    switch (row.col_type(colIdx))
    {
    case 'I':
	  columns_->push_back(colIdx, row.get_int(colIdx));
      break;
    case 'B':
	  columns_->push_back(colIdx, row.get_bool(colIdx));
      break;
    case 'F':
	  columns_->push_back(colIdx, row.get_float(colIdx));
      break;
    case 'S':
	  columns_->push_back(colIdx, row.get_string(colIdx));
      break;
    default:
      fprintf(stderr, "Invalid col type: %c", row.col_type(colIdx));
    }
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
    char colTypeFromSchema = schema_->col_type(schemaColIdx);
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
    for (int i = 0; i < r->width(); i++)
    {
      setRowValByColType_(*r, i, rowIdx, schema_->col_type(i));
    }

    return r;
  }

  /** Set value in Row, given the column type */
  void setRowValByColType_(Row &r, size_t colIdx, size_t rowIdx, char colType)
  {
    switch (colType)
    {
    case 'I':
      r.set(colIdx, columns_->get_int(colIdx, rowIdx));
      break;
    case 'B':
		r.set(colIdx, columns_->get_bool(colIdx, rowIdx));
      break;
    case 'F':
		r.set(colIdx, columns_->get_float(colIdx, rowIdx));
      break;
    case 'S':
		r.set(colIdx, columns_->get_string(colIdx, rowIdx));
      break;
    default:
      fprintf(stderr, "Invalid col type: %c", colType);
    }
  }

};