//lang: Cpp

#pragma once

#include <assert.h>
#include "../dataframe/dataframe.h"
#include "../store/key.h"
#include "../store/kvstore.h"
#include "../utils/string.h"
#include "../network/network.h"

#include "application.h"

/**
 * Example given to us in milestone 2. Augmented to have a check for each type.
 */

class Trivial : public Application {
 public:
  Trivial(size_t idx, INetwork* net) : Application(idx, net) { }

  void run_()
  {
    pln("Trivial Test Started");
    size_t SZ = 1000 * 1000;
    trial1(SZ);
    trial2(SZ);
    trial3(SZ);
    trial4(SZ);
    pln("Trivial Test Passed!");
  }

  void trial1(size_t sz)
  {
    double *vals = new double[sz];
    double sum = 0;
    for (size_t i = 0; i < sz; ++i)
      sum += vals[i] = i;
    Key key("triv", 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, sz, vals);
    assert(df->get_double(0, 1) == 1);
    DataFrame *df2 = kv_->get(&key);
    for (size_t i = 0; i < sz; ++i)
      sum -= df2->get_double(0, i);
    printf("Final Difference (double) = %3.6lf\n", sum);
    assert(sum == 0);
    delete df;
    delete df2;
    delete[] vals;
  }

  void trial2(size_t sz)
  {
    int *vals = new int[sz];
    int sum = 0;
    for (size_t i = 0; i < sz; ++i)
      sum += vals[i] = i;
    Key key("triv", 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, sz, vals);
    assert(df->get_int(0, 1) == 1);
    DataFrame *df2 = kv_->get(&key);
    for (size_t i = 0; i < sz; ++i)
      sum -= df2->get_int(0, i);
    printf("Final Difference (int) = %d\n", sum);
    assert(sum == 0);
    delete df;
    delete df2;
    delete[] vals;
  }

  void trial3(size_t sz)
  {
    StrBuff *buf = new StrBuff();
    String **vals = new String *[sz];
    String *tmp = nullptr;
    char temp_string[10];
    for (size_t i = 0; i < sz; ++i)
    {
      memset(&temp_string, 0, 10);
      sprintf(temp_string, "%zu", i);
      tmp = new String(temp_string);
      vals[i] = tmp;
      buf->c(*tmp);
    }
    String *expected = buf->get();
    delete buf;
    Key key("triv", 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, sz, vals);

    // Test df values are okay
    String* actualStr = df->get_string(0, 1);
    assert(actualStr->equals(vals[1]));
    delete actualStr;
    actualStr = df->get_string(0, 5);
    assert(actualStr->equals(vals[5]));
    delete actualStr;

    for (size_t i = 0; i < sz; ++i)
    {
      delete vals[i];
    }
    delete[] vals;

    DataFrame *df2 = kv_->get(&key);
    buf = new StrBuff();
    for (size_t i = 0; i < sz; ++i)
    {
      tmp = df2->get_string(0, i);
      buf->c(*tmp);
      delete tmp;
    }
    String *actual = buf->get();
    delete buf;

    assert(actual->equals(expected));
    delete actual;
    delete expected;
    delete df;
    delete df2;
    fprintf(stdout, "String DataFrame OK\n");
  }

  void trial4(size_t sz)
  {
    assert(sz >= 10);
    bool *vals = new bool[sz];
    for (size_t i = 0; i < sz; ++i)
    {
      vals[i] = ((i % 2) == 0 || (i > (sz - 5)));
    }
    Key key("triv", 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, sz, vals);
    assert(df->get_bool(0, 1) == false);
    assert(df->get_bool(0, 4) == true);
    assert(df->get_bool(0, sz - 3) == true);
    DataFrame *df2 = kv_->get(&key);
    for (size_t i = 0; i < sz; ++i)
    {
      assert(df2->get_bool(0, i) == ((i % 2) == 0 || (i > (sz - 5))));
    }
    delete df;
    delete df2;
    delete[] vals;
    fprintf(stdout, "Bool DataFrame OK\n");
  }
};