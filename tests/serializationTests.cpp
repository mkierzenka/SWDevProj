//lang:Cpp

#include <assert.h>
#include <stdio.h>

#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/utils/string.h"
#include "../src/dataframe/boolblock.h"
#include "../src/dataframe/schema.h"

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

/** 
 * Test serializing and deserializing on the different classes we may use it for (arrays, blocks, 
 * dataframes, etc.). Will create new object from deserialized result and make sure they equal
 */
int main()
{
    serializeKeyTest();
    serializeBoolBlockTest();
    serializeSchemaTest();
}