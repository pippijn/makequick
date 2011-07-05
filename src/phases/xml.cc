#include "phase.h"

#include "foreach.h"

#include <algorithm>

#define VALID_XML 0

namespace
{
  struct xml
    : visitor
  {
    virtual void visit (token &n);
    virtual void visit (generic_node &n);

    int in_multifile;

    xml (annotation_map &annots) : in_multifile (0) { }
  };

  static phase<xml> thisphase ("xml", noauto);
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
  transform (s.begin (), s.end (), s.begin (), xmltranslate);
  return s;
}

void
xml::visit (token &n)
{
#if VALID_XML
  std::string const name = xmlname (tokname (n.tok));
  printf ("%*s<t:%s><![CDATA[%s]]></t:%s>\n", indent, "", name.c_str (), n.string.c_str (), name.c_str ());
#else
  printf ("%*s%s[%d:%d-%d:%d]: \"%s\"\n", indent, "", tokname (n.tok),
          n.loc.first_line, n.loc.first_column,
          n.loc.last_line, n.loc.last_column,
          n.string.c_str ());
#endif
}

void 
xml::visit (generic_node &n)
{
#if VALID_XML
  printf ("%*s<n:%s>\n", indent, "", node_type_name[n.type]);
#else
  printf ("%*s<%s s='%d:%d' e='%d:%d'>\n", indent, "", node_type_name[n.type],
          n.loc.first_line, n.loc.first_column,
          n.loc.last_line, n.loc.last_column);
#endif
  indent += 2;
  foreach (node_ptr const &p, n.list)
    if (p)
      p->accept (*this);
  indent -= 2;
#if VALID_XML
  printf ("%*s</n:%s>\n", indent, "", node_type_name[n.type]);
#else
  printf ("%*s</%s>\n", indent, "", node_type_name[n.type]);
#endif
}
