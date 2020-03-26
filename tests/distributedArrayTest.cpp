//lang:cpp

#include "../src/dataframe/distributedarray.h"
#include "../src/store/kvstore.h"

/**
 * Test distributed array class
 */

int main()
{
    KVStore* kv = new KVStore(0);
    DistributedArray* da = new DistributedArray(kv);

    delete kv;
    delete da;
}