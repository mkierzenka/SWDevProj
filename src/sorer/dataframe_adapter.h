//lang:CPP

#pragma once

#include "../utils/object.h"
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

        //assuming # of rows is what first frame array indicates: will check
        for (int r = 0; r < fa[0]->len(); r++)
        {
            Row* row = new Row(*s);
            for (int c = 0; c < df->ncols(); c++)
            {
                size_t field_start = fa[c]->get_start(r);
                size_t field_end = fa[c]->get_end(r);

                switch (typ->get(c))
                {
                case BOOL:
                    row->set(c, get_bool_field(file, field_start, field_end));
                    break;
                case INT:
                    row->set(c, get_int_field(file, field_start, field_end));
                    break;
                case FLOAT:
                    row->set(c, get_float_field(file, field_start, field_end));
                    break;
                case STRING:
                    row->set(c, get_string_field(file, field_start, field_end));
                    break;
                default:
                    fprintf(stderr, "Column type not supported");
                };
            }

            //add row to df
            df->add_row(*row);

            delete row;
        }

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

        if (file[new_start] == '\"')
        {
            new_start += 1;
        }

        if (file[new_end] == '\"')
        {
            new_end -= 1;
        }

        char* strBuff = new char[2];
        strBuff[0] = '\0';
        for (size_t i = new_start; i <= new_end; ++i)
        {
            strBuff[0] = file[i];
            strBuff[1] = '\0';
            str->c(strBuff);
        }

        delete[] strBuff;

        if (file[new_start] != '\"' && file[new_end] != '\"')
        {
            str->c("");
        }

        String *res = str->get();
        delete str;
        return res;
    }
};
