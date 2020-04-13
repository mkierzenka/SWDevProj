#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/getimpls.h"
#include "../src/dataframe/dataframe.h"
#include "../src/network/pseudo/messagequeuearray.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include "../src/utils/args.h"
#include <assert.h>

Arguments args;

/**
 * Integration Tests for KVStore, DataFrame, Network.
 */
void checkGetSingle() {
    printf("KVStore.get single test starting...\n");
	size_t num_nodes = 1;
    MsgQueueArr* mqa = new MsgQueueArr(num_nodes);
	PseudoNetwork* pNet = new PseudoNetwork(mqa, 0);
	KVStore* kv = new KVStore(0, pNet);
	Key k1("key1", 0);
	Key k2("key2-0", 0);

	size_t dataSize = 2000;
	int* data = new int[dataSize];
	for (int i = 0; i < dataSize; i++) {
		data[i] = 211 * i;
	}
	DataFrame* df = DataFrame::fromArray(&k1, kv, dataSize, data);
	delete[] data;
	assert(df->ncols() == 1);
	assert(df->nrows() == dataSize);
	assert(df->get_int(0, 0) == 0);
	assert(df->get_int(0, 100) == 21100);
	assert(df->get_int(0, dataSize - 1) == 211 * (dataSize - 1));
	delete df;
	DataFrame* df2 = kv->get(&k1);
	assert(df2);
	assert(df2->ncols() == 1);
	assert(df2->nrows() == dataSize);
	assert(df2->get_int(0, 0) == 0);
	assert(df2->get_int(0, 100) == 21100);
	assert(df2->get_int(0, dataSize - 1) == 211 * (dataSize - 1));
	delete df2;
	delete kv;
	delete pNet;
	delete mqa;
    printf("KVStore.get single test passed!\n");
}

/* Half working, but really just tests like Trivial Test
// Generates and serializes DF [startI, endI)
DataFrame* genSerialDF_(KVStore* kv, Key* k, int startI, int endI) {
	assert(startI >= 0 && endI > startI);
	size_t len = endI - startI;
	int* items = new int[len];
	for(size_t i = 0; i < len; i++) {
		items[i] = startI + i;
	}
	DataFrame* out = new DataFrame(k, kv);
	out->add_array(len, items);
	return out;
}

Value* df2Val_(DataFrame* df) {
	Serializer* s = new Serializer();
	df->serialize(s);
	Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
	delete s;
	return val;
}

// Simple example of 2 KVStores working with eachother
void doubleTest() {
    printf("KVStore double test starting...\n");
    MsgQueueArr* mqa = new MsgQueueArr(2);
	PseudoNetwork* pNet0 = new PseudoNetwork(mqa, 0);
	PseudoNetwork* pNet1 = new PseudoNetwork(mqa, 1);
	KVStore* kv0 = new KVStore(0, pNet0);
	KVStore* kv1 = new KVStore(1, pNet1);

	Key* k1 = new Key("key1", 0);
	DataFrame* df1 = genSerialDF_(kv0, k1, 0, 127);
	Value* v1 = df2Val_(df1);
	Key* k2 = new Key("key2-0", 1);
	DataFrame* df2 = genSerialDF_(kv1, k2, 10, 2000);
	Value* v2 = df2Val_(df2);
	assert(df1->get_int(0, 100) == 100);
	kv0->put(k1, v1);
	assert(kv0->get(k1)->get_int(0, 100) == 100);
	//   check message was correct
	PutMsg* actualPut = dynamic_cast<PutMsg*>(mqa->get(1)->pop());
	assert(actualPut);
	assert(actualPut->getKind() == MsgKind::Put);
	assert(actualPut->getSender() == 0);
	assert(actualPut->getTarget() == 1);
	assert(actualPut->getKey()->equals(k2));
	kv1->put(k2, v2); //node1 adds the data node0 wants. responds with message
	assert(mqa->get(1)->size() == 0);
	assert(mqa->get(0)->size() == 1);
	// Check reply message has the data we want
	ReplyDataMsg* actualReply = dynamic_cast<ReplyDataMsg*>(mqa->get(1)->pop());
	assert(actualReply);
	assert(actualReply->getKind() == MsgKind::ReplyData);
	assert(actualReply->getSender() == 1);
	assert(actualReply->getTarget() == 0);
	assert(actualReply->getKey()->equals(k2));
	assert(actualReply->getValue()->equals(v2));
	delete actualReply;
	delete kv0; delete kv1;
	delete pNet0; delete pNet1;
	delete mqa;	
    printf("KVStore double test passed!\n");
}*/

int main(int argc, char** argv)
{
	args.parse(argc, argv);
	
    printf("KVStore integration tests starting...\n");
	checkGetSingle();
    printf("KVStore integration tests passed!\n");
}