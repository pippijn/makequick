#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"

using annotations::error_log;
using annotations::symbol_table;

namespace
{
  struct insert_syms
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    bool in_sources;
    error_log &errors;
    symbol_table &symtab;

    insert_syms (annotation_map &annots)
      : in_sources (false)
      , errors (annots.get ("errors"))
      , symtab (annots.put ("symtab", new symbol_table))
    {
    }

    ~insert_syms ()
    {
      symtab.print ();
    }
  };

  static phase<insert_syms> thisphase ("insert_syms", "audit");
}

bool insert_syms::visit_ac_check (nodes::generic_node &n) { return true; }
bool insert_syms::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool insert_syms::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool insert_syms::visit_arg_enable (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_alignof (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_cflags (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_functions (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_headers (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_library (nodes::generic_node &n) { return true; }
bool insert_syms::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool insert_syms::visit_code (nodes::generic_node &n) { return true; }
bool insert_syms::visit_define (nodes::generic_node &n) { return true; }
bool insert_syms::visit_description (nodes::generic_node &n) { return true; }
bool insert_syms::visit_destination (nodes::generic_node &n) { return true; }
bool insert_syms::visit_error (nodes::generic_node &n) { return true; }
bool insert_syms::visit_exclude (nodes::generic_node &n) { return true; }
bool insert_syms::visit_extra_dist (nodes::generic_node &n) { return true; }
bool insert_syms::visit_filename (nodes::generic_node &n) { return true; }
bool insert_syms::visit_filenames (nodes::generic_node &n) { return true; }
bool insert_syms::visit_flags (nodes::generic_node &n) { return true; }
bool insert_syms::visit_identifiers (nodes::generic_node &n) { return true; }
bool insert_syms::visit_if (nodes::generic_node &n) { return true; }
bool insert_syms::visit_inheritance (nodes::generic_node &n) { return true; }
bool insert_syms::visit_library (nodes::generic_node &n) { return true; }
bool insert_syms::visit_link_body (nodes::generic_node &n) { return true; }
bool insert_syms::visit_link (nodes::generic_node &n) { return true; }
bool insert_syms::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool insert_syms::visit_project_member (nodes::generic_node &n) { return true; }
bool insert_syms::visit_project_members (nodes::generic_node &n) { return true; }
bool insert_syms::visit_project (nodes::generic_node &n) { return true; }
bool insert_syms::visit_rule_line (nodes::generic_node &n) { return true; }
bool insert_syms::visit_rule_lines (nodes::generic_node &n) { return true; }
bool insert_syms::visit_rule (nodes::generic_node &n) { return true; }
bool insert_syms::visit_rules (nodes::generic_node &n) { return true; }
bool insert_syms::visit_section_members (nodes::generic_node &n) { return true; }
bool insert_syms::visit_section (nodes::generic_node &n) { return true; }
bool insert_syms::visit_sources_members (nodes::generic_node &n) { return true; }
bool insert_syms::visit_sources (nodes::generic_node &n) { return true; }
bool insert_syms::visit_sourcesref (nodes::generic_node &n) { return true; }
bool insert_syms::visit_target_definition (nodes::generic_node &n) { return true; }
bool insert_syms::visit_target_members (nodes::generic_node &n) { return true; }
bool insert_syms::visit_template (nodes::generic_node &n) { return true; }
bool insert_syms::visit_tool_flags (nodes::generic_node &n) { return true; }
bool insert_syms::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool insert_syms::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool insert_syms::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool insert_syms::visit_variable_content (nodes::generic_node &n) { return true; }
bool insert_syms::visit_variable (nodes::generic_node &n) { return true; }


bool
insert_syms::visit_vardecl (nodes::generic_node &n)
{
  std::string const &name = n[0]->as<token> ().string;
  generic_node *sym = &n[1]->as<generic_node> ();
  if (!symtab.insert (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
  return false;
}

bool
insert_syms::visit_program (nodes::generic_node &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
  return false;
}

bool
insert_syms::visit_document (nodes::generic_node &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
  return false;
}

void 
insert_syms::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
