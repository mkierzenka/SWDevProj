#pragma once

#include "../dataframe/row/writer.h"
#include "../dataframe/row/row.h"
#include "../dataframe/schema.h"
#include "../utils/datatype.h"
#include "field_array.h"
#include "sorer_helper.h"

/** This writer will be used to fill up a dataframe from the sorer files. It will visit
 * each row at a time */
class SorWriter : public Writer
{
public:
    size_t row_;        //what row we're currently on
    size_t numCols_;    //total number of columns
    size_t numRows_;    //total number of rows
    FieldArray **fa_;   //array of values to iterate through; not owned
    Schema *types_; //type of each column; array not owned
    char *file_;        //file to read
    size_t seen_;       //how many elements have we seen

    SorWriter(FieldArray **fa, Schema *types, char *file) : Writer()
    {
        row_ = 0;
        types_ = types;
        numCols_ = types->width();
        assert(fa[0]); //make sure at least one entry
        numRows_ = fa[0]->len();
        fa_ = fa;
        file_ = file;
        seen_ = 0;
    }

    ~SorWriter()
    {
    }

    void next()
    {
        row_++;
    }

    void visit(Row &r) override
    {
        for (size_t col_ = 0; col_ < numCols_; col_++)
        {
            size_t field_start = fa_[col_]->get_start(row_);
            size_t field_end = fa_[col_]->get_end(row_);

            assert(field_start <= field_end);

            switch (types_->col_type(col_))
            {
            case DataType::Boolean:
                r.set(col_, get_bool_field_(file_, field_start, field_end));
                break;
            case DataType::Integer:
                r.set(col_, get_int_field_(file_, field_start, field_end));
                break;
            case DataType::Double:
                r.set(col_, get_double_field_(file_, field_start, field_end));
                break;
            case DataType::Str:
            {
                String *val = get_string_field_(file_, field_start, field_end);
                r.set(col_, val);
                delete val; //value gets cloned in the row
                break;
            }
            default:
                fprintf(stderr, "Column type not supported");
            };

            seen_++;
        }

        next();
    }

    bool done() override
    {
        return seen_ == numRows_ * numCols_;
    }

    /** Get the int value at the given location in the file */
    int get_int_field_(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        return strtoll(&file[new_start], nullptr, 10);
    }

    /** Get the bool value at the given location in the file */
    bool get_bool_field_(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        return (file[new_start] == '1') ? true : false;
    }

    /** Get the double value at the given location in the file */
    double get_double_field_(char *file, int start, int end)
    {
        // remove empty spaces in front
        size_t new_start = triml(file, start, end);

        return strtold(&file[new_start], nullptr);
    }

    /** Get the string value at the given location in the file */
    String *get_string_field_(char *file, int start, int end)
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

        char *strBuff = new char[2];
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
