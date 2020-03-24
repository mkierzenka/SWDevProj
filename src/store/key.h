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

    Key()
    {
        kStr_ = new String("");
        homeNode_ = 0;
    }

    ~Key()
    {
        delete kStr_;
    }

    /** Serialize a Key*/
    void serialize(Serializer *s)
    {
        kStr_->serialize(s);
        s->write(homeNode_);
    }

    /** Deserialize a Key, mutate this Key*/
    void deserialize(Serializer *s)
    {
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

    bool equals(Object *other)
    {
        if (other == this)
        {
            return true;
        }

        Key *o = dynamic_cast<Key *>(other);
        return (kStr_->equals(o->getKeyStr())) &&
               homeNode_ == o->getNode();
    }

    size_t hash_me_()
    {
        return reinterpret_cast<size_t>(kStr_) + homeNode_;
    }
	
	/** Returns a new Key with cloned values */
	Key* clone() {
		return new Key(kStr_->clone(), homeNode_);
	}

    /** Add an index to the Key's string and return a new key */
    Key* addIndex(size_t idx)
    {
        //build up new key with string buffer
        //format of string should be "{kStr_->c_}-{idx}""
        StrBuff* sb = new StrBuff();
        sb->c(*kStr_);
        sb->c('-');
        sb->c(idx);
        String* newStrRes = sb->get();

        delete sb;
        return new Key(newStrRes, homeNode_);
    }
};
