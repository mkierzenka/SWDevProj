#pragma once

#include "dataframe.h"
#include "schema.h"
#include "../sorer/sorer.h"
#include "../sorer/sorwriter.h"
#include "../sorer/field_array.h"

/** This file will include the static "from" methods that create dataframes. Moved into
 * this file to reduce size of dataframe class */
class DataToDf
{
public:
    /**
  * Creates a new DataFrame from with given schema (scm) from the Writer.
  * Returns the df result, caller is responsible for deleting it.
  */
    static DataFrame *fromFile(const char *filename, Key *k, KVStore *kv)
    {
        Sorer sor(filename);
        FieldArray **fa = sor.getColumnar();
        Schema *schem = sor.getTypes(); //sorer owns this schema, so don't delete
        SorWriter *sw = new SorWriter(fa, schem, sor.getFileName());
        char *typesChar = schem->getTypesChar();
        DataFrame *df = fromVisitor(k, kv, typesChar, sw);
        delete sw;
        //length of field array array be number of columns
        for (size_t i = 0; i < schem->width(); i++)
        {
            delete fa[i];
        }

        delete[] fa;
        delete[] typesChar;
        return df;
    }

    /**
  * Creates a new DataFrame with given schema (scm) from the Writer. Adds it to
  * the distributed KV store.
  * Caller responsible for deleting scm, k, w.
  * Returns the DataFrame result, caller is responsible for deleting it.
  */
    static DataFrame *fromVisitor(Key *k, KVStore *kv, const char *scm, Writer *w)
    {
        DataFrame *df = new DataFrame(scm, k->clone(), kv);
        df->visit(w);
        addDFToStore_(df, kv);
        return df;
    }

    /**
   * Constructs a new DataFrame from an array of doubles (added as a single column).
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k and elems.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, double *elems)
    {
        DataFrame *df = new DataFrame(k->clone(), kv);
        df->add_array(numElems, elems);
        addDFToStore_(df, kv);
        return df;
    }

    /**
   * Constructs a new DataFrame from an array of integers (added as a single column).
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k and elems.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, int *elems)
    {
        DataFrame *df = new DataFrame(k->clone(), kv);
        df->add_array(numElems, elems);
        return addDFToStore_(df, kv);
    }

    /**
   * Constructs a new DataFrame from an array of booleans (added as a single column).
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k and elems.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, bool *elems)
    {
        DataFrame *df = new DataFrame(k->clone(), kv);
        df->add_array(numElems, elems);
        return addDFToStore_(df, kv);
    }

    /**
   * Constructs a new DataFrame from an array of Strings (added as a single column).
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k and elems.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromArray(Key *k, KVStore *kv, size_t numElems, String **elems)
    {
        DataFrame *df = new DataFrame(k->clone(), kv);
        df->add_array(numElems, elems);
        addDFToStore_(df, kv);
        return df;
    }

    /**
   * Constructs a new DataFrame from a single double.
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromDouble(Key *k, KVStore *kv, double elem)
    {
        double *elements = new double[1];
        elements[0] = elem;
        DataFrame *df = fromArray(k, kv, 1, elements);
        delete[] elements;
        return df;
    }

    /**
   * Constructs a new DataFrame from a single String*.
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k and elem.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromString(Key *k, KVStore *kv, String *elem)
    {
        String **elements = new String *[1];
        elements[0] = elem->clone();
        DataFrame *df = fromArray(k, kv, 1, elements);
        delete elements[0];
        delete[] elements;
        return df;
    }

    /**
   * Constructs a new DataFrame from a single integer.
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromInt(Key *k, KVStore *kv, int elem)
    {
        int *elements = new int[1];
        elements[0] = elem;
        DataFrame *df = fromArray(k, kv, 1, elements);
        delete[] elements;
        return df;
    }

    /**
   * Constructs a new DataFrame from a single boolean.
   * Adds the new DataFrame into the distributed KV store.
   * Caller responsible for deleting k.
   * Returns a local copy of the DataFrame result, caller is responsible for deleting it.
   */
    static DataFrame *fromBool(Key *k, KVStore *kv, bool elem)
    {
        bool *elements = new bool[1];
        elements[0] = elem;
        DataFrame *df = fromArray(k, kv, 1, elements);
        delete[] elements;
        return df;
    }

     /** Adds the DataFrame to the given KVStore under df's key.
   *
   *  Returns the DataFrame just added (df).
   */
  static DataFrame *addDFToStore_(DataFrame *df, KVStore *kv)
  {
    Serializer s;
    df->serialize(&s);
    Value *v = new Value(s.getBuffer(), s.getNumBytesWritten());
    kv->put(df->get_key(), v);
    delete v;
    return df;
  }
};