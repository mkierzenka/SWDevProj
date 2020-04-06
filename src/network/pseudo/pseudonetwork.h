//lang:CPP
#pragma once 

#include "messagequeuearray.h"
#include "../message.h"
#include "../inetwork.h"

/**
 * This class represents a fake network. It will be used to send messages between
 * "nodes" and will have a message queue for each node
 * 
 * authors: kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */

class PseudoNetwork : public INetwork
{
public:
    MsgQueueArr *mqa_; //holds message arrays for each node
    size_t nodeId_;

   /**
   * Pass in number of nodes
   */
    PseudoNetwork(MsgQueueArr* mqa, size_t ourNode) : INetwork()
    {
        mqa_ = mqa;
        nodeId_ = ourNode;
    }

    ~PseudoNetwork()
    {
        delete mqa_;
    }

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
