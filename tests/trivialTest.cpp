//lang:Cpp

#include "../src/application/trivial.h"
#include "../src/application/nodethread.h"
#include "../src/network/pseudo/pseudonetwork.h"

///TODO: figure out where this method implementation should go
DataFrame* KVStore::get(Key *k)
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

//The purpose of this file is to test the Trivial case given to us in M2
int main()
{
  size_t numNodes = 1;
  NodeThread** nodes = new NodeThread*[numNodes];
  PseudoNetwork *client = new PseudoNetwork(numNodes);
  for (size_t i = 0; i < numNodes; i++)
  {
      Trivial* t = new Trivial(i, client);
      nodes[i] = new NodeThread(t);
      nodes[i]->start();
  }

  for (size_t j = 0; j < numNodes; j++)
   {
     nodes[j]->join();
     printf("Thread %zu ended\n", j);
     delete nodes[j];
   }

  delete[] nodes;
  exit(0);
}