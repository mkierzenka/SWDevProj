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
 * This class represents a node's directory, maintaining a list of known nodes.
 * A user can add and lookup network information by a node's id number.
 * 
 * @author broder.c@husky.neu.edu
 */
class Directory : public Object
{
public:
    Map *ips_; //owned, map of node id to nodeinfo

    Directory()
    {
        ips_ = new Map();
    }

    ~Directory()
    {
        delete ips_;
    }

    /**
     * Add new node information to this Directory. Overwrites old value.
     */
    void addInfo(size_t nodeId, const char *addr, size_t port)
    {
        String strAddr(addr);
        NodeInfo newEntry(&strAddr, port);
        addInfo(nodeId, &newEntry);
    }

    /**
     * Add new node information to this Directory. Overwrites old value.
     * Clones - Caller responsible for deleting newInfo
     */
    void addInfo(size_t nodeId, NodeInfo* newInfo)
    {
        SizeTWrapper *id = new SizeTWrapper(nodeId);
        ips_->put(id, newInfo->clone());
    }

    /** Get ip with the given node id. Caller responsible for deleting the String */
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
     * Does not clone! Do not delete returned value.
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
            nodes[i] = dynamic_cast<SizeTWrapper*>(keys[i])->clone();
        }
        delete[] keys; //Delete the array but not the entries (they are the actual keys)
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
            Object* prevEntry = ips_->put(otherNodes[i], other->getInfo(nodeIdx)->clone());
            delete prevEntry;
        }
        delete[] otherNodes; //delete the arr, not each obj because they are in ips_ now
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
        delete[] keys; //delete the array, not each obj because they are in ips_ now
    }

    /** Deserialize into this directory (overwrites and mutates) */
    void deserialize(Serializer *s)
    {
        //read size and capacity
        size_t size = s->readSizeT();
        size_t capacity = s->readSizeT();

        //initialize map with capacity
        delete ips_;
        ips_ = new Map(capacity);

        SizeTWrapper tmpNodeId;
        NodeInfo tmpNodeInfo;
        //deserialize all items and put them into the map
        for (size_t i = 0; i < size; i++)
        {
            tmpNodeId.deserialize(s);
            tmpNodeInfo.deserialize(s);
            ips_->put(tmpNodeId.clone(), tmpNodeInfo.clone());
        }
    }
};