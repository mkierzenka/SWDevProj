#include "../src/utils/array.h"
#include "../src/utils/helper.h"
#include "../src/utils/string.h"

void OK(const char *m)
{
  Sys *c = new Sys();
  c->p("OK: ");
  c->pln(m);
  delete c;
}

void FAIL() { exit(1); }
void t_true(bool p)
{
  if (!p)
    FAIL();
}
void t_false(bool p)
{
  if (p)
    FAIL();
}

// test equal
void test1()
{
  String *s = new String("Hello");
  Array *a = new Array();
  Array *b = new Array();
  a->add(s->clone());
  b->add(s->clone());
  t_true(a->equals(a));
  t_true(b->equals(b));
  t_true(a->equals(b));
  b->add(s->clone());
  t_false(a->equals(b));
  delete a;
  delete b;
  delete s;
  OK("Test 1 passed!");
}

// test length, add, set, remove, get
void test2()
{
  String *s = new String("Hello");
  String *t = new String("World");
  Array *a = new Array();
  t_true(a->length() == 0);
  a->add(s);
  a->add(t);
  t_true(a->get(1)->equals(t));
  t_true(a->length() == 2);
  String* r = dynamic_cast<String*>(a->remove(1));
  t_true(a->length() == 1);
  t_true(a->get(0)->equals(s));
  delete a;
  delete r;
  OK("Test 2 passed!");
}

// test hash_me
void test3()
{
  String *s = new String("Hello");
  Array *a = new Array();
  a->add(s);
  t_true(a->hash() == a->hash());
  delete a;
  OK("Test 3 passed!");
}

// test index_of, clear
void test4()
{
  String *s = new String("Hello");
  String *t = new String("World");
  Array *a = new Array();
  a->add(s);
  a->add(t);
  t_true(a->index_of(s) == 0);
  t_true(a->index_of(t) == 1);
  a->clear();
  t_true(a->length() == 0);
  delete a;
  OK("Test 4 passed!");
}

void testBig() {
	String* a = new String("ABC");
	String* b = new String("CCC");
	String* c = new String("WCA");
	String* d = new String("WCB");
	String* e = new String("WCC");
	String* f = new String("HAHA!");

	Array* arr = new Array();
	arr->add(a);
	arr->add(b);
	arr->add(c);
	arr->add(d);
	arr->add(e);
	
	assert(arr->length() == 5);
	assert(arr->index_of(b) == 1);
	assert(arr->get(1)->equals(b));
	assert(arr->get(3)->equals(d));
	assert(arr->index_of(e) == (arr->length() - 1));
	String* b_rem = dynamic_cast<String*>(arr->set(1, f));
	assert(arr->length() == 5);
	assert(arr->index_of(b) == -1);
	assert(arr->index_of(f) == 1);
	assert(arr->get(3)->equals(d));
	String* c_rem = dynamic_cast<String*>(arr->remove(2));
	assert(arr->length() == 4);
	assert(arr->get(2)->equals(d));
	
  delete b_rem;
	delete c_rem;
	delete arr;
	OK("Test Big passed!");
}

// make all tests run
int main()
{
  test1();
  test2();
  test3();
  test4();
  testBig();

  OK("All array tests passed!");
  return 0;
}