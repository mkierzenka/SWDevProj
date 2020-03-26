#include "../src/dataframe/cache.h"
#include "../src/store/key.h"
#include "../src/store/value.h"

#include <assert.h>

int main()
{
    //create cache and some elements to put into the cache
    Cache* c = new Cache(3);

    //create some key-value pairs
    Key* key1 = new Key("key1", 0);
    Key* key2 = new Key("key2", 1); 
    Key* key3 = new Key("key3", 2);
    Key* key4 = new Key("key4", 3);    
    Key* key5 = new Key("key5", 2);

    Value* val1 = new Value("value1", 7);
    Value* val2 = new Value("value 2", 8);
    Value* val3 = new Value("val3", 5);
    Value* val4 = new Value("v4", 3); 
    Value* val5 = new Value("v5", 1);

    //add pairings to cache
    c->put(key1, val1);
    c->put(key2, val2);
    c->put(key3, val3);

    assert(c->keyOrder_->size() == 3);

    assert(c->containsKey(key1));
    assert(c->containsKey(key2));
    assert(c->containsKey(key3));
    assert(c->keyOrder_->size() == 3);
    assert(c->data_->size() == 3);
    assert(!c->containsKey(key4));

    c->put(key4, val4);
    assert(!c->containsKey(key1));
    assert(c->containsKey(key4));
    puts("second put");
    c->put(key5, val5);
    assert(!c->containsKey(key1));
    assert(c->containsKey(key4));
    assert(c->containsKey(key5));

    
    //delete allocated memory
    delete c;
    delete key1, key2, key3, key4, key5;
    delete val1, val2, val3, val4, val5;
}