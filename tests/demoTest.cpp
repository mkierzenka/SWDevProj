//lang:Cpp
//#include "../src/application/demo.h"
#include "../src/application/demo_simple.h"

int main()
{
    //Testing on this many nodes
    size_t nodeNum = 2;
    DemoThread** demos = new DemoThread*[nodeNum];
    for (size_t i = 0; i < nodeNum; i++)
    {
        demos[i] = new DemoThread(i);
        demos[i]->start();
    }

    for (size_t j = 0; j < nodeNum; j++)
    {
        demos[i]->join();
        delete demos[i];
    }

    delete[] demos;
}