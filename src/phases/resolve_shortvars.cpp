#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/make_code.h"
#include "util/make_filename.h"
#include "util/symbol_visitor.h"

#include <boost/filesystem/path.hpp>

struct resolve_shortvars
  : symbol_visitor
{
  virtual void visit (t_shortvar &n);
  virtual void visit (t_intvar &n);

  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);
  virtual void visit (t_log_compiler &n);

  error_log &errors;

  node_ptr replacement;
  t_filename_ptr target;
  t_filenames_ptr prereq;
  bool in_log_compiler;

  resolve_shortvars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
    , in_log_compiler (false)
  {
  }

#if 0
  ~resolve_shortvars ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<resolve_shortvars> thisphase ("resolve_shortvars", "instantiate_rules");

static std::string
filename (fs::path const &file)
{
  return file.filename ().native ();
}

static std::string
dirname (fs::path const &file)
{
  return file.parent_path ().native ();
}

static std::string
identity (fs::path const &file)
{
  return file.native ();
}

static std::string
add_builddir (std::string const &file, std::string const &orig, bool builddir)
{
  if (!builddir)
    return "`test -f '" + orig + "' || echo '$(srcdir)/'`" + file;
  return "$(builddir)/" + file;
}

static t_filename_ptr
modify (node_ptr const &n, char modifier, bool builddir)
{
  t_filename const &fn = n->as<t_filename> ();
  assert (fn.size () == 1);

  std::string (*pred) (fs::path const &)
    = modifier == 'F'
      ? filename
      : modifier == 'D'
        ? dirname
        : identity;
  assert (modifier != '\0' || pred == identity);

  fs::path orig (id (fn[0]));
  return make_filename (fn.loc, add_builddir (pred (orig), orig.native (), builddir));
}

void
resolve_shortvars::visit (t_shortvar &n)
{
  token const &tok = n.var ()->as<token> ();

  char const *name = tok.string.c_str ();

  bool builddir = false;
  char modifier = '\0';
  if (name[0] == '(')
    {
      modifier = name[2];
      name++;
    }
  if (in_log_compiler)
    switch (name[0])
      {
      case '@':
        replacement = make_code ("$@");
        break;
      default:
        throw name;
      }
  else
    {
      assert (target);
      assert (prereq);

      // uninstantiated rule:
      if (target->size () > 1)
        return;

      switch (name[0])
        {
        case '@':
          replacement = target;
          builddir = true;
          break;
        case '<':
          if (prereq->size () == 0)
            {
              errors.add<semantic_error> (&n, "no prerequisites found to resolve $<");
              return;
            }
          replacement = prereq->list[0];
          break;
        case '$':
          replacement = make_code (n.loc, "$$");
          break;
        }
      if (replacement && name[0] != '$')
        replacement = modify (replacement, modifier, builddir);
    }
}

void
resolve_shortvars::visit (t_intvar &n)
{
  token &tok = n.num ()->as<token> ();
  long var = strtol (tok.string.c_str (), 0, 10);

  if (prereq->size () < var)
    {
      errors.add<semantic_error> (&n, "prerequisite index out of bounds: $" + tok.string);
      return;
    }
  replacement = prereq->list[var - 1];
}


void
resolve_shortvars::visit (t_rule_line &n)
{
  foreach (node_ptr &p, n.list)
    {
      resume (p);
      if (replacement)
        {
          p->parent ()->replace (*p, replacement);
          replacement = 0;
        }
    }
}

void
resolve_shortvars::visit (t_rule &n)
{
  target = &n.target ()->as<t_filenames> ()[0]->as<t_filename> ();
  prereq = &n.prereq ()->as<t_filenames> ();

  symbol_visitor::visit (n);

  prereq = 0;
  target = 0;
}

void
resolve_shortvars::visit (t_log_compiler &n)
{
  in_log_compiler = true;
  symbol_visitor::visit (n);
  in_log_compiler = false;
}
