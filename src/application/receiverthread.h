//lang:Cpp

#include "../utils/thread.h"
#include "../network/pseudo/pseudonetwork.h"
#include "../serial/message.h"
#include "../serial/replydatamsg.h"
#include "../serial/msgkind.h"
#include "../serial/waitandgetmsg.h"
#include <assert.h>

/**
 * This class represents the receiver thread. It will listen for messages continuously
 * and handle them accordingly 
 * 
 * @authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
 */
class ReceiverThread : public Thread
{
public:
    size_t nodeNum_;
    PseudoNetwork *network_;
    KVStore *kv_;
	MessageQueue* msgsInProg_; //messages we should deal with (keys we don't have yet)

    ReceiverThread(size_t node, PseudoNetwork *net, KVStore *kv) : Thread()
    {
        nodeNum_ = node;
        network_ = net;
        kv_ = kv;
		msgsInProg_ = new MessageQueue();
    }

    ~ReceiverThread() {}

    void run()
    {
        while (true)
        {
			Message *m = network_->receiveMsg(nodeNum_); // blocks until new message arrives
            MsgKind kind = m->getKind();
            switch (kind)
            {
            case MsgKind::GetData:
            {
                GetDataMsg *gdMsg = dynamic_cast<GetDataMsg *>(m);
                size_t sender = gdMsg->getSender();
				// Respond with data, nullptr if we don't have it right now
				Value* val = kv_->getValue(gdMsg->getKey());
				ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
				network_->sendMsg(reply);
                break;
            }
			case MsgKind::WaitAndGet:
            {
                // respond with data
                WaitAndGetMsg *gdMsg = dynamic_cast<WaitAndGetMsg *>(m);
                size_t sender = gdMsg->getSender();
				// Respond with data, add it to the queue if we don't have it right now
				Value* val = kv_->getValue(gdMsg->getKey());
				if (val) {
					ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
					network_->sendMsg(reply);
				} else {
					msgsInProg_->push(gdMsg);
				}
                break;
            }
            case (MsgKind::ReplyData):
            {
                kv_->receivedMsgs_->push(m);
                break;
            }
			case (MsgKind::Put):
			{
				PutMsg *msg = dynamic_cast<PutMsg *>(m);
				Key* k = msg->getKey();
                if (k->getNode() == nodeNum_) {
					kv_->putLocal(k, msg->getValue());
				}
				
				// Handle messages in the Queue
				MessageQueue* tmp = new MessageQueue();
				while (msgsInProg_->size() > 0) {
					WaitAndGetMsg* m = dynamic_cast<WaitAndGetMsg *>(msgsInProg_->pop());
					size_t sender = m->getSender();
					Value* val = kv_->getValue(m->getKey());
					if(val != nullptr) {
						ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
						network_->sendMsg(reply);
					} else {
						tmp->push(m);
					}
				}
				while(tmp->size() > 0) {
					msgsInProg_->push(tmp->pop());
				}
				delete tmp;
				break;
			}
            default:
                pln("Weird msg type...");
            }
			
        }
        
        printf("End of receive thread run\n");
    }
};