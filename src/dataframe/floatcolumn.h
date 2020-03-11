#pragma once

#include "column.h"
#include "floatblock.h"

#include <stdarg.h>


/*************************************************************************
 * FloatColumn::
 * Holds primitive float values, unwrapped.
 */
class FloatColumn : public Column
{
public:
    FloatBlock** blocks_; //holds block pointers of float

    FloatColumn()
    {
        capacityBlocks_ = 1;
        blocks_ = new FloatBlock*[capacityBlocks_];
        numBlocks_ = 0;
        capacity_ = capacityBlocks_ * BLOCK_SIZE;
        size_ = 0;
    }

    FloatColumn(int n, ...)
    {
        capacityBlocks_ = (n / BLOCK_SIZE) + 1;
		blocks_ = new FloatBlock*[capacityBlocks_];
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		size_ = 0;
		numBlocks_ = 0;

        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; i++)
        {
            float tmp = va_arg(args, double);
			push_back(tmp);
        }

        va_end(args);
    }

    float get(size_t idx)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        return blocks_[blockIdx]->get(idx % BLOCK_SIZE);
    }

    FloatColumn *as_float()
    {
        return this;
    }

    void set(size_t idx, float val)
    {
        exitIfIndexOutOfBounds_(idx);
		int blockIdx = idx / BLOCK_SIZE;
        blocks_[blockIdx]->set(idx % BLOCK_SIZE, val);
    }

    void push_back(float val)
    {
		if (size_ >= capacity_) {
			addBlocks_();
		}

		int blockIdx = size_ / BLOCK_SIZE;
		if (size_ % BLOCK_SIZE == 0) {
			blocks_[blockIdx] = new FloatBlock();
			numBlocks_++;
		}
		blocks_[blockIdx]->add(val);
		size_++;
    }

    char get_type()
    {
        return 'F';
    }

    void printElement(size_t idx)
    {
        printf("<%f> ", get(idx));
    }

    void addBlocks_() {
		capacityBlocks_ *= 2;
		capacity_= capacityBlocks_ * BLOCK_SIZE;
		FloatBlock** newArr = new FloatBlock*[capacityBlocks_];
		for (int i = 0; i < numBlocks_; i++) {
			newArr[i] = blocks_[i];
		}
		delete[] blocks_;
		blocks_ = newArr;
	}
};