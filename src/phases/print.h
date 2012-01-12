#pragma once

struct print
  : visitor
{
#include "visitor_t.h"

  template<typename NodeT>
  void file_list (NodeT &n, char const *name);

  virtual void visit (token &n);

  bool in_rule;
  bool in_prereq;
  FILE *fh;

  print (FILE *fh)
    : in_rule (false)
    , in_prereq (false)
    , fh (fh)
  {
  }

  print (annotation_map &annots)
    : in_rule (false)
    , in_prereq (false)
    , fh (stdout)
  {
  }
};
