#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/extract_string.h"

using annotations::error_log;
using annotations::symbol_table;

namespace
{
  struct resolve_vars
    : visitor
  {
    virtual void visit (t_variable_content &n);
    virtual void visit (t_variable &n);
    virtual void visit (t_filename &n);
    virtual void visit (t_program &n);
    virtual void visit (t_document &n);
    virtual void visit (token &n);

    bool in_sources;
    error_log &errors;
    symbol_table &symtab;
    generic_node_ptr sym;

    enum parse_state
    {
      S_NONE,
      S_FILENAME,
      S_MULTIFILE
    };

    parse_state state;

    resolve_vars (annotation_map &annots)
      : in_sources (false)
      , errors (annots.get ("errors"))
      , symtab (annots.get ("symtab"))
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

  static phase<resolve_vars> thisphase ("resolve_vars", "insert_syms", "concat_vars");
}


void
resolve_vars::visit (t_variable_content &n)
{
  if (!n.member ())
    {
      sym = symtab.lookup (T_VARIABLE, n.name ()->as<token> ().string);
#if 0
      phases::run ("xml", sym);
#endif
    }
  visitor::visit (n);
}

void
resolve_vars::visit (t_variable &n)
{
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
          n.list.erase (n.list.begin () + i, n.list.begin () + i + 1);
          node_ptr parsed = parse_string (extract_string (sym), r_filename, state == S_MULTIFILE);
          sym = &parsed->as<t_filename> ();
          n.list.insert (n.list.begin () + i, sym->list.begin (), sym->list.end ());
          sym = 0;
        }
    }
#if 0
  phases::run ("xml", &n);
#endif
}


void
resolve_vars::visit (t_program &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}

void
resolve_vars::visit (t_document &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}

void 
resolve_vars::visit (token &n)
{
  if (state == S_FILENAME && n.tok == TK_FN_LBRACE)
    state = S_MULTIFILE;
  if (state == S_MULTIFILE && n.tok == TK_FN_RBRACE)
    state = S_FILENAME;
}
