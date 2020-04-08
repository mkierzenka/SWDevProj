//lang:CPP
#pragma once 

#include "messagequeuearray.h"
#include "../message.h"
#include "../inetwork.h"

/**
 * This class represents a fake network. It will be used to send messages between
 * "nodes" and will have a message queue for each node. The MsgQueueArr object
 * field should be the same for all threads on the same "pseudo-network".
 * 
 * authors: kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */

class PseudoNetwork : public INetwork
{
public:
    MsgQueueArr *mqa_;   //Not Owned! Holds message arrays for each node
    size_t nodeId_;

   /**
   * Pass in number of nodes and a shared Message Queue Array.
   * mqa is external, the caller is responsible for deleting it.
   */
    PseudoNetwork(MsgQueueArr* mqa, size_t ourNode) : INetwork()
    {
        mqa_ = mqa;
        nodeId_ = ourNode;
    }

    ~PseudoNetwork() {}

    void sendMsg(Message *m)
    {
        size_t target = m->getTarget();
        mqa_->get(target)->push(m);
    }

    Message *receiveMsg()
    {
        return mqa_->get(nodeId_)->pop();
    }
};
