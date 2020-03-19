

#include "../src/dataframe/dataframe.h"
#include "../src/dataframe/intcolumn.h"
#include "../src/dataframe/boolcolumn.h"
#include "../src/dataframe/floatcolumn.h"
#include "../src/dataframe/stringcolumn.h"


#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "../src/dataframe/sumrower.h"
#include "../src/dataframe/findrower.h"
#include "../src/dataframe/lengthrower.h"

Sys *SYSTEM = new Sys();


void basicTest2()
{
  SYSTEM->pln("Basic test2 started...");
  Schema s("III");
  DataFrame df(s);
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
  /*assert(df.get_int(2, 0) == 1);
  assert(df.get_int(2, 1501) == (1501 + 1502));
  assert(df.get_int(0, 45617) == 45617);
  assert(df.get_int(1, 45617) == 45618);
  assert(df.get_int(2, 45617) == (45617 + 45618));*/

  SYSTEM->pln("Basic test2 passed!");
}

void basicTest()
{
  SYSTEM->pln("Basic test started...");
  Schema s("II");
  DataFrame df(s);
  Row r(df.get_schema());
  int numRows = 100 * 1000 * 1000;
  for (int i = 0; i < numRows; i++)
  {
    r.set(0, i);
    r.set(1, i + 1);
    df.add_row(r);
  }
  assert(df.get_int(0, 10) == 10);
  SYSTEM->pln("Basic test passed!");
}

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
    scm->add_row(nullptr);
  }

  SYSTEM->pln("Schema2 test passed!");
  delete s0;
  delete s1;
  delete s2;
  delete s3;
  delete s4;
  delete s5;
  delete scm;
}

void testSchema()
{
  SYSTEM->pln("Schema test started...");
  String *s0 = new String("a");
  String *s1 = new String("a1");
  String *s2 = new String("a2");
  String *s3 = new String("a3");
  String *s4 = new String("a4");
  String *s5 = new String("a5");

  Schema *scm = new Schema();
  scm->add_column('I', s0);
  scm->add_column('F', s1);
  scm->add_column('F', s2);
  assert(scm->width() == 3);
  assert(scm->length() == 0);

  assert(scm->col_name(0)->equals(s0));
  assert(scm->col_name(1)->equals(s1));
  assert(scm->col_name(2)->equals(s2));

  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(2) == 'F');

  scm->add_column('S', s3);
  scm->add_column('B', s4);
  assert(scm->col_name(3)->equals(s3));
  assert(scm->col_type(3) == 'S');
  assert(scm->col_type(4) == 'B');
  SYSTEM->pln("Schema test passed!");
  delete s0;
  delete s1;
  delete s2;
  delete s3;
  delete s4;
  delete s5;
  delete scm;
}

void columnTests()
{

  SYSTEM->pln("Columns test started...");

  int intA = 1;
  int intB = 2;
  int intC = 3;
  IntColumn *ic = new IntColumn(4, intA, intB, intC, intB);

  ic->set(2, intB);

  assert(ic->get(0) == intA);
  assert(ic->get(2) == intB);
  assert(ic->size() == 4);

  bool boolA = true;
  bool boolB = true;
  bool boolC = false;
  BoolColumn *bc = new BoolColumn(4, boolA, boolB, boolC, boolB);

  bc->set(3, boolC);

  assert(bc->get(1) == boolB);
  assert(bc->get(3) == boolC);
  assert(bc->size() == 4);

  float floatA = 14.9;
  float floatB = -.26;
  float floatC = 3;
  FloatColumn *fc = new FloatColumn(4, floatB, floatB, floatC, floatA);

  fc->set(1, floatA);

  assert(fc->get(0) == floatB);
  assert(fc->get(1) == floatA);
  assert(fc->get(2) == floatC);
  assert(fc->size() == 4);

  String *stringA = new String("Good");
  String *stringB = new String("Morning");
  String *stringC = new String("Graduation");
  String *nptr = nullptr;
  StringColumn *sc = new StringColumn(5, stringA, stringC,
                                      stringA, stringB, nptr);

  assert(sc->get(1)->equals(stringC));
  assert(sc->get(2)->equals(stringA));
  assert(sc->get(4) == nullptr);

  sc->set(4, stringB);

  assert(sc->get(4)->equals(stringB));
  assert(sc->size() == 5);

  SYSTEM->pln("Columns test passed!");
}

