#include <iostream>

#include "../src/utils/object.h" // Your file with the CwC declaration of Object
#include "../src/utils/string.h" // Your file with the String class
#include "../src/utils/map.h"
#include <assert.h>

// This test class was added to allow for easier testing
class Test
{
public:
  Test()
  {
  }

  ~Test()
  {
  }

  void OK(const char *m) { std::cout << "OK: " << m << '\n'; }

  void test_put_0()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    String *a = new String("a");
    Object *b = new Object();
    Map *mss = new Map();
    mss->put(s, t);
    Object *e = mss->put(a->clone(), b);
    assert(mss->size() == 2);
    assert(mss->contains_key(s));
    assert(mss->contains_key(a));
    assert(mss->get(a)->equals(b));
    assert(e == nullptr);

    Object *c = new Object();
    Object *d = mss->put(a->clone(), c); //takes b out of the map
    assert(mss->get(a)->equals(c));
    assert(d->equals(b));

    delete a;
    delete b;
    delete mss;

    OK("test_put_0");
  }

  void test_put_1()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    Object *a = new Object();
    Object *b = new Object();
    MapStrObj *mss = new MapStrObj();
    mss->put(s->clone(), a);
    Object *e = mss->put(t, b);
    assert(mss->size() == 2);
    assert(mss->contains_key(s));
    assert(mss->contains_key(t));
    assert(mss->get(t)->equals(b));
    assert(e == nullptr);

    Object *c = new Object();
    Object *d = mss->put(s->clone(), c); //takes a out of map
    assert(mss->get(s)->equals(c));
    assert(d->equals(a));
    delete a;
    delete mss;
    delete s;

    OK("test_put_1");
  }

  void test_remove_0()
  {
    String *s = new String("Hello");
    String *sc = s->clone();
    String *t = new String("World");
    Object *a = new Object();
    Object *b = new Object();
    Map *mss = new Map();

    Object *res = mss->put(s, t);
    assert(!res);
    res = mss->put(a, b);
    assert(!res);
    assert(mss->size() == 2);
    assert(mss->contains_key(s));
    assert(mss->contains_key(a));
    Object *q = mss->remove(s); // deletes s, returns t. removes pair from map
    assert(t->equals(q));
    assert(!mss->contains_key(sc));
    assert(mss->size() == 1);
    delete sc;
    delete q;
    delete mss;
    OK("test_remove_0");
  }

  void test_remove_1()
  {
    String *s = new String("Hello");
    String *sc = s->clone();
    String *t = new String("World");
    Object *u = new String("hi");
    String *w = new String("w");
    MapStrObj *mss = new MapStrObj();
    mss->put(s, t);
    mss->put(w, u);
    assert(mss->size() == 2);
    assert(mss->contains_key(s));
    assert(mss->contains_key(w));
    Object *q = mss->remove(s); // deletes s, returns t. removes pair from map
    assert(t->equals(q));
    assert(!mss->contains_key(sc));
    assert(mss->size() == 1);
    delete q;
    delete sc;
    delete mss;
    OK("test_remove_1");
  }

  void test_get_0()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    Object *a = new Object();
    Object *b = new Object();
    Map *mss = new Map();
    mss->put(a, b);
    mss->put(s, t);

    Object **keys = mss->get_keys();
    Object **values = mss->get_values();
    assert(!keys[0]->equals(values[0]));
    assert(keys != nullptr);
    assert(values != nullptr);
    assert(keys[0]->equals(a));
    assert(values[0]->equals(b));
    assert(keys[1]->equals(s));
    assert(values[1]->equals(t));

    delete mss;
    delete[] keys;
    delete[] values;

    OK("test_get_0");
  }

  void test_get_2()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    MapStrObj *mss = new MapStrObj();
    Object **keys = mss->get_keys();
    Object **values = mss->get_values();
    assert(keys != nullptr);
    assert(values != nullptr);

    mss->put(s->clone(), t);
    delete[] values;
    delete[] keys;

    keys = mss->get_keys();
    values = mss->get_values();
    assert(keys != nullptr);
    assert(values != nullptr);
    assert(keys[0]->equals(s));
    assert(values[0]->equals(t));

    mss->put(s->clone(), t);
    assert(keys != nullptr);
    assert(values != nullptr);
    assert(keys[0]->equals(s));
    assert(values[0]->equals(t));

    delete mss->remove(s); // deletes s internally, returns t
    assert(keys != nullptr);
    assert(values != nullptr);
    assert(mss->size() == 0);

    delete mss;
    delete s;
    delete[] keys;
    delete[] values;
    OK("test_get_2");
  }

  void test_equals_0()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    Object *u = new String("hi");
    String *w = new String("w");
    Map *map1 = new Map();
    Map *map2 = new Map(97);
    assert(map1->equals(map2));
    assert(!map1->equals(w));

    map1->put(s, u);
    map1->put(t, w);
    map2->put(t->clone(), w->clone());
    assert(!map1->equals(map2));
    String *s_clone = s->clone();
    map2->put(s_clone, s_clone);
    assert(!map1->equals(map2));
    map2->put(s_clone->clone(), u->clone());
    assert(map1->equals(map2));
    delete map1;
    delete map2;
    OK("test_equals_0");
  }

  void test_contain_keys_0()
  {
    String *s = new String("Hello");
    String *t = new String("World");
    Object *u = new String("hi");
    String *w = new String("w");
    String *a = new String("AB");
    String *b = new String("CD");
    Map *map = new Map();
    MapStrObj *str_obj_map = new MapStrObj();
    map->put(s, t);
    assert(map->contains_key(s));
    assert(!map->contains_key(u));
    map->put(u, w);
    assert(map->contains_key(u));

    assert(!str_obj_map->contains_key(b));
    str_obj_map->put(b, a);
    assert(str_obj_map->contains_key(b));

    delete map;
    delete str_obj_map;
    OK("test_contains_keys_0");
  }

  void test_big()
  {
    String *a = new String("ABC");
    String *a1 = new String("ABC-1");
    String *b = new String("CCC");
    String *b1 = new String("CCC-1");
    String *c = new String("WCA");
    String *c1 = new String("WCA-1");
    String *d = new String("WCB");
    String *d1 = new String("WCB-1");

    String *e = new String("WCC");
    String *e1 = new String("WCC-1");

    MapStrObj *map = new MapStrObj();
    map->put(a, a1);
    map->put(b, b1);
    map->put(c, c1);
    map->put(d, d1);
    map->put(e->clone(), e1);

    assert(map->get(a)->equals(a1));
    assert(map->get(e)->equals(e1));
    assert(!(map->get(e)->equals(a1)));

    Object *oldA = map->put(e->clone(), a1->clone()); //overwrite value for key e
    String *oldA_str = dynamic_cast<String *>(oldA);
    assert(oldA_str != nullptr && oldA->equals(e1));
    delete oldA;

    assert(map->get(e)->equals(a1));

    delete map;
    delete e;
  }
};

int main(int argc, char **argv)
{
  Test *test = new Test();
  test->test_put_0();
  test->test_put_1();
  test->test_remove_0();
  test->test_remove_1();
  test->test_get_0();
  test->test_get_2();
  test->test_equals_0();
  test->test_contain_keys_0();
  test->test_big();
  test->OK("All map tests passed!");
  delete test;
  return 0;
}
