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
  struct resolve_sources
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    bool in_sources;
    annotations::file_list const &files;
    annotations::error_log &errors;

    std::vector<generic_node_ptr> plainfiles;
    std::vector<generic_node_ptr> wildcards;

    resolve_sources (annotation_map &annots)
      : in_sources (false)
      , files (annots.get ("files"))
      , errors (annots.get ("errors"))
    {
    }
  };

  static phase<resolve_sources> thisphase ("resolve_sources", "audit");
}

bool resolve_sources::visit_ac_check (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_arg_enable (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_alignof (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_cflags (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_functions (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_headers (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_library (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_code (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_define (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_description (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_destination (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_document (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_error (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_exclude (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_extra_dist (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_filenames (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_flags (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_identifiers (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_if (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_inheritance (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_library (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_link_body (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_link (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_program (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_project_member (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_project_members (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_project (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_rule_line (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_rule_lines (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_rule (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_section_members (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_section (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_sources_members (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_sourcesref (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_target_definition (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_target_members (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_template (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_tool_flags (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_vardecl (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_variable_content (nodes::generic_node &n) { return true; }
bool resolve_sources::visit_variable (nodes::generic_node &n) { return true; }



static bool
is_wildcard (node_ptr const &n)
{
  switch (n->as<token> ().tok)
    {
    case TK_FN_LBRACE:
    case TK_FN_RBRACE:
    case TK_FN_QMARK:
    case TK_FN_STARSTAR:
    case TK_FN_STAR:
      return true;
    default:
      return false;
    }
}

bool
resolve_sources::visit_filename (nodes::generic_node &n)
{
  if (in_sources)
    {
      if (find_if (n.list.begin (), n.list.end (),
                   is_wildcard) != n.list.end ())
        wildcards.push_back (&n);
      else
        plainfiles.push_back (&n);
    }
  return false;
}



static std::string &
concat_token (std::string &p1, node_ptr const &p2)
{
  return p1 += p2->as<token> ().string;
}

bool
resolve_sources::visit_sources (nodes::generic_node &n)
{
  if (!plainfiles.empty ())
    throw std::runtime_error ("found source files somewhere outside sources");

  in_sources = true;
  resume_list ();
  in_sources = false;

  //printf ("%lu plain files\n", plainfiles.size ());

  std::vector<fs::path> source_files;
  foreach (generic_node_ptr const &f, plainfiles)
    {
      fs::path filename = accumulate (f->list.begin (),
                                      f->list.end (),
                                      std::string (),
                                      concat_token);

      fs::path path;
      // relative path
      if (filename.native ()[0] != '.')
        {
          path = files.base / filename;
          if (!is_regular_file (path))
            path.clear ();
        }
      // absolute (source-root relative) path
      if (path.empty () && !filename.is_absolute ())
        {
          path = n.loc.file->parent_path () / filename;
          if (!is_regular_file (path))
            path.clear ();
        }
      if (path.empty ())
        {
          errors.add<semantic_error> (f, "could not find file " + C::filename (filename));
          continue;
        }

      if (!std::equal (files.base.begin (),
                       files.base.end (),
                       path.begin ()))
        throw std::runtime_error ("source file found outside source root: " + C::filename (path));

      source_files.push_back (path);
    }
  plainfiles.clear ();

  n.list.clear ();
  n.list.insert (n.list.end (), wildcards.begin (), wildcards.end ());

  create_file_list (source_files, n.list, n.loc);

  return false;
}


void 
resolve_sources::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
