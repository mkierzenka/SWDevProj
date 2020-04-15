#include <assert.h>
#include "../src/dataframe/schema.h"

Sys *SYSTEM = new Sys();

void testSchema()
{
  SYSTEM->pln("Schema basic test started...");

  Schema *scm = new Schema();
  scm->add_column('I');
  scm->add_column('D');
  scm->add_column('D');
  assert(scm->width() == 3);
  assert(scm->length() == 0);

  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(2) == 'D');

  scm->add_column('S');
  scm->add_column('B');
  assert(scm->col_type(3) == 'S');
  assert(scm->col_type(4) == 'B');

  delete scm;
  SYSTEM->pln("Schema basic test passed!");
}

void testSchema2()
{
  SYSTEM->pln("Schema2 test started...");
  Schema *scm = new Schema("II");
  assert(scm->width() == 2);
  assert(scm->length() == 0);
  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(1) == 'I');
  for (int i = 0; i < 1000; i++)
  {
    scm->add_row();
  }
  assert(scm->length() == 1000);
  delete scm;
  SYSTEM->pln("Schema2 test passed!");
}

void testSchema3()
{
  SYSTEM->pln("Schema3 test started...");
  Schema *scm = new Schema("II");
  assert(scm->width() == 2);
  assert(scm->length() == 0);
  assert(scm->col_type(0) == 'I');
  assert(scm->col_type(1) == 'I');
  scm->add_rows(1000);
  assert(scm->length() == 1000);
  delete scm;
  SYSTEM->pln("Schema3 test passed!");
}

void genSchemaTest()
{
  SYSTEM->pln("General schema test started...");
  Schema s("IIDSB");
  assert(s.col_type(0) == 'I');
  assert(s.col_type(4) == 'B');
  assert(s.width() == 5);
  assert(s.length() == 0);

  s.add_column('I');
  s.add_column('D');
  s.add_column('B');
  s.add_row();
  s.add_row();

  assert(s.width() == 8);
  assert(s.length() == 2);
  assert(s.col_type(5) == 'I');
  assert(s.col_type(7) == 'B');
  SYSTEM->pln("General schema test passed!");
}

int main()
{
  SYSTEM->pln("Schema tests starting...");
  testSchema();
  testSchema2();
  testSchema3();
  genSchemaTest();
  SYSTEM->pln("Schema tests passed!");
  return 0;
}
