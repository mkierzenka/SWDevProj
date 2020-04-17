#pragma once

#include "datatype.h"

#include <stdio.h>
#include <stdlib.h>

/** Class involves static methods that don't involve state, such as switch statements
 * converting between char and datatype */
class DataTypeUtils
{
public:
    /** Convert char to data type
	 * NOTE: this logic is duplicated in column; maybe make static util file
	 */
    static DataType charToType(char c)
    {
        switch (c)
        {
        case 'I':
        case 'i':
            return DataType::Integer;
        case 'S':
        case 's':
            return DataType::Str;
        case 'B':
        case 'b':
            return DataType::Boolean;
        case 'D':
        case 'd':
            return DataType::Double;
        default:
            fprintf(stderr, "Unknown char data type %c\n", c);
            exit(-1);
        }
    }

    /** Return character for given type */
    static char typeToChar(DataType t)
    {
        switch (t)
        {
        case DataType::Integer:
            return 'I';
        case DataType::Str:
            return 'S';
        case DataType::Boolean:
            return 'B';
        case DataType::Double:
            return 'D';
        default:
            fprintf(stderr, "Unknown data type\n");
            exit(-1);
        }
    }
};