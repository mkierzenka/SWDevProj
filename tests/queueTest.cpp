#include "../src/utils/helper.h" // Your file, with any C++ code that you need
#include "../src/utils/object.h" // Your file with the CwC declaration of Object
#include "../src/utils/string.h" // Your file with the String class
#include "../src/utils/queue.h"  // Your file with the two list classes

void FAIL() { exit(1); }
void OK(const char *m)
{
	Sys *c = new Sys();
	c->p("OK: ");
	c->pln(m);
}
void t_true(bool p)
{
	if (!p)
		FAIL();
}
void t_false(bool p)
{
	if (p)
		FAIL();
}

void test_queue_push_object()
{
	Object *s = new Object();
	Object *t = new Object();
	Object *u = new Object();
	Queue *q1 = new Queue();

	q1->push(s);
	t_true(q1->size() == 1);
	q1->push(t);
	t_true(q1->size() == 2);
	q1->push(u);
	t_true(q1->size() == 3);
	OK("push Object");
}

void test_queue_push_string()
{
	String *s = new String("Hello");
	String *t = new String("World");
	String *u = new String("Bye");
	Queue *q1 = new Queue();

	q1->push(s);
	t_true(q1->size() == 1);
	q1->push(t);
	t_true(q1->size() == 2);
	q1->push(u);
	t_true(q1->size() == 3);
	OK("push string");
}

void test_queue_pop_object()
{
	Object *s = new Object();
	Object *t = new Object();
	Object *u = new Object();
	Queue *q1 = new Queue();

	q1->push(s);
	q1->push(t);
	q1->push(u);
	t_true(q1->size() == 3);
	t_true(q1->pop() == s);
	t_true(q1->size() == 2);
	t_true(q1->pop() == t);
	t_true(q1->pop() == u);
	t_true(q1->pop() == nullptr);
	OK("pop Object");
}

void test_queue_pop_object2()
{
	Object *s = new Object();
	Object *t = new Object();
	Object *u = new Object();
	Queue *q1 = new Queue();

	q1->push(s);
	t_true(q1->size() == 1);
	t_true(q1->pop() == s);
	t_true(q1->size() == 0);
	q1->push(t);
	q1->push(u);
	t_true(q1->size() == 2);
	t_true(q1->pop() == t);
	OK("pop Object 2");
}

void test_queue_pop_string()
{
	String *s = new String("Hello");
	String *t = new String("World");
	String *u = new String("Bye");
	Queue *q1 = new Queue();

	q1->push(s);
	q1->push(t);
	q1->push(u);
	t_true(q1->size() == 3);
	t_true(q1->pop() == s);
	t_true(q1->size() == 2);

	q1->push(s);
	t_true(q1->pop() == t);
	t_true(q1->size() == 2);
	t_true(q1->pop() == u);
	t_true(q1->pop() == s);
	t_true(q1->pop() == nullptr);
	OK("pop string");
}

void test_queue_is_empty()
{
	String *s = new String("Hello");
	String *t = new String("World");
	String *u = new String("Bye");

	Object *s1 = new Object();
	Object *t1 = new Object();
	Object *u1 = new Object();
	Queue *q1 = new Queue();

	Queue *q2 = new Queue();

	t_true(q1->is_empty() == true);
	t_true(q2->is_empty() == true);
	q1->push(s);
	q1->push(t);
	q1->push(u);

	q2->push(s1);
	q2->push(t1);
	q2->push(u1);
	t_true(q1->is_empty() == false);
	t_true(q2->is_empty() == false);
	OK("is empty");
}

void test_queue_clear()
{
	String *s = new String("Hello");
	String *t = new String("World");
	String *u = new String("Bye");

	Object *s1 = new Object();
	Object *t1 = new Object();
	Object *u1 = new Object();
	Queue *q1 = new Queue();
	Queue *q2 = new Queue();

	q1->push(s);
	q1->push(t);
	q1->push(u);

	q2->push(s1);
	q2->push(t1);
	q2->push(u1);
	t_true(q1->size() == 3);
	t_true(q2->size() == 3);
	q1->clear();
	q2->clear();
	t_true(q1->size() == 0);
	t_true(q2->size() == 0);
	OK("clear");
}

void test_queue_equals_object()
{
	Object *s = new Object();
	Object *t = new Object();
	Object *u = new Object();

	Queue *q1 = new Queue();
	Queue *q2 = new Queue();

	t_true(q1->equals(q2));

	q1->push(s);
	q1->push(t);
	q1->push(u);

	q2->push(s);
	q2->push(t);
	q2->push(u);
	t_true(q1->size() == 3);
	t_true(q2->size() == 3);
	t_true(q1->equals(q2));
	t_true(q2->equals(q1));
	q1->clear();
	t_false(q1->equals(q2));
	OK("Object queue equals");
}

void test_queue_equals_string()
{
	String *s = new String("Hello");
	String *t = new String("World");
	String *u = new String("Bye");

	Queue *q1 = new Queue();
	Queue *q2 = new Queue();

	t_true(q1->equals(q2));

	q1->push(s);
	q1->push(t);
	q1->push(u);

	q2->push(s);
	q2->push(t);
	q2->push(u);
	t_true(q1->size() == 3);
	t_true(q2->size() == 3);
	t_true(q1->equals(q2));
	t_true(q2->equals(q1));
	q1->clear();
	t_false(q1->equals(q2));
	OK("string queue equals");
}

int main()
{
	test_queue_push_object();
	test_queue_push_string();
	test_queue_pop_object();
	test_queue_pop_object2();
	test_queue_pop_string();
	test_queue_is_empty();
	test_queue_clear();
	test_queue_equals_object();
	test_queue_equals_string();
	OK("All queue tests passed!");
	return 0;
}