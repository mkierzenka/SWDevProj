#pragma once

#include "../utils/object.h"
#include "../network/client.h"
#include "../serial/serial.h"
#include "../serial/message.h"
#include "../dataframe/dataframe.h"

#include "map.h"
#include "key.h"
#include "value.h"

/** This class represents our key-value store. It will utilize a Map object to hold the
 * key-value pairings. It will also have networking capabilities to interact with other 
 * stores. */
class KVStore : public Object
{
public:
    MapStrObj *kvMap; //holds all key-value pairings
    //Client *client;     //networking class used to talk to other stores
    size_t storeId; //node id that this store belongs to

    KVStore(size_t id)
    {
        kvMap = new MapStrObj();
        storeId = id;
    }

    ~KVStore()
    {
        delete kvMap;
    }

    /** Puts key-value pair into map. Since it is stored locally, don't need to include store id */
    void put(Key* k, String* data)
    {
        //make sure adding key to right node
        if (k->getNode() != storeId)
        {
            fprintf(stderr, "Attempting to put Key with node %zu into store for node %zu", k->getNode(), storeId);
        }

        kvMap->put(k->getKeyStr(), data);
    }

    /** Get data from the local store. Returns nullptr if data doesn't exist in map */
    DataFrame* get(Key* k)
    {
        Object* o = kvMap->get(k->getKeyStr());
        Value* val = dynamic_cast<Value*>(o);
        //set up new serializer to deserialize returned data into a dataframe
        Serializer* s = new Serializer();

        //need to be able to deserialize
        ///TODO: figure out how to use serializer here
        //s->deserialize(val);

        //delete val?
        delete s;
    }

    /** Send request to specified store to get data. Return nullptr if cannot find */
    DataFrame* waitAndGet(Key *k)
    {
        //will need to create a Message 
    }
};