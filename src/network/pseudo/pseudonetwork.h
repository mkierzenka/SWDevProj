//lang:CPP
#pragma once 

#include "messagequeuearray.h"
#include "../../serial/message.h"

/**
 * This class represents a fake network. It will be used to send messages between
 * "nodes" and will have a message queue for each node
 * 
 * authors: kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */

class PseudoNetwork : public Object
{
public:
    MsgQueueArr *mqa_; //holds message arrays for each node

   /**
   * Pass in number of nodes
   */
    PseudoNetwork(size_t numNodes)
    {
        mqa_ = new MsgQueueArr();
        for (size_t i = 0; i < numNodes; i++)
        {
            mqa_->push_back(new MessageQueue());
        }
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

    Message *receiveMsg(size_t recNode)
    {
        return mqa_->get(recNode)->pop();
    }
};
