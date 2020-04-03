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
    Cache* c = new Cache(3);

    //create some key-value pairs
    Key* key1 = new Key("key1", 0);
    Key* key2 = new Key("key2", 1); 
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
    c->put(key1, ib);
    c->put(key2, sb);
    c->put(key3, bb);

    assert(c->keyOrder_->size() == 3);

    assert(c->containsKey(key1));
    assert(c->getBlock(key1)->equals(ib));
    assert(c->containsKey(key2));
    assert(c->getBlock(key2)->equals(sb));
    assert(c->containsKey(key3));
    assert(c->getBlock(key3)->equals(bb));
    assert(c->keyOrder_->size() == 3);
    assert(c->data_->size() == 3);
    assert(!c->containsKey(key4));

    c->put(key4, db);
    assert(!c->containsKey(key1)); //because we dropped it (Cache size = 3)
    assert(c->containsKey(key4));
    assert(c->getBlock(key4)->equals(db));

    c->put(key5, ib2);
    assert(!c->containsKey(key1));
    assert(!c->containsKey(key2));
    assert(c->containsKey(key3));
    assert(c->getBlock(key3)->equals(bb));
    assert(c->containsKey(key4));
    assert(c->getBlock(key4)->equals(db));
    assert(c->containsKey(key5));
    assert(c->getBlock(key5)->equals(ib2));

    delete c;
    delete key1, key2, key3, key4, key5;
    delete ib, sb, bb, db, ib2;
    printf("Cache test passed!\n");
}