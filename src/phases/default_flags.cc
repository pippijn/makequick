#include "phase.h"

struct default_flags
  : visitor
{
  void visit (t_toplevel_declarations &n);

  default_flags (annotation_map &annots)
  {
  }
};

static phase<default_flags> thisphase ("default_flags");


static node_ptr
make_flag (char const *flag)
{
  location const &loc = location::generated;
  return new t_flag (loc, new token (loc, TK_FLAG, flag));
}

void
default_flags::visit (t_toplevel_declarations &n)
{
  location const &loc = location::generated;
  n.add (new t_tool_flags (loc,
           new token (loc, TK_FLAGS_ID, "cppflags"),
           NULL,
           (new t_flags (loc))
             ->add (make_flag ("-I$(top_srcdir) -I$(top_srcdir)/include"))
             ->add (make_flag ("-I$(top_builddir) -I$(top_builddir)/include"))));
}
