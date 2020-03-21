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
    //size_t capacity_; //how many elements the column can store
	//size_t numBlocks_;
	//size_t capacityBlocks_;
    DistributedArray* blocks_; //owned, arr of the keys for data in this column
    KVStore* store_;
    ColType type_; //what time of column (bool, int, float, string)
    Key* baseKey_;

    /** Default constructor: start column to fit two elements */
    Column(KVStore* store, Key* baseKey, ColType t) {
        blocks_ = new DistributedArray(store);
        store_ = store;
        baseKey_ = baseKey;
        size_ = 0;
        type_ = t;
    }

    Column() {
    }
    
    ~Column() {
        delete blocks_;
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
                return 'i';
            case ColType::Str:
                return 's';
            case ColType::Boolean:
                return 'b';
            case ColType::Float:
                return 'f';
            default:
                fprintf(stderr, "Unknown data type");
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
        size_t len_left = len;
        size_t len_added = 0;
        while ((len_added + BLOCK_SIZE) <= len) {
            //put BLOCK_SIZE data into store_
            IntBlock* block = new IntBlock();
            //IntBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len_added + BLOCK_SIZE; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added += BLOCK_SIZE;
        }

        if (len_added < len) {
            //put rest of values into store
            IntBlock* block = new IntBlock();
            //IntBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added = len;
        }
    }

    /** Add an entire list of booleans to this column*/
    void add_all(size_t len, bool* vals) {
        if (type_ != ColType::Boolean) {
            fprintf(stderr, "Cannot add boolean to column of type %c", getCharType());
            exit(1);
        }
        size_t len_left = len;
        size_t len_added = 0;
        while ((len_added + BLOCK_SIZE) <= len) {
            //put BLOCK_SIZE data into store_
            BoolBlock* block = new BoolBlock();
            //IntBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len_added + BLOCK_SIZE; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added += BLOCK_SIZE;
        }

        if (len_added < len) {
            //put rest of values into store
            BoolBlock* block = new BoolBlock();
            //BoolBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added = len;
        }
    }
    
    /** Add an entire list of floats to this column*/
    void add_all(size_t len, float* vals) {
        if (type_ != ColType::Float) {
            fprintf(stderr, "Cannot add float to column of type %c", getCharType());
            exit(1);
        }
        size_t len_left = len;
        size_t len_added = 0;
        while ((len_added + BLOCK_SIZE) <= len) {
            //put BLOCK_SIZE data into store_
            FloatBlock* block = new FloatBlock();
            //IntBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len_added + BLOCK_SIZE; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added += BLOCK_SIZE;
        }

        if (len_added < len) {
            //put rest of values into store
            FloatBlock* block = new FloatBlock();
            //FloatBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len; i++) {
                block->add(vals[i]);
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added = len;
        }
    }

    /** Add an entire list of Strings to this column. Clones each String*/
    void add_all(size_t len, String** vals) {
        if (type_ != ColType::Str) {
            fprintf(stderr, "Cannot add String to column of type %c", getCharType());
            exit(1);
        }
        size_t len_left = len;
        size_t len_added = 0;
        while ((len_added + BLOCK_SIZE) <= len) {
            //put BLOCK_SIZE data into store_
            StringBlock* block = new StringBlock();
            //StringBlock gets ints from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len_added + BLOCK_SIZE; i++) {
                block->add(vals[i]->clone());
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added += BLOCK_SIZE;
        }

        if (len_added < len) {
            //put rest of values into store
            StringBlock* block = new StringBlock();
            //StringBlock gets Strings from vals[len_added] : vals[len_added + BLOCK_SIZE]
            for (size_t i = len_added; i < len; i++) {
                block->add(vals[i]->clone());
            }
            Key* k = genKey_(len_added / BLOCK_SIZE);
			Serializer* s = new Serializer();
			block->serialize(s);
            Value* val = new Value(s->getBuffer(), s->getNumBytesWritten());
			delete s;
            store_->put(k, val);
            blocks_->addKey(k);
            len_added = len;
        }
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
    
	float get_float(size_t idx) { }
	
	bool get_bool(size_t idx) { }
	
	String* get_string(size_t idx);
	
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
        Value* v = store_->getValue(k);
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
        String* keyStr(buff->get());
        Key* k = new Key(keyStr, 0); //clones String; figure out node value later
        return k;
    }

    /** Helper to determine if given index is out-of-bounds */
    /*bool isIndexOutOfBounds_(size_t idx)
    {
        return idx >= size_;
    }*/

    /** Helper that exits program if index out-of-bounds */
   /* void exitIfIndexOutOfBounds_(size_t idx)
    {
        if (isIndexOutOfBounds_(idx))
        {
            //error message and exit of bounds
            fprintf(stderr, "Index %zu out of bounds", idx);
            exit(1);
        }
    }*/
	
	ColType getColType_(char c) {
		switch(c)
        {
            case 'i': return ColType::Integer;
            case 's': return ColType::Str;
            case 'b': return ColType::Boolean;
            case 'f': return ColType::Float;
            default:
                fprintf(stderr, "Unknown data type");
                exit(-1);
        }
	}

    
};