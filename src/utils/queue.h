// lang::Cwc
#pragma once
#include "object.h"

/**
 * This class represents an entry in the Queue. It steals ownership of the
 * Object passed into it. Calling delete will chain through all Nodes connected
 * to this one.
 *
 * @authors kierzenka.m@husky.neu.edu & broder.c@husky.neu.edu
 */
class Node : public Object
{
public:
    Object *val_;   //owned
    Node *next_;    //reference to next element in queue

    /** Create a new Node from an Object. Steals ownership. */
	Node(Object* val) : Object() {
		val_ = val;
		next_ = nullptr;
	}

	~Node() {
		if (val_) {
			delete val_;
		}
		if (next_) {
			delete next_;
		}
	}

	/** Returns a pointer to the value at this node. Does not copy. */
	Object* getValue() {
		return val_;
	}

	/**
	 * Sets the value of this Node to newVal. Returns the previous value
	 * Caller is responsible for deleting returned Object.
	 */
	Object* setValue(Object* newVal) {
		Object* oldVal = val_;
		val_ = newVal;
		return oldVal;
	}

	/** Returns the pointer to the next Node, does not clone */
	Node* getNext() {
		return next_;
	}

	/** Sets the value of next. Returns the old value (no clone) */
	Node* setNext(Node* newNext) {
		next_ = newNext;
	}

	/** Returns true if this node is equal to the other Object */
    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        Node *otherNode = dynamic_cast<Node *>(other);
        if (!otherNode)
        {
            return false;
        }

        if (otherNode->getValue() && !otherNode->getValue()->equals(getValue()))
        {
            return false;
        }

        if (getValue() && !getValue()->equals(otherNode->getValue()))
        {
            return false;
        }

        //Current element same and no other elements exist
        if (!next_ && !(otherNode->next_))
        {
            return true;
        }

        if ((!next_ && otherNode->next_) || (next_ && !(otherNode->next_)))
        {
            return false;
        }

        //both not null: continue
        return next_->equals(otherNode->next_);
    }
};

/**
* A Queue object serving as a FIFO queue.
*
*/
class Queue : public Object
{
public:
    Node *front_; //Owned, pointer to first element in queue
    Node *back_;  //Owned, pointer to last element in queue
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
        if (size() == 0)
        {
            front_ = back_ = new Node(o);
            len_ += 1;
            return;
        }

        //create new node to eventually push to back
        Node *newBack = new Node(o);

        //update current end of queue
        back_->setNext(newBack);

        //set new end of queue and update length
        back_ = newBack;
        len_ += 1;
    }

    /**
     * Removes and returns object from the beginning of the queue.
     * Will return nullptr if the Queue is empty;
     *
     * Caller is responsible for deleting the return value.
     */
    Object *pop()
    {
        if (size() == 0)
        {
            return nullptr;
        }
        Node *temp = front_;
        front_ = temp->getNext(); // new front is old front's next
        len_ -= 1;

        //check if queue now empty; means both front and back need to be null
        if (!front_)
        {
            back_ = nullptr;
        }
    	Object* retVal = temp->setValue(nullptr);
    	temp->setNext(nullptr);
    	delete temp;
        return retVal;
    }

    /** Removes all of elements from this queue **/
    void clear()
    {
        deleteQueue_();
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
        if (!front_ && !(other->front_))
        {
            return true;
        }

        if ((!front_ && other->front_) || (front_ && !(other->front_)))
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
            cur = cur->getNext();
        }

        return hash;
    }

    /**
	 * Helper to delete queue and its nodes
	 */
    void deleteQueue_()
    {
        delete front_;
    }
};