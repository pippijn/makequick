#include "phase.h"

#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "util/foreach.h"
#include "util/inference_engine.h"
#include "util/regex_escape.h"

#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/home/phoenix.hpp>

struct inference
  : visitor
{
  void visit (t_filename &n);
  void visit (t_rule &n);

  void visit (t_target_definition &n);
  void visit (t_program &n);
  void visit (t_library &n);

  enum rule_state
  {
    SR_NONE,
    SR_TARGET,
    SR_PREREQ
  };

  enum target_state
  {
    ST_NONE,
    ST_PROGRAM,
    ST_LIBRARY
  };

  inference_engine engine;
  rule_info &rules;

  rule_state rstate;
  target_state tstate;

  std::string target;
  std::vector<inference_engine::prerequisite> prereq;

  inference (annotation_map &annots)
    : rules (annots.put ("rule_info", new rule_info))
    , rstate (SR_NONE)
    , tstate (ST_NONE)
  {
    file_list const &files = annots.get ("files");
    foreach (fs::path const &f, boost::make_iterator_range (files.begin, files.end))
      engine.add_file (f);
  }

  ~inference ()
  {
    engine.infer ();
#if 0
    engine.print ();
    if (!std::uncaught_exception ())
      throw "inference test";
#endif

    swap (rules.files, engine.info.files);
    foreach (inference_engine::rule const &r, engine.info.rules)
      {
        using namespace boost::phoenix;
        using namespace boost::phoenix::arg_names;
        rules.rules.push_back (rule_info::rule ());
        rule_info::rule &ri = rules.rules.back ();
        ri.target = r.target;
        transform (r.prereqs.begin (),
                   r.prereqs.end (),
                   back_inserter (ri.prereq),
                   bind (&inference_engine::prerequisite::str, arg1));
        ri.stem = r.stem;
        ri.code = r.code;
      }
  }
};

static phase<inference> thisphase ("inference", "reparse_vars");


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

struct make_prerequisite
{
  static bool needs_regex (node_ptr const &n)
  {
    int tok = n->as<token> ().tok;
    return tok != TK_FILENAME;// && tok != TK_FN_PERCENT;
  }

  static bool needs_wildcard (node_ptr const &n)
  {
    int tok = n->as<token> ().tok;
    return tok != TK_FILENAME;
  }
};

struct make_string
{
  std::string &operator () (std::string &pr, node_ptr const &n)
  {
    token const &t = n->as<token> ();
    pr += t.string;
    return pr;
  }
};

struct make_regex
{
  make_regex ()
    : in_multi (false)
  {
  }

  bool in_multi;

  std::string &operator () (std::string &pr, node_ptr const &n)
  {
    token const &t = n->as<token> ();
    switch (t.tok)
      {
      case TK_FILENAME:
        pr += regex_escape (t.string);
        if (in_multi)
          pr += "|";
        break;
      case TK_FN_PERCENT:
        assert (!in_multi);
        pr += "(.+)";
        break;
      case TK_FN_PERPERCENT:
        assert (!in_multi);
        pr += ".*?([^/]+)";
        break;
      case TK_FN_LBRACE:
        assert (!in_multi);
        pr += "(?:";
        in_multi = true;
        break;
      case TK_FN_RBRACE:
        assert (in_multi);
        in_multi = false;
        pr[pr.size () - 1] = ')';
        break;
      default:
        throw tokname (t.tok);
      }
    return pr;
  }
};

template<typename Maker>
static std::string
make_prereq (node_vec const &list)
{
  return accumulate (list.begin (), list.end (), std::string (), Maker ());
}

static inference_engine::prerequisite
make_prereq (node_vec const &list)
{
  if (find_if (list.begin (), list.end (), make_prerequisite::needs_regex) != list.end ())
    return boost::regex (make_prereq<make_regex> (list));

  std::string pr = make_prereq<make_string> (list);

  if (find_if (list.begin (), list.end (), make_prerequisite::needs_wildcard) != list.end ())
    return inference_engine::wildcard (pr);

  return pr;
}



void
inference::visit (t_filename &n)
{
  switch (rstate)
    {
    case SR_NONE:
      break;
    case SR_TARGET:
      target = make_target (n.list);
      break;
    case SR_PREREQ:
      prereq.push_back (make_prereq (n.list));
      break;
    }
}

void
inference::visit (t_rule &n)
{
  rstate = SR_TARGET;
  n.target ()->accept (*this);
  rstate = SR_PREREQ;
  n.prereq ()->accept (*this);
  rstate = SR_NONE;

  engine.add_rule (target, prereq, n.code ());
  target.clear ();
  prereq.clear ();
}


void
inference::visit (t_target_definition &n)
{
  engine.add_file (n.name ()->as<token> ().string);
  visitor::visit (n);
}

void
inference::visit (t_program &n)
{
  tstate = ST_PROGRAM;
  visitor::visit (n);
}

void
inference::visit (t_library &n)
{
  tstate = ST_LIBRARY;
  visitor::visit (n);
}
