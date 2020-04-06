//lang:Cpp

#include "../src/application/trivial.h"
#include "../src/application/nodethread.h"
#include "../src/network/inetwork.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include "../src/network/pseudo/messagequeuearray.h"
#include "../src/store/getImpls.h"

//The purpose of this file is to test the Trivial case given to us in M2
int main()
{
  size_t numNodes = 1;
  NodeThread** nodes = new NodeThread*[numNodes];
  //PseudoNetwork *client = new PseudoNetwork(numNodes);
  MsgQueueArr* sharedMQA = new MsgQueueArr(numNodes);
  for (size_t i = 0; i < numNodes; i++)
  {
      Trivial* t = new Trivial(i, new PseudoNetwork(sharedMQA, i));
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