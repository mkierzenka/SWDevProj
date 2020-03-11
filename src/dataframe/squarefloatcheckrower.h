#pragma once

#include "rower.h"
#include "part1/dataframe.h"
#include "row.h"

/*******************************************************************************
 *  SquareFloatCheckRower::
 *  A Rower implementation that returns true and sets a dataframe entry (col i=2)
 *    if row[0] * row[1] = row[2]. Operates on rows with schema FFB
 */
class SquareFloatCheckRower : public Rower {
 public:
	DataFrame* df_;
	size_t colToReportIdx_ = 2;
	
	SquareFloatCheckRower(DataFrame* df) {
		df_ = df;
	}
	
  /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
  bool accept(Row& r) {
	  bool result = (r.get_float(0) * r.get_float(1)) == r.get_float(2);
	  df_->set(colToReportIdx_, r.get_idx(), result);
	  return result;
  }
 
  /** Once traversal of the data frame is complete the rowers that were
      split off will be joined.  There will be one join per split. The
      original object will be the last to be called join on. The join method
      is reponsible for cleaning up memory. */
  void join_delete(Rower* other) {}
};