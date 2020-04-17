//lang:Cpp

#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/map.h"
#include "../serial/serial.h"
#include "sizetwrapper.h"
#include "nodeinfo.h"

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
     * Add new node information to this Directory
     */
    void addInfo(size_t nodeId, String *addr, size_t port)
    {
        SizeTWrapper *id = new SizeTWrapper(nodeId);
        NodeInfo* newEntry = new NodeInfo(addr, port);
        ips_->put(id->clone(), newEntry);
        delete id;
    }

    /**
     * Add new node information to this Directory
     */
    void addInfo(size_t nodeId, const char *addr, size_t port)
    {
        String strAddr(addr);
        addInfo(nodeId, &strAddr, port);
    }

    /**
     * Add new node information to this Directory
     */
    void addInfo(size_t nodeId, NodeInfo* newInfo)
    {
        SizeTWrapper *id = new SizeTWrapper(nodeId);
        ips_->put(id, newInfo->clone());
        delete id;
    }

    /** Get ip with the given node id */
    String* getAddress(size_t nodeId)
    {
        SizeTWrapper *node = new SizeTWrapper(nodeId);
        NodeInfo *addr = dynamic_cast<NodeInfo *>(ips_->get(node));
        assert(addr);
        String* out = new String(addr->getIPAddr());
        delete node;
        return out;
    }

    /**
     * Return NodeInfo for the specified node, else nullptr.
     * Does not clone! Do not delete.
     */
    NodeInfo* getInfo(size_t nodeId) {
        SizeTWrapper node(nodeId);
        NodeInfo *info = dynamic_cast<NodeInfo *>(ips_->get(&node));
        assert(info);
        return info;
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
        return nodes;
    }

    void mergeIn(Directory* other) {
        size_t otherSize = other->size();
        if (otherSize == 0) {
            return;
        }
        SizeTWrapper** otherNodes = other->getNodes();
        assert(otherNodes);
        for (int i = 0; i < other->size(); i++) {
            size_t nodeIdx = otherNodes[i]->asSizeT();
            ips_->put(otherNodes[i]->clone(), other->getInfo(nodeIdx)->clone());
        }
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
            NodeInfo* ni = dynamic_cast<NodeInfo *>(ips_->get(node));
            node->serialize(s);
            ni->serialize(s);
        }
    }

    /** Deserialize this directory */
    void deserialize(Serializer *s)
    {
        //read size and capacity
        size_t size = s->readSizeT();
        size_t capacity = s->readSizeT();

        //initialize map with capacity
        delete ips_;
        ips_ = new Map(capacity);

        SizeTWrapper *node = new SizeTWrapper();
        NodeInfo* ni = new NodeInfo();
        //deserialize all items and put them in the map
        for (size_t i = 0; i < size; i++)
        {
            node->deserialize(s);
            ni->deserialize(s);
            ips_->put(node->clone(), ni->clone());
        }
        delete node;
        delete ni;
    }
};