#pragma once

#include "../utils/object.h"
#include "../utils/array.h"

/**
 * This class represents a node's directory, maintaining a list of its 
 * client connections
 * 
 * @author broder.c@husky.neu.edu
 */
class Directory : public Object {
    Array* ipList_; // list of ips

    Directory()
    {
        ipList_ = new Array();
    }

    ~Directory()
    {
        delete ipList_;
    }

    void addIp()
    {
        ipList_->add
    }
}; 