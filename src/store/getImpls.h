//lang::CwC

#pragma once

#include "../dataframe/dataframe.h"
#include "../serial/serial.h"
#include "key.h"
#include "value.h"


DataFrame* KVStore::waitAndGet(Key *k)
{
    Value* val = getValue(k, true);
    assert(val);
    Serializer* s = new Serializer(val->getSize(), val->getData());
    DataFrame* df = new DataFrame(k->clone(), this);
    df->deserialize(s);
    delete s;
    return df;
}

DataFrame* KVStore::get(Key *k)
{
    Value* val = getValue(k, false);
	if (!val) return nullptr;
    Serializer* s = new Serializer(val->getSize(), val->getData());
    DataFrame* df = new DataFrame(k->clone(), this);
    df->deserialize(s);
    delete s;
    return df;
}
