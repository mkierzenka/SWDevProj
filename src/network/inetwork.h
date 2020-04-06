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

    virtual void sendMsg(Message* m)
    {
    }

    virtual Message* receiveMsg()
    {
    }
};