#pragma once

#include "rower.h"
#include "dataframe.h"
#include "row.h"

/*******************************************************************************
 *  SumRower::
 *  A Rower implementation that computes the total sum of entries in a data frame.
 */
class SumRower : public Rower {
 public:
	DataFrame* df_;
	int sum_;
	
	SumRower(DataFrame* df, int sum) {
		df_ = df;
		sum_ = sum;
	}
	
	SumRower(DataFrame* df) : SumRower(df, 0) {}
	
	~SumRower() {} //Nothing to delete
	
	/** Returns the total sum found by this Rower. */
	int getSum() {
		return sum_;
	}
	
  /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
  bool accept(Row& r) {
	  size_t numCols = df_->ncols();
	  for (size_t i = 0; i < numCols; i++) {
			sum_ += r.get_int(i);
	  }
	  return true;
  }
 
  /** Once traversal of the data frame is complete the rowers that were
      split off will be joined.  There will be one join per split. The
      original object will be the last to be called join on. The join method
      is reponsible for cleaning up memory. */
  void join_delete(Rower* other) {
		SumRower* otherSR = dynamic_cast<SumRower*>(other);
		if (otherSR == nullptr) {
			perror("Trying to join non FindRower with this FindRower\n");
		}

		sum_ += otherSR->getSum();
  }
};