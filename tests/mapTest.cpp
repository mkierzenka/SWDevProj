#include "../src/utils/string.h"
#include "../src/store/map.h"

#include <assert.h>


int main() {
	Sys system;
	system.pln("Map test started...");
	
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

	Object* oldA = map->put(e, a1->clone());
	String* oldA_str = dynamic_cast<String*>(oldA);
	assert(oldA_str != nullptr && oldA->equals(e1));
	delete e1;
	
	assert(map->get(e)->equals(a1));

	delete map;
	system.pln("Map test passed!");
	return 0;
}