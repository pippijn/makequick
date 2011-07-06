#include "phase.h"

#include "foreach.h"

#include <algorithm>

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

#define CYAN "\e[0;36m"
#define GREEN "\e[0;32m"
#define WHITE "\e[1;37m"
#define YELLOW "\e[0;33m"
#define R "\e[0m"

void
xml::visit (token &n)
{
  printf ("%*s%s", indent, "", tokname (n.tok));
  if (n.loc.first_line)
    printf ("["YELLOW"%d:%d"R"-"YELLOW"%d:%d"R"]",
            n.loc.first_line, n.loc.first_column,
            n.loc.last_line, n.loc.last_column);
  printf (": "WHITE"\"%s\""R"\n", n.string.c_str ());
}

void 
xml::visit (generic_node &n)
{
  printf ("%*s<"CYAN"%s"R, indent, "", node_type_name[n.type]);
  if (n.loc.first_line)
    printf (" s='"YELLOW"%d:%d"R"' e='"YELLOW"%d:%d"R"'",
            n.loc.first_line, n.loc.first_column,
            n.loc.last_line, n.loc.last_column);
  else
    printf (" "GREEN"generated='true'"R);
  printf (">\n");
  indent += 2;
  foreach (node_ptr const &p, n.list)
    if (p)
      p->accept (*this);
  indent -= 2;
  printf ("%*s</"CYAN"%s"R">\n", indent, "", node_type_name[n.type]);
}
