#include "../src/dataframe/dataframe.h"

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "../src/dataframe/sumrower.h"
#include "../src/dataframe/findrower.h"
#include "../src/dataframe/lengthrower.h"

Sys *SYSTEM = new Sys();

/*
void basicTest2()
{
  SYSTEM->pln("Basic test2 started...");
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
  SYSTEM->pln("Basic test2 passed!");
}*/

void testSchema2()
{
  SYSTEM->pln("Schema2 test started...");
  String *s0 = new String("a");
  String *s1 = new String("a1");
  String *s2 = new String("a2");
  String *s3 = new String("a3");
  String *s4 = new String("a4");
  String *s5 = new String("a5");

  Schema *scm = new Schema("II");
  assert(scm->width() == 2);
  assert(scm->length() == 0);
  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(1) == 'I');

  for (int i = 0; i < 1000; i++)
  {
    scm->add_row();
  }

  delete s0;
  delete s1;
  delete s2;
  delete s3;
  delete s4;
  delete s5;
  delete scm;
  SYSTEM->pln("Schema2 test passed!");
}

void testSchema()
{
  SYSTEM->pln("Schema test started...");

  Schema *scm = new Schema();
  scm->add_column('I');
  scm->add_column('F');
  scm->add_column('F');
  assert(scm->width() == 3);
  assert(scm->length() == 0);

  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(2) == 'F');

  scm->add_column('S');
  scm->add_column('B');
  assert(scm->col_type(3) == 'S');
  assert(scm->col_type(4) == 'B');

  delete scm;
  SYSTEM->pln("Schema test passed!");
}

void columnTests()
{
  SYSTEM->pln("Columns test started...");
  KVStore* store = new KVStore(0);

  Key* baseKeyI = new Key("baseInts", 0);
  int numsI[] = {1, 2, 3, 4};
  Column* ic = new Column(store, baseKeyI, ColType::Integer);
  ic->add_all(4, numsI);

  assert(ic->size() == 4);
  assert(ic->get_int(0) == numsI[0]);
  assert(ic->get_int(1) == numsI[1]);
  assert(ic->get_int(2) == numsI[2]);
  assert(ic->get_int(3) == numsI[3]);
  delete ic;

  //
  Key* baseKeyD = new Key("baseDbls", 0);
  double numsD[] = {4.5, 2.1, 0.0, 0.004};
  Column* dc = new Column(store, baseKeyD, ColType::Double);
  dc->add_all(4, numsD);

  assert(dc->size() == 4);
  assert(dc->get_double(0) == numsD[0]);
  assert(dc->get_double(1) == numsD[1]);
  assert(dc->get_double(2) == numsD[2]);
  assert(dc->get_double(3) == numsD[3]);
  delete dc;

  //
  Key* baseKeyB = new Key("baseBools", 0);
  bool valsB[] = {true, false, false, true};
  Column* bc = new Column(store, baseKeyB, ColType::Boolean);
  bc->add_all(4, valsB);

  assert(bc->size() == 4);
  assert(bc->get_bool(0) == valsB[0]);
  assert(bc->get_bool(1) == valsB[1]);
  assert(bc->get_bool(2) == valsB[2]);
  assert(bc->get_bool(3) == valsB[3]);
  delete bc;

  //
  Key* baseKeyS = new Key("baseStrs", 0);
  String *stringA = new String("Good");
  String *stringB = new String("Morning");
  String *stringC = new String("Graduation");
  String* valsS[] = {stringA, stringC, stringA, stringB};
  Column* sc = new Column(store, baseKeyS, ColType::Str);
  sc->add_all(4, valsS);
 
  assert(sc->size() == 4);
  assert(sc->get_string(0)->equals(valsS[0]));
  assert(sc->get_string(1)->equals(valsS[1]));
  assert(sc->get_string(2)->equals(valsS[2]));
  assert(sc->get_string(3)->equals(valsS[3]));
  delete sc;
  
  delete store;

  SYSTEM->pln("Columns test passed!");
}

/* Adding multiple columns to DF via add_array
void dataFrameTest()
{
  SYSTEM->pln("Data frame test started...");

  KVStore kv(0);
  Key k("main", 0);
  DataFrame df(&k, &kv);
  assert(df.ncols() == 0);
  assert(df.get_schema().width() == 0);
  assert(df.nrows() == 0);
  assert(df.get_schema().length() == 0);

  int valsI[] = {36, 75, 309};
  double valsD[] = {34.2, 2.2, 0.8};
  bool valsB[] = {false, true, false};
  String s1("abc");
  String s2("aaa");
  String s3("!@#");
  String* valsS[] = {&s1, &s2, &s3};
  
  df.add_array(3, valsI);
  df.add_array(3, valsD);
  df.add_array(3, valsB);
  df.add_array(3, valsS);

  //make sure both schema and dataframe got updated
  assert(df.ncols() == 5);
  assert(df.get_schema().width() == 4);
  assert(df.nrows() == 3);
  assert(df.get_schema().length() == 3);

  assert(df.get_int(0, 0) == valsI[0]);
  assert(df.get_double(1, 0) == valsD[0]);
  assert(df.get_bool(2, 0) == valsB[0]);
  assert(df.get_string(3, 0)->equals(valsS[0]));
  assert(df.get_int(0, 2) == valsI[2]);
  assert(df.get_double(1, 1) == valsD[1]);
  assert(df.get_bool(2, 2) == valsB[2]);
  assert(df.get_string(3, 1)->equals(valsS[1]));
  SYSTEM->pln("Data frame test passed!");
}*/

void genSchemaTest()
{
  SYSTEM->pln("General schema test started...");

  Schema s("IIFSB");
  assert(s.col_type(0) == 'I');
  assert(s.col_type(4) == 'B');
  assert(s.width() == 5);
  assert(s.length() == 0);

  s.add_column('I');
  s.add_column('F');
  s.add_column('B');

  s.add_row();
  s.add_row();

  assert(s.width() == 8);
  assert(s.length() == 2);
  assert(s.col_type(5) == 'I');
  assert(s.col_type(7) == 'B');

  SYSTEM->pln("General schema test passed!");
}
/*
void lengthRowerTest() {
  SYSTEM->pln("Length rower test started...");

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
  SYSTEM->pln("Length rower test passed!");
}
*/

int main()
{
  columnTests();
  testSchema();
  testSchema2();
  genSchemaTest();
  //basicTest2
  //dataFrameTest();
  //lengthRowerTest();
  return 0;
}
