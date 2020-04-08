#include "../src/dataframe/column/columnarray.h"
#include "../src/dataframe/column/column.h"
#include "../src/store/kvstore.h"
#include "../src/store/key.h"
#include "../src/network/network.h"
#include "../src/utils/string.h"

#include <assert.h>

/** Tests adding columns to an array and making sure we can retrieve data. Data added to columns with
 * add_all, push_back not implemented
 */
void addColumnsTest()
{
    printf("Column array add columns test started\n");
    Network* net = new Network("127.0.0.1", 0);
    KVStore* store = new KVStore(0, net);
    Key* baseKey = new Key("hi", 0);
    ColumnArray* ca = new ColumnArray(store, baseKey);

    //Do not delete these columns; column array will own them
    Column* c1 = new Column(store, baseKey->addIndex(0), 'I');
    Column* c2 = new Column(store, baseKey->addIndex(1), ColType::Double);
    Column* c3 = new Column(store, baseKey->addIndex(2), ColType::Boolean);
    Column* c4 = new Column(store, baseKey->addIndex(3), 's');

    //have constant column length; not currently enforced by our program
    int colLen = 5;

    //add some data to the columns
    int* intData = new int[colLen];
    for (int i = 0; i < colLen; i++)
    {
        intData[i] = i;
    }
    c1->add_all(colLen, intData);

    double* doubleData = new double[colLen];
    for (int i = 0; i < colLen; i++)
    {
        doubleData[i] = i * 5.76;
    }
    c2->add_all(colLen, doubleData);

    bool* boolData = new bool[colLen];
    for (int i = 0; i < colLen; i++)
    {
        boolData[i] = (i % 2 == 0);
    }
    c3->add_all(colLen, boolData);

    String** stringData = new String*[colLen];
    stringData[0] = new String("this");
    stringData[1] = new String("is");
    stringData[2] = new String("a");
    stringData[3] = new String("fake");
    stringData[4] = new String("video");
    c4->add_all(colLen, stringData);

    //add columns to column array
    ca->add_column(c1);
    ca->add_column(c2);
    ca->add_column(c3);
    ca->add_column(c4);

    //Column equality
    Column* caCol = ca->get(1);
    assert(caCol->equals(c2));

    //Make sure column array can properly retrieve data and info
    assert(ca->length() == 4);
    assert(ca->get_int(0, 3) == 3);
    assert(ca->get_double(1, 2) - 11.52 < 0.0001);
    assert(!(ca->get_bool(2, 1)));
    assert(ca->get_string(3, 4)->equals(stringData[4]));

    delete baseKey;
    delete net;
    delete store;
    delete ca;
    delete[] intData;
    delete[] doubleData;
    delete[] boolData;
    for (int i = 0; i < colLen; i++)
    {
        delete stringData[i];
    }

    delete[] stringData;
    printf("Column array add columns test passed!\n");
}

/** Tests adding columns to an array by just using the char type succeeds. Doesn't test adding
 * elements to columns, because push_back not implemented
 */
void addColumnCharTest()
{
    //assert(false);
    printf("Column array add columns with char test started\n");
    Network* net = new Network("127.0.0.1", 0);
    KVStore* store = new KVStore(0, net);
    Key* baseKey = new Key("hi", 0);
    ColumnArray* ca = new ColumnArray(store, baseKey);

    //add a few columns based on character
    ca->add_column('i');
    ca->add_column('S');
    ca->add_column('s');
    ca->add_column('B');
    ca->add_column('D');

    assert(ca->length() == 5);
    Column* mtBoolCol = new Column(store, baseKey->addIndex(3), ColType::Boolean);
    assert(ca->get(3)->equals(mtBoolCol));

    delete baseKey;
    delete net;
    delete store;
    delete ca;
    delete mtBoolCol;
    printf("Column array add columns with char test passed!\n");
}

void addColumnsFromArrayTest()
{
    printf("Column array add columns from array test started\n");

    Network* net = new Network("127.0.0.1", 1);
    KVStore* store = new KVStore(1, net);
    Key* baseKey = new Key("base", 1);
    ColumnArray* ca = new ColumnArray(store, baseKey);

    //have constant column length; not currently enforced by our program
    int colLen = 7;

    double* doubleData = new double[colLen];
    for (int i = 0; i < colLen; i++)
    {
        doubleData[i] = i * 0.01;
    }
    ca->add_column_fromarray(colLen, doubleData);

    //add some data to the columns
    int* intData = new int[colLen];
    for (int i = 0; i < colLen; i++)
    {
        intData[i] = colLen - i;
    }
    ca->add_column_fromarray(colLen, intData);

    String** stringData = new String*[colLen];
    stringData[0] = new String("i");
    stringData[1] = new String("am");
    stringData[2] = new String("testing");
    stringData[3] = new String("the");
    stringData[4] = new String("column");
    stringData[5] = new String("array");
    stringData[6] = new String("implementation");
    ca->add_column_fromarray(colLen, stringData);

    bool* boolData = new bool[colLen];
    for (int i = 0; i < colLen; i++)
    {
        boolData[i] = (i % 2 == 1);
    }
    ca->add_column_fromarray(colLen, boolData);

    //add same column twice
    ca->add_column_fromarray(colLen, doubleData);

    //check for column equality
    Column* caCol = ca->get(0);
    Column* colComp = new Column(store, baseKey->addIndex(0), ColType::Double);
    colComp->add_all(colLen, doubleData);
    assert(caCol->equals(colComp));

    //check for proper data retrieval from column array
    assert(ca->length() == 5);
    assert(ca->get_double(0, 3) - 0.03 < 0.0001);
    assert(ca->get_int(1, 1) == 6);
    String* strCompare = new String("array");
    assert(ca->get_string(2, 5)->equals(strCompare));
    assert(ca->get_bool(3, 3));
    assert(ca->get_double(4, 4) - 0.04 < 0.0001);

    delete baseKey;
    delete net;
    delete store;
    delete ca;
    delete colComp;
    delete[] intData;
    delete[] doubleData;
    delete[] boolData;
    delete strCompare;
    for (int i = 0; i < colLen; i++)
    {
        delete stringData[i];
    }

    delete[] stringData;

    printf("Column array add columns from array test passed!\n");
}

/** Tests fields and methods of column array class */
int main()
{
    addColumnsTest();
    addColumnCharTest();
    addColumnsFromArrayTest();
}