#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/rule_info.h"
#include "annotations/target_objects.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/make_filename.h"

#include <boost/filesystem/path.hpp>

struct instantiate_rules
  : visitor
{
  void instantiate (rule_info::rule const &r);
  void instantiate (t_target_definition &n, file_vec const &targets, bool accept_existing);

  virtual void visit (t_target_members &n);
  virtual void visit (t_toplevel_declarations &n);

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

  error_log &errors;
  rule_info const &rules;
  target_objects const &objs;
  generic_node_ptr members;

  instantiate_rules (annotation_map &annots)
    : errors (annots.get ("errors"))
    , rules (annots.get ("rule_info"))
    , objs (annots.get ("target_objects"))
  {
  }

#if 0
  ~instantiate_rules ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<instantiate_rules> thisphase ("instantiate_rules",
                                           "infer_target_objects",
                                           "inference",
                                           "resolve_wildcards",
                                           "default_prereq");

struct resolve_stem
  : visitor
{
  std::string const &stem;
  bool replace;

  explicit resolve_stem (std::string const &stem)
    : stem (stem)
    , replace (false)
  {
  }

  node_ptr const &operator () (node_ptr const &p) { p->accept (*this); return p; }

  virtual void visit (t_shortvar &n)
  {
    if (id (n.var ()) == "*")
      replace = true;
  }

  virtual void visit (t_rule_line &n)
  {
    foreach (node_ptr &p, n.list)
      {
        resume (p);
        if (replace)
          {
            n.replace (*p, new token (location::generated, TK_CODE, stem));
            replace = false;
          }
      }
  }
};

static node_ptr
make_prereq (std::vector<fs::path> const &files)
{
  t_filenames_ptr prereq = new t_filenames (location::generated);
  foreach (fs::path const &file, files)
    prereq->add (make_filename (file.native ()));
  return prereq;
}

void
instantiate_rules::instantiate (rule_info::rule const &r)
{
  // if r.code is NULL, this is an import rule
  if (r.code)
    {
      node_ptr target = (new t_filenames (location::generated))->add (
                           make_filename (r.target));
      node_ptr prereq = make_prereq (r.prereq);

      t_rule_ptr rule = new t_rule (location::generated, target, prereq,
                                    resolve_stem (r.stem) (r.code->clone ()));

      members->add (rule);
    }
}


void
instantiate_rules::instantiate (t_target_definition &n,
                                file_vec const &targets,
                                bool accept_existing = false)
{
  foreach (fs::path const &obj, targets)
    {
      //printf ("rule for %s\n", obj.c_str ());
      if (rules.files.find (obj) == rules.files.end ())
        {
          errors.add<semantic_error> (&n, "no rule to build " + C::filename (obj));
          continue;
        }

      rule_info::rule const *r = rules.find (obj.native ());
      // no rule, but file found => file already exists
      if (!r)
        {
          if (!accept_existing)
            errors.add<semantic_error> (&n, "found file " + C::filename (obj)
                                          + " in source directory without rule to rebuild it");
          continue;
        }

      assert (r->target == obj);

      instantiate (*r);

      instantiate (n, r->prereq, true);
    }
}


void
instantiate_rules::visit (t_target_members &n)
{
  assert (!members);
  members = &n;
}

void
instantiate_rules::visit (t_toplevel_declarations &n)
{
  assert (!members);
  members = &n;
  visitor::visit (n);
}


void
instantiate_rules::visit (t_target_definition &n)
{
  local (members) = NULL;

  visitor::visit (n);

  std::string name = id (n.name ());
  if (target == T_LIBRARY)
    name = "lib" + name + ".la";

  target_objects::target_map::const_iterator target = objs.targets.find (name);
  assert (target != objs.targets.end ());

  instantiate (n, target->second);
}

void
instantiate_rules::visit (t_library &n)
{
  local (target) = T_LIBRARY;
  visitor::visit (n);
}

void
instantiate_rules::visit (t_program &n)
{
  local (target) = T_PROGRAM;
  visitor::visit (n);
}
