//lang:Cpp

#include "../utils/thread.h"
#include "../network/pseudo/pseudonetwork.h"
#include "../serial/message.h"
#include "../serial/replydatamsg.h"
#include "../serial/msgkind.h"

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
            printf("Node %zu Received message!\n", nodeNum_);
            MsgKind kind = m->getKind();
            switch (kind)
            {
            case (MsgKind::GetData):
            {
                // respond with data
                GetDataMsg *gdMsg = dynamic_cast<GetDataMsg *>(m);
                size_t target = gdMsg->getTarget();
                ReplyDataMsg *reply = new ReplyDataMsg(kv_->getValue(gdMsg->getKey()), nodeNum_, target);
                network_->sendMsg(reply);
                puts("Get data message type");
                break;
            }
            default:
                puts("Weird msg type...");
            }
        }
    }
};