#include "phase.h"

#include "annotations/symbol_table.h"
#include "annotations/rule_info.h"
#include "annotations/target_objects.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"
#include "util/object_name.h"

#include <boost/filesystem/path.hpp>

struct infer_target_objects
  : symbol_visitor
{
  virtual void visit (t_filename &n);
  virtual void visit (t_sources &n);
  virtual void visit (t_nodist_sources &n);

  virtual void visit (t_target_definition &n);
  virtual void visit (t_library &n);
  virtual void visit (t_program &n);
  virtual void visit (t_template &n) { /* ignore */ }

  symbol_type target;

  enum parse_state
  {
    S_NONE,
    S_SOURCES
  } state;

  file_vec objects;
  target_objects &objs;
  rule_info &rules;
  std::string const large_stem;

  infer_target_objects (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , state (S_NONE)
    , objs (annots.put ("target_objects", new target_objects))
    , rules (annots.get<rule_info> ("rule_info"))
    , large_stem (1024, '_')
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


static bool
in_root (fs::path const &path)
{
  return *path.native ().rbegin () == 'y'
      || *path.native ().rbegin () == 'z'
      || *path.native ().rbegin () == 'l'
      ;
}

void
infer_target_objects::visit (t_filename &n)
{
  if (state == S_SOURCES)
    {
      fs::path path (id (n[0]));
      std::string const &target_name = id (symtab.lookup<t_target_definition> (target, "TARGET").name ());
      std::string const &root_o = object_name (target, target_name, path);
      std::string const &subdir_o = object_name (target, path);

      objects.push_back (root_o);

      file_vec prereq;
      if (in_root (path))
        prereq.push_back (fs::path (subdir_o).filename ());
      else
        prereq.push_back (subdir_o);

      rules.files.insert (root_o);
      rules.rules.insert (rule_info::rule (root_o, prereq, std::string (), NULL));
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
  symbol_visitor::visit (n);
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
  symbol_visitor::visit (n);
}
