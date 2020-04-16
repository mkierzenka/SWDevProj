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
        char *schemaTypes = getTypes_(sor.getTypes());
        SorWriter* sw = new SorWriter(sor.getColumnar(), sor.getTypes(), sor.getFileName());
        DataFrame* df = DataFrame::fromVisitor(dfKey, store, schemaTypes, sw);
        delete[] schemaTypes;
        return df;
    }

    //get character representation of schema types from the types array
    static char *getTypes_(Schema *types)
    {
        //buffer for storing all the types
        char* typeBuf = new char[types->width()+1];
        typeBuf[0] = '\0';
        for (int i = 0; i < types->width(); i++)
        {
            char typ = types->col_type_char(i);
            typeBuf[i] = typ;
            typeBuf[i+1] = '\0';
        }

        return typeBuf;
    }

    /**convert Type to character */
    static char toChar_(DataType t)
    {
        switch (t)
        {
        case Boolean:
            return 'B';
        case Integer:
            return 'I';
        case Double:
            return 'D';
        case Str:
            return 'S';
        default:
            fprintf(stderr, "Column type not supported");
        };
    }
};
