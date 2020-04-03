//lang::CwC

#pragma once

#include "../network/message.h"

DataFrame *KVStore ::waitAndGet(Key *k)
{
    // if k has our node num, just get it
    if (k->getNode() == storeId)
    {
        return get(k);
    }
    WaitAndGetMsg *dm = new WaitAndGetMsg(k, storeId, k->getNode());
    client_->sendMsg(dm);
	ReplyDataMsg *dataMsg = dynamic_cast<ReplyDataMsg *>(receivedMsgs_->pop());
	assert(dataMsg);
    Value *val = dataMsg->getValue();
	assert(val);
    Serializer *s = new Serializer(val->getSize(), val->getData());
    DataFrame *df = new DataFrame(k, this);
    df->deserialize(s);
    delete val;
    delete dataMsg;
    delete s;
    return df;
}

DataFrame *KVStore::get(Key *k)
{
    Value *val = getValue(k);
    Serializer *s = new Serializer(val->getSize(), val->getData());
    DataFrame *d = new DataFrame(k, this);
    d->deserialize(s);

    //delete val?
    delete s;

    return d;
}
