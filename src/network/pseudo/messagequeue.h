//lang:Cpp

#pragma once

#include "../../utils/queue.h"
#include "../../utils/thread.h"
#include "../../utils/object.h"
#include "../../serial/message.h"

/**
 * A queue of messages. Wraps a queue object and makes proper cast when necessary.
 * Also will hold a lock so only one process can read/write from queue at one time
 */
class MessageQueue : public Object {
    public:
        Queue* messages_;
        Lock lock_;

        MessageQueue() : Object()
        {
            messages_ = new Queue();
        }

        ~MessageQueue() {
            delete messages_;
        }

        /**
         * Add a message to the queue
         */
        void push(Message* msg)
        {
            lock_.lock();
            messages_->push(msg);
            lock_.notify_all();
            lock_.unlock();
        }
        
        /**
         * Get next mesaage from queue
         */
        Message* pop() {
            lock_.lock();
            while (messages_->size() == 0) 
            {
                //printf("Waiting for message...\n");
                lock_.wait();
            }

            //printf("Got message!\n");
            Message* res = dynamic_cast<Message*>(messages_->pop());
            lock_.unlock();
            return res;
        }

        /** Returns the number of entries in this MessageQueue */
        size_t size() {
            return messages_->size();
        }
        
};