#include <stdio.h>

#include "../src/network/message.h"
#include "../src/dataframe/dataframe.h"
#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/serial/serial.h"

/**
 * Check that reply message serialization and deserialization works
 */
void replyDataMsgTest()
{
    printf("Reply data message test started\n");

    //create key and kvstore for df
    Key* k = new Key("what's poppin", 0);
    KVStore* kv_ = new KVStore(0, nullptr);

    //create a dataframe, serialize, put it in KVStore
    size_t SZ = 1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
    DataFrame* dfa = DataFrame::fromArray(k, kv_, SZ, vals);

    //get value of key
    Value* v = kv_->getValue(k);

    //create reply data msg
    ReplyDataMsg* rdm = new ReplyDataMsg(k, v, 0, 1);

    //create serializer
    Serializer* s = new Serializer();

    //serialize message
    rdm->serialize(s);

    //create new message and deserialize
    ReplyDataMsg* m = new ReplyDataMsg();
    m->deserialize(s);

    //check for equality
    assert(m->getKey()->equals(rdm->getKey()));
    printf("Original message value: %s\n", rdm->getValue()->val_);
    printf("New message value: %s\n", m->getValue()->val_);
    printf("Original message capacity: %zu\n", rdm->getValue()->capacity_);
    printf("New message capacity: %zu\n", m->getValue()->capacity_);
    assert(memcpy(m->getValue()->val_, rdm->getValue()->val_, m->getValue()->capacity_) == 0);
    assert(m->getValue()->equals(rdm->getValue()));
    assert(m->equals(rdm));

    //delete memory allocated
    delete k;
    delete kv_;
    delete dfa;
    delete[] vals;
    delete rdm;
    delete s;
    delete m;

    printf("Reply data message test passed!\n");
}

/**
 * Will check that all messages behave as expected (particularly with serialization and deserialization)
 */
int main()
{
    replyDataMsgTest();


    printf("All message tests passed!\n");
    return 0;
}
 