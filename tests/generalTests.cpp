#include <assert.h>
#include "../src/dataframe/dataframe.h"
#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/utils/string.h"
#include "incwriter.h"

Sys SYSTEM;

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
  
  DataFrame* df = DataFrame::fromVisitor(&k, &kv, "I", &wr);
  assert(df->ncols() == 1);
  assert(df->nrows() == SZ);

  for (size_t i = 0; i < SZ; i++) {
    assert(df->get_int(0, i) == i);
  }

  SYSTEM.pln("Data frame from writer test passed!");
}

int main()
{
  SYSTEM.pln("General tests starting...");
  //basicTest2();
  dataFrameTest();
  //lengthRowerTest();
  dataFrameFromWriter();
  SYSTEM.pln("General tests passed!");
  return 0;
}
