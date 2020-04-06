//a variant of our Rower -> 2 methods (visit and done)
//a visitor that talk to an empty DF and provides new rows to it (~9:20)

//while done() == false, we keep calling visit which fills a row then that row is added to the current DF



#pragma once

#include "../dataframe/dataframe.h"
#include "../dataframe/rower.h"
#include "../dataframe/row.h"

class Writer : public Rower {
public:
    Writer(DataFrame* df) : Rower() { }

    virtual void visit(Row& r);

    virtual bool done();

}