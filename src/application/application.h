//lang:Cpp

#pragma once

#include "../utils/object.h"
#include "../store/kvstore.h"
#include "../network/network.h"

/** This class represents the highest layer of the program. This is where the user will specify
 * what operations they'd like to perform on certain data. The application class itself will
 * serve more as an interface, and can be subclasses. Applications will run on each node
 * 
 * @authors Chase Broder and Marcin Kierzenka
 */
class Application : public Object
{
public:
    size_t idx_; //what index node this application is running on
    Network* net_; //network for sending and receiving messages; application will own
    KVStore* kv_;

    Application(size_t i)
    {
        idx_ = i;
        net_ = new Network(generateIp_(), idx_);
        kv_ = new KVStore(i, net_);
    }
	
    ~Application() {
        delete kv_;
    }

    size_t this_node()
    {
        return idx_;
    }

    /** Get network node from application */
    Network* getNetwork()
    {
        return net_;
    }

    /** Get store from application */
    KVStore* getStore()
    {
        return kv_;
    }

    /** Create an ip address for this application node, using the home node */
    char* generateIp_()
    {
        char* ip;
        sprintf(ip, "127.0.0.%zu", idx_ + 1);
        return ip;
    }

    virtual void run_() {}
};