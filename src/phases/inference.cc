#include <typeinfo>
#include "phase.h"

#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "colours.h"
#include "foreach.h"
#include "util/inference_engine.h"
#include "util/regex_escape.h"

using annotations::file_list;
using annotations::rule_info;

namespace
{
  struct inference
    : visitor
  {
#include "node_visit.h"
    virtual void visit (generic_node &n);

    enum rule_state
    {
      S_NONE,
      S_TARGET,
      S_PREREQ
    };

    inference_engine engine;
    rule_info &info;

    rule_state state;

    std::string target;
    std::vector<inference_engine::promise> prereq;

    inference (annotation_map &annots)
      : info (annots.put ("rule_info", new rule_info))
      , state (S_NONE)
    {
      file_list const &files = annots.get ("files");
      foreach (fs::path const &f, boost::make_iterator_range (files.begin, files.end))
        engine.add_file (files.rel (f));
    }

    ~inference ()
    {
      engine.infer ();
#if 0
      engine.print ();
#endif

      swap (info.files, engine.info.files);
      foreach (inference_engine::rule const &r, engine.info.rules)
        {
          using namespace boost::phoenix;
          using namespace boost::phoenix::arg_names;
          info.rules.push_back (rule_info::rule ());
          rule_info::rule &ri = info.rules.back ();
          ri.target = r.target;
          std::transform (r.prereqs.begin (),
                          r.prereqs.end (),
                          back_inserter (ri.prereq),
                          bind (&inference_engine::promise::str, arg1));
          ri.stem = r.stem;
          ri.code = r.code;
        }

#if 0
      if (!std::uncaught_exception ())
        throw "inference test";
#endif
    }
  };

  static phase<inference> thisphase ("inference", "concat_sources");
}

bool inference::visit_ac_check (nodes::generic_node &n) { return true; }
bool inference::visit_arg_enable_choice (nodes::generic_node &n) { return true; }
bool inference::visit_arg_enable_choices (nodes::generic_node &n) { return true; }
bool inference::visit_arg_enable (nodes::generic_node &n) { return true; }
bool inference::visit_check_alignof (nodes::generic_node &n) { return true; }
bool inference::visit_check_cflags (nodes::generic_node &n) { return true; }
bool inference::visit_check_functions (nodes::generic_node &n) { return true; }
bool inference::visit_check_headers (nodes::generic_node &n) { return true; }
bool inference::visit_check_library (nodes::generic_node &n) { return true; }
bool inference::visit_check_sizeof (nodes::generic_node &n) { return true; }
bool inference::visit_code (nodes::generic_node &n) { return true; }
bool inference::visit_define (nodes::generic_node &n) { return true; }
bool inference::visit_description (nodes::generic_node &n) { return true; }
bool inference::visit_destination (nodes::generic_node &n) { return true; }
bool inference::visit_document (nodes::generic_node &n) { return true; }
bool inference::visit_error (nodes::generic_node &n) { return true; }
bool inference::visit_exclude (nodes::generic_node &n) { return true; }
bool inference::visit_extra_dist (nodes::generic_node &n) { return true; }
bool inference::visit_filenames (nodes::generic_node &n) { return true; }
bool inference::visit_flags (nodes::generic_node &n) { return true; }
bool inference::visit_identifiers (nodes::generic_node &n) { return true; }
bool inference::visit_if (nodes::generic_node &n) { return true; }
bool inference::visit_inheritance (nodes::generic_node &n) { return true; }
bool inference::visit_library (nodes::generic_node &n) { return true; }
bool inference::visit_link_body (nodes::generic_node &n) { return true; }
bool inference::visit_link (nodes::generic_node &n) { return true; }
bool inference::visit_nodist_sources (nodes::generic_node &n) { return true; }
bool inference::visit_program (nodes::generic_node &n) { return true; }
bool inference::visit_project_member (nodes::generic_node &n) { return true; }
bool inference::visit_project_members (nodes::generic_node &n) { return true; }
bool inference::visit_project (nodes::generic_node &n) { return true; }
bool inference::visit_rule_line (nodes::generic_node &n) { return true; }
bool inference::visit_rule_lines (nodes::generic_node &n) { return true; }
bool inference::visit_rules (nodes::generic_node &n) { return true; }
bool inference::visit_section_members (nodes::generic_node &n) { return true; }
bool inference::visit_section (nodes::generic_node &n) { return true; }
bool inference::visit_sources_members (nodes::generic_node &n) { return true; }
bool inference::visit_sources (nodes::generic_node &n) { return true; }
bool inference::visit_sourcesref (nodes::generic_node &n) { return true; }
bool inference::visit_target_definition (nodes::generic_node &n) { return true; }
bool inference::visit_target_members (nodes::generic_node &n) { return true; }
bool inference::visit_template (nodes::generic_node &n) { return true; }
bool inference::visit_tool_flags (nodes::generic_node &n) { return true; }
bool inference::visit_toplevel_declaration (nodes::generic_node &n) { return true; }
bool inference::visit_toplevel_declarations (nodes::generic_node &n) { return true; }
bool inference::visit_vardecl_body (nodes::generic_node &n) { return true; }
bool inference::visit_vardecl (nodes::generic_node &n) { return true; }
bool inference::visit_variable_content (nodes::generic_node &n) { return true; }
bool inference::visit_variable (nodes::generic_node &n) { return true; }


static std::string
make_target (node_vec const &list)
{
  std::string target;
  foreach (node_ptr const &n, list)
    {
      token const &t = n->as<token> ();
      switch (t.tok)
        {
        case TK_FILENAME:
        case TK_FN_PERCENT:
          target += t.string;
          break;
        default:
          throw semantic_error (n, "invalid token `" + t.string + "' of type " + tokname (t.tok));
        }
    }

  return target;
}

static inference_engine::promise
make_prereq (node_vec const &list)
{
  bool is_re = false;
  std::string pr;
  foreach (node_ptr const &n, list)
    {
      token const &t = n->as<token> ();
      switch (t.tok)
        {
        case TK_FILENAME:
          if (is_re)
            pr += regex_escape (t.string);
          else
            pr += t.string;
          break;
        case TK_FN_PERCENT:
          if (!is_re)
            pr = regex_escape (pr);
          pr += "(.+)";
          is_re = true;
          break;
        case TK_FN_PERPERCENT:
          if (!is_re)
            pr = regex_escape (pr);
          pr += ".*?([^/]+)";
          is_re = true;
          break;
        default:
          throw tokname (t.tok);
        }
    }

  if (is_re)
    return boost::regex (pr);
  return pr;
}



bool
inference::visit_filename (nodes::generic_node &n)
{
  switch (state)
    {
    case S_NONE:
      break;
    case S_TARGET:
      target = make_target (n.list);
      break;
    case S_PREREQ:
      prereq.push_back (make_prereq (n.list));
      break;
    }

  return false;
}

bool
inference::visit_rule (nodes::generic_node &n)
{
  state = S_TARGET;
  n[0]->as<generic_node> ()[0]->accept (*this);
  state = S_PREREQ;
  n[1]->as<generic_node> ()[0]->accept (*this);
  state = S_NONE;

  engine.add_rule (target, prereq, n[2]);
  target.clear ();
  prereq.clear ();

  return false;
}

void 
inference::visit (generic_node &n)
{
  bool resume = false;
#include "node_switch.h"
  if (resume)
    resume_list ();
}
