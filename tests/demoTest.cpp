//lang:Cpp
#include "../src/application/nodethread.h"
#include "../src/application/demo.h"
#include "../src/network/pseudo/pseudonetwork.h"
#include "../src/network/message.h"
#include "../src/store/getImpls.h"

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