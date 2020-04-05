#pragma once

#include "../utils/object.h"
#include "../utils/map.h"
#include "../serial/serial.h"
#include "Integer.h"

/**
 * This class represents a node's directory, maintaining a list of its 
 * client connections. It will wrap a map, allowing look up by node id
 * 
 * @author broder.c@husky.neu.edu
 */
class Directory : public Object
{
public:
    Map *ips_; //map of node id to ip

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
    void addIp(size_t nodeId, String *ipAddr)
    {
        Integer *id = new Integer((int)nodeId);
        ips_->put(id->clone(), ipAddr->clone());
        delete id;
    }

    /** Get ip with the given node id */
    String *getAddress(size_t nodeId)
    {
        Integer *node = new Integer(nodeId);
        String *addr = dynamic_cast<String *>(ips_->get(node));
        delete node;
        return addr;
    }

    /** Clone this directory */
    Directory* clone()
    {
        Directory* res = new Directory;
        Object** keys = ips_->get_keys();
        //add all values to new directory
        for (size_t i = 0; i < ips_->size(); i++)
        {
            Integer* node = dynamic_cast<Integer*>(keys[i]);
            String* ip = dynamic_cast<String*>(ips_->get(node));
            res->addIp(node->asUnsignedInt(), ip->clone());
        }

        //check for equality before returning?
        return res;
    }

    /** Serialize this directory */
    void serialize(Serializer* s)
    {
        ips_->serialize(s);
    }

    /** Deserialize this directory */
    void deserialize(Serializer* s)
    {
        ips_->deserialize(s);
    }
};