//lang:Cpp

#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/map.h"
#include "../serial/serial.h"
#include "sizetwrapper.h"

#include <arpa/inet.h>

/**
 * This class represents a node's directory, maintaining a list of its 
 * client connections. It will wrap a map, allowing look up by node id
 * 
 * @author broder.c@husky.neu.edu
 */
class Directory : public Object
{
public:
    Map *ips_; //map of node id to ip address

    Directory()
    {
        ips_ = new Map();
    }

    ~Directory()
    {
        delete ips_;
    }

    /**
     * Add a new node id and ip address to this directory
     */
    void addIp(size_t nodeId, String *addr)
    {
        SizeTWrapper *id = new SizeTWrapper(nodeId);
        ips_->put(id->clone(), addr->clone());
        delete id;
    }

    /** Get ip with the given node id */
    String* getAddress(size_t nodeId)
    {
        SizeTWrapper *node = new SizeTWrapper(nodeId);
        String *addr = dynamic_cast<String *>(ips_->get(node));
        assert(addr);
        delete node;
        return addr;
    }

    /**
     * Return node ids. Caller will own resulting list of wrappers
     */
    SizeTWrapper** getNodes() {
        SizeTWrapper** nodes  = new SizeTWrapper*[ips_->size()];
        Object** keys = ips_->get_keys();
        for (int i = 0; i < ips_->size(); i++)
        {
            assert(keys[i]);
            nodes[i] = dynamic_cast<SizeTWrapper*>(keys[i]);
        }

        //delete keys;
        return nodes;
        //return dynamic_cast<SizeTWrapper**>(ips_->get_keys());
    }

    void mergeIn(Directory* other) {
        size_t otherSize = other->size();
        if (otherSize == 0) {
            return;
        }
        SizeTWrapper** otherNodes = other->getNodes();
        assert(otherNodes);
        for (int i = 0; i < other->size(); i++) {
            ips_->put(otherNodes[i]->clone(), other->getAddress(otherNodes[i]->asSizeT())->clone());
            //delete otherNodes[i];
        }
        //delete[] otherNodes;
    }

    /** Get number of entries in this directory */
    size_t size()
    {
        return ips_->size();   
    }

    /** Clone this directory */
    Directory* clone()
    {
        Directory* newDir = new Directory();
        newDir->mergeIn(this);
        return newDir;
    }

    /** Serialize this directory */
    void serialize(Serializer *s)
    {
        //write size and capacity
        s->write(ips_->size());
        s->write(ips_->capacity());

        Object **keys = ips_->get_keys();
        //serialize all key-value pairings
        for (size_t i = 0; i < ips_->size(); i++)
        {
            SizeTWrapper *node = dynamic_cast<SizeTWrapper *>(keys[i]);
            String *ip = dynamic_cast<String *>(ips_->get(node));

            node->serialize(s);
            ip->serialize(s);
        }
    }

    /** Deserialize this directory */
    void deserialize(Serializer *s)
    {
        //read size and capacity
        size_t size = s->readSizeT();
        size_t capacity = s->readSizeT();

        //initialize map with capacity
        ips_ = new Map(capacity);

        SizeTWrapper *node = new SizeTWrapper();
        String *addr = new String("");
        //deserialize all items and put them in the map
        for (size_t i = 0; i < size; i++)
        {
            node->deserialize(s);
            addr->deserialize(s);
            ips_->put(node->clone(), addr->clone());
        }

        delete node;
        delete addr;
    }
};