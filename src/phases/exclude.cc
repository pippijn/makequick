#include "phase.h"

#include "util/ancestor.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/grep.h"

struct exclude
  : visitor
{
  virtual void visit (t_exclude &n);

  exclude (annotation_map &annots)
  {
  }
};

static phase<exclude> thisphase ("exclude", "resolve_sourcesref");


void
exclude::visit (t_exclude &n)
{
  t_target_members &target = *ancestor<t_target_members> (n);

  foreach (t_sources &src, grep<t_sources> (target))
    foreach (t_filename &filename, grep<t_filename> (src.sources ()))
      foreach (t_filename &exclude, grep<t_filename> (n.sources ()))
        if (id (filename[0]) == id (exclude[0]))
          {
            filename.unlink ();
            break;
          }

  n.unlink ();
}