void dataFrameTest()
{
  SYSTEM->pln("Data frame test started...");

  Schema s;
  s.add_column('I', new String("counts"));
  s.add_column('B', new String("isLarge"));
  s.add_row(new String("First line"));

  assert(s.width() == 2);
  assert(s.length() == 1);
  assert(s.col_type(0) == 'I');
  assert(s.col_type(1) == 'B');
  assert(strcmp(s.col_name(0)->c_str(), "counts") == 0);
  assert(s.col_idx("isLarge") == 1);
  assert(strcmp(s.row_name(0)->c_str(), "First line") == 0);

  DataFrame df(s);
  assert(df.ncols() == 2);
  assert(df.get_schema().width() == 2);
  assert(df.nrows() == 0);
  assert(df.get_schema().length() == 0);

  Column *col = new IntColumn();
  String *colName = new String("nums");
  df.add_column(col, colName);
  //make sure both schema and dataframe got updated
  assert(df.ncols() == 3);
  assert(df.get_schema().width() == 3);
  assert(df.nrows() == 0);
  assert(df.get_schema().length() == 0);
  assert(df.get_col(*colName) == 2);

  //create row and set the values
  Row r(df.get_schema());
  r.set(0, 4);
  r.set(1, true);
  r.set(2, 77);

  df.add_row(r);
  assert(df.nrows() == 1);
  assert(s.length() == 1);
  assert(df.get_schema().length() == 1);

  assert(df.get_int(0, 0) == 4);
  assert(df.get_bool(1, 0) == true);
  assert(df.get_int(2, 0) == 77);

/*  Row ro(df.get_schema());
  ro.set(0, 3);
  ro.set(1, false);
  ro.set(2, 0);

  df.fill_row(0, ro);

  assert(df.nrows() == 1);
  assert(s.length() == 1);

  assert(df.get_int(0, 0) == 3);
  assert(df.get_bool(1, 0) == false);
  assert(df.get_int(2, 0) == 0);*/

  SYSTEM->pln("Data frame test passed!");
}

void dataFrameLargeDataTest()
{
  Sys *c = new Sys();

  c->pln("Large data frame test started...");

  int numCols = 50 * 1000;
  int numRows = 500;

  char *colName = new char[6];
  Schema s;
  for (int i = 0; i < numCols; i++)
  {
    sprintf(colName, "%05d", i);
    colName[5] = 0;
    s.add_column('I', new String(colName));
  }
  DataFrame df(s);
  Row r(df.get_schema());
  for (int j = 0; j < numRows; j++)
  {
    for (int i = 0; i < numCols; i++)
    {
      r.set(i, i + j);
    }
    df.add_row(r);
  }

  assert(df.ncols() == numCols);
  assert(df.nrows() == numRows);
  assert(df.get_int(0, 0) == 0);
  assert(df.get_int(40574, 200) == 40774);
  assert(df.get_int(numCols - 1, numRows - 1) == (numCols - 1) + (numRows - 1));

  SYSTEM->pln("Large data frame test passed!");
}

void rowTest()
{
  Sys *c = new Sys();

  SYSTEM->pln("Row test started...");

  Schema s("IFBSFS");
  Row r(s);

  assert(r.width() == 6);
  assert(r.col_type(0) == 'I');
  assert(r.col_type(2) == 'B');
  assert(r.col_type(5) == 'S');

  //tolerance for this float comparison
  float tol = 0.0001;

  float f = 6.7;
  String *inStr = new String("Marcin");

  r.set(0, 4);
  r.set(1, f);
  r.set(2, false);
  r.set(3, inStr);

  assert(r.get_int(0) == 4);
  assert(abs(f - r.get_float(1)) < tol);
  assert(r.get_bool(2) == false);
  assert(r.get_string(3)->equals(inStr));
  SYSTEM->pln("Row test passed!");
}

