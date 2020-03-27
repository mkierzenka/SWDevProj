//lang:Cpp

#include "../utils/thread.h"
#include "demo_simple.h"

 /** An inner class of DataFrame representing a Thread for doing work on a contiguous
	 *    subset of rows of a DataFrame.
	 *  
	 *  author: kierzenka.m@husky.neu.edu */
class DemoThread : public Thread
  {
  public:
    size_t nodeNum_;

    DemoThread(size_t node) {
        nodeNum_ = node;
    }

    void run()
    {
        Demo* dem = new Demo(nodeNum_);
        dem->run_();
        delete dem;
    }
  };