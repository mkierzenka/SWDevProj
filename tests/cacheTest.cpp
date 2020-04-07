#include "../src/dataframe/cache.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/dataframe/block.h"
#include "../src/dataframe/intblock.h"
#include "../src/dataframe/doubleblock.h"
#include "../src/dataframe/boolblock.h"
#include "../src/dataframe/stringblock.h"

#include <assert.h>

int main()
{
    printf("Cache test starting...\n");
    //create cache and some elements to put into the cache
    Cache* myCache = new Cache(3);

    //create some key-value pairs
    Key* key1 = new Key("key1", 0);
    Key* key1c = key1->clone();
    Key* key2 = new Key("key2", 1);
    Key* key2c = key2->clone();
    Key* key3 = new Key("key3", 2);
    Key* key4 = new Key("key4", 3);
    Key* key5 = new Key("key5", 2);

    IntBlock* ib = new IntBlock();
    ib->add(17);
    ib->add(44);
    ib->add(123);

    StringBlock* sb = new StringBlock();
    String* s1 = new String("value1");
    String* s2 = new String("value 2");
    String* s3 = new String("james");
    String* s4 = new String("bond");
    String* s5 = new String("1234");
    sb->add(s1);
    sb->add(s2);
    sb->add(s3);
    sb->add(s4);
    sb->add(s5);

    BoolBlock* bb = new BoolBlock();
    bb->add(true);
    bb->add(true);
    bb->add(false);

    DoubleBlock* db = new DoubleBlock();
    db->add(1.92);
    db->add(0.0);
    db->add(7.777);
    db->add(0.99999);

    IntBlock* ib2 = new IntBlock();
    ib2->add(3);
    ib2->add(88888);
    ib2->add(229375);

    //add pairings to cache
    myCache->put(key1, ib);
    myCache->put(key2, sb);
    myCache->put(key3, bb);

    assert(myCache->keyOrder_->size() == 3);

    assert(myCache->containsKey(key1));
    assert(myCache->getBlock(key1)->equals(ib));
    assert(myCache->containsKey(key2));
    assert(myCache->getBlock(key2)->equals(sb));
    assert(myCache->containsKey(key3));
    assert(myCache->getBlock(key3)->equals(bb));
    assert(myCache->keyOrder_->size() == 3);
    assert(myCache->data_->size() == 3);
    assert(!myCache->containsKey(key4));

    myCache->put(key4, db);
    assert(!myCache->containsKey(key1c)); //because we dropped it (Cache size = 3)
    assert(myCache->containsKey(key4));
    assert(myCache->getBlock(key4)->equals(db));

    myCache->put(key5, ib2);
    assert(!myCache->containsKey(key1c));
    assert(!myCache->containsKey(key2c));
    assert(myCache->containsKey(key3));
    assert(myCache->getBlock(key3)->equals(bb));
    assert(myCache->containsKey(key4));
    assert(myCache->getBlock(key4)->equals(db));
    assert(myCache->containsKey(key5));
    assert(myCache->getBlock(key5)->equals(ib2));

    delete key1c;
    delete key2c;
    delete myCache;
    printf("Cache test passed!\n");
}