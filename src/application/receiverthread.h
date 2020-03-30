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

    ReceiverThread(size_t node, PseudoNetwork *net, KVStore *kv) : Thread()
    {
        nodeNum_ = node;
        network_ = net;
        kv_ = kv;
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
                printf("Node %zu received getdata\n", nodeNum_);
                // respond with data
                GetDataMsg *gdMsg = dynamic_cast<GetDataMsg *>(m);
                size_t sender = gdMsg->getSender();  // who sent me the msg
                printf("Sending from %zu to %zu\n", nodeNum_, sender);
                Value* val = kv_->getValue(gdMsg->getKey());
                //assert(val != nullptr);
                while (val == nullptr)
                {
                    val = kv_->getValue(gdMsg->getKey());
                }

                ReplyDataMsg *reply = new ReplyDataMsg(val, nodeNum_, sender);
                printf("ABOUT TO SEND REPLY WITH TARGET %zu\n", reply->getTarget());
                network_->sendMsg(reply);
                break;
            }
            case (MsgKind::ReplyData):
            {
                printf("Node %zu received replydata\n", nodeNum_);
                kv_->receivedMsgs_->push(m);
                break;
            }
            default:
                puts("Weird msg type...");
            }
        }
        
        printf("End of receive thread run\n");
    }
};