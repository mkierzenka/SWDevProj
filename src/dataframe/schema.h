#pragma once

#include "columnmetadata.h"
#include "object.h"
#include "string.h"


/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object {
 public:
 
  ColumnMetadata** colData_;
  size_t numCols_;
  size_t capCols_;
  String** rowNames_;
  size_t numRows_;
  size_t capRows_;
  
  /** Create an empty schema **/
  Schema() {
	  capCols_ = 2;
	  capRows_ = 2;
	  numCols_ = 0;
	  numRows_ = 0;
	  colData_ = new ColumnMetadata*[capCols_];
	  rowNames_ = new String*[capRows_];
  }

  /** Copying constructor */
  Schema(Schema& from) : Schema(from, true) {	  
  }
 
  /** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr argument is
    * undefined. **/
  Schema(const char* types) {
	  size_t typesLen = strlen(types);
	  capCols_ = typesLen;
	  capRows_ = 2;
	  numCols_ = 0;
	  numRows_ = 0;
	  colData_ = new ColumnMetadata*[capCols_];
	  rowNames_ = new String*[capRows_];
	  for (size_t i = 0; i < typesLen; i++) {
		  this->add_column(types[i], nullptr);
	  }
  }

  /** Constructor that copies over values from passed in schema. If passed in
   * boolean is true, copy over row information as well */
  Schema(Schema& from, bool copyRows) {
	  capCols_ = (from.width() == 0 ? 2 : from.width()); // from.width != from.capacity
	  capRows_ = (from.length() == 0 ? 2 : from.length()); // we need some capacity
	  numCols_ = 0;
	  numRows_ = 0;
	  colData_ = new ColumnMetadata*[capCols_];
	  rowNames_ = new String*[capRows_];
	  if (from.width() != 0) {
		  for (int i = 0; i < from.width(); i++) {
			  add_column(from.col_type(i), from.col_name(i) == nullptr ? nullptr : from.col_name(i)->clone());
		  }
		  assert(numCols_ == capCols_);
	  }

	  //only copy rows if specified to
	  if (copyRows && from.length() != 0) {
		for (int i = 0; i < from.length(); i++) {
		  add_row(from.row_name(i) == nullptr ? nullptr : from.row_name(i)->clone()); // add the above check here!! TODO
		}
		assert(numRows_ == capRows_);
	  }
  }
  
  ~Schema() {
	  for (size_t i = 0; i < numCols_; i++) {
		  delete colData_[i];
	  }
	  delete[] colData_;
	  
	  for (size_t i = 0; i < numRows_; i++) {
		  delete rowNames_[i];
	  }
	  delete[] rowNames_;
  }
 
  /** Add a column of the given type and name (can be nullptr), name
    * is external. Names are expectd to be unique, duplicates result
    * in undefined behavior. */
  void add_column(char typ, String* name) {
	  if (numCols_ >= capCols_) {
		  growColList_();
	  }
	  colData_[numCols_] = new ColumnMetadata(name, typ);
	  numCols_++;
  }
 
  /** Add a row with a name (possibly nullptr), name is external.  Names are
   *  expectd to be unique, duplicates result in undefined behavior. */
  void add_row(String* name) {
	  if (numRows_ >= capRows_) {
		  growRowList_();
	  }
	  rowNames_[numRows_] = (name == nullptr) ? name : name->clone();
	  numRows_++;
  }
 
  /** Return name of row at idx; nullptr indicates no name. An idx >= width
    * is undefined. */
  String* row_name(size_t idx) {
	  if (idx >= numRows_) {
		  fprintf(stderr, "Index out of bounds: Bad row index (%zu) in Schema", idx);
		  exit(1);
	  }
	  return rowNames_[idx];
  }
 
  /** Return name of column at idx; nullptr indicates no name given.
    *  An idx >= width is undefined.*/
  String* col_name(size_t idx) {
	  if (idx >= numCols_) {
		  fprintf(stderr, "Index out of bounds: Bad col index (%zu) in Schema", idx);
		  exit(1);
	  }
	  return colData_[idx]->getName();
  }
 
  /** Return type of column at idx. An idx >= width is undefined. */
  char col_type(size_t idx) {
	  if (idx >= numCols_) {
		  fprintf(stderr, "Index out of bounds: Bad col index (%zu) in Schema", idx);
		  exit(1);
	  }
	  return colData_[idx]->getType();
  }
 
  /** Given a column name return its index, or -1. */
  int col_idx(const char* name){
	  String* curColName = NULL;
	  for (int i = 0; i < numCols_; i++) {
		  curColName = colData_[i]->getName();
		  if (curColName == nullptr)
		  {
			  //column doesn't have a name
			  continue;
		  }

		  if (strcmp(curColName->c_str(), name) == 0) {
			  return i;
		  }
	  }
	  return -1;
  }
 
  /** Given a row name return its index, or -1. */
  int row_idx(const char* name) {
	  String* curRowName = NULL;
	  for (int i = 0; i < numRows_; i++) {
		  curRowName = rowNames_[i];
		  if (curRowName == nullptr)
		  {
			  //row doesn't have a name
			  continue;
		  }

		  if (strcmp(curRowName->c_str(), name) == 0) {
			  return i;
		  }
	  }
	  return -1;
  }
 
  /** The number of columns */
  size_t width() {
	  return numCols_;
  }
 
  /** The number of rows */
  size_t length() {
	  return numRows_;
  }
  
  void growColList_() {
	  capCols_ *= 2;
	  ColumnMetadata** newColArr = new ColumnMetadata*[capCols_];
	  for (int i = 0; i < numCols_; i++) {
		  newColArr[i] = colData_[i];
	  }
	  delete[] colData_;
	  colData_ = newColArr;
  }
  
    void growRowList_() {
	  capRows_ *= 2;
	  String** newRowArr = new String*[capRows_];
	  for (int i = 0; i < numRows_; i++) {
		  newRowArr[i] = rowNames_[i];
	  }
	  delete[] rowNames_;
	  rowNames_ = newRowArr;
  }
};