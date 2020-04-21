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

/*void basicTest2()
{
  SYSTEM.pln("Basic test2 started...");
  Schema s("III");
  Key k("main", 0);
  DataFrame df(s, &k);
  Row r(df.get_schema());
  int numRows = 100 * 1000;
  int expectedSum = 0;
  for (int i = 0; i < numRows; i++)
  {
    r.set(0, i);
    r.set(1, i + 1);
    r.set(2, i);
    df.add_row(r);
	expectedSum += (i + i + 1 + i);
  }
  assert(df.nrows() == numRows);
  assert(df.ncols() == 3);

  SumRower sr(&df);
  df.map(sr);

  assert(sr.getSum() == expectedSum);
  SYSTEM.pln("Basic test2 passed!");
}*/

void dataFrameTest()
{
  SYSTEM.pln("Data frame test started...");
  KVStore* kv = new KVStore(0, nullptr);
  Key* k = new Key("main", 0);
  DataFrame* df = new DataFrame(k, kv);
  assert(df->ncols() == 0);
  assert(df->get_schema().width() == 0);
  assert(df->nrows() == 0);
  assert(df->get_schema().length() == 0);

  String s1("abc");
  String s2("aaa");
  String s3("!@#");
  String* valsS[] = {&s1, &s2, &s3};
  
  df->add_array(3, valsS);

  assert(df->ncols() == 1);
  assert(df->nrows() == 3);
  assert(df->get_string(0, 0)->equals(valsS[0]));
  assert(df->get_string(0, 1)->equals(valsS[1]));

  delete kv;
  delete df;
  SYSTEM.pln("Data frame test passed!");
}

/*
void lengthRowerTest() {
  SYSTEM.pln("Length rower test started...");

  //last column needs to be an int
  Schema s("IIBFBSI");
  DataFrame d(s);

  Row rOne(d.get_schema());
  Row rTwo(d.get_schema());
  Row rThree(d.get_schema());

  String s1("Hello");
  String s2("Computer");
  String s3("Khoury");

  rOne.set(0, 4);
  rOne.set(1, 46);
  rOne.set(2, false);
  rOne.set(3, 677.9f);
  rOne.set(4, true);
  rOne.set(5, &s1);
  rOne.set(6, 0);
  d.add_row(rOne);

  rTwo.set(0, 400);
  rTwo.set(1, 460);
  rTwo.set(2, true);
  rTwo.set(3, 6.9f);
  rTwo.set(4, true);
  rTwo.set(5, &s2);
  rTwo.set(6, 0);
  d.add_row(rTwo);

  rThree.set(0, 40);
  rThree.set(1, 460436);
  rThree.set(2, false);
  rThree.set(3, 86.94f);
  rThree.set(4, false);
  rThree.set(5, &s3);
  rThree.set(6, 0);
  d.add_row(rThree);

  LengthRower lr(&d);
  d.map(lr);

  assert(lr.getLen() == 45);
  SYSTEM.pln("Length rower test passed!");
}
*/

void dataFrameFromWriter() {
  SYSTEM.pln("Data frame from writer test started...");
  KVStore kv(0, nullptr);
  Key k("main", 0);
  int SZ = 30;
  IncWriter wr(SZ);
  
  DataFrame* df = DataToDf::fromVisitor(&k, &kv, "I", &wr);
  assert(df->ncols() == 1);
  assert(df->nrows() == SZ);

  for (size_t i = 0; i < SZ; i++) {
    assert(df->get_int(0, i) == i);
  }

  SYSTEM.pln("Data frame from writer test passed!");
}

void dataFrameFromSummer()
{
  printf("Data frame from summer test started\n");

  //SIMap map;
  SIMap* map = new SIMap();

  //does SIMap own its elements?
  // map.set(*new String("this"), new Num(1));
  // map.set(*new String("is"), new Num(2));
  // map.set(*new String("a"), new Num(3));
  // map.set(*new String("test"), new Num(1));

  String* key1 = new String("this");
  String* key2 = new String("is");
  String* key3 = new String("a");
  String* key4 = new String("test");

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
  DataFrame* df = DataToDf::fromVisitor(&k, &kv, "SI", &sum);
  assert(df->ncols() == 2);
  assert(df->nrows() == 4);

  assert(df->get_string(0,0)->equals(key3));
  assert(df->get_string(0,1)->equals(key1));
  assert(df->get_string(0,2)->equals(key2));
  assert(df->get_string(0,3)->equals(key4));
  
  assert(df->get_int(1, 0) == 3);
  assert(df->get_int(1, 1) == 1);
  assert(df->get_int(1, 2) == 2);
  assert(df->get_int(1, 3) == 1);
  printf("Data frame from summer test passed!\n");
}

int main(int argc, char** argv)
{
  args.parse(argc, argv);

  SYSTEM.pln("General tests starting...");
  //basicTest2();
  dataFrameTest();
  //lengthRowerTest();
  dataFrameFromWriter();
  dataFrameFromSummer();
  SYSTEM.pln("General tests passed!");
  return 0;
}
