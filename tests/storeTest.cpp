#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/getimpls.h"
#include "../src/network/pseudo/messagequeuearray.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include <assert.h>

void basicSingle() {
    printf("KVStore basic-single test starting...\n");
	size_t num_nodes = 1;
    MsgQueueArr* mqa = new MsgQueueArr(num_nodes);
	PseudoNetwork* pNet = new PseudoNetwork(mqa, 0);
	KVStore* kv = new KVStore(0, pNet);
	Key k1("key1", 0);
	Key k2("key2-0", 0);
	Value v1("My First Value!", 15); //should work with/without null terminator
	Value v2("Val 2", 5);
	kv->put(&k1, &v1);
	kv->put(&k2, &v2);

	Value* actual = kv->getValue(&k1, true);
	assert(actual->equals(&v1));
	delete actual;

	actual = kv->getValue(&k1, false);
	assert(actual->equals(&v1));
	delete actual;

	actual = kv->getValue(&k1, true);
	assert(!actual->equals(&v2));
	delete actual;

	actual = kv->getValue(&k2, true);
	assert(actual->equals(&v2));
	delete actual;
	delete kv;
	delete pNet;
	delete mqa;
    printf("KVStore basic-single test passed!\n");
}

// Simple example of 2 KVStores working with eachother - WaitAndGet
void doubleTestWag() {
    printf("KVStore WaitAndGet double test starting...\n");
    MsgQueueArr* mqa = new MsgQueueArr(2);
	PseudoNetwork* pNet0 = new PseudoNetwork(mqa, 0);
	PseudoNetwork* pNet1 = new PseudoNetwork(mqa, 1);
	KVStore* kv0 = new KVStore(0, pNet0);
	KVStore* kv1 = new KVStore(1, pNet1);

	Key k1("key1", 0);
	Value v1("val1", 4);
	Key k2("key2-0", 1);
	Value v2("another value", 13);
	
	WaitAndGetMsg* wag = new WaitAndGetMsg(&k1, 1, 0);
	kv0->addMsgWaitingOn(wag); //Node1 wants k1 from Node0
	assert(mqa->get(1)->size() == 0);
	kv0->put(k1.clone(), &v1); //Node0 got the data
	assert(mqa->get(1)->size() == 1);
	// Check reply message has the data we want
	ReplyDataMsg* actualReply = dynamic_cast<ReplyDataMsg*>(mqa->get(1)->pop());
	assert(actualReply);
	assert(actualReply->getKind() == MsgKind::ReplyData);
	assert(actualReply->getSender() == 0);
	assert(actualReply->getTarget() == 1);
	assert(actualReply->getKey()->equals(&k1));
	assert(actualReply->getValue()->equals(&v1));
	
	delete actualReply;
	delete kv0; delete kv1;
	delete pNet0; delete pNet1;
	delete mqa;	
    printf("KVStore WaitAndGet double test passed!\n");
}


// Simple example of 2 KVStores working with eachother - Put
void doubleTestPut() {
    printf("KVStore Put double test starting...\n");
    MsgQueueArr* mqa = new MsgQueueArr(2);
	PseudoNetwork* pNet0 = new PseudoNetwork(mqa, 0);
	PseudoNetwork* pNet1 = new PseudoNetwork(mqa, 1);
	KVStore* kv0 = new KVStore(0, pNet0);
	KVStore* kv1 = new KVStore(1, pNet1);

	Key k1("key1", 0);
	Value v1("val1", 4);
	Key k2("key2-0", 1);
	Value v2("another value", 13);

	kv0->put(&k1, &v1); // k1 belongs in kv0
	assert(mqa->get(0)->size() == 0);
	assert(mqa->get(1)->size() == 0);
	kv0->put(&k2, &v2); // k2 belongs in kv1
	assert(mqa->get(0)->size() == 0);
	assert(mqa->get(1)->size() == 1);
	PutMsg* actualPut = dynamic_cast<PutMsg*>(mqa->get(1)->pop());
	assert(actualPut);
	assert(actualPut->getKind() == MsgKind::Put);
	assert(actualPut->getSender() == 0);
	assert(actualPut->getTarget() == 1);
	assert(actualPut->getKey()->equals(&k2));

	delete actualPut;
	delete kv0; delete kv1;
	delete pNet0; delete pNet1;
	delete mqa;
    printf("KVStore Put double test passed!\n");
}

int main()
{
    printf("KVStore tests starting...\n");
	basicSingle();
	doubleTestWag();
	doubleTestPut();
    printf("KVStore tests passed!\n");
}