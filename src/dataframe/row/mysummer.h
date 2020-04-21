#pragma once

#include "reader.h"


/**
 * A Reader implementation for summing single-column integer dataframes.
 * 
 * @authors broder.c@husky.neu.edu & kierzenka.m@husky.neu.edu
 */
class MySummer : public Reader {
public:
    int sum_;
    MySummer() {
		sum_ = 0;
	}

    ~MySummer() {}

    virtual bool visit(Row& r) {
        sum_ += r.get_int(0);
	}

    int getSum() {
        return sum_;
    }
};