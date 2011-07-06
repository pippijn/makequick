#include "phase.h"

#include "foreach.h"

struct default_destdir
  : visitor
{
  void visit (t_target_definition &n);
  void visit (t_program &n);
  void visit (t_library &n);

  t_destination_ptr bindir;
  t_destination_ptr libdir;

  enum visit_state
  {
    S_NONE,
    S_PROGRAM,
    S_LIBRARY
  } state;

  static t_destination_ptr make_dest (char const *dir)
  {
    return new t_destination (location::generated, new token (location::generated, TK_IDENTIFIER, "bin"));
  }

  default_destdir (annotation_map &annots)
    : bindir (make_dest ("bin"))
    , libdir (make_dest ("lib"))
  {
  }
};

static phase<default_destdir> thisphase ("default_destdir", "audit");


void
default_destdir::visit (t_target_definition &n)
{
  if (!n.dest ())
    {
      if (state == S_PROGRAM)
        n.dest (bindir);
      else if (state == S_LIBRARY)
        n.dest (libdir);
    }
}

void
default_destdir::visit (t_program &n)
{
  local (state) = S_PROGRAM;
  visitor::visit (n);
}

void
default_destdir::visit (t_library &n)
{
  local (state) = S_LIBRARY;
  visitor::visit (n);
}
