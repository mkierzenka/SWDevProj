//lang:Cpp
#include "../src/application/demothread.h"
#include "../src/application/nodethread.h"
//#include "../src/application/demo_simple.h"
#include "../src/application/demo.h"
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
    while (receivedMsgs_->size() == 0) {
		//do nothing, waiting until there is a message received
	}
    //ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(client_->receiveMsg(storeId)); //blocks until received
	ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(receivedMsgs_->pop());
	assert(dataMsg != nullptr);
    Value *val = dataMsg->getValue();
	assert(val != nullptr);
    Serializer *s = new Serializer(val->getSize(), val->getData());
    //printf("Serializer created\n");
    DataFrame *df = new DataFrame(k, this);
    //printf("Dataframe created\n");
    df->deserialize(s);
    //printf("Dataframe deserialized\n");
    //delete val;
    //delete dataMsg;
    //delete s;
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
     //for (size_t j = nodeNum - 1; j >= 0; j--)
     {
         nodes[j]->join();
         printf("Thread %zu ended\n", j);
         delete nodes[j];
     }

    //delete[] nodes;
    exit(0);
}