#pragma once
#include "application.h"
#include "../dataframe/dataframe.h"
#include "../store/key.h"

/** 
 * An even simpler demo application to test the functionality of our Distributed KV Store
 *      with multiple nodes. From Milestone 1 Assignment.
 */
class Demo : public Application {
public:
  //had to initialize new key to prevent errors
  Key* main = new Key("main", 0);

  Demo(size_t idx): Application(idx) {}

  void run_() override {
    switch(this_node()) {
    case 0:   producer();     break;
    case 1:   verifier();     break;
    default: puts("bad thread number");
   }
  }
 
  void producer() {
    puts("producer started");
    size_t SZ = 100;
    int* vals = new int[SZ];
    for (size_t i = 0; i < SZ; ++i) vals[i] = i;
    DataFrame::fromArray(main, kv_, SZ, vals);
  }
 
  void verifier() {
    puts("verifier started");
    DataFrame* result = kv_->waitAndGet(main);
    pln(result->get_int(0,5) == 5 ? "SUCCESS":"FAILURE");
  }
};