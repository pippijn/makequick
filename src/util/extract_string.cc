#include "util/extract_string.h"
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
extract_string (node_ptr const &n)
{
  string_extractor e;
  n->accept (e);
  return e.s;
}
