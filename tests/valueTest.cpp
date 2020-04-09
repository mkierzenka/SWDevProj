#include <assert.h>
#include <string.h>
#include "../src/utils/string.h"
#include "../src/store/value.h"

Sys SYSTEM;

void valueTest1() {
	const char* v1_str = "Value1";
	Value* v1 = new Value(v1_str, strlen(v1_str));
	assert(memcmp(v1->getData(), v1_str, strlen(v1_str)) == 0);
	assert(v1->getSize() == strlen(v1_str));
	assert(v1->equals(v1));
	char* tricky = new char[6];
	tricky[0] = 'a';
	tricky[1] = 'b';
	tricky[2] = 'c';
	tricky[3] = '\0';
	tricky[4] = 'D';
	tricky[5] = '\0';
	Value* v2 = new Value(tricky, 6);
	assert(memcmp(v1->getData(), v1_str, 6) == 0);
	assert(v1->getSize() == 6);
	assert(!v1->equals(v2));

	char* v1_str_fake = new char[8];
	strcpy(v1_str_fake, v1_str);
	v1_str_fake[6] = '\0';
	v1_str_fake[7] = 'M';
	Value* v1_fake = new Value(v1_str_fake, 8);
	assert(!v1->equals(v1_fake));
	assert(!v1_fake->equals(v1));

	delete v1_fake;
	delete[] v1_str_fake;
	delete v2;
	delete[] tricky;
	delete v1;
}

int main()
{
  SYSTEM.pln("Value test started...");
  valueTest1();
  SYSTEM.pln("Value test passed!");
  return 0;
}
