#include "../src/utils/string.h"
#include "../src/utils/object.h"
#include "../src/utils/helper.h"

#include "../src/dataframe/schema.h"
#include "../src/dataframe/dataframe.h"
#include <assert.h>


int main() {
	Sys system;
	Schema scm("IFBS");
	String str("Hello World!");
	String str2("Hello World! 2");
	int num = 617;
	bool b = true;
	float f = 1.2f;
	Row r(scm);
	r.set(0, num);
	r.set(1, f);
	r.set(2, b);
	r.set(3, &str);
	DataFrame* df = new DataFrame(scm);
	
	df->add_row(r);
	assert(df->get_int(0,0) == num);
	assert(df->get_float(1,0) == f);
	assert(df->get_bool(2,0) == b);
	assert(df->get_string(3,0)->equals(&str));
	
	/*df->set(0,0,num - 1);
	df->set(1,0,f - 1);
	df->set(2,0,!b);
	df->set(3,0,&str2);
	
	assert(df->get_int(0,0) == num - 1);
	assert(df->get_float(1,0) == f - 1);
	assert(df->get_bool(2,0) == !b);
	assert(df->get_string(3,0)->equals(&str2));*/

	system.pln("It worked!");
	
	delete df;
	return 0;
}