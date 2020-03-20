#pragma once

#include "column.h"
#include "stringblock.h"


#include <stdarg.h>

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column
{
public:
    StringBlock** blocks_;

    StringColumn()
    {
		capacityBlocks_ = 1;
		blocks_ = new StringBlock*[capacityBlocks_];
		numBlocks_ = 0;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
    }

    StringColumn(int n, ...)
    {
		capacityBlocks_ = (n / BLOCK_SIZE) + 1;
		blocks_ = new StringBlock*[capacityBlocks_];
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
		numBlocks_ = 0;

        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; i++)
        {
			push_back(va_arg(args, String*));
        }

        va_end(args);
    }
    
    StringColumn *as_string()
    {
        return this;
    }
	
	~StringColumn() {
		for (int i = 0; i < numBlocks_; i++) {
			delete blocks_[i];
		}
		delete[] blocks_;
	}

    /** Returns the actual string at idx, no copy; undefined on invalid idx.*/
    String *get(size_t idx)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        return blocks_[blockIdx]->get(idx % BLOCK_SIZE);
    }

    void set(size_t idx, String *val)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        blocks_[blockIdx]->set(idx % BLOCK_SIZE, val->clone());
    }
	
	void push_back(String* val)
    {
		if (size_ >= capacity_) {
			addBlocks_();
		}

		int blockIdx = size_ / BLOCK_SIZE;
		if (size_ % BLOCK_SIZE == 0) {
			blocks_[blockIdx] = new StringBlock();
			numBlocks_++;
		}
		if (val != nullptr) {
			val = val->clone();
		}
		blocks_[blockIdx]->add(val);
		size_++;
    }

	char get_type()
    {
        return 'S';
    }

	void printElement(size_t idx)
    {
        printf("<\"%s\">\t", get(idx)->c_str());
    }
	
	void addBlocks_() {
		capacityBlocks_ *= 2;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		StringBlock** newArr = new StringBlock*[capacityBlocks_];
		for (int i = 0; i < numBlocks_; i++) {
			newArr[i] = blocks_[i];
		}
		delete[] blocks_;
		blocks_ = newArr;
	}
};