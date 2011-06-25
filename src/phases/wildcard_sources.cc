#include "phase.h"

#include "algorithm/grep.h"
#include "annotations/error_log.h"
#include "annotations/file_list.h"

namespace
{
  struct wildcard_sources
    : visitor
  {
#include "node_visit.h"
    virtual void visit (token &n);
    virtual void visit (generic_node &n);

    bool in_sources;
    annotations::file_list const &files;
    annotations::error_log &errors;

    std::vector<generic_node_ptr> plainfiles;
    std::vector<generic_node_ptr> wildcards;

    wildcard_sources (annotation_map &annots)
      : in_sources (false)
      , files (annots.get ("files"))
      , errors (annots.get ("errors"))
    {
    }
  };

  static phase<wildcard_sources> thisphase ("wildcard_sources", "audit", 0);
}

bool wildcard_sources::visit_ac_check (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_arg_enable (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_alignof (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_cflags (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_functions (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_headers (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_library (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_code (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_define (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_description (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_destination (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_document (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_error (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_exclude (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_extra_dist (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_filenames (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_flags (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_identifiers (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_if (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_inheritance (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_library (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_link_body (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_link (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_program (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_project_member (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_project_members (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_project (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_rule_line (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_rule_lines (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_rule (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_section_members (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_section (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_sources_members (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_sourcesref (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_target_definition (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_target_members (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_template (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_tool_flags (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_vardecl (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_variable_content (nodes::generic_node &n) { return true; }
bool wildcard_sources::visit_variable (nodes::generic_node &n) { return true; }



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
wildcard_sources::visit_filename (nodes::generic_node &n)
{
  if (in_sources)
    {
      if (std::find_if (n.list.begin (), n.list.end (),
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

static fs::path &
concat_path (fs::path &p1, fs::path const &p2)
{
  return p1 /= p2;
}

static void
escape_wildcard (std::string &regex, std::string const &wc)
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

struct translate_wildcard
{
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
        escape_wildcard (regex, t.string);
        break;
      }
    return regex;
  }

  int in_alt;
};

struct regex_match
{
  explicit regex_match (boost::regex const &re, std::vector<fs::path> const &source_files, node_ptr const &node, annotations::error_log &errors)
    : re (re)
    , source_files (source_files)
    , node (node)
    , errors (errors)
  {
  }

  bool operator () (fs::path const &p) const
  {
    if (std::find (source_files.begin (), source_files.end (), p) != source_files.end ())
      return false;
    boost::cmatch results;
    bool matched = boost::regex_match (p.c_str (), results, re);
    if (matched)
      errors.add<warning> (node, "wildcard matched file: `" + p.native () + "'");
    return matched;
  }

  boost::regex const &re;
  std::vector<fs::path> const &source_files;
  node_ptr const &node;
  annotations::error_log &errors;
};

bool
wildcard_sources::visit_sources (nodes::generic_node &n)
{
  plainfiles.clear ();
  wildcards.clear ();

  in_sources = true;
  resume_list ();
  in_sources = false;

  //printf ("%lu wildcards, %lu plain files\n", wildcards.size (), plainfiles.size ());

  std::vector<fs::path> source_files;
  foreach (generic_node_ptr const &f, plainfiles)
    {
      std::string const &filename = std::accumulate (f->list.begin (),
                                                     f->list.end (),
                                                     std::string (),
                                                     concat_token);

      fs::path path;
      if (filename[0] == '/')
        {
          path = files.base / filename;
          if (!is_regular_file (path))
            path.clear ();
        }
      else
        {
          path = n.loc.file->parent_path () / filename;
          if (!is_regular_file (path))
            path.clear ();
        }
      if (path.empty ())
        {
          errors.add<semantic_error> (f, "could not find file `" + filename + "'");
          continue;
        }

      if (!std::equal (files.base.begin (),
                       files.base.end (),
                       path.begin ()))
        throw std::runtime_error ("source file found outside source root: `" + path.native () + "'");

      source_files.push_back (std::accumulate (path.begin (), path.end (), fs::path (), concat_path));
    }

  if (!wildcards.empty ())
    {
      std::string regex = "(";
      foreach (generic_node_ptr const &wc, wildcards)
        {
          if (wc->list.front ()->as<token> ().tok == TK_FN_SLASH)
            escape_wildcard (regex, files.base.native ());
          else
            escape_wildcard (regex, wc->loc.file->parent_path ().native () + "/");
          regex += std::accumulate (wc->list.begin (), wc->list.end (), std::string (), translate_wildcard ());
          if (wc != wildcards.back ())
            regex += '|';
        }
      regex += ")";

      grep (files.begin, files.end, std::back_inserter (source_files),
            regex_match (boost::regex (regex), source_files, &n, errors));
    }

  if (source_files.empty ())
    {
      errors.add<semantic_error> (&n, "no source files found");
      return false;
    }

  n.list.clear ();

  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;
  std::transform (source_files.begin (),
                  source_files.end (),
                  std::back_inserter (n.list),
                  new_<generic_node>
                  ( n_filename,
                    n.loc,
                    new_<token>
                    ( n.loc,
                      TK_FILENAME,
                      bind (&fs::path::native, arg1))));

  return false;
}


void
wildcard_sources::visit (token &n)
{
}

void 
wildcard_sources::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
