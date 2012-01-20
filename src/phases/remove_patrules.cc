#include "phase.h"

struct remove_patrules
  : visitor
{
  virtual void visit (t_rule &n);

  remove_patrules (annotation_map &annots)
  {
  }
};

static phase<remove_patrules> thisphase ("remove_patrules", "inference");

void
remove_patrules::visit (t_rule &n)
{
  t_filenames &target = n.target ()->as<t_filenames> ();
  if (target.size () > 1
      || target[0]->as<t_filename> ().size () > 1)
    n.unlink ();
}
