#pragma once
#include <string>
#include <cassert>
#include "object.h"
#include "../serial/serial.h"


/** An immutable string class that wraps a character array.
 * The character array is zero terminated. The size() of the
 * String does count the terminator character. Most operations
 * work by copy, but there are exceptions (this is mostly to support
 * large strings and avoid them being copied).
 *  author: vitekj@me.com 
 *  modified by broder.c@husky.neu.edu */
class String : public Object {
public:
    std::string inner_;

    /** Build a string from a string constant */
    String(char const* cstr, size_t len) : inner_(cstr, len) {}

    /** Builds a string from a char*, steal must be true, we do not copy!
     *  cstr must be allocated for len+1 and must be zero terminated. */
    String(bool steal, char* cstr, size_t len) : inner_(cstr, len) {}

    String(char const* cstr) : String(cstr, strlen(cstr)) {}

    /** Build a string from another String */
    String(const String& from):
        Object(from) {
        inner_.assign(from.inner_);
        inner_.shrink_to_fit();
    }

    /** Delete the string */
    ~String() {}
    
    /** Return the number characters in the string (does not count the terminator) */
    size_t size() { return inner_.size(); }
    
    /** Return the raw char*. The result should not be modified or freed. */
    const char* c_str() {
        return inner_.c_str();
    }

    void serialize(Serializer* s) {
        s->write(size());
        s->write(c_str());
    }

    /** Deserialize as a String into this String*/
    void deserialize(Serializer* s) {
        inner_.clear();
        size_t size = s->readSizeT();
        char* cstr = s->readString();
        inner_.assign(cstr, size);
        inner_.shrink_to_fit();
        delete[] cstr;
    }
    
    /** Returns the character at index */
    char at(size_t index) {
        assert(index < size());
        return inner_[index];
    }
    
    /** Compare two strings. */
    bool equals(Object* other) {
        if (other == this) return true;
        String* x = dynamic_cast<String *>(other);
        if (x == nullptr) return false;
        return inner_.compare(x->inner_) == 0;
    }
    
    /** Deep copy of this string */
    String* clone() { return new String(*this); }

    /** This consumes cstr_, the String must be deleted next */
    char* steal() {
        size_t lenInner = size();
        char* out = new char[lenInner + 1];
        size_t len = inner_.copy(out, lenInner);
        assert(len == lenInner);
        out[len] = '\0';
        inner_.clear();
        return out;
    }

    /** Compute a hash for this string. */
    size_t hash_me() {
        std::hash<std::string> str_hash;
        return str_hash(inner_);
    }
 };

/** A string buffer builds a string from various pieces.
 *  author: jv */
class StrBuff : public Object {
public:
    std::string innerBuf_;

    StrBuff() {}

    StrBuff(const char* s)
    {
        innerBuf_.assign(s);
    }

    StrBuff(String* s)
    {
        innerBuf_.assign(s->c_str(), s->size());
    }

    StrBuff& c(const char* str)
    {
        return c(str, strlen(str));
    }

    /** Write character pointer to buffer, but specify how many bytes */
    StrBuff& c(const char* str, size_t step)
    {
        innerBuf_.append(str, step);
        return *this;
    }

    StrBuff& c(String &s) { return c(s.c_str());  }
    StrBuff& c(size_t v) { return c(std::to_string(v).c_str());  }

    /** Return this string. Caller responsible for deleting this result */
    String* get()
    {
        // consider changing to make the returned value actual steal ownership, not copy
        // String *res = new String(true, innerBuf_.c_str(), innerBuf_.size());
        String *res = new String(innerBuf_.c_str(), innerBuf_.size());
        innerBuf_.clear();
        return res;
    }

    /** Return size of buffer */
    size_t size()
    {
        return innerBuf_.size();
    }
};
