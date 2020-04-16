//lang:CPP

#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "../utils/datatype.h"
#include "sorer_helper.h"
#include "sorwriter.h"
#include "sorer.h"
#include "../dataframe/dataframe.h"
#include "../dataframe/schema.h"
#include "../store/kvstore.h"
#include "../store/key.h"

/** This class will be in charge of converting field arrays to
 * dataframes. It will take in a field array, and convert it to
 * a dataframe and return the result.
 * 
 * Authors: broder.c@husky.neu.edu and kierzenka.m@husky.neu.edu
 */
class DataFrameAdapter : public Object
{
public:
    static DataFrame *convertToFrame(const char* filename, Key* dfKey, KVStore* store)
    {
        Sorer sor(filename);
        SorWriter* sw = new SorWriter(sor.getColumnar(), sor.getTypes(), sor.getFileName());
        DataFrame* df = DataFrame::fromVisitor(dfKey, store, sor.getTypes()->getTypesChar(), sw);
        return df;
    }
};
