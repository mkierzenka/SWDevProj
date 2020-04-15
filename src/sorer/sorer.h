//lang::CPP

#pragma once

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>

#include "sorer_helper.h"
//#include "dataframe_adapter.h"

#include "../utils/object.h"
//#include "../dataframe/dataframe.h"

/**
 * This class is a sorer implementation. It will take in a file name. It will read in
 * the file and convert it to a FieldArray object, representing the 
 * file's data columns and values.
 * 
 * Refactor of program written by cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu.
 */
class Sorer : public Object
{
    public:
        int fd; //file descriptor of file to be read
        size_t fileSize; //size of file to be read in
        char* file; //file to be read in
        TypesArray* schema; //inferred schema from file

        Sorer(const char* fileName)
        {
            fd = open(fileName, O_RDONLY);
            //make sure file exists
            assert(fd != -1);
            fileSize = getFileSize_(fileName);
            size_t mapLen = getMapLen_();
            file = (char*) mmap(nullptr, mapLen, PROT_READ, MAP_PRIVATE, fd, 0);
            schema = parse_schema(file);
        }

        ~Sorer()
        {
			munmap(file, getMapLen_());            
			close(fd);
			delete schema;
        }

        /** Return columnar form of the data. The length of the FieldArray is schema->len() */
        FieldArray** getColumnar()
        {
            //make column for whole file
            return make_columnar(file, 0, fileSize, schema);
        }

        /** Return types of this dataframe */
        TypesArray* getTypes()
        {
            return schema;
        }

        /** Return file name */
        char* getFileName()
        {
            return file;
        }

        /**
         * Return dataframe form of the data. Will produce the columnar, then
         * use an adapter to convert it to a dataframe
         */
        // DataFrame* getFrame()
        // {
        //     FieldArray** fa = getColumnar();
        //     DataFrameAdapter* dfa = new DataFrameAdapter();
        //     DataFrame* d = dfa->convertToFrame(fa, file, schema);
        //     delete dfa;
        //     for (int i = 0; i < schema->len(); i++)
        //     {
        //         delete fa[i];
        //     }
        //     delete[] fa;
        //     return d;
        // }

        /** Get file size of the read in file */
        size_t getFileSize_(const char* name)
        {
             // Use stat the get the file size
            struct stat st;
            stat(name, &st);
            return st.st_size;
        }

        /** Get length of data to map on */
        size_t getMapLen_()
        {
            size_t pageSize = getpagesize();
            size_t pages = (size_t) (fileSize / pageSize) + 1;
            return pages * pageSize;
        }



};