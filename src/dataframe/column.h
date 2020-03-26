#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../store/kvstore.h"

#include "distributedarray.h"
#include "block.h"
#include "intblock.h"
#include "boolblock.h"
#include "stringblock.h"
#include "floatblock.h"

#include <stddef.h>
#include <math.h>
#include <stdarg.h>

enum ColType {
    Str = 0, 
    Float = 1, 
    Boolean = 2, 
    Integer = 3
};


/**************************************************************************
 * Column ::
 * Represents one column of a distributed dataframe. Its members are not
 * necessarily stored locally, but are all of the same type. The actual
 * data is distributed in a KVStore. Equality is pointer equality.
 */
class Column : public Object
{
public:
    size_t size_;     //how many elements are in the column, in theory
    DistributedArray* blocks_; //owned, arr of the keys for data in this column
    KVStore* store_;
    ColType type_; //what time of column (bool, int, float, string)
    Key* baseKey_; //owned

    /** Default constructor: start column to fit two elements. Steals key ownership */
    Column(KVStore* store, Key* baseKey, ColType t) {
        blocks_ = new DistributedArray(store);
        store_ = store;
        baseKey_ = baseKey;
        size_ = 0;
        type_ = t;
    }

    // For deserializing, hopefully these values will be updated in a few calls
    Column() {
        blocks_ = new DistributedArray(nullptr);
        store_ = nullptr;
        baseKey_ = new Key();
        size_ = 0;
        type_ = ColType::Str; //arbitrary
    }
    
    ~Column() {
        delete baseKey_;
        delete blocks_;
    }

    /** Set column's KVStore */
    void setStore(KVStore* store)
    {
        store_ = store;
    }

    /** Constructor allows you to set the starting capacity of
     * the column */
     //maybe we need this?
    /*Column(size_t capacity)
    {
        size_ = 0;
        capacity_ = capacity;
    }*/

	/** 
     * Get character representation of column's type
     */
    char getCharType()
    {
        switch(type_)
        {
            case ColType::Integer:
                return 'I';
            case ColType::Str:
                return 'S';
            case ColType::Boolean:
                return 'B';
            case ColType::Float:
                return 'F';
            default:
                fprintf(stderr, "Unknown enum data type");
                exit(-1);
        }
    }
	
	
	void serialize(Serializer* s) {
		s->write(size_);
		blocks_->serialize(s);
		s->write(getCharType());
		baseKey_->serialize(s);
	}
	
	void deserialize(Serializer* s) {
		size_ = s->readSizeT();

        //pass store to distributed array; assume already set here
        blocks_->setStore(store_);

		blocks_->deserialize(s);
        type_ = getColType_(s->readChar());
		baseKey_->deserialize(s);
	}

    /** Type appropriate push_back methods. Calling the wrong method is
    * undefined behavior. **/
    void push_back(int val) {
        if (type_ != ColType::Integer) {
            fprintf(stderr, "Cannot add integer to column of type %c", getCharType());
            exit(1);
        }
    }

    void push_back(bool val) {
        if (type_ != ColType::Boolean) {
            fprintf(stderr, "Cannot add boolean to column of type %c", getCharType());
            exit(1);
        }
    }

    void push_back(float val) {
        if (type_ != ColType::Float) {
            fprintf(stderr, "Cannot add float to column of type %c", getCharType());
            exit(1);
        }
    }

    void push_back(String* val) {
        if (type_ != ColType::Str) {
            fprintf(stderr, "Cannot add String to column of type %c", getCharType());
            exit(1);
        }
    }

    /** Add an entire list of integers to this column*/
    void add_all(size_t len, int* vals) {
        if (type_ != ColType::Integer) {
            fprintf(stderr, "Cannot add integer to column of type %c", getCharType());
            exit(1);
        }
		IntBlock* block = new IntBlock();
        size_t len_added = 0;
        while (len_added < len) {
			size_t amount_to_add = std::min((len - len_added), BLOCK_SIZE);
            //IntBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
			for (size_t i = len_added; i < len_added + amount_to_add; i++) {
                block->add(vals[i]);
            }
			Serializer* s = new Serializer();
			block->serialize(s);
			addBlockToStore_(s, len_added / BLOCK_SIZE);
			delete s;
			block->clear();
            len_added += amount_to_add;
        }
		size_ += len_added;
		delete block;
    }

