#include "phase.h"

#include "annotations/target_objects.h"
#include "colours.h"
#include "foreach.h"

#include <boost/filesystem/path.hpp>

using annotations::target_objects;

struct infer_target_objects
  : visitor
{
  virtual void visit (t_filename &n);
  virtual void visit (t_sources &n);

  virtual void visit (t_target_definition &n);
  virtual void visit (t_library &n);
  virtual void visit (t_program &n);

  enum target_type
  {
    T_NONE,
    T_LIBRARY,
    T_PROGRAM,
  } target;

  enum parse_state
  {
    S_NONE,
    S_SOURCES
  } state;

  std::vector<fs::path> objects;
  target_objects &objs;

  infer_target_objects (annotation_map &annots)
    : state (S_NONE)
    , objs (annots.put ("target_objects", new target_objects))
  {
  }

#if 0
  ~infer_target_objects ()
  {
    objs.print ();
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<infer_target_objects> thisphase ("infer_target_objects", "resolve_sources");


void
infer_target_objects::visit (t_filename &n)
{
  if (state == S_SOURCES)
    {
      fs::path path (n[0]->as<token> ().string);
      switch (target)
        {
        case T_LIBRARY:
          objects.push_back (path.filename ().replace_extension (".lo"));
          break;
        case T_PROGRAM:
          objects.push_back (path.filename ().replace_extension (".o"));
          break;
        default:
          throw std::runtime_error ("invalid sources container");
        }
    }
}

void
infer_target_objects::visit (t_sources &n)
{
  local (state) = S_SOURCES;
  visitor::visit (n);
}


void
infer_target_objects::visit (t_target_definition &n)
{
  assert (objects.empty ());
  visitor::visit (n);
  assert (!objects.empty ());

  std::string name = n.name ()->as<token> ().string;

  if (target == T_LIBRARY)
    name = "lib" + name + ".la";

  assert (objs.targets.find (name) == objs.targets.end ());

  swap (objs.targets[name], objects);
}

void
infer_target_objects::visit (t_library &n)
{
  local (target) = T_LIBRARY;
  visitor::visit (n);
}

void
infer_target_objects::visit (t_program &n)
{
  local (target) = T_PROGRAM;
  visitor::visit (n);
}
