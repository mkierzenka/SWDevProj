#pragma once

#include "../../utils/object.h"
#include "../../utils/string.h"
#include "../../utils/args.h"
#include "../../utils/datatype.h"
#include "../../utils/datatypeutils.h"
#include "../../store/kvstore.h"

#include "distributedarray.h"
#include "../block/block.h"
#include "../block/intblock.h"
#include "../block/boolblock.h"
#include "../block/stringblock.h"
#include "../block/doubleblock.h"

#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

/**************************************************************************
 * Column ::
 * Represents one column of a distributed dataframe. Its members are not
 * necessarily stored locally, but are all of the same type. The actual
 * data is distributed in a KVStore. Equality is pointer equality.
 */
class Column : public Object
{
public:
    size_t size_;              //how many elements are in the column, in theory
    DistributedArray *blocks_; //owned, arr of the keys for data in this column
    KVStore *store_;
    DataType type_; //what time of column (bool, int, double, string)
    Key *baseKey_; //owned, base key for the column

    /** Default constructor: start column to fit two elements. Steals key ownership */
    Column(KVStore *store, Key *baseKey, DataType t)
    {
        blocks_ = new DistributedArray(store);
        store_ = store;
        baseKey_ = baseKey;
        size_ = 0;
        type_ = t;
    }

    /** Default constructor: start column to fit two elements. Steals key ownership */
    Column(KVStore *store, Key *baseKey, const char type)
    {
        blocks_ = new DistributedArray(store);
        store_ = store;
        baseKey_ = baseKey;
        size_ = 0;
        type_ = DataTypeUtils::charToType(type);
    }

    // For deserializing, hopefully these values will be updated in a few calls
    Column()
    {
        blocks_ = new DistributedArray(nullptr);
        store_ = nullptr;
        baseKey_ = new Key();
        size_ = 0;
        type_ = DataType::Str; //arbitrary
    }

    ~Column()
    {
        delete baseKey_;
        delete blocks_;
    }

    /** Set column's KVStore */
    void setStore(KVStore *store)
    {
        store_ = store;
    }

    void serialize(Serializer *s)
    {
        s->write(size_);
        blocks_->serialize(s);
        s->write(DataTypeUtils::typeToChar(type_));
        baseKey_->serialize(s);
    }

    void deserialize(Serializer *s)
    {
        size_ = s->readSizeT();

        //pass store to distributed array; assume already set here
        blocks_->setStore(store_);

        blocks_->deserialize(s);
        //type_ = getDataType_(s->readChar());
        type_ = DataTypeUtils::charToType(s->readChar());
        baseKey_->deserialize(s);
    }

    /** Type appropriate push_back methods. Calling the wrong method is
    * undefined behavior. **/
    /*void push_back(int val)
    {
        assert(false);
        if (type_ != DataType::Integer)
        {
            fprintf(stderr, "Cannot add integer to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
    }

    void push_back(bool val)
    {
        assert(false);
        if (type_ != DataType::Boolean)
        {
            fprintf(stderr, "Cannot add boolean to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
    }

    void push_back(double val)
    {
        assert(false);
        if (type_ != DataType::Double)
        {
            fprintf(stderr, "Cannot add double to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
    }

    void push_back(String *val)
    {
        assert(false);
        if (type_ != DataType::Str)
        {
            fprintf(stderr, "Cannot add String to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
    }*/

