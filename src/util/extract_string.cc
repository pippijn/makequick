#include "util/extract_string.h"
#include "util/foreach.h"
#include "phase.h"

struct string_extractor
  : visitor
{
  virtual void visit (token &n)
  {
    s += n.string;
  }

  virtual void visit (generic_node &n)
  {
    resume_list ();
  }

  std::string s;
};

std::string
extract_string (node &n)
{
  string_extractor e;
  n.accept (e);
  return e.s;
}

std::string
extract_string (t_vardecl_body const &n)
{
  std::string body;
  foreach (node_ptr const &p, n.list)
    body += p->as<token> ().string;
  return body;
}
