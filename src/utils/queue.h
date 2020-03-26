// lang::Cwc
#pragma once
#include "object.h"

//This class will be used to represent an element on the queue.
//It will contain the value of the element and a reference to the next index
class Node : public Object
{
public:
    Object *o;  //element
    Node *next; //reference to next element in queue

    //checks if two nodes are equal
    bool equals(Object *other)
    {
        //same memory; must be equal
        if (this == other)
        {
            return true;
        }

        Node *otherNode = dynamic_cast<Node *>(other);
        //can't cast to Node
        if (!otherNode)
        {
            return false;
        }

        if (otherNode->o != nullptr && !otherNode->o->equals(o))
        {
            return false;
        }

        if (o != nullptr && !o->equals(otherNode->o))
        {
            return false;
        }

        //Current element same and no other elements exist
        if (next == nullptr && otherNode->next == nullptr)
        {
            return true;
        }

        if ((next == nullptr && otherNode->next != nullptr) || (next != nullptr && otherNode->next == nullptr))
        {
            return false;
        }

        //both not null: continue
        return next->equals(otherNode->next);
    }
};

/**
* A Queue object serving as a FIFO queue.
*
*/
class Queue : public Object
{
public:
    Node *front_; //pointer to first element in queue
    Node *back_;  //pointer to last element in queue
    size_t len_;  //number of elements in queue

    /** Default constructor ensure that everything is initialized */
    Queue()
    {
        front_ = nullptr;
        back_ = nullptr;
        len_ = 0;
    }

    /** Destructor for queue **/
    ~Queue()
    {
        deleteQueue_();
    }

    /**
	 * Pushes given object o to the end of the queue
	 * 
	 * @arg o Object to append to end of queue
	 **/
    void push(Object *o)
    {
        //if queue is empty, make it front and back of queue
        if (len_ == 0)
        {
            front_ = back_ = createNode_(o);
            len_ += 1;
            return;
        }

        //create new node to eventually push to back
        Node *newBack = createNode_(o);

        //update current end of queue
        back_->next = newBack;

        //set new end of queue and update length
        back_ = newBack;
        len_ += 1;
    }

    /** Returns and removes object from the beginning of the queue if anything is in queue**/
    Object *pop()
    {
        //first check if queue has elements in it
        if (is_empty())
        {
            return nullptr;
        }

        //store removed node
        Node *temp = front_;
        //delete(temp);

        //update front node
        front_ = front_->next;
        len_ -= 1;

        //check if queue now empty; means both front and back need to be null
        if (front_ == nullptr)
        {
            back_ = nullptr;
        }

        return temp->o;
    }

    /** Returns whether the queue is empty **/
    bool is_empty()
    {
        return len_ == 0;
    }

    /** Removes all of elements from this queue **/
    void clear()
    {
        //delete queue
        deleteQueue_();

        //reset values
        front_ = nullptr;
        back_ = nullptr;
        len_ = 0;
    }

    /** Compares o with this queue for equality.
	 * 
	 * @arg object to compare with this queue
	 **/
    bool equals(Object *o)
    {
        //if same memory, then they're equal
        if (o == this)
        {
            return true;
        }

        //cast object to queue
        Queue *other = dynamic_cast<Queue *>(o);

        //check to make sure other isn't null
        if (!other)
        {
            return false;
        }

        //Current element same and no other elements exist
        if (front_ == nullptr && other->front_ == nullptr)
        {
            return true;
        }

        if ((front_ == nullptr && other->front_ != nullptr) || (front_ != nullptr && other->front_ == nullptr))
        {
            return false;
        }

        return front_->equals(other->front_);
    }

    /** Return the number of elements in the queue **/
    size_t size()
    {
        return len_;
    }

    /** Returns the hash code value for this list **/
    size_t hash_me_()
    {
        size_t hash = 0;
        Node *cur = front_;
        while (cur != nullptr)
        {
            size_t nodeHash = reinterpret_cast<size_t>(cur);
            hash += nodeHash;
            cur = cur->next;
        }

        return hash;
    }

    /**
	 * Helper to create a queue node with the given object
	 */
    Node *createNode_(Object *obj)
    {
        Node *n = new Node();
        n->o = obj;
        n->next = nullptr;

        return n;
    }

    /**
	 * Helper to delete queue and its nodes
	 */
    void deleteQueue_()
    {
        Node *cur = front_;
        while (cur != nullptr)
        {
            //set next element, delete existing, and swap
            Node *next = cur->next;
            delete (cur);
            cur = next;
        }
    }
};