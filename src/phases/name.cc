#include "phase.h"

namespace
{
  struct name
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    name (annotation_map &annots)
    {
    }
  };

  static phase<name> thisphase ("name", "audit");
}

bool
name::visit_ac_check (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_arg_enable_choice (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_arg_enable_choices (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_arg_enable (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_alignof (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_cflags (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_functions (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_headers (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_library (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_check_sizeof (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_code (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_define (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_description (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_destination (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_document (nodes::generic_node &n)
{
  puts ("");
  return false;
}

bool
name::visit_error (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_exclude (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_extra_dist (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_filename (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_filenames (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_flags (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_identifiers (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_if (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_inheritance (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_library (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_link_body (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_link (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_nodist_sources (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_program (nodes::generic_node &n)
{
  puts (n[0]->as<generic_node> ()[0]->as<token> ().string.c_str ());
  return false;
}

bool
name::visit_project_member (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_project_members (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_project (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_rule_line (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_rule_lines (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_rule (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_rules (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_section_members (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_section (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_sources_members (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_sources (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_sourcesref (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_target_definition (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_target_members (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_template (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_tool_flags (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_toplevel_declaration (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_toplevel_declarations (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_vardecl_body (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_vardecl (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_variable_content (nodes::generic_node &n)
{
  puts ("");
  return true;
}

bool
name::visit_variable (nodes::generic_node &n)
{
  puts ("");
  return true;
}

void 
name::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
}
