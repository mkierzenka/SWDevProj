//lang:CPP

#pragma once

#include "../utils/object.h"
#include "../utils/string.h"
#include "sorer_helper.h"
#include "sorwriter.h"
#include "../dataframe/dataframe.h"
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
    DataFrame *convertToFrame(FieldArray **fa, char *file, TypesArray *typ)
    {
        //initialize new dataframe
        char *schemaTypes = getTypes_(typ);
        Key* dfKey = new Key("df", 0); //df steals ownership of this key; don't delete
        KVStore* store = new KVStore(0, nullptr);

        SorWriter* sw = new SorWriter(fa, typ, file);

        DataFrame* df = DataFrame::fromVisitor(dfKey, store, schemaTypes, sw);

        delete[] schemaTypes;

        return df;
    }

    //get character representation of schema types from the types array
    char *getTypes_(TypesArray *types)
    {
        //buffer for storing all the types
        char* typeBuf = new char[types->len()+1];
        typeBuf[0] = '\0';
        for (int i = 0; i < types->len(); i++)
        {
            char typ = toChar_(types->get(i));
            typeBuf[i] = typ;
            typeBuf[i+1] = '\0';
        }

        return typeBuf;
    }

    /**convert Type to character */
    char toChar_(Types t)
    {
        switch (t)
        {
        case BOOL:
            return 'B';
        case INT:
            return 'I';
        case DOUBLE:
            return 'D';
        case STRING:
            return 'S';
        default:
            fprintf(stderr, "Column type not supported");
        };
    }

    // int get_int_field(char *file, int start, int end)
    // {
    //     // remove empty spaces in front
    //     size_t new_start = triml(file, start, end);

    //     return strtoll(&file[new_start], nullptr, 10);
    // }

    // bool get_bool_field(char *file, int start, int end)
    // {
    //     // remove empty spaces in front
    //     size_t new_start = triml(file, start, end);

    //     //return file[new_start];

    //     return (file[new_start] == '1') ? true : false;
    // }

    // double get_double_field(char *file, int start, int end)
    // {
    //     // remove empty spaces in front
    //     size_t new_start = triml(file, start, end);

    //     return strtold(&file[new_start], nullptr);
    // }

    // String *get_string_field(char *file, int start, int end)
    // {
    //     StrBuff *str = new StrBuff();
    //     // remove empty spaces in front and back
    //     size_t new_start = triml(file, start, end);
    //     size_t new_end = trimr(file, start, end);

    //     if (file[new_start] == '\"')
    //     {
    //         new_start += 1;
    //     }

    //     if (file[new_end] == '\"')
    //     {
    //         new_end -= 1;
    //     }

    //     char* strBuff = new char[2];
    //     strBuff[0] = '\0';
    //     for (size_t i = new_start; i <= new_end; ++i)
    //     {
    //         strBuff[0] = file[i];
    //         strBuff[1] = '\0';
    //         str->c(strBuff);
    //     }

    //     delete[] strBuff;

    //     if (file[new_start] != '\"' && file[new_end] != '\"')
    //     {
    //         str->c("");
    //     }

    //     String *res = str->get();
    //     delete str;
    //     return res;
    // }
};
