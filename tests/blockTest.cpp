#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/dataframe/block/block.h"
#include "../src/dataframe/block/intblock.h"
#include "../src/dataframe/block/doubleblock.h"
#include "../src/dataframe/block/boolblock.h"
#include "../src/dataframe/block/stringblock.h"
#include "../src/utils/args.h"

#include <assert.h>

Arguments args;

void intBlockTest() {
    printf("Int Block test starting...\n");
	IntBlock* ib = new IntBlock();
	size_t cap = ib->capacity();
	assert(cap > 3);
	ib->add(12);
	ib->add(345);
	ib->add(6789);
	assert(ib->size() == 3);
	assert(ib->get(0) == 12);
	assert(ib->get(1) == 345);
	assert(ib->get(2) == 6789);
	assert(ib->set(1, 99) == 345);
	assert(ib->get(1) == 99);
	assert(ib->size() == 3);
	assert(ib->get(2) == 6789);
	assert(ib->set(1, 345) == 99);
	IntBlock* ibc = ib->clone();
	assert(ibc->size() == ib->size());
	assert(ibc->equals(ib));
	assert(ib->equals(ibc));
	IntBlock* ib_myc = new IntBlock();
	ib_myc->add(12);
	ib_myc->add(345);
	ib_myc->add(6789);
	assert(ib->equals(ib_myc));
	ib->clear();
	assert(ib->size() == 0);
	assert(ib->capacity() == cap);
	assert(ibc->size() == 3);
	assert(!ibc->equals(ib));
	delete ib_myc;
	delete ibc;
	delete ib;
	printf("Int Block test passed!\n");
}

void stringBlockTest() {
    printf("String Block test starting...\n");
	String* s1 = new String("S1");
	String* s1c = s1->clone();
	String* s2 = new String("S2");
	String* s2c = s2->clone();
	String* s3 = new String("S3");
	String* s3c = s3->clone();
	String* s4 = new String("Df354");
	String* s4c = s4->clone();
	String* s5 = new String("Samsung");
	StringBlock* sb = new StringBlock();
	size_t cap = sb->capacity();
	assert(cap > 4);
	sb->add(s1); //steals ownership
	sb->add(s2);
	sb->add(s3);
	sb->add(s4);
	assert(sb->size() == 4);
	assert(sb->get(0)->equals(s1));
	assert(sb->get(1)->equals(s2));
	assert(sb->get(2)->equals(s3));
	assert(sb->get(3)->equals(s4));
	assert(sb->set(2, s5) == s3);
	assert(sb->get(2)->equals(s5));
	assert(sb->size() == 4);
	assert(sb->get(3)->equals(s4));
	delete sb->set(2, s3);  // deletes s5
	StringBlock* sbc = sb->clone();
	assert(sbc->size() == sb->size());
	assert(sbc->equals(sb));
	assert(sb->equals(sbc));
	StringBlock* sb_myc = new StringBlock();
	sb_myc->add(s1c); // steals ownership
	sb_myc->add(s2c);
	sb_myc->add(s3c);
	sb_myc->add(s4c);
	assert(sb->equals(sb_myc));
	sb->clear();
	assert(sb->size() == 0);
	assert(sb->capacity() == cap);
	assert(sbc->size() == 4);
	assert(!sbc->equals(sb));
	delete sb_myc;
	delete sbc;
	delete sb;
	printf("String Block test passed!\n");
}

void doubleBlockTest() {
    printf("Double Block test starting...\n");
	DoubleBlock* db = new DoubleBlock();
	size_t cap = db->capacity();
	assert(cap > 5);
	db->add(184.5);
	db->add(0.00001);
	db->add(999999.0);
	db->add(-124.644);
	assert(db->size() == 4);
	double dblTol = 0.00000001;
	assert(abs(db->get(0) - 184.5) < dblTol);
	assert(abs(db->get(1) - 0.00001) < dblTol);
	assert(abs(db->get(2) - 999999.0) < dblTol);
	assert(abs(db->get(3) - (-124.644)) < dblTol);
	double oldV = db->set(2, 77.123);
	assert(abs(db->get(2) - 77.123) < dblTol);
	assert(abs(oldV - 999999.0) < dblTol);
	assert(abs(db->get(3) - (-124.644)) < dblTol);
	db->set(2, 999999.0);
	DoubleBlock* dbc = db->clone();
	assert(dbc->size() == db->size());
	assert(dbc->equals(db));
	assert(db->equals(dbc));
	DoubleBlock* db_myc = new DoubleBlock();
	db_myc->add(184.5);
	db_myc->add(0.00001);
	db_myc->add(999999.0);
	db_myc->add(-124.644);
	assert(db->equals(db_myc));
	db->clear();
	assert(db->size() == 0);
	assert(db->capacity() == cap);
	assert(dbc->size() == 4);
	assert(!dbc->equals(db));
	delete db_myc;
	delete dbc;
	delete db;
	printf("Double Block test passed!\n");
}

void boolBlockTest() {
    printf("Bool Block test starting...\n");
	BoolBlock* bb = new BoolBlock();
	size_t cap = bb->capacity();
	assert(cap > 5);
	bb->add(true);
	bb->add(false);
	bb->add(false);
	bb->add(true);
	bb->add(true);
	assert(bb->size() == 5);
	assert(bb->get(0) == true);
	assert(bb->get(1) == false);
	assert(bb->get(2) == false);
	assert(bb->get(3) == true);
	assert(bb->get(4) == true);
	assert(bb->set(3, false) == true);
	assert(bb->get(3) == false);
	assert(bb->size() == 5);
	assert(bb->get(4) == true);
	bb->set(3, true);
	BoolBlock* bbc = bb->clone();
	assert(bbc->size() == bb->size());
	assert(bbc->equals(bb));
	assert(bb->equals(bbc));
	BoolBlock* bb_myc = new BoolBlock();
	bb_myc->add(true);
	bb_myc->add(false);
	bb_myc->add(false);
	bb_myc->add(true);
	bb_myc->add(true);
	assert(bb->equals(bb_myc));
	bb->clear();
	assert(bb->size() == 0);
	assert(bb->capacity() == cap);
	assert(bbc->size() == 5);
	assert(!bbc->equals(bb));
	delete bb_myc;
	delete bbc;
	delete bb;
	printf("Bool Block test passed!\n");
}

int main(int argc, char** argv)
{
	args.parse(argc, argv);
    args.print();
	
    printf("Block tests starting...\n");
	intBlockTest();
	stringBlockTest();
	doubleBlockTest();
	boolBlockTest();
    printf("Block tests passed!\n");
}