//lang::Cpp

#include <iostream>
#include <cassert>
#include <cstdio>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "../src/sorer/sorer.h"
#include "../src/dataframe/row/lengthrower.h"
#include "../src/dataframe/column/column.h"


const char *USAGE = "Usage: ./sorer [-f] [-from] [-len] [-print_col_type] " \
             "[-print_col_idx] [-is_missing_idx]\n" \
             "\n" \
             "\t-f [filename] must be the first argument\n" \
             "\t-from [uint] must come after -f option, if used\n" \
             "\t-len [uint] must come after -f option, and if -from is used, after -from\n" \
             "\t only one of -print_col_type [uint] / -print_col_idx [uint] [uint] / -is_missing_idx [uint] [uint] can be used\n" \
             "\n" \
             "Only one option of each kind can be used.\n";


/** Entrypoint for sorer program. Sorer used to read in data and then convert
 * it to a DataFrame */
int main(int argc, char **argv) {
	puts("Sorer test started...");
    Sorer* s = new Sorer(argv[1]);
    DataFrame* d = s->getFrame();

    //some checks to make sure our dataframe reading worked properly
    assert(d->ncols() == 12);
    assert(d->nrows() == 1000);
    assert(d->get_int(0, 0) == -1437879);
    assert(d->get_bool(1, 0) == false);
    assert(d->get_bool(1, 1) == true);
    assert(d->get_double(3, 0) - 1040.435425 < 0.00001);
    assert(strcmp(d->get_string(2, 0)->c_str(), "inflexive") == 0);
    assert(strcmp(d->get_string(2, 1)->c_str(), "abdominal's") == 0);
    assert(strcmp(d->get_string(2, 2)->c_str(), "discourtesy's") == 0);
    assert(d->get_int(4, 1) == -1924364);
    assert(strcmp(d->get_string(6, 143)->c_str(), "cornworms") == 0);

    puts("Dataframe successfully transferred!");

    LengthRower* lr = new LengthRower(d);
    d->map(*lr);
	assert(lr->getLen() == 60631); //check rower is correct

	delete lr;
    delete d;
    delete s;
	puts("Sorer test passed!");
}