#include "phase.h"

#include "util/foreach.h"

#include <algorithm>

#include "print.h"

static phase<print> thisphase ("print", noauto);

void
print::visit (t_filenames &n)
{
  visitor::visit (n);
}

void
print::visit (t_variable_content &n)
{
  fprintf (fh, "(");
  resume (n.name ());
  if (n.member ())
    fprintf (fh, ".") && resume (n.member ());
  if (n.filter ())
    fprintf (fh, " : ") && resume (n.filter ());
  fprintf (fh, ")");
}

void
print::visit (t_destination &n)
{
  fprintf (fh, "-> ");
  visitor::visit (n);
}

void
print::visit (t_document &n)
{
  //puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_program &n)
{
  fprintf (fh, "program ");
  visitor::visit (n);
}

void
print::visit (t_inheritance &n)
{
  fprintf (fh, ": ");
  visitor::visit (n);
}

void
print::visit (t_if &n)
{
  fprintf (fh, "if ");
  visitor::visit (n);
}

void
print::visit (t_identifiers &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_rule_line &n)
{
  fprintf (fh, "      ");
  visitor::visit (n);
  fprintf (fh, "\n");
}

void
print::visit (t_ac_check &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_rule_lines &n)
{
  visitor::visit (n);
}

void
print::visit (t_check_functions &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_template &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_project_members &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_link_body &n)
{
  foreach (node_ptr const &p, n.list)
    {
      fprintf (fh, "      ");
      resume (p);
      fprintf (fh, "\n");
    }
}

void
print::visit (t_project_version &n)
{
  puts (__PRETTY_FUNCTION__);
}

template<typename NodeT>
void
print::file_list (NodeT &n, char const *name)
{
  fprintf (fh, "   %s ", name);
  resume (n[0]) && fprintf (fh, " ");
  fprintf (fh, "{\n");
  visitor::visit (n);
  fprintf (fh, "   }\n");
}

void
print::visit (t_extra_dist &n)
{
  file_list (n, "extra_dist");
}

void
print::visit (t_sources &n)
{
  file_list (n, "sources");
}

void
print::visit (t_nodist_sources &n)
{
  file_list (n, "nodist_sources");
}

void
print::visit (t_vardecl &n)
{
  fprintf (fh, "   ");
  resume (n.var ());
  fprintf (fh, " =");
  resume (n.body ());
  fprintf (fh, "\n");
}

void
print::visit (t_arg_enable_choices &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_check_cflags &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_target_definition &n)
{
  resume (n.name ()) && fprintf (fh, " ");
  resume (n.cond ()) && fprintf (fh, " ");
  resume (n.derive ()) && fprintf (fh, " ");
  resume (n.dest ()) && fprintf (fh, " ");
  fprintf (fh, "{\n");
  resume (n.body ());
  fprintf (fh, "}\n");
}

void
print::visit (t_shortvar &n)
{
  fprintf (fh, "$");
  visitor::visit (n);
}

void
print::visit (t_intvar &n)
{
  fprintf (fh, "$");
  visitor::visit (n);
}

void
print::visit (t_roundvar &n)
{
  fprintf (fh, "$");
  visitor::visit (n);
}

void
print::visit (t_squarevar &n)
{
  fprintf (fh, "$");
  fprintf (fh, "[");
  visitor::visit (n);
  fprintf (fh, "]");
}

void
print::visit (t_check_sizeof &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_toplevel_declarations &n)
{
  visitor::visit (n);
}

void
print::visit (t_project &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_project_header &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_vardecl_body &n)
{
  foreach (node_ptr const &p, n.list)
    {
      resume (p);
      fprintf (fh, " ");
    }
}

void
print::visit (t_section &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_rule &n)
{
  in_rule = true;
  fprintf (fh, "   ");
  resume (n.target ());
  fprintf (fh, ": ");
  {
    in_prereq = true;
    resume (n.prereq ());
    in_prereq = false;
  }
  if (n.code ())
    {
      fprintf (fh, "{\n");
      resume (n.code ());
      fprintf (fh, "   }\n");
    }
  else
    fprintf (fh, ";\n");
  in_rule = false;
}

void
print::visit (t_library &n)
{
  fprintf (fh, "library ");
  visitor::visit (n);
}

void
print::visit (t_arg_enable_choice &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_sourcesref &n)
{
  fprintf (fh, "      sources (");
  visitor::visit (n);
  fprintf (fh, ")\n");
}

void
print::visit (t_error &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_check_alignof &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_link &n)
{
  fprintf (fh, "   link ");
  resume (n.cond ());
  fprintf (fh, "{\n");
  resume (n.items ());
  fprintf (fh, "   }\n");
}

void
print::visit (t_tool_flags &n)
{
  fprintf (fh, "   ");
  resume (n.keyword ());
  fprintf (fh, " ");
  resume (n.cond ());
  fprintf (fh, "{\n");
  resume (n.flags ());
  fprintf (fh, "   }\n");
}

void
print::visit (t_arg_enable &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_flags &n)
{
  foreach (node_ptr const &p, n.list)
    {
      fprintf (fh, "      ");
      resume (p);
      fprintf (fh, "\n");
    }
}

void
print::visit (t_project_contact &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_check_library &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_define &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_sources_members &n)
{
  visitor::visit (n);
}

void
print::visit (t_filename &n)
{
  if (!in_rule)
    fprintf (fh, "      ");
  visitor::visit (n);
  if (!in_rule)
    fprintf (fh, "\n");
  if (in_prereq)
    fprintf (fh, " ");
}

void
print::visit (t_section_members &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_target_members &n)
{
  visitor::visit (n);
}

void
print::visit (t_check_headers &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_exclude &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (token &n)
{
  fprintf (fh, "%s", n.string.c_str ());
}
