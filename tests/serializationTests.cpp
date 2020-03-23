//lang:Cpp

#include <assert.h>
#include <stdio.h>

#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/kvstore.h"
#include "../src/utils/string.h"
#include "../src/dataframe/boolblock.h"
#include "../src/dataframe/floatblock.h"
#include "../src/dataframe/intblock.h"
#include "../src/dataframe/distributedarray.h"
#include "../src/dataframe/schema.h"
#include "../src/dataframe/column.h"

void serializeKeyTest()
{
    printf("Serialize key test started\n");

    String* keyStr = new String("hello");
    Key* k = new Key(keyStr, 0);
    Serializer *s = new Serializer();
    k->serialize(s);

    Key* newK = new Key();
    newK->deserialize(s);

    assert(k->equals(newK));

    printf("Serialize key test passed!\n");

    delete keyStr;
    delete k;
    delete s;
    delete newK;
}

void serializeBoolBlockTest()
{
    printf("Serialize bool block test started\n");

    BoolBlock* b = new BoolBlock();
    b->add(true);
    b->add(true);
    b->add(false);

    Serializer* s = new Serializer();
    b->serialize(s);

    BoolBlock* newB = new BoolBlock();
    newB->deserialize(s);

    assert(b->equals(newB));

    printf("Serialize bool block test passed!\n");

    delete b;
    delete s;
    delete newB;
}

void serializeFloatBlockTest()
{
    printf("Serialize float block test started\n");

    FloatBlock* b = new FloatBlock();
    b->add(0);
    b->add(3.9);
    b->add(57.92);

    Serializer* s = new Serializer();
    b->serialize(s);

    FloatBlock* newB = new FloatBlock();
    newB->deserialize(s);

    assert(b->equals(newB));

    printf("Serialize float block test passed!\n");

    delete b;
    delete s;
    delete newB;
}

void serializeSchemaTest()
{
    printf("Serialize schema test started\n");

    Schema* schem = new Schema("IBFSSI");
    Serializer* serial = new Serializer();
    schem->serialize(serial);

    Schema* newSchem = new Schema();
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

    KVStore* store = new KVStore(0);
    String* keyStr = new String("0-0");
    Key* k = new Key(keyStr, 0);

    Column* c = new Column(store, k, ColType::Integer);
    int* vals = new int[5];
    for (int i = 0; i < 5; i++)
    {
        vals[i] = 2*i + 3;
    }

    //add values to the column
    c->add_all(5, vals);

    Serializer* s = new Serializer();
    c->serialize(s);

    Column* newC = new Column();
    newC->deserialize(s);

    assert(c->equals(newC));

    printf("Column serialization test passed!\n");
}

/** 
 * Test serializing and deserializing on the different classes we may use it for (arrays, blocks, 
 * dataframes, etc.). Will create new object from deserialized result and make sure they equal
 */
int main()
{
    // serializeKeyTest();
    // serializeBoolBlockTest();
    // serializeFloatBlockTest();
    // serializeSchemaTest();
    serializeColumnTest();
}