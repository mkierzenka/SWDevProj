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
    trial1();
    trial2();
    trial3();
    trial4();
    pln("Trivial Test Passed!");
  }

  void trial1()
  {
    size_t SZ = 1000 * 1000;
    //size_t SZ = 10;
    double *vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i)
      sum += vals[i] = i;
    String* trivKey = new String("triv");
    //Key key(trivKey, 0);
    Key* key = new Key(trivKey, 0);
    // DataFrame *df = DataFrame::fromArray(&key, kv_, SZ, vals);
    DataFrame *df = DataFrame::fromArray(key->clone(), kv_, SZ, vals);
    assert(df->get_double(0, 1) == 1);
    //DataFrame *df2 = kv_->get(&key);
    DataFrame *df2 = kv_->get(key);
    for (size_t i = 0; i < SZ; ++i)
      sum -= df2->get_double(0, i);
    fprintf(stdout, "Final double sum = %3.6lf\n", sum);
    assert(sum == 0);
    delete df;
    delete df2;
    delete[] vals;
    delete trivKey;
    delete key;
  }

  void trial2()
  {
    size_t SZ = 1000 * 1000;
    int *vals = new int[SZ];
    int sum = 0;
    for (size_t i = 0; i < SZ; ++i)
      sum += vals[i] = i;
    Key key(new String("triv"), 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, SZ, vals);
    assert(df->get_int(0, 1) == 1);
    DataFrame *df2 = kv_->get(&key);
    for (size_t i = 0; i < SZ; ++i)
      sum -= df2->get_int(0, i);
    fprintf(stdout, "Final int sum = %d\n", sum);
    assert(sum == 0);
    delete df;
    delete df2;
    delete[] vals;
  }

  void trial3()
  {
    size_t SZ = 1000 * 1000;
    StrBuff *buf = new StrBuff();
    String **vals = new String *[SZ];
    String *tmp = nullptr;
    char temp_string[10];
    for (size_t i = 0; i < SZ; ++i)
    {
      memset(&temp_string, 0, 10);
      sprintf(temp_string, "%zu", i);
      tmp = new String(temp_string);
      vals[i] = tmp;
      buf->c(*tmp);
    }
    String *expected = buf->get();
    delete buf;
    Key key(new String("triv"), 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, SZ, vals);
    assert(df->get_string(0, 1)->equals(vals[1]));
    assert(df->get_string(0, 5)->equals(vals[5]));
    for (size_t i = 0; i < SZ; ++i)
    {
      delete vals[i];
    }
    delete[] vals;

    DataFrame *df2 = kv_->get(&key);
    buf = new StrBuff();
    for (size_t i = 0; i < SZ; ++i)
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

  void trial4()
  {
    size_t SZ = 1000 * 1000;
    bool *vals = new bool[SZ];
    for (size_t i = 0; i < SZ; ++i)
    {
      vals[i] = ((i % 2) == 0 || (i > (SZ - 5)));
    }
    Key key(new String("triv"), 0);
    DataFrame *df = DataFrame::fromArray(&key, kv_, SZ, vals);
    assert(df->get_bool(0, 1) == false);
    assert(df->get_bool(0, 4) == true);
    assert(df->get_bool(0, SZ - 3) == true);
    DataFrame *df2 = kv_->get(&key);
    for (size_t i = 0; i < SZ; ++i)
    {
      assert(df2->get_bool(0, i) == ((i % 2) == 0 || (i > (SZ - 5))));
    }
    delete df;
    delete df2;
    delete[] vals;
    fprintf(stdout, "Bool DataFrame OK\n");
  }
};