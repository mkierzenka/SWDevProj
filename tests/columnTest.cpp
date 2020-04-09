#include <assert.h>
#include "../src/dataframe/column/column.h"
#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/store/value.h"
#include "../src/utils/string.h"

Sys SYSTEM;

void columnTests()
{
  KVStore* store = new KVStore(0, nullptr);
  Key* baseKeyI = new Key("baseInts", 0);
  int numsI[] = {1, 2, 3, 4};
  Column* ic = new Column(store, baseKeyI, ColType::Integer);
  ic->add_all(4, numsI);

  assert(ic->size() == 4);
  assert(ic->get_int(0) == numsI[0]);
  assert(ic->get_int(1) == numsI[1]);
  assert(ic->get_int(2) == numsI[2]);
  assert(ic->get_int(3) == numsI[3]);
  delete ic;

  //
  Key* baseKeyD = new Key("baseDbls", 0);
  double numsD[] = {4.5, 2.1, 0.0, 0.004};
  Column* dc = new Column(store, baseKeyD, ColType::Double);
  dc->add_all(4, numsD);

  assert(dc->size() == 4);
  assert(dc->get_double(0) == numsD[0]);
  assert(dc->get_double(1) == numsD[1]);
  assert(dc->get_double(2) == numsD[2]);
  assert(dc->get_double(3) == numsD[3]);
  delete dc;

  //
  Key* baseKeyB = new Key("baseBools", 0);
  bool valsB[] = {true, false, false, true};
  Column* bc = new Column(store, baseKeyB, ColType::Boolean);
  bc->add_all(4, valsB);

  assert(bc->size() == 4);
  assert(bc->get_bool(0) == valsB[0]);
  assert(bc->get_bool(1) == valsB[1]);
  assert(bc->get_bool(2) == valsB[2]);
  assert(bc->get_bool(3) == valsB[3]);
  delete bc;

  //
  Key* baseKeyS = new Key("baseStrs", 0);
  String *stringA = new String("Good");
  String *stringB = new String("Morning");
  String *stringC = new String("Graduation");
  String** valsS = new String*[4];
  valsS[0] = stringA;
  valsS[1] = stringC;
  valsS[2] = stringA;
  valsS[3] = stringB;

  Column* sc = new Column(store, baseKeyS, ColType::Str);
  sc->add_all(4, valsS);
 
  assert(sc->size() == 4);
  assert(sc->get_string(0)->equals(valsS[0]));
  assert(sc->get_string(1)->equals(valsS[1]));
  assert(sc->get_string(2)->equals(valsS[2]));
  assert(sc->get_string(3)->equals(valsS[3]));
  delete sc;
  
  delete store;
  delete stringA;
  delete stringB;
  delete stringC;
  delete[] valsS;
}

int main()
{
  SYSTEM.pln("Column tests started...");
  columnTests();
  SYSTEM.pln("Column tests passed!");
  return 0;
}
