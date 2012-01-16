#include "phase.h"

#include "annotations/error_log.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <boost/filesystem/path.hpp>

struct resolve_shortvars
  : symbol_visitor
{
  virtual void visit (t_shortvar &n);
  virtual void visit (t_intvar &n);

  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  error_log &errors;

  enum parse_state
  {
    S_NONE,
    S_FILENAME,
    S_MULTIFILE
  };

  parse_state state;
  node_ptr replacement;
  t_filename_ptr target;
  t_filenames_ptr prereq;

  resolve_shortvars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
    , state (S_NONE)
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
        : modifier == 0
          ? identity
          : NULL;
  assert (pred != NULL);

  std::string const &orig = fn[0]->as<token> ().string;
  return new t_filename (fn.loc,
           new token (fn[0]->loc,
             TK_CODE,
             add_builddir (pred (orig), orig, builddir)));
}

void
resolve_shortvars::visit (t_shortvar &n)
{
  assert (target);
  assert (prereq);

  token const &tok = n.var ()->as<token> ();

  // uninstantiated rule:
  if (target->size () > 1)
    return;

  char const *name = tok.string.c_str ();

  bool builddir = false;
  char modifier = 0;
  if (name[0] == '(')
    {
      modifier = name[2];
      name++;
    }
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
    }
  if (replacement)
    replacement = modify (replacement, modifier, builddir);
}

void
resolve_shortvars::visit (t_intvar &n)
{
  token &tok = n.num ()->as<token> ();
  long var = strtol (tok.string.c_str (), 0, 10);

  if (var == 0)
    {
      generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
      std::string const &tname = TARGET->as<generic_node> ()[0]->as<token> ().string;
      replacement = new token (n.loc, TK_CODE, "$(builddir)/" + tname + "$(EXEEXT)");
      return;
    }

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
          swap (p, replacement);
          replacement = 0;
        }
    }
}

void
resolve_shortvars::visit (t_rule &n)
{
  target = &n.target ()->as<t_filenames> ()[0]->as<t_filename> ();
  prereq = &n.prereq ()->as<t_filenames> ();

  visitor::visit (n);

  prereq = 0;
  target = 0;

  //phases::run ("print", &n);
}
