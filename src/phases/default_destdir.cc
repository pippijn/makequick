#include "phase.h"

struct default_destdir
  : visitor
{
  void visit (t_target_definition &n);
  void visit (t_program &n);
  void visit (t_library &n);

  enum visit_state
  {
    S_NONE,
    S_PROGRAM,
    S_LIBRARY
  } state;

  static t_destination_ptr make_dest (char const *dir)
  {
    return new t_destination (location::generated, new token (location::generated, TK_IDENTIFIER, dir));
  }

  default_destdir (annotation_map &annots)
  {
  }
};

static phase<default_destdir> thisphase ("default_destdir", "inheritance");


void
default_destdir::visit (t_target_definition &n)
{
  if (!n.dest ())
    {
      if (state == S_PROGRAM)
        n.dest (make_dest ("bin"));
      else if (state == S_LIBRARY)
        n.dest (make_dest ("lib"));
    }
}

void
default_destdir::visit (t_program &n)
{
  state = S_PROGRAM;
  visitor::visit (n);
  state = S_NONE;
}

void
default_destdir::visit (t_library &n)
{
  state = S_LIBRARY;
  visitor::visit (n);
  state = S_NONE;
}
