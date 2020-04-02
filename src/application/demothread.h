//lang:Cpp

#include "../utils/thread.h"
#include "../network/pseudo/pseudonetwork.h"
//#include "demo_simple.h"
#include "demo.h"

 /** An inner class of DataFrame representing a Thread for doing work on a contiguous
	 *    subset of rows of a DataFrame.
	 *  
	 *  author: kierzenka.m@husky.neu.edu */
class DemoThread : public Thread
  {
  public:
    size_t nodeNum_;
    PseudoNetwork* network_;

    DemoThread(size_t node, PseudoNetwork* net) : Thread() {
        nodeNum_ = node;
        network_ = net;
    }

    ~DemoThread() {}

    void run()
    {
        Demo* dem_ = new Demo(nodeNum_, network_);
        dem_->run_();
        delete dem_;
    }
  };