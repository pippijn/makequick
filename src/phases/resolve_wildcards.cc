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
  struct resolve_wildcards
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    bool in_sources;
    annotations::file_list const &files;
    annotations::error_log &errors;

    std::vector<fs::path> source_files;
    generic_node_vec wildcards;

    resolve_wildcards (annotation_map &annots)
      : in_sources (false)
      , files (annots.get ("files"))
      , errors (annots.get ("errors"))
    {
    }
  };

  //static phase<resolve_wildcards> thisphase ("resolve_wildcards", "resolve_sources");
}

bool resolve_wildcards::visit_ac_check (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_arg_enable (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_alignof (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_cflags (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_functions (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_headers (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_library (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_code (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_define (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_description (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_destination (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_document (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_error (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_exclude (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_extra_dist (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_filenames (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_flags (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_identifiers (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_if (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_inheritance (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_library (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_link_body (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_link (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_program (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_project_member (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_project_members (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_project (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_rule_line (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_rule_lines (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_rule (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_rules (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_section_members (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_section (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_sources_members (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_sourcesref (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_target_definition (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_target_members (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_template (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_tool_flags (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_vardecl (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_variable_content (nodes::generic_node &n) { return true; }
bool resolve_wildcards::visit_variable (nodes::generic_node &n) { return true; }



bool
resolve_wildcards::visit_filename (nodes::generic_node &n)
{
  if (in_sources)
    {
      // resolve_sources reduced all non-wildcard sources to a single filename token
      // also, a wildcard consists of at least two tokens
      // thus, everything with more than one token must be a wildcard
      if (n.list.size () > 1)
        wildcards.push_back (&n);
      else
        source_files.push_back (n.list.front ()->as<token> ().string);
    }
  return false;
}



struct translate_wildcard
{
  static void escape (std::string &regex, std::string const &wc)
  {
    foreach (char c, wc)
      switch (c)
        {
        case '(':
        case ')':
        case '.':
          regex += '\\';
        default:
          regex += c;
          break;
        }
  }

  std::string &operator () (std::string &regex, node_ptr const &n)
  {
    token const &t = n->as<token> ();
    switch (t.tok)
      {
      case TK_FN_LBRACE:
        regex += '(';
        in_alt = 1;
        break;
      case TK_FN_RBRACE:
        regex += ')';
        in_alt = 0;
        break;
      case TK_FN_QMARK:
        regex += '.';
        break;
      case TK_FN_STARSTAR:
        regex += ".*";
        break;
      case TK_FN_STAR:
        regex += "[^/]*";
        break;
      default:
        if (in_alt == 2)
          regex += '|';
        in_alt = !!in_alt * 2;
        escape (regex, t.string);
        break;
      }
    return regex;
  }

  int in_alt;
};

struct regex_matcher
{
  explicit regex_matcher (boost::regex const &re, std::vector<fs::path> const &source_files, node_ptr const &node, annotations::error_log &errors)
    : re (re)
    , source_files (source_files)
    , node (node)
    , errors (errors)
  {
  }

  bool operator () (fs::path const &p) const
  {
    if (find (source_files.begin (), source_files.end (), p) != source_files.end ())
      return false;
    bool matched = regex_match (p.c_str (), re);
    if (matched)
      errors.add<warning> (node, "wildcard matched file: " + C::filename (p));
    return matched;
  }

  boost::regex const &re;
  std::vector<fs::path> const &source_files;
  node_ptr const &node;
  annotations::error_log &errors;
};

bool
resolve_wildcards::visit_sources (nodes::generic_node &n)
{
  if (!wildcards.empty ())
    throw std::runtime_error ("found wildcards somewhere outside sources");

  in_sources = true;
  resume_list ();
  in_sources = false;

  //printf ("%lu wildcards\n", wildcards.size ());

  if (!wildcards.empty ())
    {
      std::string regex = "(";
      foreach (generic_node_ptr const &wc, wildcards)
        {
          if (wc->list.front ()->as<token> ().tok == TK_FN_SLASH)
            translate_wildcard::escape (regex, files.base.native ());
          else
            translate_wildcard::escape (regex, wc->loc.file->parent_path ().native () + "/");
          regex += accumulate (wc->list.begin (),
                               wc->list.end (),
                               std::string (),
                               translate_wildcard ());
          if (wc != wildcards.back ())
            regex += '|';
        }
      wildcards.clear ();
      regex += ")";

      grep (files.begin, files.end,
            back_inserter (source_files),
            regex_matcher (boost::regex (regex), source_files, &n, errors));
    }

  n.list.clear ();

  create_file_list (source_files, n.list, n.loc);
  source_files.clear ();

  return false;
}


void 
resolve_wildcards::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
