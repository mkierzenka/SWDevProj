#pragma once

#include "../utils/object.h"
#include "../dataframe/row/rower.h"
#include "../dataframe/row/row.h"

//a variant of our Rower -> 2 methods (visit and done)
//a visitor that talk to an empty DF and provides new rows to it (~9:20)

//while done() == false, we keep calling visit which fills a row then that row is added to the current DF

class Writer : public Object {
public:
    Writer() { }

    ~Writer() { }
    
    virtual void visit(Row& r) {

    }

    virtual bool done() {
        
    }
};