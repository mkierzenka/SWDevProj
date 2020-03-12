#include "../src/utils/string.h"
#include "../src/utils/object.h"
#include "../src/utils/helper.h"

#include "../src/dataframe/schema.h"
#include "../src/dataframe/dataframe.h"

int main() {
	Sys system();
	Schema scm("IFBS");
	String str("Hello World!");
	int num = 617;
	int b = 0;
	float f = 1.2f;
	Row r(scm);
	r.set(0, num);
	r.set(1, f);
	r.set(2, b);
	r.set(3, &str);
	DataFrame* df = new DataFrame(scm);
	
	df->add_row(r);
	
	
	
	delete df;
	return 0;
}