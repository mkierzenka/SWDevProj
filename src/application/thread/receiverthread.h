//lang:Cpp

#include "../../utils/thread.h"
#include "../../network/inetwork.h"
#include "../../network/message.h"
#include <assert.h>

/**
 * This class represents the receiver thread, which manages network communications.
 * An assistant to a NodeThread, all fields are external and pointers to what's kept
 * in the parent NodeThread. ReceiverThread will listen for and handle messages
 * continuously until a Teardown Message is received.
 * 
 * @authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
 */
class ReceiverThread : public Thread
{
public:
    size_t nodeNum_;
    INetwork *network_;   //external, used for communication with other nodes
    KVStore *kv_;         //external, a pointer to this node's store

    /** Constructs a new ReceiverThread from external arguments */
    ReceiverThread(size_t node, INetwork *net, KVStore *kv) : Thread()
    {
        nodeNum_ = node;
        network_ = net;
        kv_ = kv;
    }

    ~ReceiverThread() {}

    void run()
    {
        bool isDone = false; //when to stop responding to messages
        while (!isDone)
        {
			Message *m = network_->receiveMsg(); // blocks until new message arrives
            MsgKind kind = m->getKind();
            switch (kind)
            {
            case MsgKind::GetData:
            {
                GetDataMsg *gdMsg = dynamic_cast<GetDataMsg *>(m);
                size_t sender = gdMsg->getSender();
				// Respond with data, nullptr if we don't have it right now
				Value* val = kv_->getValue(gdMsg->getKey(), false);
				ReplyDataMsg *reply = new ReplyDataMsg(gdMsg->getKey(), val, nodeNum_, sender);
				network_->sendMsg(reply);
				delete gdMsg;
                break;
            }
			case MsgKind::WaitAndGet:
            {
                WaitAndGetMsg *wagMsg = dynamic_cast<WaitAndGetMsg *>(m);
                size_t sender = wagMsg->getSender();
                // Respond with data, add it to the queue if we don't have it yet
                Value* val = kv_->getValue(wagMsg->getKey(), true);
                if (val) {
                    ReplyDataMsg *reply = new ReplyDataMsg(wagMsg->getKey(), val, nodeNum_, sender);
                    network_->sendMsg(reply);
                    delete wagMsg;
                } else {
                    kv_->addMsgWaitingOn(wagMsg);
                }
                break;
            }
            case (MsgKind::ReplyData):
            {
				ReplyDataMsg* rdMsg = dynamic_cast<ReplyDataMsg*>(m);
				kv_->addReply(rdMsg);
                break;
            }
			case (MsgKind::Put):
			{
				PutMsg *msg = dynamic_cast<PutMsg *>(m);
				Key* k = msg->getKey();
				assert(k->getNode() == nodeNum_);
				kv_->put(k, msg->getValue());
				delete msg;
                break;
			}
            case (MsgKind::Register):
            {
                RegisterMsg* rMsg = dynamic_cast<RegisterMsg*>(m);
                assert(rMsg);
                network_->handleRegisterMsg(rMsg);
                break;
            }
            case (MsgKind::Dir):
            {
                DirectoryMsg* dirMsg = dynamic_cast<DirectoryMsg*>(m);
                network_->handleDirectoryMsg(dirMsg);
                break;
            }
            case (MsgKind::Done):
            {
                DoneMsg* doneMsg = dynamic_cast<DoneMsg*>(m);
                network_->handleDoneMsg(doneMsg);
                break;
            }
            case (MsgKind::Teardown):
            {
                delete m;
                isDone = true;
                break;
            }
            default:
                p("Node ").p(nodeNum_).pln(" ReceiverThread found unexpected msg type...");
            }
        }
        printf("End of receive thread run\n");
    }
};