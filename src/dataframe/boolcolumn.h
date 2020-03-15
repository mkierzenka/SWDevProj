#pragma once

#include "column.h"
#include "boolblock.h"
#include <stdarg.h>


/*************************************************************************
 * BoolColumn::
 * Holds primitive bool values, unwrapped.
 */
class BoolColumn : public Column
{
public:
    BoolBlock** blocks_; //holds blocks of data in columns

    BoolColumn()
    {
        capacityBlocks_ = 1;
		blocks_ = new BoolBlock*[capacityBlocks_];
		numBlocks_ = 0;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
    }

    BoolColumn(int n, ...)
    {
        capacityBlocks_ = (n / BLOCK_SIZE) + 1;
		blocks_ = new BoolBlock*[capacityBlocks_];
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
		numBlocks_ = 0;

        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; i++)
        {
            bool tmp = va_arg(args, int);
			push_back(tmp);
        }

        va_end(args);
    }
	
	~BoolColumn() {
		for (int i = 0; i < numBlocks_; i++) {
			delete blocks_[i];
		}
		delete[] blocks_;
	}

    bool get(size_t idx)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        return blocks_[blockIdx]->get(idx % BLOCK_SIZE);
    }

    BoolColumn *as_bool()
    {
        return this;
    }

    void set(size_t idx, bool val)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        blocks_[blockIdx]->set(idx % BLOCK_SIZE, val);
    }

    void push_back(int val)
    {
		if (size_ >= capacity_) {
			addBlocks_();
		}

		int blockIdx = size_ / BLOCK_SIZE;
		if (size_ % BLOCK_SIZE == 0) {
			blocks_[blockIdx] = new BoolBlock();
			numBlocks_++;
		}
		blocks_[blockIdx]->add(val);
		size_++;
    }

    char get_type()
    {
        return 'B';
    }

    void printElement(size_t idx)
    {
        //Cited from https://stackoverflow.com/questions/17307275/what-is-the-printf-format-specifier-for-bool
        printf("<%s>\t", get(idx) ? "true" : "false");
    }
	
	void addBlocks_() {
		capacityBlocks_ *= 2;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		BoolBlock** newArr = new BoolBlock*[capacityBlocks_];
		for (int i = 0; i < numBlocks_; i++) {
			newArr[i] = blocks_[i];
		}
		delete[] blocks_;
		blocks_ = newArr;
	}
};
