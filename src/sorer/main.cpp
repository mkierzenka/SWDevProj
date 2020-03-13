//lang::Cpp


/**
 * sorer: This program reads a file passed in with -f option.
 *
 * Optionally, -from tells you the location where you start the read in the file
 *             -len tells you how many bytes to read from the file
 *             -print_col_type asks for the type of the col indicated
 *             -print_col_idx asks for the value of the field at the col, idx
 *             -is_missing_idx asks whether the field at col, idx is an empty/missing value
 *
 * Unless, -print_col_type, print_col_idx, is_missing_idx options are used, nothing will happen.
 *
 * If those options are used, a -f must be included.
 *
 * If no -len or no -from options are given, we assume the whole file must be read.
 */


#include <iostream>
#include <cassert>
#include <cstdio>
#include <string.h>
#include <stdio.h>


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "sorer.h"


const char *USAGE = "Usage: ./sorer [-f] [-from] [-len] [-print_col_type] " \
             "[-print_col_idx] [-is_missing_idx]\n" \
             "\n" \
             "\t-f [filename] must be the first argument\n" \
             "\t-from [uint] must come after -f option, if used\n" \
             "\t-len [uint] must come after -f option, and if -from is used, after -from\n" \
             "\t only one of -print_col_type [uint] / -print_col_idx [uint] [uint] / -is_missing_idx [uint] [uint] can be used\n" \
             "\n" \
             "Only one option of each kind can be used.\n";


int main(int argc, char **argv) {
    Sorer* s = new Sorer(argv[1]);
    DataFrame* d = s->getFrame();

    assert(d->ncols() == 5);
    //Num rows does not update properly (makes sense)
    //printf("NUM ROWS: %zu\n", d->nrows());
    //assert(d->nrows() == 3);
    assert(d->get_int(0, 0) == 4);
    assert(d->get_bool(1, 0) == true);
    assert(d->get_float(2, 0) - 2.7 < 0.00001);
    printf("%s\n", d->get_string(3, 0)->c_str());
    printf("%s\n", "\"facts\"");
    assert(strcmp(d->get_string(3, 0)->c_str(), "\"facts\"\n") == 0);
    assert(d->get_int(4, 1) == 7);

    printf("Dataframe successfully transferred!\n");

    delete s;
    delete d;
}