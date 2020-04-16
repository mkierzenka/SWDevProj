#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/helper.h"
#include "../utils/array.h"
#include "../utils/thread.h"
#include "../utils/args.h"
#include "../store/kvstore.h"
#include "../store/key.h"
#include "../serial/serial.h"
#include "../filereader/writer.h"
#include "../filereader/adder.h"
#include "../filereader/reader.h"
//#include "../sorer/dataframe_adapter.h"
#include "column/columnarray.h"
#include "schema.h"
#include "row/rower.h"
#include "row/row.h"


/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, D). A dataframe has a schema that
 * describes it.
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

  /** This constructor is for the purpose of adding arrays */
  DataFrame(Key *k, KVStore *kv)
  {
    schema_ = new Schema();
    store_ = kv;
    size_t numCols = schema_->width();
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
  }

  /**
   * Create a data frame from a schema description and columns and a store.
   * All columns are created empty.
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

  /** Create a data frame with the same columns as the give df but no rows */
/*  DataFrame(DataFrame &df, Key *k) : DataFrame(df.get_schema(), k)
  {
  }
*/
  /** Create a data frame from a schema and columns. All columns are created
    * empty. */
 /* DataFrame(Schema &schema, Key *k)
  {
    //don't copy rows
    schema_ = new Schema(schema, false);
    size_t numCols = schema_->width();
    key_ = k;
    columns_ = new ColumnArray(store_, key_);
  }*/

  ~DataFrame()
  {
    delete schema_;
    delete columns_;
    delete key_;
  }

  /**
  * Creates a new DataFrame from with given schema (scm) from the Writer.
  * Returns the df result, caller is responsible for deleting it.
  */
  // static DataFrame *fromFile(const char* filename, Key *k, KVStore *kv)
  // {
  //   //DataFrame *df = sor.getFrame();//new DataFrame(scm, k->clone(), kv);
  //   DataFrame* df = DataFrameAdapter::convertToFrame(filename, k, kv);
  //   addDFToStore_(df, kv, k);
  //   return df;
  // }

  /**
  * Creates a new DataFrame with given schema (scm) from the Writer.
  * Returns the df result, caller is responsible for deleting it.
  */
  static DataFrame *fromVisitor(Key *k, KVStore *kv, const char *scm, Writer *wr)
  {
    DataFrame *df = new DataFrame(scm, k->clone(), kv);
    df->visit(wr);
    addDFToStore_(df, kv, k);
    return df;
  }

  /**
   * Converts an array into a dataframe object.
   * Returns the df result, caller is responsible for deleting it.
   */
  static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, double *elems)
  {
    DataFrame *df = new DataFrame(k->clone(), kv);
    df->add_array(numElems, elems);
    addDFToStore_(df, kv, k);
    return df;
  }

  /**
   * Converts an array of ints into a dataframe object.
   * Returns the df result, caller is responsible for deleting it.
   */
  static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, int *elems)
  {
    DataFrame *df = new DataFrame(k->clone(), kv);
    df->add_array(numElems, elems);
    return addDFToStore_(df, kv, k);
  }

  /**
   * Converts an array of booleans into a dataframe object.
   * Returns the df result, caller is responsible for deleting it.
   */
  static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, bool *elems)
  {
    DataFrame *df = new DataFrame(k->clone(), kv);
    df->add_array(numElems, elems);
    return addDFToStore_(df, kv, k);
  }

  /**
   * Converts an array of Strings into a dataframe object.
   * Returns the df result, caller is responsible for deleting it.
   */
  static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, String **elems)
  {
    DataFrame *df = new DataFrame(k->clone(), kv);
    df->add_array(numElems, elems);
    addDFToStore_(df, kv, k);
    return df;
  }

  /**
   * Adds a double to a new dataframe object. Returns df result, which will be owned
   * by caller
   */
  static DataFrame *fromDouble(Key *k, KVStore *kv, double elem)
  {
    double *elements = new double[1];
    elements[0] = elem;
    DataFrame *df = fromArray(k, kv, 1, elements);
    delete[] elements;
    return df;
  }

  /**
   * Adds a String (cloned) to a new dataframe object. Returns df result, which will be owned
   * by caller
   */
  static DataFrame *fromString(Key *k, KVStore *kv, String *elem)
  {
    String **elements = new String *[1];
    elements[0] = elem->clone();
    DataFrame *df = fromArray(k, kv, 1, elements);
    delete elements[0];
    delete[] elements;
    return df;
  }

  /**
   * Adds an int to a new dataframe object. Returns df result, which will be owned
   * by caller
   */
  static DataFrame *fromInt(Key *k, KVStore *kv, int elem)
  {
    int *elements = new int[1];
    elements[0] = elem;
    DataFrame *df = fromArray(k, kv, 1, elements);
    delete[] elements;
    return df;
  }

  /**
   * Adds a bool to a new dataframe object. Returns df result, which will be owned
   * by caller
   */
  static DataFrame *fromBool(Key *k, KVStore *kv, bool elem)
  {
    bool *elements = new bool[1];
    elements[0] = elem;
    DataFrame *df = fromArray(k, kv, 1, elements);
    delete[] elements;
    return df;
  }

  /**
   * Add array of doubles to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Column needs to get dataframe's key and key-value store
   */
  void add_array(size_t numElements, double *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('D');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of ints to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Column needs to get dataframe's key and key-value store
   */
  void add_array(size_t numElements, int *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('I');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of booleans to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Column needs to get dataframe's key and key-value store
   */
  void add_array(size_t numElements, bool *elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('B');
    schema_->add_rows(numElements);
  }

  /**
   * Add array of Strings to dataframe as a column. Add the data into chunks, and generate
   * keys for them. Column needs to get dataframe's key and key-value store
   */
  void add_array(size_t numElements, String **elements)
  {
    columns_->add_column_fromarray(numElements, elements);
    schema_->add_column('S');
    schema_->add_rows(numElements);
  }

  /** Check if two dataframes equal */
  bool equals(Object* o)
  {
    if (this == o) return true;

    DataFrame* other = dynamic_cast<DataFrame*>(o);

    return columns_->equals(other->columns_) && schema_->equals(other->schema_)
    && key_->equals(other->key_);
  }
  
  /** Returns hash of this Dataframe - deprecated */
  size_t hash() {
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

    columns_->add_column(col);      //Append to column array
    char type = col->getCharType();
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

  String *get_string(size_t col, size_t row)
  {
    return columns_->get_string(col, row)->clone();
  }

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undefined.  */
  void add_row(Row &row)
  {
    size_t rowWidth = row.width();
    checkRowLen_(rowWidth);
    for (int i = 0; i < rowWidth; i++)
    {
      checkColTypes_(row.col_type(i), i);
      addToEndOfColByType_(i, row);
    }
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


  /** Add rows to this DataFrame built by Writer. Until wr->done() */
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
      for (int colIdx = 0; colIdx < row->width(); colIdx++)
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
      if ((rowIdx / args.blockSize) % args.numNodes == store_->storeId) {
        row->set_idx(rowIdx);
        //iterate through each column to get value
        for (int colIdx = 0; colIdx < row->width(); colIdx++)
        {
          setRowValByColType_(*row, colIdx, rowIdx, schema_->col_type_char(colIdx));
        }

        r.visit(*row);
        row->clear();
      } else {
        // skip this block
        rowIdx += args.blockSize;
      }
    }

    delete row;
  }

  /** An inner class of DataFrame representing a Thread for doing work on a contiguous
	 *    subset of rows of a DataFrame.
	 *  
	 *  author: kierzenka.m@husky.neu.edu */
  /*class DataFrameThread : public Thread
  {
  public:
    size_t startRowIdx_; // inclusive
    size_t endRowIdx_;   // exclusive
    DataFrame *df_;
    Reader &r_;

    DataFrameThread(DataFrame *df, Reader &r, size_t startRowIdx, size_t endRowIdx) : df_(df), r_(r), startRowIdx_(startRowIdx), endRowIdx_(endRowIdx) {}

    void run()
    {
      df_->map(r_, startRowIdx_, endRowIdx_);
    }

    Reader &getReader()
    {
      return r_;
    }
  };*/

  /**
   * This method clones the Rower and executes the map in parallel over
   * the specified number of threads.
   */
  /*void pmap(Rower &r, size_t numThreads)
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
  }*/

  /** This method clones the Rower and executes the map in parallel. Join is
	* used at the end to merge the results. Creates (number of threads / 2) + 1
	* parallel worker threads. This means that for Intel CPUs with hyperthreading,
	* this will run 1 thread per physical CPU core + 1 extra
	*/
  /*void pmap(Rower &r)
  {
    size_t numThreads = (std::thread::hardware_concurrency() / 2) + 1;
    p("Number of threads in pmap: ").pln(numThreads);
    pmap(r, numThreads);
  }*/

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
    case 'D':
      columns_->push_back(colIdx, row.get_double(colIdx));
      break;
    case 'S':
      columns_->push_back(colIdx, row.get_string(colIdx));
      break;
    default:
      fprintf(stderr, "Invalid col type: %c", row.col_type(colIdx));
    }
  }

  /** Adds the DataFrame to the given KVStore with the given key.
   *  Returns the DataFrame just added (df).
   */
  static DataFrame *addDFToStore_(DataFrame *df, KVStore *kv, Key *k)
  {
    Serializer *s = new Serializer();
    df->serialize(s);
    Value *v = new Value(s->getBuffer(), s->getNumBytesWritten());
    kv->put(k, v);
    delete s;
    delete v;
    return df;
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
    for (int i = 0; i < r->width(); i++)
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
      r.set(colIdx, columns_->get_int(colIdx, rowIdx));
      break;
    case 'B':
      r.set(colIdx, columns_->get_bool(colIdx, rowIdx));
      break;
    case 'D':
      r.set(colIdx, columns_->get_double(colIdx, rowIdx));
      break;
    case 'S':
      r.set(colIdx, columns_->get_string(colIdx, rowIdx)->clone());
      break;
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
        break;
      }
      case 'S':
      {
        String **strs = new String *[numRowsInChunk];
        for (size_t j = 0; j < numRowsInChunk; j++)
        {
          strs[j] = (dynamic_cast<Row *>(rowChunk->get(j)))->get_string(i)->clone();
        }
        columns_->get(i)->add_all(numRowsInChunk, strs, i);
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