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
    return getHelper_(k, val);
}

DataFrame* KVStore::get(Key *k)
{
    Value* val = getValue(k, false);
	if (!val) return nullptr;
    return getHelper_(k, val);
}

DataFrame* KVStore::getHelper_(Key* k, Value* val)
{
    Serializer* s = new Serializer(val->getSize(), val->getData());
    DataFrame* df = new DataFrame(k->clone(), this);
    df->deserialize(s);
    delete s;
    delete val;
    return df;
}
