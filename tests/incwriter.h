#pragma once

#include "../src/filereader/writer.h"

#include <assert.h>

/**
 * This is a simple writer that will write out an integer value for each row, incrementing each time.
 * Starts at 0 or startValue. DataFrame schema must be: "I"
 * 
 * @authors Marcin Kierzenka and Chase Broder
 */
class IncWriter : public Writer {
    public:
        int curVal_;
        int numRowsAtEnd_;
        int numRowsSoFar_;

        IncWriter(int numRowsExpected) : IncWriter(0, numRowsExpected) { }

        IncWriter(int startValue, int numRowsExpected) : Writer() {
            curVal_ = startValue;
            assert(numRowsExpected >= 0);
            numRowsAtEnd_ = numRowsExpected;
            numRowsSoFar_ = 0;
        }

        ~IncWriter() {}

        void visit(Row& r) override {
            r.set(0, curVal_);
            curVal_++;
            numRowsSoFar_++;
        }

        bool done() override {
            return numRowsSoFar_ == numRowsAtEnd_;
        }
};