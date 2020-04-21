//lang:Cpp
#pragma once

#include "application.h"
#include "../dataframe/dataframe.h"
#include "../dataframe/datatodf.h"
#include "../store/key.h"
#include "../network/inetwork.h"
#include "../dataframe/row/mysummer.h"
#include <assert.h>


/** 
 * Built based on the Demo application to test our DataFrame.local_map() method.
 */
class DemoLocalMap : public Application {
public:
  Key* main;
  Key* sumN1;
  Key* sumN2;
 
  DemoLocalMap(size_t idx, INetwork* net): Application(idx, net) {
    main = new Key("main", 0);
    sumN1 = new Key("sn1", 0);
    sumN2 = new Key("sn2", 0);
  }

  ~DemoLocalMap()
  {
    delete main;
    delete sumN1;
    delete sumN2;
  }
 
  void run_() override {
    switch(this_node()) {
    case 0:   leader(); break;
    case 1:   localSum();       break;
    case 2:   localSum();
   }
  }
 
  void leader() {
    pln("Leader Started");
    size_t SZ = 25000;
    int expectedSum = 0;
    int* vals = new int[SZ];
    for (size_t i = 0; i < SZ; ++i) expectedSum += vals[i] = i;
    DataFrame* dfa = DataToDf::fromArray(main, kv_, SZ, vals);
    delete[] vals;
    MySummer s;
    dfa->local_map(s);
    int actualSumL = s.getSum();
    assert(actualSumL != expectedSum);
    p("Leader local sum = ").pln(actualSumL);
    delete dfa;

    DataFrame* dfN1 = kv_->waitAndGet(sumN1);
    int actualSumN1 = dfN1->get_int(0, 0);
    assert(actualSumN1 != expectedSum);
    p("Leader got a local sum of ").p(actualSumN1).pln(" from Node 1");
    delete dfN1;

    DataFrame* dfN2 = kv_->waitAndGet(sumN2);
    int actualSumN2 = dfN2->get_int(0, 0);
    assert(actualSumN2 != expectedSum);
    p("Leader got a local sum of ").p(actualSumN2).pln(" from Node 2");
    delete dfN2;

    int actualSum = actualSumL + actualSumN1 + actualSumN2;
    pln(actualSum == expectedSum ? "SUCCESS":"FAILURE");
	p("Actual: ").pln(actualSum);
	p("Expected: ").pln(expectedSum);
    pln("Leader Finished");
  }

  void localSum() {
    DataFrame* df = kv_->waitAndGet(main);
    MySummer s;
    df->local_map(s);
    int mySum = s.getSum();
    if (idx_ == 1) delete DataToDf::fromInt(sumN1, kv_, mySum);
    else if (idx_ == 2) delete DataToDf::fromInt(sumN2, kv_, mySum);
    p("Node ").p(idx_).p(" local sum = ").pln(mySum);
    delete df;
  }
};