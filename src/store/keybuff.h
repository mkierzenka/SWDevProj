#pragma once

#include "key.h"
#include "../utils/object.h"
#include "../utils/string.h"

/**
 * A class analogous to StringBuffer but for Keys. The Key is owned.
 * Provided as part of Milestone 4.
 *
 */
class KeyBuff : public Object {                                                  
  public:                                                                        
  Key* orig_; // owned                                                        
  StrBuff buf_;                                                                  

  /** Caller should delete passed in key */                                                                               
  KeyBuff(Key* orig) : orig_(orig->clone()), buf_(orig->getKeyStr()) {}
                 
  ~KeyBuff()
  {
    delete orig_;
  }

  // Methods that append to the key.				 
  KeyBuff& c(String &s) { buf_.c(s); return *this;  }                            
  KeyBuff& c(size_t v) { buf_.c(v); return *this; }                              
  KeyBuff& c(const char* v) { buf_.c(v); return *this; }                         
                   
  /** Get the key we've constructed in this buffer. Caller responsible
   * for deleting key */  
  Key* get() {                                                                   
    String* s = buf_.get();                                                      
    char* bufCstr = s->steal();
    buf_.c(orig_->c_str());                                                      
    Key* k = new Key(bufCstr, orig_->getNode());
    delete s;                                                                    
    delete[] bufCstr;
    return k;                                                                    
  }                                                                              
}; // KeyBuff 