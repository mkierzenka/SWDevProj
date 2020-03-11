#pragma once

#include "object.h"
#include "string.h"

/** This union represents a value of a row element. It can be one of the four data types */
union RowValue {
    int iElem;
    float fElem;
    bool bElem;
    String *sElem;
};

/** This class represents an element in a row. It includes a union of one of the four data types, and a
boolean of if the element has already been set in the row. Its functionality includes setting the value,
getting value of a certain type, and returning whether or not it has been set */
class RowElement : public Object
{
public:
    RowValue *val_; //value of element
    bool set_;      //has the element been set within the row

    RowElement()
    {
        val_ = new RowValue();
        set_ = false;
    }

    ~RowElement()
    {
        delete val_;
    }

    void set(int val)
    {
        val_->iElem = val;
        set_ = true;
    }

    void set(float val)
    {
        val_->fElem = val;
        set_ = true;
    }

    void set(bool val)
    {
        val_->bElem = val;
        set_ = true;
    }

    void set(String *val)
    {
        val_->sElem = val->clone();
        set_ = true;
    }

    int getInt()
    {
        return val_->iElem;
    }

    bool getBool()
    {
        return val_->bElem;
    }

    float getFloat()
    {
        return val_->fElem;
    }

    String* getString()
    {
        return val_->sElem;
    }

    bool isSet()
    {
        return set_;
    }
};