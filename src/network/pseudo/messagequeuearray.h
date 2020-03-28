//lang:Cpp
#pragma once 

#include "../../utils/object.h"
#include "../../utils/array.h"
#include "messagequeue.h"

/**
 * Will contain a list of message queues for each node. Will wrap an array
 * object
 */
class MsgQueueArr : public Object {
    public:
        Array* arr_;
   
        MsgQueueArr() : Object() {
            arr_ = new Array();
        }
        
        ~MsgQueueArr()
        {
            delete arr_;
        }

        /**
         * Add queue to end of the array
         */
        void push_back(MessageQueue* mq)
        {
            arr_->add(mq);
        }

        /**
         * Get queue from the array at given index
         */
        MessageQueue* get(size_t idx)
        {
            return dynamic_cast<MessageQueue*>(arr_->get(idx));
        }

};