#pragma once

#include "column.h"
#include "intblock.h"
#include <stdarg.h>
#include <iostream>

/*************************************************************************
 * IntColumn::
 * Holds primitive int values, unwrapped.
 */
class IntColumn : public Column {
public:
    IntBlock** blocks_;
	
    IntColumn()
    {
		capacityBlocks_ = 1;
		blocks_ = new IntBlock*[capacityBlocks_];
		numBlocks_ = 0;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
    }

    IntColumn(int n, ...)
    {
		capacityBlocks_ = (n / BLOCK_SIZE) + 1;
		blocks_ = new IntBlock*[capacityBlocks_];
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
		numBlocks_ = 0;

        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; i++)
        {
            int tmp = va_arg(args, int);
			push_back(tmp);
        }

        va_end(args);
    }
	
	~IntColumn() {
		for (int i = 0; i < numBlocks_; i++) {
			delete blocks_[i];
		}
		delete[] blocks_;
	}

    int get(size_t idx)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        return blocks_[blockIdx]->get(idx % BLOCK_SIZE);
    }

    IntColumn *as_int()
    {
        return this;
    }

    void set(size_t idx, int val)
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
			blocks_[blockIdx] = new IntBlock();
			numBlocks_++;
		}
		blocks_[blockIdx]->add(val);
		size_++;
    }

	char get_type()
    {
        return 'I';
    }

	void printElement(size_t idx)
    {
        printf("<%d>\t", get(idx));
    }
	
	void addBlocks_() {
		capacityBlocks_ *= 2;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		IntBlock** newArr = new IntBlock*[capacityBlocks_];
		for (int i = 0; i < numBlocks_; i++) {
			newArr[i] = blocks_[i];
		}
		delete[] blocks_;
		blocks_ = newArr;
	}
};