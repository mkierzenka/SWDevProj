
#include "src/utils/args.h"
#include "src/application/application.h"
#include "src/application/demo.h"
#include "src/application/trivial.h"
#include "src/application/wordcount.h"
#include "src/application/thread/nodethread.h"
#include "src/network/network.h"
#include "src/network/pseudo/pseudonetwork.h"
#include "src/network/pseudo/messagequeuearray.h"

#include <string.h>


Arguments args;

Application* initializeApplication(INetwork* net, size_t nodeIdx)
{
    Application* a;
    if (strcmp(args.appName, "trivial") == 0) {
        a = new Trivial(nodeIdx, net);
    } else if (strcmp(args.appName, "demo") == 0) {
        a = new Demo(nodeIdx, net);
    } else if(strcmp(args.appName, "wordcount") == 0) {
        a = new WordCount(nodeIdx, net);
    } else {
        fprintf(stderr, "Unknown application type: %s\n", args.appName);
        exit(0);
    }

    return a;
}

void runPseudo() {
    NodeThread** nodes = new NodeThread*[args.numNodes];
    MsgQueueArr* sharedMQA = new MsgQueueArr(args.numNodes);
    Application** apps = new Application*[args.numNodes];
    for (size_t i = 0; i < args.numNodes; i++)
    {
        //need to determine what kind of application you want
        INetwork* net = new PseudoNetwork(sharedMQA, i);
        apps[i] = initializeApplication(net, i);
        nodes[i] = new NodeThread(apps[i]);
        nodes[i]->start();
    }

    for (size_t j = 0; j < args.numNodes; j++)
    {
        nodes[j]->join();
        printf("Thread %zu ended\n", j);
        delete nodes[j];
        delete apps[j];
    }

    delete[] nodes;
    delete[] apps;
    delete sharedMQA;

    return;
}

/* Represents the entry point of our program. Takes in the arguments and handles them to
 * set up the program */
int main(int argc, char** argv) {
    args.parse(argc, argv);

    if (args.usePseudoNet) {
        runPseudo();
    } else {
        INetwork* net = new Network();
        Application* a = initializeApplication(net, args.index);
        NodeThread* node = new NodeThread(a);
        node->start();
		// Node runs the application. Waits for server to send Teardown msg
        node->join();
        printf("Thread %zu ended\n", args.index);
        delete node;
        delete a;
    }

    return 0;
}

