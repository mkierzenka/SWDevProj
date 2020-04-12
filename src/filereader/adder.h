#pragma once

#include "reader.h"
#include "../utils/num.h"
#include "../utils/map.h"

/** Adder class provided to us in M4.
 * 
 * @authors Marcin Kierzenka & Chase Broder
*/
class Adder : public Reader {
public:
  SIMap& map_;  // String to Num map;  Num holds an int
 
  Adder(SIMap& map) : map_(map) {}
 
  bool visit(Row& r) override {
    String* word = r.get_string(0)->clone();
    assert(word != nullptr);
    Num* num = map_.contains_key(word) ? dynamic_cast<Num*>(map_.get(*word))->clone() : new Num();
    assert(num != nullptr);
    num->val_++;
    map_.put(word->clone(), num->clone());
    return false;
  }
};