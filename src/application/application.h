//lang:Cpp

#pragma once

#include "../utils/object.h"
#include "../store/kvstore.h"

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
    KVStore* kv_;

    Application(size_t i)
    {
        idx_ = i;
        kv_ = new KVStore(i);
    }
	
    ~Application() {
        delete kv_;
    }

    size_t this_node()
    {
        return idx_;
    }

    void run_() {}
};