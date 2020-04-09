#include <assert.h>
#include "../src/dataframe/dataframe.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/utils/string.h"
#include "incwriter.h"

Sys SYSTEM;

void keyTest1() {
	Key* k1 = new Key("ABC", 0);
	String* abc = new String("ABC");
	Key* k2 = new Key("4-7", 12);
	String* s = new String("KeyStr");
	Key* k3 = new Key(s, 1);
	Key* k3_2 = k3->addIndex(2);
	Key* k3c = k3->clone();
	Key* k3cc = new Key(s, 1);

	assert(k1->getKeyStr()->equals(abc));
	assert(k1->getNode() == 0);
	assert(k3->getKeyStr()->equals(s));
	assert(k2->getNode() == 12);
	assert(k3->equals(k3));
	assert(k3c->getKeyStr()->equals(k3->getKeyStr()));
	assert(k3c->getNode() == k3->getNode());
	assert(k3c->equals(k3));
	assert(k3cc->equals(k3));
	String* expected = new String("KeyStr-2");
	assert(k3_2->getKeyStr()->equals(expected));

	delete expected;
	delete k3cc;
	delete k3c;
	delete k3_2;
	delete k3;
	delete s;
	delete k2;
	delete abc;
	delete k1;
}

int main()
{
  SYSTEM.pln("Key test started...");
  keyTest1();
  SYSTEM.pln("Key test passed!");
  return 0;
}
