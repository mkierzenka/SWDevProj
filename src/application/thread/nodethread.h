//lang:Cpp

#include "../../utils/thread.h"
#include "../../utils/args.h"
#include "../../network/inetwork.h"
#include "../application.h"
#include "receiverthread.h"


/**
 * This class represents a single node of our distributed system.
 * The "parent" thread (where this node is running) will run the actual application code.
 * This class starts/manages a new thread that will handle receiving/reacting to messages
 * from other nodes. Its fields are external, it is a vehicle for running a given
 * application.
 *
 * @authors Chase Broder and Marcin Kierzenka
 */
class NodeThread : public Thread
  {
  public:
    Application* app_;  //external
    INetwork* net_;     //external

    NodeThread(Application* a) : Thread() {
        app_ = a;
        net_ = app_->getNetwork();
    }

    ~NodeThread() {}

    void run()
    {
        printf("Starting Node %zu\n", app_->this_node());
        if (app_->this_node() == args.serverIndex) {
          net_->server_init();
        } else {
          net_->client_init();
        }

        // Create receiver thread, to handle communication with other nodes
        ReceiverThread* rt = new ReceiverThread(app_->this_node(), net_, app_->getStore());
        rt->start();
        app_->run_();

        // Let server know that we're done executing
        net_->sendMsg(new DoneMsg(app_->this_node(), args.serverIndex));
        rt->join();
        delete rt;
    }
  };