void basicDfTest()
{
  SYSTEM->pln("Basic df test started...");

  Schema s("III");

  DataFrame df(s);
  Row r(df.get_schema());
  for (int i = 0; i < 5; i++)
  {
    r.set(0, i * 2);
    r.set(1, i * 4);
    r.set(2, i * 8);
    df.add_row(r);
  }

  DataFrame dfTwo(df);

  //Make sure all column info is same
  assert(df.ncols() == 3);
  assert(df.ncols() == dfTwo.ncols());

  //row info should not be same
  assert(df.nrows() == 5);
  assert(dfTwo.nrows() == 0);
  assert(df.nrows() != dfTwo.nrows());

  Row rTwo(s);

  rTwo.set(0, 0);
  rTwo.set(1, 5);
  rTwo.set(2, 1);

  dfTwo.add_row(rTwo);

  assert(dfTwo.nrows() == 1);
  assert(dfTwo.ncols() == 3);
  assert(df.nrows() != dfTwo.nrows());

  SYSTEM->pln("Basic df test passed!");
}

void asIntTest()
{
  SYSTEM->pln("As int test started...");
  ColumnArray *columns_ = new ColumnArray();
  IntColumn *i = new IntColumn();
  columns_->add(i);

  Column *c = columns_->get(0);
  IntColumn *ic = columns_->get(0)->as_int();
  assert(ic != nullptr);
  BoolColumn *bc = columns_->get(0)->as_bool();
  assert(bc == nullptr);
  SYSTEM->pln("As int test passed!");
}

void genSchemaTest()
{
  SYSTEM->pln("General schema test started...");

  Schema s("IIFSB");
  assert(s.col_type(0) == 'I');
  assert(s.col_type(4) == 'B');
  assert(s.width() == 5);
  assert(s.length() == 0);

  s.add_column('I', new String("Next col"));
  s.add_column('F', new String("Anotha col"));
  s.add_column('B', new String("One more plz"));

  s.add_row(new String("Now the rows"));
  s.add_row(new String("Last row... for now :)"));

  assert(s.width() == 8);
  assert(s.length() == 2);
  assert(s.col_type(5) == 'I');
  assert(s.col_type(7) == 'B');

  assert(s.col_idx("Next col") == 5);
  assert(s.row_idx("Now the rows") == 0);

  SYSTEM->pln("General schema test passed!");
}

void findIntRowerTest()
{
  SYSTEM->pln("Find int rower test started...");

  //fill data -> a DataFrame full of multiples of 11
  Schema s("IIIII");
  DataFrame data(s);
  Row r(data.get_schema());
  int numRows = 100 * 1000;
  int mult = 11;
  for (int i = 0; i < numRows; i++)
  {
    for (int j = 0; j < r.width(); j++)
    {
      r.set(j, (i + j) * mult);
    }

    data.add_row(r);
  }

  //setup row dataframe
  Schema rowSchem("IIIIII");
  DataFrame vals(rowSchem);
  Row rTwo(vals.get_schema());
  int numRowsRDF = 9; // Num rows in Row DataFrame
  int colNum = 6;

  for (int i = 0; i < numRowsRDF; i++) {
	  rTwo.clear();
	  for (int j = 0; j < colNum; j++) {
		if (i == j) {
			rTwo.set(j, j * mult); //odd cols should be found/counted
		} else if (j == 3) {
			rTwo.set(j, j * mult);
		} else {
			rTwo.set(j, j * (mult - 1)); //should not be counted
		}
	  }
	  vals.add_row(rTwo);
  }
  /*SYSTEM->pln("Vals before map");
  vals.print();*/

  // Run FindRower
  FindRower ir(&data, &vals);
  vals.pmap(ir);

  /*SYSTEM->pln("\nVals after map");
  vals.print();*/

  // Check final count of Rower
  assert(ir.getCount() == 22);
  SYSTEM->pln("Find int rower test passed!");
}

