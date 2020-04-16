#include <assert.h>
#include "../src/network/nodeinfo.h"

Sys SYSTEM;

void test1() {
	String str1("127.0.0.1");
	size_t port1 = 8080;
	String str2("127.0.0.2");
	size_t port2 = 7000;

	NodeInfo n1(&str1, port1);
	assert(n1.equals(&n1));
	assert(strcmp(n1.getIPAddr(), "127.0.0.1") == 0);
	assert(n1.getPort() == port1);

	NodeInfo* n1c = n1.clone();
	NodeInfo n3(&str1, port2); //n1 but different port
	NodeInfo n4(&str2, port1); //n1 but different ip
	assert(n1c->equals(n1c));
	assert(n1c->equals(&n1));
	assert(!(n1.equals(&n3)));
	assert(!(n1.equals(&n4)));
	Serializer s;
	n1.serialize(&s);
	NodeInfo n1_s;
	n1_s.deserialize(&s);
	assert(n1.equals(&n1_s));
	delete n1c;
}

int main()
{
	SYSTEM.pln("NodeInfo tests started...");
	test1();
	SYSTEM.pln("NodeInfo tests passed!");
	return 0;
}
