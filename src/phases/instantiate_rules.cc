#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/rule_info.h"
#include "annotations/target_objects.h"
#include "colours.h"
#include "foreach.h"

#include <boost/filesystem/path.hpp>

using annotations::error_log;
using annotations::rule_info;
using annotations::target_objects;

struct instantiate_rules
  : visitor
{
  void instantiate (rule_info::rule const &r);
  template<typename PrereqT>
  void instantiate (t_target_definition &n, std::vector<PrereqT> const &targets, bool accept_existing);

  virtual void visit (t_target_members &n);
  virtual void visit (t_target_definition &n);
  virtual void visit (t_library &n);
  virtual void visit (t_program &n);

  enum target_type
  {
    T_NONE,
    T_LIBRARY,
    T_PROGRAM,
  } target;

  error_log &errors;
  rule_info const &rules;
  target_objects const &objs;
  t_target_members_ptr members;

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

static phase<instantiate_rules> thisphase ("instantiate_rules", "infer_target_objects", "inference", "resolve_wildcards", "default_prereq");

struct resolve_stem
  : visitor
{
  token_ptr stem;
  bool replace;

  explicit resolve_stem (std::string const &stem)
    : stem (new token (location::generated, TK_FILENAME, stem))
    , replace (false)
  {
  }

  node_ptr const &operator () (node_ptr const &p) { p->accept (*this); return p; }

  virtual void visit (t_shortvar &n)
  {
    if (n.var ()->as<token> ().string[0] == '*')
      replace = true;
  }

  virtual void visit (t_rule_line &n)
  {
    foreach (node_ptr &p, n.list)
      {
        resume (p);
        if (replace)
          {
            phases::run ("print", p);
            p = stem;
            replace = false;
          }
      }
  }
};

static node_ptr
make_prereq (std::vector<std::string> const &files)
{
  t_filenames_ptr prereq = new t_filenames (location::generated);
  foreach (std::string const &file, files)
    prereq->add (new t_filename (location::generated,
                   new token (location::generated,
                     TK_FILENAME, file)));
  return prereq;
}

void
instantiate_rules::instantiate (rule_info::rule const &r)
{
  // if r.code is NULL, this is an import rule
  if (r.code)
    {
      node_ptr target = (new t_filenames (location::generated))->add (
                           new t_filename (location::generated,
                             new token (location::generated,
                               TK_FILENAME, r.target)));
      node_ptr prereq = make_prereq (r.prereq);

      t_rule_ptr rule = new t_rule (location::generated, target, prereq,
                                    resolve_stem (r.stem) (r.code->clone ()));

      members->add (rule);
    }
}


static std::string const &native (fs::path    const &path) { return path.native (); }
static std::string const &native (std::string const &path) { return path;           }

template<typename PrereqT>
void
instantiate_rules::instantiate (t_target_definition &n,
                                std::vector<PrereqT> const &targets,
                                bool accept_existing)
{
  foreach (PrereqT const &obj, targets)
    {
      if (std::find (rules.files.begin (),
                     rules.files.end (),
                     obj) == rules.files.end ())
        {
          errors.add<semantic_error> (&n, "no rule to build " + C::filename (obj));
          continue;
        }

      rule_info::rule const *r = rules.find (native (obj));
      // no rule, but file found => file already exists
      if (!r)
        {
          if (!accept_existing)
            errors.add<semantic_error> (&n, "found file " + C::filename (obj)
                                          + " in source directory without rule to rebuild it");
          continue;
        }

      instantiate (*r);

      instantiate (n, r->prereq, true);
    }
}


void
instantiate_rules::visit (t_target_members &n)
{
  members = &n;
}

void
instantiate_rules::visit (t_target_definition &n)
{
  visitor::visit (n);

  std::string name = n.name ()->as<token> ().string;
  if (target == T_LIBRARY)
    name = "lib" + name + ".la";

  target_objects::target_map::const_iterator target = objs.targets.find (name);
  assert (target != objs.targets.end ());

  instantiate (n, target->second, false);
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
