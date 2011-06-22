#include "phase.h"

#include <algorithm>
#include <cstdio>

#include <boost/foreach.hpp>

#define VALID_XML 1

namespace
{
  struct nopvisitor
    : visitor
  {
    virtual void visit (token &n);
    virtual void visit (generic_node &n);
  };

  static phase<nopvisitor> thisphase ("nop");
}

static int indent;

static char
xmltranslate (char c)
{
  if (isalnum (c))
    return c;
  return '_';
}

static std::string
xmlname (char const *p)
{
  std::string s (p + 1, strlen (p) - 2);
  std::transform (s.begin (), s.end (), s.begin (), xmltranslate);
  return s;
}

void
nopvisitor::visit (token &n)
{
#if VALID_XML
  std::string const name = xmlname (tokname (n.tok - 255));
  printf ("%*s<t:%s><![CDATA[%s]]></t:%s>\n", indent, "", name.c_str (), n.string.c_str (), name.c_str ());
#else
  printf ("%*s%s: \"%s\"\n", indent, "", tokname (n.tok - 255), n.string.c_str ());
#endif
}

void 
nopvisitor::visit (generic_node &n)
{
#if VALID_XML
  printf ("%*s<n:%s>\n", indent, "", n.name);
#else
  printf ("%*s<%s>\n", indent, "", n.name);
#endif
  indent += 2;
  BOOST_FOREACH (node_ptr const &p, n.list)
    if (p)
      p->accept (*this);
  indent -= 2;
#if VALID_XML
  printf ("%*s</n:%s>\n", indent, "", n.name);
#else
  printf ("%*s</%s>\n", indent, "", n.name);
#endif
}
