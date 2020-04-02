//lang:Cpp
#include "../src/application/nodethread.h"
//#include "../src/application/demo_simple.h"
#include "../src/application/demo.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include "../src/network/message.h"

DataFrame *KVStore ::waitAndGet(Key *k)
{
    // if k has our node num, just get it
    if (k->getNode() == storeId)
    {
        return get(k);
    }
    WaitAndGetMsg *dm = new WaitAndGetMsg(k, storeId, k->getNode());
    client_->sendMsg(dm);
	ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(receivedMsgs_->pop());
	assert(dataMsg);
    Value *val = dataMsg->getValue();
	assert(val);
    Serializer *s = new Serializer(val->getSize(), val->getData());
    DataFrame *df = new DataFrame(k, this);
    df->deserialize(s);
    delete val;
    delete dataMsg;
    delete s;
    return df;
}

DataFrame *KVStore::get(Key *k)
{
    Value *val = getValue(k);
    Serializer *s = new Serializer(val->getSize(), val->getData());
    DataFrame *d = new DataFrame(k, this);
    d->deserialize(s);

    //delete val?
    delete s;

    return d;
}

int main()
{
    //Testing on this many nodes
    size_t nodeNum = 3;
    NodeThread** nodes = new NodeThread*[nodeNum];
    PseudoNetwork *client = new PseudoNetwork(nodeNum);
    for (size_t i = 0; i < nodeNum; i++)
    {
        Demo* d = new Demo(i, client);
        nodes[i] = new NodeThread(d);
        nodes[i]->start();
    }

     for (size_t j = 0; j < nodeNum; j++)
     {
         nodes[j]->join();
         printf("Thread %zu ended\n", j);
         delete nodes[j];
     }

    delete[] nodes;
    exit(0);
}