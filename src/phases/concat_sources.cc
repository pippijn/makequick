/**
 * concat_sources:
 *
 * This phase concatenates all non-wildcard path parts in filenames.
 */
#include "phase.h"

#include "algorithm/grep.h"
#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "colours.h"
#include "foreach.h"
#include "util/create_file_list.h"

#include <numeric>

#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace
{
  struct concat_sources
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    concat_sources (annotation_map &annots)
    {
    }
  };

  static phase<concat_sources> thisphase ("concat_sources", "audit");
}

bool concat_sources::visit_ac_check (nodes::generic_node &n) { return true; }
bool concat_sources::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool concat_sources::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool concat_sources::visit_arg_enable (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_alignof (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_cflags (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_functions (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_headers (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_library (nodes::generic_node &n) { return true; }
bool concat_sources::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool concat_sources::visit_code (nodes::generic_node &n) { return true; }
bool concat_sources::visit_define (nodes::generic_node &n) { return true; }
bool concat_sources::visit_description (nodes::generic_node &n) { return true; }
bool concat_sources::visit_destination (nodes::generic_node &n) { return true; }
bool concat_sources::visit_document (nodes::generic_node &n) { return true; }
bool concat_sources::visit_error (nodes::generic_node &n) { return true; }
bool concat_sources::visit_exclude (nodes::generic_node &n) { return true; }
bool concat_sources::visit_extra_dist (nodes::generic_node &n) { return true; }
bool concat_sources::visit_filenames (nodes::generic_node &n) { return true; }
bool concat_sources::visit_flags (nodes::generic_node &n) { return true; }
bool concat_sources::visit_identifiers (nodes::generic_node &n) { return true; }
bool concat_sources::visit_if (nodes::generic_node &n) { return true; }
bool concat_sources::visit_inheritance (nodes::generic_node &n) { return true; }
bool concat_sources::visit_library (nodes::generic_node &n) { return true; }
bool concat_sources::visit_link_body (nodes::generic_node &n) { return true; }
bool concat_sources::visit_link (nodes::generic_node &n) { return true; }
bool concat_sources::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool concat_sources::visit_program (nodes::generic_node &n) { return true; }
bool concat_sources::visit_project_member (nodes::generic_node &n) { return true; }
bool concat_sources::visit_project_members (nodes::generic_node &n) { return true; }
bool concat_sources::visit_project (nodes::generic_node &n) { return true; }
bool concat_sources::visit_rule_line (nodes::generic_node &n) { return true; }
bool concat_sources::visit_rule_lines (nodes::generic_node &n) { return true; }
bool concat_sources::visit_rule (nodes::generic_node &n) { return true; }
bool concat_sources::visit_rules (nodes::generic_node &n) { return true; }
bool concat_sources::visit_section_members (nodes::generic_node &n) { return true; }
bool concat_sources::visit_section (nodes::generic_node &n) { return true; }
bool concat_sources::visit_sources_members (nodes::generic_node &n) { return true; }
bool concat_sources::visit_sourcesref (nodes::generic_node &n) { return true; }
bool concat_sources::visit_target_definition (nodes::generic_node &n) { return true; }
bool concat_sources::visit_target_members (nodes::generic_node &n) { return true; }
bool concat_sources::visit_template (nodes::generic_node &n) { return true; }
bool concat_sources::visit_tool_flags (nodes::generic_node &n) { return true; }
bool concat_sources::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool concat_sources::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool concat_sources::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool concat_sources::visit_vardecl (nodes::generic_node &n) { return true; }
bool concat_sources::visit_variable_content (nodes::generic_node &n) { return true; }
bool concat_sources::visit_variable (nodes::generic_node &n) { return true; }
bool concat_sources::visit_sources (nodes::generic_node &n) { return true; }



static bool
can_concat (node_ptr const &n)
{
  if (token const *t = n->is<token> ())
    switch (t->tok)
      {
      case TK_FN_LBRACE:
      case TK_FN_PERCENT:
      case TK_FN_PERPERCENT:
      case TK_FN_QMARK:
      case TK_FN_RBRACE:
      case TK_FN_STAR:
      case TK_FN_STARSTAR:
        return false;
      default:
        return true;
      }
  return false;
}

static std::string &
concat_token (std::string &p1, node_ptr const &p2)
{
  return p1 += p2->as<token> ().string;
}

static void
commit (std::string &fn, node_vec &list, location &loc)
{
  if (!fn.empty ())
    list.push_back (new token (loc, TK_FILENAME, fn));
  fn.clear ();

  loc.first_line = loc.last_line;
  loc.first_column = loc.last_column;
}

bool
concat_sources::visit_filename (nodes::generic_node &n)
{
  node_vec list;
  swap (list, n.list);

  location loc = n.loc;
  std::string fn;
  foreach (node_ptr const &p, list)
    {
      loc.last_line = p->loc.last_line;
      loc.last_column = p->loc.last_column;
      if (!can_concat (p))
        {
          commit (fn, n.list, loc);
          n.list.push_back (p);
        }
      else
        fn += p->as<token> ().string;
    }
  commit (fn, n.list, loc);

  return false;
}


void 
concat_sources::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
