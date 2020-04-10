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
  Map& map_;  // String to Num map;  Num holds an int
 
  Adder(Map& map) : map_()  {}
 
  bool visit(Row& r) override {
    String* word = r.get_string(0);
    assert(word != nullptr);
    Num* num = map_.contains_key(word) ? dynamic_cast<Num*>(map_.get(word)) : new Num();
    assert(num != nullptr);
    num->val_++;
    map_.put(word, num);
    return false;
  }
};