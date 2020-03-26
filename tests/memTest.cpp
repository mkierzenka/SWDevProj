#include "../src/utils/string.h"
#include "../src/utils/object.h"
#include "../src/utils/helper.h"

#include "../src/dataframe/schema.h"
#include "../src/dataframe/dataframe.h"
#include "../src/store/key.h"
#include <assert.h>

/**
 * NOTICE: This test depends on DataFrame.add_row() -> DataFrame.push_back()
 * 			Need to implement that method for it to work.
 */
int main() {
	Sys system;
	Schema* scm = new Schema("IDBS");
	String* str = new String("Hello World!");
	int num = 617;
	bool b = true;
	double d = 1.2;
	Row* r = new Row(*scm);
	r->set(0, num);
	r->set(1, d);
	r->set(2, true);
	r->set(3, str);
	Key k("df", 0);
	DataFrame* df = new DataFrame(*scm, &k);
	
	df->add_row(*r);
	assert(df->get_int(0,0) == num);
	assert(df->get_double(1,0) == d);
	assert(df->get_bool(2,0) == b);
	assert(df->get_string(3,0)->equals(str));
	
	delete df;
	delete r;
	delete str;
	delete scm;
	system.pln("It worked!");
	return 0;
}