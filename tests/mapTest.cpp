#include <iostream>

#include "../src/utils/object.h"  // Your file with the CwC declaration of Object
#include "../src/utils/string.h"  // Your file with the String class
#include "../src/utils/map.h"
#include <assert.h>
 
// This test class was added to allow for easier testing
class Test {
public:

  Test() {
  }

  ~Test() {
  }

  void FAIL() {   exit(1);    }
  void OK(const char* m) { std::cout << "OK: " << m << '\n'; }
  void t_true(bool p) { if (!p) FAIL(); }
  void t_false(bool p) { if (p) FAIL(); }

  void test_put_0() {
    String* s = new String("Hello");
    String* t = new String("World");
    Object * a = new Object();
    Object * b = new Object();
    Map * mss = new Map();
    mss->put(s, t);
    Object * e = mss->put(a, b);
    t_true(mss->size() == 2);
    t_true(mss->contains_key(s));
    t_true(mss->contains_key(a));
    t_true(mss->get(a)->equals(b));
    t_true(e == nullptr);

    Object * c = new Object();
    Object * d = mss->put(a, c); //takes b out of the map
    t_true(mss->get(a)->equals(c));
    t_true(d->equals(b));
	delete b;
    delete mss;

    OK("test_put_0");
  }

  void test_put_1() {
	String* s = new String("Hello");
	String* t = new String("World");
    Object * a = new Object();
    Object * b = new Object();
    MapStrObj * mss = new MapStrObj();
    mss->put(s, a);
    Object * e = mss->put(t, b);
    t_true(mss->size() == 2);
    t_true(mss->contains_key(s));
    t_true(mss->contains_key(t));
    t_true(mss->get(t)->equals(b));
    t_true(e == nullptr);

    Object * c = new Object();
    Object * d = mss->put(s, c); //takes a out of map
    t_true(mss->get(s)->equals(c));
    t_true(d->equals(a));
	delete a;
    delete mss;

    OK("test_put_1");
  }

  void test_remove_0() {
    String* s = new String("Hello");
	String* sc = s->clone();
    String* t = new String("World");
    Object * a = new Object();
    Object * b = new Object();
    Map * mss = new Map();

    Object* res = mss->put(s, t);
	t_true(!res);
    res = mss->put(a, b);
	t_true(!res);
    t_true(mss->size() == 2);
    t_true(mss->contains_key(s));
    t_true(mss->contains_key(a));
    Object * q = mss->remove(s); // deletes s, returns t. removes pair from map
    t_true(t->equals(q));
    t_false(mss->contains_key(sc));
    t_true(mss->size() == 1);
	delete sc;
	delete q;
    delete mss;
    OK("test_remove_0");
  }

  void test_remove_1() {
    String* s = new String("Hello");
	String* sc = s->clone();
    String* t = new String("World");
    Object * u = new String("hi");
    String * w = new String("w");
    MapStrObj * mss = new MapStrObj();
    mss->put(s, t);
    mss->put(w, u);
    t_true(mss->size() == 2);
    t_true(mss->contains_key(s));
    t_true(mss->contains_key(w));
    Object * q = mss->remove(s); // deletes s, returns t. removes pair from map
    t_true(t->equals(q));
    t_false(mss->contains_key(sc));
    t_true(mss->size() == 1);
	delete q;
	delete sc;
    delete mss;
    OK("test_remove_1");
  }

  void test_get_0() {
    String* s = new String("Hello");
    String* t = new String("World");
    Object * a = new Object();
    Object * b = new Object();
    Map * mss = new Map();
    mss->put(a, b);
	mss->put(s, t);

    Object** keys = mss->get_keys();
    Object** values = mss->get_values();
    t_false(keys[0]->equals(values[0]));
    t_true(keys != nullptr);
    t_true(values != nullptr);
    t_true(keys[0]->equals(a));
    t_true(values[0]->equals(b));
	t_true(keys[1]->equals(s));
    t_true(values[1]->equals(t));

    delete mss;
    delete[] keys;
    delete[] values;

    OK("test_get_0");
  }