// pmap version
void findIntRowerTest_pmap()
{
  SYSTEM->pln("Find int rower pmap test started...");

  //fill data -> a DataFrame full of multiples of 11
  Schema s("IIIII");
  DataFrame data(s);
  Row r(data.get_schema());
  int numRows = 100 * 1000;
  int mult = 11;
  for (int i = 0; i < numRows; i++)
  {
    for (int j = 0; j < r.width(); j++)
    {
      r.set(j, (i + j) * mult);
    }

    data.add_row(r);
  }

  //setup row dataframe
  Schema rowSchem("IIIIII");
  DataFrame vals(rowSchem);
  Row rTwo(vals.get_schema());
  int numRowsRDF = 9; // Num rows in Row DataFrame
  int colNum = 6;

  for (int i = 0; i < numRowsRDF; i++) {
	  rTwo.clear();
	  for (int j = 0; j < colNum; j++) {
		if (i == j) {
			rTwo.set(j, j * mult); //odd cols should be found/counted
		} else if (j == 3) {
			rTwo.set(j, j * mult);
		} else {
			rTwo.set(j, j * (mult - 1)); //should not be counted
		}
	  }
	  vals.add_row(rTwo);
  }
  /*SYSTEM->pln("Vals before map");
  vals.print();*/

  // Run FindRower
  FindRower ir(&data, &vals);
  vals.pmap(ir);

  /*SYSTEM->pln("\nVals after map");
  vals.print();*/
  
  // Check final count of Rower
  assert(ir.getCount() == 22);
  SYSTEM->pln("Find int rower pmap test passed!");
}

//this test is to measure accuracy, not as much performance
void findRower_variousTypes()
{
  SYSTEM->pln("Find rower test for various types started...");

  //fill data
  Schema s("IBFSB");
  DataFrame data(s);
  Row rOne(data.get_schema());
  Row rTwo(data.get_schema());
  Row rThree(data.get_schema());
  
  String str1("hello");
  String str2("world");
  String str3("goodbye");

  rOne.set(0, 4);
  rOne.set(1, true);
  rOne.set(2, 2.3f);
  rOne.set(3, &str1);
  rOne.set(4, false);
  data.add_row(rOne);

  rTwo.set(0, 7);
  rTwo.set(1, false);
  rTwo.set(2, 3.43f);
  rTwo.set(3, &str2);
  rTwo.set(4, true);
  data.add_row(rTwo);

  rThree.set(0, 667);
  rThree.set(1, false);
  rThree.set(2, 343.76f);
  rThree.set(3, &str3);
  rThree.set(4, false);
  data.add_row(rThree);

  Schema rowSchem("IBSFISBFI");
  DataFrame vals(rowSchem);
  Row rFour(vals.get_schema());
  Row rFive(vals.get_schema());
  
  String str4("not");
  String str5("in");

  rFour.set(0, 4); //Y
  rFour.set(1, true); //Y
  rFour.set(2, &str1); //Y
  rFour.set(3, 4.88f);
  rFour.set(4, 66);
  rFour.set(5, &str4);
  rFour.set(6, false); //Y
  rFour.set(7, 2.3f); //Y
  rFour.set(8, 7); //Y

  rFive.set(0, 46); 
  rFive.set(1, true);//Y
  rFive.set(2, &str3); //Y
  rFive.set(3, 3.43f); //Y
  rFive.set(4, 2);
  rFive.set(5, &str5);
  rFive.set(6, true); //Y
  rFive.set(7, 2.1f);
  rFive.set(8, 0);

  vals.add_row(rFour);
  vals.add_row(rFive);

  FindRower ir(&data, &vals);
  vals.map(ir);

  assert(ir.getCount() == 10);
  SYSTEM->pln("Find rower test for various types passed!");
}

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

int main()
{
  columnTests();
  testSchema();
  rowTest();
  testSchema2();
  genSchemaTest();
  basicDfTest();
  asIntTest();
  dataFrameLargeDataTest();

  // longer tests
  basicTest();
  dataFrameTest();
  basicTest2();

  // rowers
  findIntRowerTest();
  findIntRowerTest_pmap();
  findRower_variousTypes();
  lengthRowerTest();
  return 0;
}
