#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"
#include "util/extract_string.h"

using annotations::error_log;

struct resolve_vars
  : symbol_visitor
{
  virtual void visit (t_variable_content &n);
  virtual void visit (t_filename &n);

  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  virtual void visit (generic_node &n);
  virtual void visit (token &n);

  bool in_sources;
  error_log &errors;
  node_ptr sym;
  generic_node_ptr root;

  enum parse_state
  {
    S_NONE,
    S_FILENAME,
    S_MULTIFILE,
    S_RULE_LINE
  };

  parse_state state;

  resolve_vars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , in_sources (false)
    , errors (annots.get ("errors"))
    , state (S_NONE)
  {
  }

#if 0
  ~resolve_vars ()
  {
    if (!std::uncaught_exception ())
      exit (0);
  }
#endif
};

static phase<resolve_vars> thisphase ("resolve_vars", "insert_syms", "concat_vardecls");


void
resolve_vars::visit (t_variable_content &n)
{
  if (!n.member ())
    {
      std::string const &name = n.name ()->as<token> ().string;
      if (name == "THIS")
        {
          generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
          std::string const &tname = TARGET->as<generic_node> ()[0]->as<token> ().string;
          sym = new token (n.loc, TK_CODE, "$(builddir)/" + tname + "$(EXEEXT)");
        }
      else
        sym = symtab.lookup (T_VARIABLE, name);
    }
  visitor::visit (n);
}


void
resolve_vars::visit (t_filename &n)
{
  local (state) = S_FILENAME;
  for (size_t i = 0; i < n.size (); i++)
    {
      node_ptr &p = n[i];
      p->accept (*this);

      if (sym)
        {
          n.list.erase  (n.list.begin () + i, n.list.begin () + i + 1);

          node_ptr parsed = parse_string (extract_string (sym), errors, r_filename, state == S_MULTIFILE);
          t_filename_ptr new_filename = &parsed->as<t_filename> ();
          sym = 0;

          n.list.insert (n.list.begin () + i, new_filename->list.begin (), new_filename->list.end ());
        }
    }
}

void
resolve_vars::visit (t_rule_line &n)
{
  local (state) = S_RULE_LINE;
  for (size_t i = 0; i < n.size (); i++)
    {
      node_ptr &p = n[i];
      p->accept (*this);

      if (sym)
        {
          p = sym;
          sym = 0;
        }
    }
}

void
resolve_vars::visit (t_rule &n)
{
  visitor::visit (n);
  sym = 0;
}


void 
resolve_vars::visit (generic_node &n)
{
  if (n.type == n_toplevel_declarations)
    root = &n;
  visitor::visit (n);
}

void 
resolve_vars::visit (token &n)
{
  if (state == S_FILENAME && n.tok == TK_FN_LBRACE)
    state = S_MULTIFILE;
  if (state == S_MULTIFILE && n.tok == TK_FN_RBRACE)
    state = S_FILENAME;
}