    /** Append an entire list of integers to this column */
    void add_all(size_t len, int *vals, size_t colIdx)
    {
        if (type_ != DataType::Integer)
        {
            fprintf(stderr, "Cannot add integer to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
        size_t blockCountBefore = blocks_->length();
        IntBlock *block = new IntBlock();
        size_t len_added = 0;
        while (len_added < len)
        {
            size_t amount_to_add = std::min((len - len_added), args.blockSize);
            //IntBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++)
            {
                block->add(vals[i]);
            }
            Serializer *s = new Serializer();
            block->serialize(s);
            addBlockToStore_(s, blockCountBefore + (len_added / args.blockSize), colIdx);
            delete s;
            block->clear();
            len_added += amount_to_add;
        }
        size_ += len_added;
        delete block;
    }

    /** Append an entire list of booleans to this column */
    void add_all(size_t len, bool *vals, size_t colIdx)
    {
        if (type_ != DataType::Boolean)
        {
            fprintf(stderr, "Cannot add boolean to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
        size_t blockCountBefore = blocks_->length();
        BoolBlock *block = new BoolBlock();
        size_t len_added = 0;
        while (len_added < len)
        {
            size_t amount_to_add = std::min((len - len_added), args.blockSize);
            //BoolBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++)
            {
                block->add(vals[i]);
            }
            Serializer *s = new Serializer();
            block->serialize(s);
            addBlockToStore_(s, blockCountBefore + (len_added / args.blockSize), colIdx);
            delete s;
            block->clear();
            len_added += amount_to_add;
        }
        size_ += len_added;
        delete block;
    }

    /** Append an entire list of doubles to this column */
    void add_all(size_t len, double *vals, size_t colIdx)
    {
        if (type_ != DataType::Double)
        {
            fprintf(stderr, "Cannot add double to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
        size_t blockCountBefore = blocks_->length();
        DoubleBlock *block = new DoubleBlock();
        size_t len_added = 0;
        while (len_added < len)
        {
            size_t amount_to_add = std::min((len - len_added), args.blockSize);
            //DoubleBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++)
            {
                block->add(vals[i]);
            }
            Serializer *s = new Serializer();
            block->serialize(s);
            addBlockToStore_(s, blockCountBefore + (len_added / args.blockSize), colIdx);
            delete s;
            block->clear();
            len_added += amount_to_add;
        }
        size_ += len_added;
        delete block;
    }

    /** Append an entire list of Strings to this column. Clones each String */
    void add_all(size_t len, String **vals, size_t colIdx)
    {
        if (type_ != DataType::Str)
        {
            fprintf(stderr, "Cannot add String to column of type %c", DataTypeUtils::charToType(type_));
            exit(1);
        }
        size_t blockCountBefore = blocks_->length();
        StringBlock *block = new StringBlock();
        size_t len_added = 0;
        while (len_added < len)
        {
            size_t amount_to_add = std::min((len - len_added), args.blockSize);
            //StringBlock gets ints from vals[len_added] : vals[len_added + amount_to_add]
            for (size_t i = len_added; i < len_added + amount_to_add; i++)
            {
                block->add(vals[i]->clone());
            }
            Serializer *s = new Serializer();
            block->serialize(s);
            addBlockToStore_(s, blockCountBefore + (len_added / args.blockSize), colIdx);
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

    /** Get double from column at certain index */
    double get_double(size_t idx, size_t colIdx)
    {
        if (!properType(DataType::Double))
        {
            fprintf(stderr, "Cannot get double from non-double col");
            return -1;
        }

        size_t chunk = idx / args.blockSize;
        size_t idxInChunk = idx % args.blockSize;

        Key *k = genKey_(chunk, colIdx); //Key to look up data
        double out = blocks_->getDouble(k, idxInChunk);
        delete k;
        return out;
    }

    /** Get boolean from column at certain index */
    bool get_bool(size_t idx, size_t colIdx)
    {
        if (!properType(DataType::Boolean))
        {
            fprintf(stderr, "Cannot get bool from non-bool col");
            return -1;
        }

        size_t chunk = idx / args.blockSize;
        size_t idxInChunk = idx % args.blockSize;

        Key *k = genKey_(chunk, colIdx); //Key to look up data
        bool out = blocks_->getBool(k, idxInChunk);
        delete k;
        return out;
    }

    /** Get string from column at certain index.
     * Caller responsible for deleting string
     */
    String *get_string(size_t idx, size_t colIdx)
    {
        if (!properType(DataType::Str))
        {
            fprintf(stderr, "Cannot get String from non-String col");
            exit(1);
        }

        size_t chunk = idx / args.blockSize;
        size_t idxInChunk = idx % args.blockSize;

        Key *k = genKey_(chunk, colIdx); //Key to look up data
        String *out = blocks_->getString(k, idxInChunk);
        delete k;
        return out;
    }

    /** Get int from the column at specified index */
    int get_int(size_t idx, size_t colIdx)
    {
        if (!properType(DataType::Integer))
        {
            fprintf(stderr, "Cannot get int from non-int col");
            return -1;
        }

        size_t chunk = idx / args.blockSize;
        size_t idxInChunk = idx % args.blockSize;

        Key *k = genKey_(chunk, colIdx); //Key to look up data
        int out = blocks_->getInt(k, idxInChunk);
        delete k;
        return out;
    }

    /** Check if the type of this column matches the given type */
    bool properType(DataType ct)
    {
        return type_ == ct;
    }

    /** Return type of column */
    DataType getType()
    {
        return type_;
    }

    /** Check if two columns equal */
    bool equals(Object *other)
    {
        if (this == other)
        {
            return true;
        }

        Column *c = dynamic_cast<Column *>(other);

        // store_ is not included
        if (c == nullptr || size_ != c->size_ || !(blocks_->equals(c->blocks_)) || type_ != c->type_ || !(baseKey_->equals(c->baseKey_)))
        {
            return false;
        }

        return true;
    }

    Key *genKey_(size_t blockNum, size_t colIdx)
    {
        StrBuff *buff = new StrBuff();
        buff->c(*(baseKey_->getKeyStr())); //copies memory into buffer
        buff->c("-");
        buff->c(colIdx);
        buff->c("-");
        buff->c(blockNum);
        String *keyStr = buff->get();
        delete buff;
        Key *k = new Key(keyStr, blockNum % args.numNodes); //clones String
        delete keyStr;
        return k;
    }

    /** Compute hash code of this column */
    size_t hash_me_()
    {
        size_t hash_ = 0;
        hash_ += size_;
        hash_ += reinterpret_cast<size_t>(baseKey_);
        hash_ += reinterpret_cast<size_t>(blocks_);
        hash_ += DataTypeUtils::typeToChar(type_);
        return hash_;
    }

    /** Constructs a new KV pair from the keyIdx and value within the serializer.
     * Adds it to the store.
     */
    void addBlockToStore_(Serializer *s, size_t keyIdx, size_t colIdx)
    {
        Key *k = genKey_(keyIdx, colIdx);
        Value *val = new Value(s->getBuffer(), s->getNumBytesWritten());
        store_->put(k, val);
        blocks_->addKey(k);
        delete val;
        delete k;
    }
};