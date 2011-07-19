#include "phase.h"

#include "foreach.h"

#include <algorithm>

struct print
  : visitor
{
#include "visitor_t.h"

  template<typename NodeT>
  void file_list (NodeT &n, char const *name);

  virtual void visit (token &n);

  bool in_rule;

  print (annotation_map &annots)
    : in_rule (false)
  {
  }
};

static phase<print> thisphase ("print", noauto);

void
print::visit (t_filenames &n)
{
  visitor::visit (n);
}

void
print::visit (t_variable_content &n)
{
  printf ("(");
  resume (n.name ());
  if (n.member ())
    printf (".") && resume (n.member ());
  if (n.filter ())
    printf (" : ") && resume (n.filter ());
  printf (")");
}

void
print::visit (t_destination &n)
{
  printf ("-> ");
  visitor::visit (n);
}

void
print::visit (t_document &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_program &n)
{
  printf ("program ");
  visitor::visit (n);
}

void
print::visit (t_inheritance &n)
{
  printf (": ");
  visitor::visit (n);
}

void
print::visit (t_if &n)
{
  printf ("if ");
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
  printf ("      ");
  visitor::visit (n);
  printf ("\n");
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
      printf ("      ");
      resume (p);
      printf ("\n");
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
  printf ("   %s ", name);
  resume (n[0]) && printf (" ");
  printf ("{\n");
  visitor::visit (n);
  printf ("   }\n");
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
  printf ("   ");
  resume (n.var ());
  printf (" =");
  resume (n.body ());
  printf ("\n");
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
  resume (n.name ()) && printf (" ");
  resume (n.cond ()) && printf (" ");
  resume (n.derive ()) && printf (" ");
  resume (n.dest ()) && printf (" ");
  printf ("{\n");
  resume (n.body ());
  printf ("}\n");
}

void
print::visit (t_shortvar &n)
{
  printf ("$");
  visitor::visit (n);
}

void
print::visit (t_intvar &n)
{
  printf ("$");
  visitor::visit (n);
}

void
print::visit (t_roundvar &n)
{
  printf ("$");
  visitor::visit (n);
}

void
print::visit (t_squarevar &n)
{
  printf ("$");
  printf ("[");
  visitor::visit (n);
  printf ("]");
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
      printf (" ");
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
  printf ("   ");
  resume (n.target ());
  printf (": ");
  resume (n.prereq ()) && printf (" ");
  if (n.code ())
    {
      printf ("{\n");
      resume (n.code ());
      printf ("   }\n");
    }
  else
    printf (";\n");
  in_rule = false;
}

void
print::visit (t_library &n)
{
  printf ("library ");
  visitor::visit (n);
}

void
print::visit (t_rules &n)
{
  printf ("global {\n");
  visitor::visit (n);
  printf ("}\n");
}

void
print::visit (t_arg_enable_choice &n)
{
  puts (__PRETTY_FUNCTION__);
}

void
print::visit (t_sourcesref &n)
{
  printf ("      sources (");
  visitor::visit (n);
  printf (")\n");
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
  printf ("   link ");
  resume (n.cond ());
  printf ("{\n");
  resume (n.items ());
  printf ("   }\n");
}

void
print::visit (t_tool_flags &n)
{
  printf ("   ");
  resume (n.keyword ());
  printf (" ");
  resume (n.cond ());
  printf ("{\n");
  resume (n.flags ());
  printf ("   }\n");
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
      printf ("      ");
      resume (p);
      printf ("\n");
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
    printf ("      ");
  visitor::visit (n);
  if (!in_rule)
    printf ("\n");
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
  printf ("%s", n.string.c_str ());
}
