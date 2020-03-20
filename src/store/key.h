#pragma once

#include "../utils/object.h"
#include "../utils/string.h"

/** This class represents a key in our distributed key-value store. It contains a String
 * identifier for the piece of data it maps to and information for the node on which the
 * data is stored. Keys are immutable
 * 
 * Authors: Chase Broder and Marcin Kierzenka 
 */
class Key : public Object
{
public:
    String *kStr_;    //string that identifies data
    size_t homeNode_; //where data lives

    Key(String *str, size_t node)
    {
        kStr_ = str->clone();
        homeNode_ = node;
    }

    ~Key()
    {
        delete kStr_;
    }

    /** Serialize a Key*/
    void serialize(Serializer* s) {
        kStr_->serialize(s);
        s->write(homeNode_);
    }

    /** Deserialize a Key, mutate this Key*/
    void deserialize(Serializer* s) {
        kStr_->deserialize(s);
        homeNode_ = s->readSizeT();
    }

    /**
     * Return this key's String value
     */
    String *getKeyStr()
    {
        return kStr_;
    }

    /**
     * Return node that this key's data lives on
     */
    size_t getNode()
    {
        return homeNode_;
    }
};
