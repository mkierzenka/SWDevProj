#pragma once

#include "../utils/object.h"
#include "message.h"

/** Network interface. Has methods for sending and receiving */
class INetwork : public Object
{
public:
    INetwork() : Object()
    {
    }

    virtual void sendMsg(Message* m) {}

    virtual Message* receiveMsg() {}

    //NOTE: pseudonetwork does not need these last 4 methods and won't implement them
    virtual void server_init() {}

    virtual void client_init() {}

    virtual void handleRegisterMsg(Message* m) {}

    virtual void handleDirectoryMsg(Message* m) {}

    virtual void handleDoneMsg(DoneMsg* m) {}
};