#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/store/getimpls.h"
#include "../src/dataframe/dataframe.h"
#include "../src/dataframe/datatodf.h"
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
	DataFrame* df = DataToDf::fromArray(&k1, kv, dataSize, data);
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

int main(int argc, char** argv)
{
	args.parse(argc, argv);
	
    printf("KVStore integration tests starting...\n");
	checkGetSingle();
    printf("KVStore integration tests passed!\n");
}