    /** Add an entire list of booleans to this column*/
    void add_all(size_t len, bool* vals) {
        if (type_ != ColType::Boolean) {
            fprintf(stderr, "Cannot add boolean to column of type %c", getCharType());
            exit(1);
        }
        BoolBlock* block = new BoolBlock();
        size_t len_added = 0;
        while (len_added < len) {
			size_t amount_to_add = std::min((len - len_added), BLOCK_SIZE);
            //BoolBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++) {
                block->add(vals[i]);
            }
			Serializer* s = new Serializer();
			block->serialize(s);
			addBlockToStore_(s, len_added / BLOCK_SIZE);
			delete s;
			block->clear();
            len_added += amount_to_add;
        }
		size_ += len_added;
		delete block;
    }
    
    /** Add an entire list of floats to this column*/
    void add_all(size_t len, float* vals) {
        if (type_ != ColType::Float) {
            fprintf(stderr, "Cannot add float to column of type %c", getCharType());
            exit(1);
        }
		FloatBlock* block = new FloatBlock();
        size_t len_added = 0;
        while (len_added < len) {
			size_t amount_to_add = std::min((len - len_added), BLOCK_SIZE);
            //FloatBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++) {
                block->add(vals[i]);
            }
			Serializer* s = new Serializer();
			block->serialize(s);
			addBlockToStore_(s, len_added / BLOCK_SIZE);
			delete s;
			block->clear();
            len_added += amount_to_add;
        }
		size_ += len_added;
		delete block;
    }

    /** Add an entire list of Strings to this column. Clones each String*/
    void add_all(size_t len, String** vals) {
        if (type_ != ColType::Str) {
            fprintf(stderr, "Cannot add String to column of type %c", getCharType());
            exit(1);
        }
		StringBlock* block = new StringBlock();
        size_t len_added = 0;
        while (len_added < len) {
			size_t amount_to_add = std::min((len - len_added), BLOCK_SIZE);
            //StringBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++) {
                block->add(vals[i]->clone());
            }
			Serializer* s = new Serializer();
			block->serialize(s);
			addBlockToStore_(s, len_added / BLOCK_SIZE);
			delete s;
			block->clear();
            len_added += amount_to_add;
        }
		size_ += len_added;
		delete block;
    }
	
    /** Returns the number of elements in the column. */
    size_t size()
    {
        return size_;
    }

    /** Print the element in sorer format, with brackets */
    virtual void printElement(size_t rowIdx)
    {
    }
    
	float get_float(size_t idx) {
        if (!properType(ColType::Float))
        {
            fprintf(stderr, "Cannot get float from non-float col");
            return -1;
        }

        size_t chunk = idx / BLOCK_SIZE;
        size_t idxInChunk = idx % BLOCK_SIZE;

        //Key to look up data
        Key* k = genKey_(chunk);
		float out = blocks_->getFloat(k, idxInChunk);
		delete k;
		return out;
     }
	
	bool get_bool(size_t idx) {
        if (!properType(ColType::Boolean))
        {
            fprintf(stderr, "Cannot get bool from non-bool col");
            return -1;
        }

        size_t chunk = idx / BLOCK_SIZE;
        size_t idxInChunk = idx % BLOCK_SIZE;

        //Key to look up data
        Key* k = genKey_(chunk);
        Value* v = blocks_->get(k);
		Serializer* s = new Serializer(v->getSize(), v->getData());
        BoolBlock* boolData = new BoolBlock();
		boolData->deserialize(s);
        return boolData->get(idxInChunk);
     }
	
	String* get_string(size_t idx) {
        if (!properType(ColType::Str))
        {
            fprintf(stderr, "Cannot get String from non-String col");
            exit(1);
        }

        size_t chunk = idx / BLOCK_SIZE;
        size_t idxInChunk = idx % BLOCK_SIZE;

        //Key to look up data
        Key* k = genKey_(chunk);
        Value* v = blocks_->get(k);
		Serializer* s = new Serializer(v->getSize(), v->getData());
        StringBlock* strData = new StringBlock();
		strData->deserialize(s);
        return strData->get(idxInChunk);
    }
	
    /** Get int from the column at specified index */
    int get_int(size_t idx)
    {
        if (!properType(ColType::Integer))
        {
            fprintf(stderr, "Cannot get int from non-int col");
            return -1;
        }

        size_t chunk = idx / BLOCK_SIZE;
        size_t idxInChunk = idx % BLOCK_SIZE;

        //Key to look up data
        Key* k = genKey_(chunk);
        Value* v = blocks_->get(k);
		Serializer* s = new Serializer(v->getSize(), v->getData());
        IntBlock* intData = new IntBlock();
		intData->deserialize(s);
        return intData->get(idxInChunk);
    }

    /** Check if the type of this column matches the given type */
    bool properType(ColType ct)
    {
        return type_ == ct;
    }

    Key* genKey_(size_t blockNum) {
        StrBuff* buff = new StrBuff();
        buff->c(*(baseKey_->getKeyStr()));
        buff->c("-");
        buff->c(blockNum);
        String* keyStr = buff->get();
        delete buff;
        Key* k = new Key(keyStr, 0); //clones String; figure out node value later
        delete keyStr;
        return k;
    }
	
	ColType getColType_(char c) {
		switch(c)
        {
            case 'I': return ColType::Integer;
            case 'S': return ColType::Str;
            case 'B': return ColType::Boolean;
            case 'F': return ColType::Float;
            default:
                fprintf(stderr, "Unknown char data type: %c\n", c);
                exit(-1);
        }
	}

    /** Check if two columns equal */
	bool equals(Object* other)
	{
		if (this == other)
		{
			return true;
		}

		Column* c = dynamic_cast<Column*>(other);
	
        // store_ is not included
		if (c == nullptr || size_ != c->size_ || !(blocks_->equals(c->blocks_)) || type_ != c->type_ || !(baseKey_->equals(c->baseKey_)))
        {
			return false;
		}

		return true;
	}
	
	/** Compute hash code of this column */
	size_t hash_me_()
	{
        size_t hash_ = 0;
        hash_ += size_;
        hash_ += reinterpret_cast<size_t>(baseKey_);
        hash_ += reinterpret_cast<size_t>(blocks_);
        hash_ += getCharType();
        return hash_;
	}

	/** Constructs a new KV pair from the keyIdx and value within the serializer.
     * Adds it to the store.
     */
	void addBlockToStore_(Serializer* s, size_t keyIdx) {
        Key* k = genKey_(keyIdx);
        Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
        store_->put(k, val);
        blocks_->addKey(k);
        delete k;
        delete val;
    }
};