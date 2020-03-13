//lang:CPP

#pragma once

#include "_object.h"
#include "helper.h"
#include "../dataframe/dataframe.h"
#include "../utils/string.h"

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
        Schema *s = new Schema(schemaTypes);
        DataFrame *df = new DataFrame(*s);

        //get data from the file and put it in the dataframe
        for (int c = 0; c < df->ncols(); c++)
        {
            for (int r = 0; r < fa[c]->len(); r++)
            {
                size_t field_start = fa[c]->get_start(r);
                size_t field_end = fa[c]->get_end(r);

                switch (typ->get(c))
                {
                case BOOL:
                    df->set(c, r, get_bool_field(file, field_start, field_end));
                    break;
                case INT:
                    df->set(c, r, get_int_field(file, field_start, field_end));
                    break;
                case FLOAT:
                    df->set(c, r, get_float_field(file, field_start, field_end));
                    break;
                case STRING:
                    df->set(c, r, get_string_field(file, field_start, field_end));
                    break;
                default:
                    fprintf(stderr, "Column type not supported");
                };
            }
        }

        return df;
    }

    //get character representation of schema types from the types array
    char *getTypes_(TypesArray *types)
    {
        //buffer for storing all the types
        StrBuff *typeBuf = new StrBuff();
        for (int i = 0; i < types->len(); i++)
        {
            char typ = toChar_(types->get(i));
            typeBuf->c(&typ);
        }

        String *res = typeBuf->get();
        delete typeBuf;

        return res->c_str();
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
        case FLOAT:
            return 'F';
        case STRING:
            return 'S';
        default:
            fprintf(stderr, "Column type not supported");
        };
    }

    int get_int_field(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        return strtoll(&file[new_start], nullptr, 10);
    }

    bool get_bool_field(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        //will this work?
        return file[new_start];
    }

    float get_float_field(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        return strtold(&file[new_start], nullptr);
    }

    String *get_string_field(char *file, int start, int end)
    {
        StrBuff *str = new StrBuff();
        // remove empty spaces in front and back
        size_t new_start = triml(file, start, end);
        size_t new_end = trimr(file, start, end);

        if (file[new_start] != '\"' && file[new_end] != '\"')
        {
            str->c("");
        }

        for (size_t i = new_start; i <= new_end; ++i)
        {
            str->c(&file[i]);
        }

        if (file[new_start] != '\"' && file[new_end] != '\"')
        {
            str->c("");
        }

        str->c("\n");

        String *res = str->get();
        delete str;
        return res;
    }
};
