//lang::CwC

#pragma once

#include "../network/message.h"

DataFrame* KVStore::waitAndGet(Key *k)
{
    Value* val = getValue(k);
    assert(val);
    Serializer* s = new Serializer(val->getSize(), val->getData());
    DataFrame* df = new DataFrame(k, this);
    df->deserialize(s);
    delete s;
    return df;
}

DataFrame* KVStore::get(Key *k)
{
    Value* val = nullptr;
    if (k->getNode() == storeId) {
        val = dynamic_cast<Value *>(kvMap->get(k->getKeyStr()));
    } else {
        GetDataMsg* dm = new GetDataMsg(k, storeId, k->getNode());
        node_->sendMsg(dm);
		if (msgCache_->contains_key(k)) {
			ReplyDataMsg* dataMsg = dynamic_cast<ReplyDataMsg*>(msgCache_->remove(k));
            val = dataMsg->getValue();
		}
	}
	if (!val) {
		return nullptr;
	}
    Serializer* s = new Serializer(val->getSize(), val->getData());
    DataFrame* d = new DataFrame(k, this);
    d->deserialize(s);
    delete s;
    return d;	
}
