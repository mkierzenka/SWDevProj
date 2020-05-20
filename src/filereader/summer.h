#pragma once

#include "../utils/map.h"
#include "../utils/string.h"
#include "../dataframe/row/writer.h"

/** Summer over an SIMap, modified from M4 starter code */
/***************************************************************************/
class Summer : public Writer {
public:
  SIMap& map_;
  String** strs_;          //owned
  size_t size_;
  size_t curIdx_;
 
  Summer(SIMap& map) : map_(map) {
    strs_ = map.get_str_keys();
    size_ = map.size();
    curIdx_ = 0;
  }

  ~Summer() {
    for (size_t i = 0; i < size_; i++) {
      delete strs_[i];
	}
	delete[] strs_;
  }

  void visit(Row& r) {
    String* key = strs_[curIdx_];
    Num* val = (Num*)(map_.get(key));
    size_t valInt = val->asInt();
    r.set(0, key);
    r.set(1, (int) valInt);
    curIdx_++;
  }
 
  bool done() {
    return curIdx_ == map_.size();
  }
};
