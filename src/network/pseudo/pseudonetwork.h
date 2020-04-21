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
    MsgQueueArr *mqa_;   //external, Holds message queues for each node
    size_t nodeId_;
    bool* isNodeDone_;   //owned, arr to keep of track of all node statuses

    /**
     * Pass in number of nodes and a shared Message Queue Array.
     * mqa is external, the caller is responsible for deleting it.
     */
    PseudoNetwork(MsgQueueArr* mqa, size_t ourNode) : INetwork()
    {
        mqa_ = mqa;
        nodeId_ = ourNode;
        isNodeDone_ = new bool[mqa->size()];
        for(size_t i = 0; i < mqa->size(); i++) {
            isNodeDone_[i] = false;
        }
    }

    ~PseudoNetwork() {
        delete[] isNodeDone_;
    }

    //The following few methods are intentionally blank, as a PseudoNetwork does
    //not need to do anything for those events.
    //--------------------------------------------------------------------------
    void server_init() override {}

    void client_init() override {}

    void handleRegisterMsg(RegisterMsg* m) override { delete m; }

    void handleDirectoryMsg(DirectoryMsg* m) override { delete m; }
    //--------------------------------------------------------------------------

    void sendMsg(Message *m) override
    {
        size_t target = m->getTarget();
        mqa_->get(target)->push(m);
    }

    Message *receiveMsg() override
    {
        return mqa_->get(nodeId_)->pop();
    }

    /** Handle done message. Only for the server */
    void handleDoneMsg(DoneMsg* m) override
    {
        assert(m && nodeId_ == 0);
        isNodeDone_[m->getSender()] = true;
        delete m;
        for (size_t i = 0; i < mqa_->size(); i++) {
            if (!isNodeDone_[i]) {
                return; // Not all the nodes are done yet
            }
        }
        
        sendTeardownMsgs_();
    }

    /**
     * A helper to send teardown messages to all the nodes (including its own ReceiverThread)
     * Only the Server should call this method
     */
    void sendTeardownMsgs_() {
        for (size_t i = 0; i < mqa_->size(); i++) {
            sendMsg(new TeardownMsg(nodeId_, i));
        }
    }
};
