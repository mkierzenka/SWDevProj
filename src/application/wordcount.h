//lang:Cpp
#pragma once

#include "application.h"
#include "../filereader/filereader.h"
#include "../filereader/summer.h"
#include "../filereader/adder.h"
#include "../dataframe/dataframe.h"
#include "../store/key.h"
#include "../store/keybuff.h"
#include "../network/inetwork.h"
#include "../utils/map.h"
#include "../utils/args.h"


/****************************************************************************
 * Calculate a word count for given file:
 *   1) read the data (single node)
 *   2) produce word counts per homed chunks, in parallel
 *   3) combine the results
 **********************************************************author: pmaj ****/
class WordCount : public Application
{
public:
  static const size_t BUFSIZE = 1024;
  Key in;
  Key bufKey;
  KeyBuff kbuf;
  SIMap all;
  DataFrame* df;

  WordCount(size_t idx, INetwork *net) : Application(idx, net), in("data", 0), bufKey("wc-m", 0), kbuf(&bufKey) {}

  /** The master nodes reads the input, then all of the nodes count. */
  void run_() override
  {
    if (idx_ == 0)
    {
      FileReader fr;
      delete DataToDf::fromVisitor(&in, kv_, "S", &fr);
    }
    local_count();
    reduce();
  }

  /** Returns a key for given node.  These keys are homed on master node
   *  which then joins them one by one. Caller responsible for deleting key */
  Key *mk_key(size_t idx)
  {
    Key *k = kbuf.c(idx).get();
    p("Created key ").pln(k->c_str());
    return k;
  }

  /** Compute word counts on the local node and build a data frame. */
  void local_count()
  {
    DataFrame *words = (kv_->waitAndGet(&in));
    p("Node ").p(idx_).pln(": starting local count...");
    SIMap map;
    Adder add(map);
    words->local_map(add);
    delete words;
    Summer cnt(map);
    Key* localKey = mk_key(idx_);
    delete DataToDf::fromVisitor(localKey, kv_, "SI", &cnt);
    delete localKey;
  }

  /** Merge the data frames of all nodes */
  void reduce()
  {
    if (idx_ != 0)
      return;
    pln("Node 0: reducing counts...");
    SIMap map;
    Key *own = mk_key(0);
    DataFrame *tmp = kv_->get(own);
    merge(tmp, map);
    for (size_t i = 1; i < args.numNodes; ++i)
    { // merge other nodes
      Key *ok = mk_key(i);
      merge(kv_->waitAndGet(ok), map);
      delete ok;
    }
    p("Different words: ").pln(map.size());
    delete own;
  }

  void merge(DataFrame *df, SIMap &m)
  {
    Adder add(m);
    df->map(add);
    delete df;
  }
}; // WordcountDemo