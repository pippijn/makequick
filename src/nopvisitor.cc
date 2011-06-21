#include "nopvisitor.h"
#include "node.h"
#include "ylcode.h"

#include <cstdio>

#include <boost/foreach.hpp>

void 
nopvisitor::visit (nodes::document &n)
{
  n.defs->accept (*this);
}

void 
nopvisitor::visit (nodes::definition_list &n)
{
  BOOST_FOREACH (node_ptr const &p, n.list)
    {
      p->accept (*this);
    }
}

void
nopvisitor::visit (nodes::token &n)
{
  printf ("%-16s: \"%s\"\n", tokname (n.tok - 255), n.string.c_str ());
}
