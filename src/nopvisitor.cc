#include "nopvisitor.h"
#include "node.h"
#include "ylcode.h"

#include <cstdio>

#include <boost/foreach.hpp>

static int indent;

void 
nopvisitor::visit (nodes::generic_node &n)
{
  printf ("%*s<%s>\n", indent, "", n.name);
  indent += 2;
  BOOST_FOREACH (node_ptr const &p, n.list)
    if (p)
      p->accept (*this);
  indent -= 2;
  printf ("%*s</%s>\n", indent, "", n.name);
}

void
nopvisitor::visit (nodes::token &n)
{
  printf ("%*s%s: \"%s\"\n", indent, "", tokname (n.tok - 255), n.string.c_str ());
}
