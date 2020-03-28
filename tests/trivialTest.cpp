//lang:Cpp

#include "../src/application/trivial.h"

///TODO: figure out where this method implementation should go
DataFrame* KVStore::get(Key *k)
{
        Value *val = getValue(k);
        //set up new serializer to deserialize returned data into a dataframe
        Serializer *s = new Serializer(val->getSize(), val->getData());

        //create new dataframe and mutate by deserializing
        DataFrame *d = new DataFrame(k, this);
        d->deserialize(s);

        //delete val?
        delete s;

        return d;
}

void testTrivialCase() {
  printf("Trivial case started\n");
  Trivial* tv = new Trivial(0);
  tv->run_();
  tv->run_2();
  tv->run_3();
  tv->run_4();

  printf("Trivial case passed!\n");
  delete tv;
}

//The purpose of this file is to test the Trivial case given to us in M2
int main()
{
  testTrivialCase();
  
  return 0;
}