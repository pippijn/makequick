#include "phase.h"

#include <algorithm>

struct remove_patrules
  : visitor
{
  virtual void visit (t_rule &n);

  remove_patrules (annotation_map &annots)
  {
  }
};

static phase<remove_patrules> thisphase ("remove_patrules", "inference");

static bool
not_filename (node_ptr const &p)
{
  return p->as<token> ().tok != TK_FILENAME;
}

void
remove_patrules::visit (t_rule &n)
{
  t_filenames &target = n.target ()->as<t_filenames> ();
  t_filename &filename = target[0]->as<t_filename> ();
  if (target.size () > 1
      || filename.size () > 1
      || find_if (filename.list.begin (), filename.list.end (), not_filename)
         != filename.list.end ())
    n.unlink ();
}
