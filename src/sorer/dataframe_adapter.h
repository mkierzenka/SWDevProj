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
        printf("Schema types: %s\n", schemaTypes);
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
                    df->push_back(c, get_bool_field(file, field_start, field_end));
                    break;
                case INT:
                    df->push_back(c, get_int_field(file, field_start, field_end));
                    break;
                case FLOAT:
                    df->push_back(c, get_float_field(file, field_start, field_end));
                    break;
                case STRING:
                    df->push_back(c, get_string_field(file, field_start, field_end));
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
        printf("SCHEMA LENGTH: %zu\n", types->len());
        //buffer for storing all the types
        //StrBuff *typeBuf = new StrBuff();
        char* typeBuf = new char[types->len()+1];
        typeBuf[0] = '\0';
        printf("BUFFER: %s\n", typeBuf);
        for (int i = 0; i < types->len(); i++)
        {
            char typ = toChar_(types->get(i));
            typeBuf[i] = typ;
            typeBuf[i+1] = '\0';
            printf("Adding char for column %d: %c\n", i, typ);
            //typeBuf->c(&typ);
            //typeBuf->c(type);
            printf("Type buffer: %s\n", typeBuf);
        }


        //String *res = typeBuf->get();
        //delete typeBuf;

        //printf("SCHEMA STRING %s\n", res->c_str());
        //return res->c_str();
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

        char* strBuff = new char[2];
        strBuff[0] = '\0';
        for (size_t i = new_start; i <= new_end; ++i)
        {
            strBuff[0] = file[i];
            strBuff[1] = '\0';
            str->c(strBuff);
            //str->c((char*)file[i]);
        }

        delete[] strBuff;

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
