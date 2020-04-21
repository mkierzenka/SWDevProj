#include <assert.h>
#include "../src/dataframe/dataframe.h"
#include "../src/dataframe/datatodf.h"
#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/getImpls.h"
#include "../src/utils/string.h"
#include "../src/utils/args.h"
#include "../src/filereader/summer.h"
#include "incwriter.h"

Sys SYSTEM;

Arguments args;

void dataFrameTest()
{
  SYSTEM.pln("Data frame test started...");
  KVStore *kv = new KVStore(0, nullptr);
  Key *k = new Key("main", 0);
  DataFrame *df = new DataFrame(k, kv);
  assert(df->ncols() == 0);
  assert(df->get_schema().width() == 0);
  assert(df->nrows() == 0);
  assert(df->get_schema().length() == 0);

  String s1("abc");
  String s2("aaa");
  String s3("!@#");
  String *valsS[] = {&s1, &s2, &s3};

  df->add_array(3, valsS);

  assert(df->ncols() == 1);
  assert(df->nrows() == 3);
  assert(df->get_string(0, 0)->equals(valsS[0]));
  assert(df->get_string(0, 1)->equals(valsS[1]));

  delete kv;
  delete df;
  SYSTEM.pln("Data frame test passed!");
}

void dataFrameFromWriter()
{
  SYSTEM.pln("Data frame from writer test started...");
  KVStore kv(0, nullptr);
  Key k("main", 0);
  int SZ = 30;
  IncWriter wr(SZ);

  DataFrame *df = DataToDf::fromVisitor(&k, &kv, "I", &wr);
  assert(df->ncols() == 1);
  assert(df->nrows() == SZ);

  for (size_t i = 0; i < SZ; i++)
  {
    assert(df->get_int(0, i) == i);
  }

  SYSTEM.pln("Data frame from writer test passed!");
}

void dataFrameFromSummer()
{
  printf("Data frame from summer test started\n");

  SIMap *map = new SIMap();

  String *key1 = new String("this");
  String *key2 = new String("is");
  String *key3 = new String("a");
  String *key4 = new String("test");

  map->set(*key1, new Num(1));
  map->set(*key2, new Num(2));
  map->set(*key3, new Num(3));
  map->set(*key4, new Num(1));

  assert(map->get(*key1)->equals(new Num(1)));
  assert(map->get(*key2)->equals(new Num(2)));
  assert(map->get(*key3)->equals(new Num(3)));
  assert(map->get(*key4)->equals(new Num(1)));

  Summer sum(*map);
  Key k("df", 0);
  KVStore kv(0, nullptr);
  DataFrame *df = DataToDf::fromVisitor(&k, &kv, "SI", &sum);
  assert(df->ncols() == 2);
  assert(df->nrows() == 4);

  assert(df->get_string(0, 0)->equals(key3));
  assert(df->get_string(0, 1)->equals(key1));
  assert(df->get_string(0, 2)->equals(key2));
  assert(df->get_string(0, 3)->equals(key4));

  assert(df->get_int(1, 0) == 3);
  assert(df->get_int(1, 1) == 1);
  assert(df->get_int(1, 2) == 2);
  assert(df->get_int(1, 3) == 1);
  printf("Data frame from summer test passed!\n");
}

int main(int argc, char **argv)
{
  args.parse(argc, argv);

  SYSTEM.pln("General tests starting...");
  dataFrameTest();
  dataFrameFromWriter();
  dataFrameFromSummer();
  SYSTEM.pln("General tests passed!");
  return 0;
}
