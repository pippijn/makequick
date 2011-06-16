#pragma once

#include <string>
#include <vector>

#include <boost/intrusive_ptr.hpp>

#include "visitor.h"

struct YYLTYPE
{
  std::string const *file;
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

namespace nodes
{
  struct node
  {
    virtual void accept (visitor &v) = 0;
    node () : refcnt (0) { }
    virtual ~node () { }

    friend void intrusive_ptr_release (node *n);
    friend void intrusive_ptr_add_ref (node *n);

  private:
    int refcnt;
  };

  typedef boost::intrusive_ptr<node> node_ptr;

  struct node_list
    : node
  {
    void add (node *n) { list.push_back (n); }

    std::vector<node_ptr> list;
  };

  struct token
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
    token (int tok, char const *text, int leng)
      : tok (tok)
      , string (text, leng)
    {
    }

    int const tok;
    std::string const string;
  };


  struct document
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
    document (node *defs) : defs (defs) { }

    node_ptr defs;
  };


  struct definition_list
    : node_list
  {
    virtual void accept (visitor &v) { v.visit (*this); }
  };
}

using nodes::node;
using nodes::node_ptr;
