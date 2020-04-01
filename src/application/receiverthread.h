//lang:Cpp

#include "../utils/thread.h"
#include "../network/pseudo/pseudonetwork.h"
#include "../serial/message.h"
#include "../serial/replydatamsg.h"
#include "../serial/msgkind.h"

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
                // respond with data
                GetDataMsg *gdMsg = dynamic_cast<GetDataMsg *>(m);
                size_t sender = gdMsg->getSender();  // who sent me the msg
				printf("Node %zu received getdata[%s] from node %zu\n", nodeNum_, gdMsg->getKey()->getKeyStr()->cstr_, sender);
				
				Value* val = kv_->getValue(gdMsg->getKey());
				if (val != nullptr) {
					ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
					printf("Sending ReplyData[%s] from %zu to %zu\n", gdMsg->getKey()->getKeyStr()->cstr_, nodeNum_, sender);
					network_->sendMsg(reply);
				} else {
					//msgsInProg_->push(gdMsg);
					network_->sendMsg(gdMsg);
				}
                break;
            }
            case (MsgKind::ReplyData):
            {
				printf("Node %zu received replydata\n", nodeNum_);
                kv_->receivedMsgs_->push(m);
                break;
            }
			case (MsgKind::Put):
			{
				printf("Node %zu received put\n", nodeNum_);
				PutMsg *msg = dynamic_cast<PutMsg *>(m);
				Key* k = msg->getKey();
                if (k->getNode() == nodeNum_) {
					kv_->put(k, msg->getValue());
				}
				break;
			}
            default:
                puts("Weird msg type...");
            }

			// Handle messages in the Queue
			/*MessageQueue* tmp = new MessageQueue();
			while (msgsInProg_->size() > 0) {
				GetDataMsg* m = dynamic_cast<GetDataMsg *>(msgsInProg_->pop());
				size_t sender = m->getSender();  // who sent me the msg
				Value* val = kv_->getValue(m->getKey());
				if(val != nullptr) {
					ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
					printf("Sending ReplyData[%s] from %zu to %zu\n", m->getKey()->getKeyStr()->cstr_, nodeNum_, sender);
					network_->sendMsg(reply);
				} else {
					tmp->push(m);
				}
			}
			while(tmp->size() > 0) {
				msgsInProg_->push(tmp->pop());
			}
			delete tmp;*/
        }
        
        printf("End of receive thread run\n");
    }
};