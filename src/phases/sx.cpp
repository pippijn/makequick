#include "phase.h"

#include "util/foreach.h"

struct sx
  : visitor
{
  virtual void visit (token &n);
  virtual void visit (generic_node &n);

  int in_multifile;

  sx (annotation_map &annots) : in_multifile (0) { }
  ~sx () { puts (""); }
};

static phase<sx> thisphase ("sx", noauto);


static int indent;

static std::string
escape (std::string const &s)
{
  std::string x;
  foreach (char c, s)
    switch (c)
      {
      case '"' : x += "\\\""; break;
      case '\t': x += "\\t" ; break;
      case '\n': x += "\\n" ; break;
      case '\v': x += "\\v" ; break;
      case '\f': x += "\\f" ; break;
      default  : x += c     ; break;
      }
  return x;
}

static std::string
sxname (char const *s)
{
  return '|' + std::string (s + 1, strlen (s) - 2) + '|';
}

#define CYAN "\e[0;36m"
#define GREEN "\e[0;32m"
#define WHITE "\e[1;37m"
#define YELLOW "\e[0;33m"
#define R "\e[0m"

void
sx::visit (token &n)
{
  printf ("\n%*s(%s", indent, "", sxname (tokname (n.tok)).c_str ());
  if (n.loc.first_line)
    printf (" (loc " YELLOW "%d:%d" R " " YELLOW "%d:%d" R ")",
            n.loc.first_line, n.loc.first_column,
            n.loc.last_line, n.loc.last_column);
  else
    printf (" (loc " GREEN "generated" R ")");
  printf (" \"" WHITE "%s" R "\")", escape (n.string).c_str ());
}

void 
sx::visit (generic_node &n)
{
  printf ("\n%*s(" CYAN "%s" R, indent, "", node_type_name[n.type]);
  if (n.loc.first_line)
    printf (" (loc " YELLOW "%d:%d" R " " YELLOW "%d:%d" R ")",
            n.loc.first_line, n.loc.first_column,
            n.loc.last_line, n.loc.last_column);
  else
    printf (" (loc " GREEN "generated" R ")");
  indent += 2;
  foreach (node_ptr const &p, n.list)
    if (p)
      p->accept (*this);
    else
      printf ("\n%*s" CYAN "#NIL" R, indent, "");
  indent -= 2;
  printf (")");
}
