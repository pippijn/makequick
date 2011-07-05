#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"

using annotations::error_log;
using annotations::symbol_table;

namespace
{
  struct resolve_vars
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    bool in_sources;
    error_log &errors;
    symbol_table &symtab;

    resolve_vars (annotation_map &annots)
      : in_sources (false)
      , errors (annots.get ("errors"))
      , symtab (annots.get ("symtab"))
    {
    }

    ~resolve_vars ()
    {
    }
  };

  static phase<resolve_vars> thisphase ("resolve_vars", "insert_syms");
}

bool resolve_vars::visit_ac_check (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_arg_enable (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_alignof (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_cflags (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_functions (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_headers (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_library (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_code (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_define (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_description (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_destination (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_error (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_exclude (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_extra_dist (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_filename (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_filenames (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_flags (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_identifiers (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_if (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_inheritance (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_library (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_link_body (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_link (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_project_member (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_project_members (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_project (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_rule_line (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_rule_lines (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_rule (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_rules (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_section_members (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_section (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_sources_members (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_sources (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_sourcesref (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_target_definition (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_target_members (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_template (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_tool_flags (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool resolve_vars::visit_vardecl (nodes::generic_node &n) { return true; }


bool
resolve_vars::visit_variable_content (nodes::generic_node &n)
{
  if (!n[1])
    {
      puts ("oi!");
      generic_node_ptr sym = symtab.lookup (T_VARIABLE, n[0]->as<token> ().string);
      phases::run ("xml", sym);
    }
  return true;
}

bool
resolve_vars::visit_variable (nodes::generic_node &n)
{
  return true;
}


bool
resolve_vars::visit_program (nodes::generic_node &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
  return false;
}

bool
resolve_vars::visit_document (nodes::generic_node &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
  return false;
}

void 
resolve_vars::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
