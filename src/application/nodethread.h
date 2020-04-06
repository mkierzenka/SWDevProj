//lang:Cpp


#include "../utils/thread.h"
#include "../network/network.h"
#include "application.h"
#include "demo.h"
#include "receiverthread.h"


/**
 * This class represents a node thread. It represents a node on our distributed system.
 * The "parent" thread (where this node is running) will run the actual application code.
 * This class starts/manages a new thread that will handle receiving/reacting to messages
 * from other nodes.
 */
class NodeThread : public Thread
  {
  public:
    Application* app_;
    Network* net_;

    NodeThread(Application* a) : Thread() {
        app_ = a;
        net_ = app_->getNetwork();
    }

    ~NodeThread() {}

    void run()
    {
        printf("Starting NodeThread %zu\n", app_->this_node());

        if (app_->this_node() == 0) {
          net_->server_init();
        } else {
          net_->client_init();
        }

        // Create receiver thread, to handle communication with other nodes
        ReceiverThread* rt = new ReceiverThread(app_->this_node(), net_, app_->getStore());
        rt->start();
        app_->run_();
    }
  };
