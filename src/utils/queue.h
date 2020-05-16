// lang::Cwc
#pragma once
#include "object.h"
#include <deque>

/**
* A Queue object serving as a FIFO queue.
*
*/
class Queue : public Object
{
public:
    std::deque<Object*> inner_;

    /** Default constructor ensure that everything is initialized */
    Queue() {}

    /** Destructor for queue **/
    ~Queue()
    {
        deleteQueue_();
    }

    /**
     * Pushes the Object o to the end of the queue. Steals ownership
     *
     * @arg o Object to append to end of queue
     **/
    void push(Object *o)
    {
        inner_.push_back(o);
    }

    /**
     * Removes and returns object from the beginning of the queue.
     * Will return nullptr if the Queue is empty;
     *
     * Caller is responsible for deleting the return value.
     */
    Object *pop()
    {
        if (inner_.empty()) {
            return nullptr;
        }
        Object* out = *(inner_.begin());
        inner_.erase(inner_.begin());
        return out;
    }

    /** Removes all of elements from this queue **/
    void clear()
    {
        deleteQueue_();
    }

    /** Compares o with this queue for equality.
     *
     * @arg object to compare with this queue
     **/
    bool equals(Object *other)
    {
        //if same memory, then they're equal
        if (other == this)
        {
            return true;
        }
        Queue *otherQ = dynamic_cast<Queue *>(other);
        if (!otherQ || this->size() != otherQ->size())
        {
            return false;
        }
        auto myElems = inner_.begin();
        auto otherElems = otherQ->inner_.begin();
        size_t len = size();
        for (size_t i = 0; i < len; i++)
        {
            Object* myE = *(myElems + i);
            Object* otherE = *(otherElems + i);
            if (!(myE->equals(otherE)))
            {
                return false;
            }
        }
        return true;
    }

    /** Return the number of elements in the queue **/
    size_t size()
    {
        return inner_.size();
    }

    /** Returns the hash code value for this list **/
    size_t hash_me_()
    {
        size_t hash = 0;
        for (Object* item : inner_ )
        {
            hash += item->hash();
        }
        return hash;
    }

    /**
     * Helper to delete queue and its nodes
     */
    void deleteQueue_()
    {
        while (!inner_.empty())
        {
            delete pop();
        }
    }
};