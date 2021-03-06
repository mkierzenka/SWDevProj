//lang:Cpp
#pragma once 

#include "../../utils/object.h"
#include "../../utils/array.h"
#include "messagequeue.h"

#include <assert.h>

/**
 * Will contain a list of message queues for each node. Will wrap an array
 * object
 */
class MsgQueueArr : public Object {
    public:
        Array* arr_;
   
        MsgQueueArr(size_t numNodes) : Object() {
            arr_ = new Array();
            //set up queue for each node
            for (size_t i = 0; i < numNodes; i++)
            {
                arr_->add(new MessageQueue());
            }
        }
        
        ~MsgQueueArr()
        {
            delete arr_;
        }

        /**
         * Get queue from the array at given index
         */
        MessageQueue* get(size_t idx)
        {
            assert(idx < arr_->length());
            return dynamic_cast<MessageQueue*>(arr_->get(idx));
        }

        /**
         * Get the number of MessageQueues in this array.
         * ie. the number of pseudo-nodes on this "network"
         */
        size_t size()
        {
            return arr_->length();
        }
};