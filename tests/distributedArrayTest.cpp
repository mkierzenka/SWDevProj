//lang:cpp

#include "../src/dataframe/column/distributedarray.h"
#include "../src/store/kvstore.h"
#include "../src/dataframe/block/block.h"
#include "../src/dataframe/block/intblock.h"
#include "../src/dataframe/block/boolblock.h"
#include "../src/dataframe/block/stringblock.h"
#include "../src/dataframe/block/doubleblock.h"
#include "../src/utils/args.h"
#include <assert.h>
#include <math.h>

Arguments args;

/**
 * Test distributed array class
 */

// Checking DA keeps track of keys okay. And can serialize/deserialize
void tracksKeys() {
	puts("Starting DA Key test...");
	KVStore* kv = new KVStore(0, nullptr);
    DistributedArray* da = new DistributedArray(kv);

    Key k1("key1-0", 0);
    Key k2("key2-me", 0);
    Key k3("key3", 10);

    assert(da->length() == 0);
    assert(!da->containsKey(&k1));
	da->addKey(&k1);
    assert(da->length() == 1);
    assert(da->containsKey(&k1));
	assert(!da->containsKey(&k2));

	da->addKey(&k2);
	da->addKey(&k3);
	assert(da->length() == 3);
    assert(da->containsKey(&k1));
	assert(da->containsKey(&k2));
	assert(da->containsKey(&k3));

	Serializer s;
	da->serialize(&s);
	DistributedArray* da2 = new DistributedArray(kv);
	da2->deserialize(&s);
	assert(da->equals(da2));
    delete da2;
	delete kv;
    delete da;
	puts("DA Key test passed!");
}

// The definition of 2 doubles being equal in this test suite
bool dblEquals_(double a, double b) {
	return fabs(a - b) < 0.000001;
}

// Checking you can get data from a DistributedArray
void dataTest() {
	puts("Starting DA Data test...");
	KVStore* kv = new KVStore(0, nullptr);
    DistributedArray* da = new DistributedArray(kv);

    Key k1("key1-0", 0);
    Key k2("key2-me", 0);
	Key k3("ints", 0);
	Key k4("strs", 0);

	Serializer s;
	BoolBlock* bb = new BoolBlock();
	bb->add(true);
	bb->add(false);
	bb->add(true);
	bb->add(true);
	bb->serialize(&s);
	Value v1(s.getBuffer(), s.getNumBytesWritten());
	s.clear();
	kv->put(&k1, &v1);

	DoubleBlock* db = new DoubleBlock();
	db->add(1.0);
	db->add(2.0);
	db->add(3.0);
	db->add(4.0);
	db->serialize(&s);
	Value v2(s.getBuffer(), s.getNumBytesWritten());
	s.clear();
	kv->put(&k2, &v2);

	IntBlock* ib = new IntBlock();
	ib->add(10);
	ib->add(12);
	ib->add(14);
	ib->add(16);
	ib->add(18);
	ib->add(20);
	ib->serialize(&s);
	Value v3(s.getBuffer(), s.getNumBytesWritten());
	s.clear();
	kv->put(&k3, &v3);

	StringBlock* sb = new StringBlock();
	sb->add(new String("AB"));
	sb->add(new String("CDE"));
	sb->add(new String("FGHI"));
	sb->add(new String("JKLMN"));
	sb->add(new String("12345"));
	sb->serialize(&s);
	Value v4(s.getBuffer(), s.getNumBytesWritten());
	s.clear();
	kv->put(&k4, &v4);

	da->addKey(&k1);
	da->addKey(&k2);
	da->addKey(&k3);
	da->addKey(&k4);
	assert(da->length() == 4);

	assert(da->getBool(&k1, 0) == bb->get(0));
	assert(da->getBool(&k1, 2) == bb->get(2));
	assert(da->getBool(&k1, 3) == bb->get(3));
	delete bb;

	assert(dblEquals_(da->getDouble(&k2, 0), db->get(0)));
	assert(dblEquals_(da->getDouble(&k2, 1), db->get(1)));
	assert(dblEquals_(da->getDouble(&k2, 3), db->get(3)));
	delete db;

	assert(da->getInt(&k3, 0) == ib->get(0));
	assert(da->getInt(&k3, 2) == ib->get(2));
	assert(da->getInt(&k3, 4) == ib->get(4));
	assert(da->getInt(&k3, 5) == ib->get(5));
	delete ib;

	String* actual = da->getString(&k4, 0);
	assert(actual->equals(sb->get(0)));
	delete actual;
	actual = da->getString(&k4, 1);
	assert(actual->equals(sb->get(1)));
	delete actual;
	actual = da->getString(&k4, 2);
	assert(actual->equals(sb->get(2)));
	delete actual;
	delete sb;

	da->serialize(&s);
	DistributedArray* da2 = new DistributedArray(kv);
	da2->deserialize(&s);
	assert(da2->length() == 4);
    delete da2;
	delete kv;
    delete da;
	puts("DA Data test passed!");
}

int main()
{
	puts("Starting Distributed Array tests...");
	args.blockSize = 1024;
	tracksKeys();
	dataTest();
	puts("Distributed Array tests passed!");
}