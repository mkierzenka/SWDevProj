#pragma once

#include "../utils/object.h"
#include "../utils/string.h"

#include <stddef.h>
#include <math.h>
#include <stdarg.h>



/**************************************************************************
 * ColumnMetadata ::
 * Represents metadata about a column of a data frame.
 * Specifically, its name (can be nullptr), its type, and the number of elements.
 * Pointer equality. */
class ColumnMetadata : public Object
{
public:
    String* name_;    //the name of this column, owned
	char type_;    //the type of this column
    
	ColumnMetadata() {
		name_ = nullptr;
		type_ = 0;
	}
	
	// no checks on type, copies name (can be nullptr)
	ColumnMetadata(String* name, char type) {
		name_ = (name == nullptr) ? name : name->clone();
		type_ = type;
	}
	
	// copy constructor. doesn't take ownership
	ColumnMetadata(ColumnMetadata* c2) {
		String* newName = c2->getName();
		name_ = (newName == nullptr) ? newName : newName->clone();
		type_ = c2->getType();
	}
	
	~ColumnMetadata() {
		delete name_;
	}

	// return old name. clones name
	String* setName(String* newName) {
		String* oldName = name_;
		name_ = (newName == nullptr) ? newName : newName->clone();
		return oldName;
	}
	
	String* getName() {
		return name_;
	}

	char getType() {
		return type_;
	}

	// sets type, no checks. return old type
	char setType(char newType) {
		char oldType = type_;
		type_ = newType;
		return oldType;
	}
};