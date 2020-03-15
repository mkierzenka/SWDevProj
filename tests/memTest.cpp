#include "../src/utils/string.h"
#include "../src/utils/object.h"
#include "../src/utils/helper.h"

#include "../src/dataframe/schema.h"
#include "../src/dataframe/dataframe.h"
#include <assert.h>


int main() {
	Sys system;
	Schema* scm = new Schema("IFBS");
	String* str = new String("Hello World!");
	int num = 617;
	bool b = true;
	float f = 1.2f;
	Row* r = new Row(*scm);
	r->set(0, num);
	r->set(1, f);
	r->set(2, true);
	r->set(3, str);
	DataFrame* df = new DataFrame(*scm);
	
	df->add_row(*r);
	assert(df->get_int(0,0) == num);
	assert(df->get_float(1,0) == f);
	assert(df->get_bool(2,0) == b);
	assert(df->get_string(3,0)->equals(str));

	
	delete df;
	delete r;
	delete str;
	delete scm;
	system.pln("It worked!");
	return 0;
}