#include "phase.h"

#include "algorithm/grep.h"
#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "colours.h"
#include "foreach.h"

#include <boost/filesystem/operations.hpp>

template<typename ForwardIterator>
static bool
is_sorted (ForwardIterator it, ForwardIterator et)
{
  ForwardIterator prev = it;
  while (++it != et)
    {
      if (*prev > *it)
        return false;
      prev = it;
    }
  return true;
}

namespace
{
  struct resolve_sources
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    bool in_sources;
    annotations::file_list const &files;
    annotations::rule_info const &rule_info;
    annotations::error_log &errors;

    resolve_sources (annotation_map &annots)
      : in_sources (false)
      , files (annots.get ("files"))
      , rule_info (annots.get ("rule_info"))
      , errors (annots.get ("errors"))
    {
      assert (is_sorted (files.begin, files.end));
      assert (is_sorted (rule_info.files.begin (), rule_info.files.end ()));
    }

    ~resolve_sources ()
    {
#if 0
      if (!std::uncaught_exception ())
        throw __func__;
#endif
    }
  };

  static phase<resolve_sources> thisphase ("resolve_sources", "inference");
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
bool resolve_sources::visit_rules (nodes::generic_node &n) { return true; }
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
try_resolve (annotations::file_list const &files,
             std::vector<fs::path> const &buildable,
             std::string &file, fs::path const &dir)
{
  fs::path const &path = dir / file;
  fs::path const &relpath = files.rel (path);
#if 0
  std::cout << "looking for " << files.rel (path) << "\n";
#endif
  if (exists (path) || std::binary_search (buildable.begin (), buildable.end (), relpath))
    {
      file = relpath.native ();
      return true;
    }

  return false;
}

bool
resolve_sources::visit_filename (nodes::generic_node &n)
{
  if (in_sources)
    {
      // only plain files, no wildcards
      if (n.size () == 1)
        {
          token &t = n[0]->as<token> ();
          fs::path file = t.string;
#if 0
          std::cout << "resolving " << file << "\n";
#endif

          if (!try_resolve (files, rule_info.files, t.mutable_string, n.loc.file->parent_path ()))
            if (!try_resolve (files, rule_info.files, t.mutable_string, files.base))
              errors.add<semantic_error> (&n, "could not resolve file: " + C::filename (t.string),
                                          "not in file system and no rule to build it");
        }
    }
  return false;
}



bool
resolve_sources::visit_sources (nodes::generic_node &n)
{
  in_sources = true;
  resume_list ();
  in_sources = false;

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
