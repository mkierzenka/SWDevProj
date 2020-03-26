//lang: Cpp

#pragma once

#include <assert.h>
#include "../dataframe/dataframe.h"
#include "../store/key.h"
#include "../store/kvstore.h"

#include "application.h"

/**
 * Example given to us in milestone 2
 */

class Trivial : public Application {
 public:
  Trivial(size_t idx) : Application(idx) { }

  void run2_() {
    size_t SZ = 1000;//1000 1000
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
    Key key(new String("triv"), 0);
    DataFrame* df = DataFrame::fromArray(&key, kv_, SZ, vals);
    assert(df->get_double(0,1) == 1);
    DataFrame* df2 = kv_->get(&key);
    for (size_t i = 0; i < SZ; ++i) sum -= df2->get_double(0,i);
	fprintf(stderr, "Final double sum = %6.6lf\n", sum);
    assert(sum==0);
    delete df; delete df2;
  }

  void run_() {
    size_t SZ = 1000 * 1000;//1000 * 1000;
    int* vals = new int[SZ];
    int sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
    Key key(new String("triv"), 0);
    DataFrame* df = DataFrame::fromArray(&key, kv_, SZ, vals);
    assert(df->get_int(0,1) == 1);
    DataFrame* df2 = kv_->get(&key);
    for (size_t i = 0; i < SZ; ++i) {
		int tmp = df2->get_int(0,i);
		assert(tmp == i);
		sum -= tmp;
	}
    fprintf(stderr, "Final int sum = %d\n", sum);
	assert(sum==0);
    delete df; delete df2;
  }
};