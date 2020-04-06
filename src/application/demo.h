//lang:Cpp
#pragma once

#include "application.h"
#include "../dataframe/dataframe.h"
#include "../store/key.h"


/** 
 * A simple demo application to test the functionality of our Distributed KV Store
 *      with multiple nodes. From Milestone 1 Assignment.
 */
class Demo : public Application {
public:
  Key* main;
  Key* verify;
  Key* check;
 
  Demo(size_t idx): Application(idx) {
    main = new Key("main", 0);
    verify = new Key("verif", 0);
    check = new Key("ck", 0);
  }

  ~Demo()
  {
    delete main;
    delete verify;
    delete check;
  }
 
  void run_() override {
    switch(this_node()) {
    case 0:   producer();     break;
    case 1:   counter();      break;
    case 2:   summarizer();
   }
  }
 
  void producer() {
    pln("Producer Started");
    size_t SZ = 1000;
    //size_t SZ = 100*1000;//100*1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
    DataFrame* dfa = DataFrame::fromArray(main, kv_, SZ, vals);
    DataFrame* dfs = DataFrame::fromScalar(check, kv_, sum);

    delete dfa;
    delete dfs;
    pln("Producer Finished");
  }
 
  void counter() {
    pln("Counter Started");
    DataFrame* v = kv_->waitAndGet(main);
    double sum = 0;
    for (size_t i = 0; i < 1000; ++i) sum += v->get_double(0,i);
    //for (size_t i = 0; i < 100*1000; ++i) sum += v->get_double(0,i);
    p("The sum is  ").pln(sum);
    DataFrame::fromScalar(verify, kv_, sum);
    pln("Counter Finished");
  }
 
  void summarizer() {
    pln("Summarizer Started");
    DataFrame* result = kv_->waitAndGet(verify);
    DataFrame* expected = kv_->waitAndGet(check);
    pln(expected->get_double(0,0)==result->get_double(0,0) ? "SUCCESS":"FAILURE");
    pln("Summarizer Finished");
  }
};