#pragma once

#include "../utils/map.h"
#include "../utils/string.h"
#include "writer.h"

/** Summer provided to us in M4 */
/***************************************************************************/
class Summer : public Writer {
public:
  SIMap& map_;
  size_t i = 0;
  size_t j = 0;
  size_t seen = 0;
 
   Summer(SIMap& map) : map_(map) {}

  void next() {
      if (i == map_.capacity_) return;

      if (map_.buckets_[i] && j < map_.buckets_[i]->length()) {
          j++;
          ++seen;
      } else {
          ++i;
          j = 0;
          while( i < map_.capacity_ && !map_.buckets_[i] )  i++;
          //if (k()) ++seen;
      }
  }
 
  String* k() {
      if (i==map_.capacity_ || !map_.buckets_[i] || j == map_.buckets_[i]->length()) {
          return nullptr;
      }
      return (String*) (map_.buckets_[i]->getKey(j));
  }

  size_t v() {
      if (i == map_.capacity_ || !map_.buckets_[i] || j == map_.buckets_[i]->length()) {
          assert(false); return 0;
      }
      return ((Num*)(map_.buckets_[i]->getValue(j)))->asInt();
  }
 
  void visit(Row& r) {
      if (!k()) next();
      String & key = *k();
      size_t value = v();
      r.set(0, &key);
      r.set(1, (int) value);
      next();
  }
 
  bool done() {
        return seen == map_.size(); 
    }
};
