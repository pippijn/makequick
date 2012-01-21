#include "phase.h"

#include "annotations/target_objects.h"
#include "util/colours.h"
#include "util/foreach.h"

#include <boost/filesystem/path.hpp>

struct infer_target_objects
  : visitor
{
  virtual void visit (t_filename &n);
  virtual void visit (t_sources &n);
  virtual void visit (t_nodist_sources &n);

  virtual void visit (t_target_definition &n);
  virtual void visit (t_library &n);
  virtual void visit (t_program &n);
  virtual void visit (t_template &n) { /* ignore */ }

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

  file_vec objects;
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
      fs::path path (id (n[0]));
      switch (target)
        {
        case T_LIBRARY:
          // XXX: this generates for %.lo: %.c, which is not what automake does
          // (unless subdir-rules is on)
          objects.push_back (path.replace_extension (".lo"));
          break;
        case T_PROGRAM:
          objects.push_back (path.replace_extension (".o"));
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
infer_target_objects::visit (t_nodist_sources &n)
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

  std::string name = id (n.name ());

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
