#pragma once
//lang:Cwc

#include "../utils/object.h"
#include "../utils/string.h"
#include "../serial/serial.h"
#include <arpa/inet.h>

/**
 * A class to track the information we need to know about a node
 * in order to have it work with our system.
 * 
 * @authors kierzenka.m@husky.neu.edu and broder.c@husky.neu.edu
 */
class NodeInfo : public Object
{
public:
    struct sockaddr_in addr_;

    NodeInfo()
    {
        addr_.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &addr_.sin_addr);
        addr_.sin_port = htons(8080);
    }

    /**
		 * Constructs a new NodeInfo from given params.
		 * Caller responsible for deleting ipAddr.
		 */
    NodeInfo(String *ipAddr, size_t port)
    {
        addr_.sin_family = AF_INET;
        inet_pton(AF_INET, ipAddr->c_str(), &addr_.sin_addr);
        addr_.sin_port = htons(port);
    }

    //compute hash
    size_t hash_me()
    {
        return (((short)addr_.sin_family) << 2) +
				(long)addr_.sin_addr.s_addr +
				(((short)addr_.sin_port) << 4);
    }

    /** Does this NodeInfo equal the other? */
    bool equals(Object *o) override
    {
        if (this == o)
            return true;
        NodeInfo *otherNode = dynamic_cast<NodeInfo *>(o);
        return otherNode && ntohl(addr_.sin_family) == ntohl(otherNode->addr_.sin_family) //ntohl guarantees network byte order
               && ntohl(addr_.sin_addr.s_addr) == ntohl(otherNode->addr_.sin_addr.s_addr) && ntohl(addr_.sin_port) == ntohl(otherNode->addr_.sin_port);
    }

    struct sockaddr_in getAddr()
    {
        return addr_;
    }

    const char *getIPAddr()
    {
        char *out = new char[INET_ADDRSTRLEN];
        assert(inet_ntop(AF_INET, &addr_.sin_addr, out, INET_ADDRSTRLEN));
        return out;
    }

    size_t getPort()
    {
        return htons(addr_.sin_port);
    }

    /** clones this integer */
    NodeInfo *clone()
    {
        NodeInfo *out = new NodeInfo();
        out->addr_.sin_family = addr_.sin_family;
        out->addr_.sin_addr.s_addr = addr_.sin_addr.s_addr;
        out->addr_.sin_port = addr_.sin_port;
        return out;
    }

    /** Serialize this SizeTWrapper */
    void serialize(Serializer *s)
    {
        s->write((short)addr_.sin_family);
        s->write((long)addr_.sin_addr.s_addr);
        s->write((short)addr_.sin_port);
    }

    /** Deserialize a serializer into this SizeTWrapper */
    void deserialize(Serializer *s)
    {
        addr_.sin_family = s->readShort();
        addr_.sin_addr.s_addr = s->readLong();
        addr_.sin_port = s->readShort();
    }
};