//lang:Cpp

#include <assert.h>
#include <stdio.h>

#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/kvstore.h"
#include "../src/utils/string.h"
#include "../src/utils/args.h"
#include "../src/dataframe/block/boolblock.h"
#include "../src/dataframe/block/doubleblock.h"
#include "../src/dataframe/block/stringblock.h"
#include "../src/dataframe/block/intblock.h"
#include "../src/dataframe/column/distributedarray.h"
#include "../src/dataframe/column/columnarray.h"
#include "../src/dataframe/schema.h"
#include "../src/dataframe/column/column.h"

Arguments args;

void serializeKeyTest()
{
    printf("Serialize key test started\n");

    String *keyStr = new String("hello");
    Key *k = new Key(keyStr, 0);
    Serializer *s = new Serializer();
    k->serialize(s);

    Key *newK = new Key();
    newK->deserialize(s);

    assert(k->equals(newK));

    printf("Serialize key test passed!\n");

    delete keyStr;
    delete k;
    delete s;
    delete newK;
}

void serializeStringBlockTest()
{
    printf("Serialize String block test started\n");

    StringBlock *b = new StringBlock();
    b->add(new String("ABC"));
    b->add(new String("DDDD$"));
    b->add(new String("fdsafdsaf"));
    b->add(new String("2122222"));
    b->add(new String("apple"));

    Serializer *s = new Serializer();
    b->serialize(s);

    StringBlock *newB = new StringBlock();
    newB->deserialize(s);

    assert(b->equals(newB));

    printf("Serialize String block test passed!\n");

    delete b;
    delete s;
    delete newB;
}

void serializeBoolBlockTest()
{
    printf("Serialize bool block test started\n");

    BoolBlock *b = new BoolBlock();
    b->add(true);
    b->add(true);
    b->add(false);

    Serializer *s = new Serializer();
    b->serialize(s);

    BoolBlock *newB = new BoolBlock();
    newB->deserialize(s);

    assert(b->equals(newB));

    printf("Serialize bool block test passed!\n");

    delete b;
    delete s;
    delete newB;
}

void serializeDoubleBlockTest()
{
    printf("Serialize double block test started\n");

    DoubleBlock *b = new DoubleBlock();
    b->add(0);
    b->add(3.9);
    b->add(57.92);

    Serializer *s = new Serializer();
    b->serialize(s);

    DoubleBlock *newB = new DoubleBlock();
    newB->deserialize(s);

    assert(b->equals(newB));

    printf("Serialize double block test passed!\n");

    delete b;
    delete s;
    delete newB;
}

void serializeIntBlockTest()
{
    printf("Serialize int block test started\n");

    IntBlock *ib = new IntBlock();
    ib->add(0);
    ib->add(3);
    ib->add(5792);

    Serializer *s = new Serializer();
    ib->serialize(s);

    IntBlock *newIB = new IntBlock();
    newIB->deserialize(s);

    assert(ib->equals(newIB));

    printf("Serialize int block test passed!\n");

    delete ib;
    delete s;
    delete newIB;
}

void serializeSchemaTest()
{
    printf("Serialize schema test started\n");

    Schema *schem = new Schema("IBFSSI");
    Serializer *serial = new Serializer();
    schem->serialize(serial);

    Schema *newSchem = new Schema();
    newSchem->deserialize(serial);

    assert(schem->equals(newSchem));

    printf("Serialize schema test passed!\n");

    delete schem;
    delete serial;
    delete newSchem;
}

void serializeColumnTest()
{
    printf("Column serialization test started\n");

    KVStore *store = new KVStore(0, nullptr);
    String *keyStr = new String("0-0");
    Key *k = new Key(keyStr, 0);
    delete keyStr;

    Column *c = new Column(store, k, ColType::Integer);
    int *vals = new int[5];
    for (int i = 0; i < 5; i++)
    {
        vals[i] = 2 * i + 3;
    }

    //add values to the column
    c->add_all(5, vals, 0);

    Serializer *s = new Serializer();
    c->serialize(s);

    Column *newC = new Column();
    newC->setStore(store);
    newC->deserialize(s);

    assert(c->equals(newC));

    delete store;
    delete c;
    delete s;
    delete newC;
    delete[] vals;
    printf("Column serialization test passed!\n");
}

void serializeDistArrTest()
{
    printf("Distributed Array serialization test started\n");

    KVStore *store = new KVStore(0, nullptr);
    String keyStr("data-0-0");
    Key k(&keyStr, 0);

    String keyStr1("data-0-1");
    Key k1(&keyStr1, 0);

    String keyStr2("data-0-2");
    Key k2(&keyStr2, 0);

    String keyStr3("data-0-3");
    Key k3(&keyStr3, 0);

    DistributedArray *da = new DistributedArray(store);
    da->addKey(&k);
    da->addKey(&k1);
    da->addKey(&k2);
    da->addKey(&k3);

    Serializer *s = new Serializer();
    da->serialize(s);
    DistributedArray *da2 = new DistributedArray(store);
    da2->deserialize(s);
    assert(da->equals(da2));
    delete da2;
    delete s;
    delete da;
    delete store;
    printf("Distributed Array serialization test passed!\n");
}

void serializeColumnArrTest()
{
    printf("Column Array serialization test started\n");

    KVStore *store = new KVStore(0, nullptr);
    String *keyStr = new String("data-0");
    Key *k = new Key(keyStr, 0);
    delete keyStr;

    ColumnArray *ca = new ColumnArray(store, k);
    size_t numElems = 7;
    double *dbls = new double[numElems];
    for (size_t i = 0; i < numElems; i++)
    {
        dbls[i] = (i * 1.5f);
    }

    ca->add_column_fromarray(7, dbls);
    delete[] dbls;

    Serializer *s = new Serializer();
    ca->serialize(s);
    ColumnArray *ca2 = new ColumnArray(store, k);
    ca2->deserialize(s);
    assert(ca->equals(ca2));
    delete ca2;
    delete s;
    delete ca;
    delete k;
    delete store;
    printf("Column Array serialization test passed!\n");
}

/** 
 * Test serializing and deserializing on the different classes we may use it for (arrays, blocks, 
 * dataframes, etc.). Will create new object from deserialized result and make sure they equal
 */
int main(int argc, char** argv)
{
    args.blockSize = 1024;
    serializeKeyTest();
    serializeBoolBlockTest();
    serializeDoubleBlockTest();
    serializeStringBlockTest();
    serializeIntBlockTest();
    serializeSchemaTest();
    serializeColumnTest();
    serializeDistArrTest();
    serializeColumnArrTest();
    printf("All Serialization Tests Passed!\n");
}