  void test_get_2() {
    String* s = new String("Hello");
    String* t = new String("World");
    MapStrObj * mss = new MapStrObj();
    Object** keys = mss->get_keys();
    Object** values = mss->get_values();
    t_true(keys != nullptr);
    t_true(values != nullptr);

    mss->put(s, t);
    delete[] values;
    delete[] keys;
    
    keys = mss->get_keys();
    values = mss->get_values();
    t_true(keys != nullptr);
    t_true(values != nullptr);
    t_true(keys[0]->equals(s));
    t_true(values[0]->equals(t));

    mss->put(s, t);
    t_true(keys != nullptr);
    t_true(values != nullptr);
    t_true(keys[0]->equals(s));
    t_true(values[0]->equals(t));

    delete mss->remove(s); // deletes s internally, returns t
    t_true(keys != nullptr);
    t_true(values != nullptr);
	t_true(mss->size() == 0);

    delete mss;
    delete[] keys;
    delete[] values;
    OK("test_get_2");
  }

  void test_equals_0() {
    String* s = new String("Hello");
    String* t = new String("World");
    Object * u = new String("hi");
    String * w = new String("w");
    Map* map1 = new Map();
    Map* map2 = new Map(97);
    t_true(map1->equals(map2));
    t_false(map1->equals(w));

    map1->put(s, u);
    map1->put(t, w);
    map2->put(t->clone(), w->clone());
    t_false(map1->equals(map2));
	String* s_clone = s->clone();
    map2->put(s_clone, s_clone);
    t_false(map1->equals(map2));
    map2->put(s_clone, u->clone());
    t_true(map1->equals(map2));
    delete map1;
    delete map2;
    OK("test_equals_0");
  }

  void test_contain_keys_0() {
    String* s = new String("Hello");
    String* t = new String("World");
    Object * u = new String("hi");
    String * w = new String("w");
	String * a = new String("AB");
	String * b = new String("CD");
    Map* map = new Map();
    MapStrObj* str_obj_map = new MapStrObj();
    map->put(s, t);
    t_true(map->contains_key(s));
    t_false(map->contains_key(u));
    map->put(u, w);
    t_true(map->contains_key(u));

    t_false(str_obj_map->contains_key(b));
    str_obj_map->put(b, a);
    t_true(str_obj_map->contains_key(b));

    delete map;
    delete str_obj_map;
    OK("test_contains_keys_0");
  }

  /*void test_hash_1() { adding same object twice will break on 
    MapStrObj* m1 = new MapStrObj();
    MapStrObj* m2 = new MapStrObj();
    Object * a = new Object();
    String * s = new String("s");

    t_true(m1->hash() == m2->hash());
    m1->put(s, a);
    t_false(m1->hash() == m2->hash());
    m2->put(s, a);
    t_true(m1->hash() == m2->hash());

    delete m1;
    delete m2;

    OK("test_hash_1");
  }*/
  
  void test_big() {
	String* a = new String("ABC");
	String* a1 = new String("ABC-1");
	String* b = new String("CCC");
	String* b1 = new String("CCC-1");
	String* c = new String("WCA");
	String* c1 = new String("WCA-1");
	String* d = new String("WCB");
	String* d1 = new String("WCB-1");


	String* e = new String("WCC");
	String* e1 = new String("WCC-1");

	MapStrObj* map = new MapStrObj();
	map->put(a, a1);
	map->put(b, b1);
	map->put(c, c1);
	map->put(d, d1);
	map->put(e, e1);
	
	assert(map->get(a)->equals(a1));
	assert(map->get(e)->equals(e1));
	assert(!(map->get(e)->equals(a1)));

	Object* oldA = map->put(e, a1->clone()); //overwrite value for key e
	String* oldA_str = dynamic_cast<String*>(oldA);
	assert(oldA_str != nullptr && oldA->equals(e1));
	delete oldA;
	
	assert(map->get(e)->equals(a1));

	delete map;
  }
};

int main(int argc, char** argv) {
  Test* test = new Test();
  test->test_put_0();
  test->test_put_1();
  test->test_remove_0();
  test->test_remove_1();
  test->test_get_0();
  test->test_get_2();
  test->test_equals_0();
  test->test_contain_keys_0();
  //test->test_hash_1();
  test->test_big();
  test->OK("All map tests passed!");
  delete test;
  return 0;
}

