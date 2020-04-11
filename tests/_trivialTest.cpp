//lang:Cpp

#include "../src/application/trivial.h"
#include "../src/application/nodethread.h"
#include "../src/network/inetwork.h"
#include "../src/network/network.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include "../src/network/pseudo/messagequeuearray.h"
#include "../src/store/getImpls.h"

/** Create an ip address for an application node, using the home node */
char *generateIp_(size_t idx_)
{
  char *ip = new char[15]; //should change this to String
  sprintf(ip, "127.0.0.%zu", idx_ + 1);
  return ip;
}

//The purpose of this file is to test the Trivial case given to us in M2
int main()
{
  size_t numNodes = 1;
  NodeThread **nodes = new NodeThread *[numNodes];
  MsgQueueArr *sharedMQA = new MsgQueueArr(numNodes);

  //Create list of demos so we can delete them once they're done
  Trivial** demos = new Trivial*[numNodes];
  for (size_t i = 0; i < numNodes; i++)
  {
    //here, you decide whether you want your nodes to have a real or fake network
    demos[i] = new Trivial(i, new Network(generateIp_(i), i));
    //demos[i] = new Trivial(i, new PseudoNetwork(sharedMQA, i));
    nodes[i] = new NodeThread(demos[i]);
    nodes[i]->start();
  }

  for (size_t j = 0; j < numNodes; j++)
  {
    nodes[j]->join();
    printf("Thread %zu ended\n", j);
    delete nodes[j];
    delete demos[j];
  }

  delete[] nodes;
  delete[] demos;
  delete sharedMQA;
  return 0;
}