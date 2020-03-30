//lang:Cpp
//#include "../src/application/demo.h"
#include "../src/application/demothread.h"
#include "../src/application/nodethread.h"
#include "../src/application/demo_simple.h"
#include "../src/network/pseudo/pseudonetwork.h"

DataFrame *KVStore ::waitAndGet(Key *k)
{
    // if k has our node num, just get it
    if (k->getNode() == storeId)
    {
        return get(k);
    }
    // else
    GetDataMsg *dm = new GetDataMsg(k, storeId, k->getNode());
    client_->sendMsg(dm);
    ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(client_->receiveMsg(storeId)); //blocks until received
    printf("Node %zu got reply data message: handle\n", storeId);
    Value *val = dataMsg->getValue();
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
    //set up new serializer to deserialize returned data into a dataframe
    Serializer *s = new Serializer(val->getSize(), val->getData());

    //create new dataframe and mutate by deserializing
    DataFrame *d = new DataFrame(k, this);
    d->deserialize(s);

    //delete val?
    delete s;

    return d;
}

int main()
{
    //Testing on this many nodes
    size_t nodeNum = 2;
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
        delete nodes[j];
    }

    delete[] nodes;
    exit(1);

    // DemoThread **demos = new DemoThread *[nodeNum];
    // PseudoNetwork *client = new PseudoNetwork(nodeNum);
    // for (size_t i = 0; i < nodeNum; i++)
    // {
    //     demos[i] = new DemoThread(i, client);
    //     demos[i]->start();
    // }

    // for (size_t j = 0; j < nodeNum; j++)
    // {
    //     demos[j]->join();
    //     delete demos[j];
    // }

    //delete[] demos;
}