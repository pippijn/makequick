#include "phase.h"

#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "util/foreach.h"
#include "util/inference_engine.h"
#include "util/regex_escape.h"
#include "util/object_name.h"

#include <numeric>

#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>

#define EXPERIMENTAL_WILDCARD_CODE 1

struct inference
  : visitor
{
  void visit (t_filename &n);
  void visit (t_rule &n);

  void visit (t_target_definition &n);

  enum state
  {
    S_NONE,
    S_TARGET,
    S_PREREQ
  } state;

  typedef std::vector<inference_engine::prerequisite> prereq_vec;

  inference_engine engine;
  rule_info &rules;

  std::string target;
  prereq_vec prereq;

  inference (annotation_map &annots)
    : rules (annots.put ("rule_info", new rule_info))
    , state (S_NONE)
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
        std::vector<fs::path> prereq;
        prereq.reserve (r.prereqs.size ());
        foreach (inference_engine::prerequisite const &pr, r.prereqs)
          prereq.push_back (pr.str ());
        assert (prereq.size () == r.prereqs.size ());

        rules.rules.insert (rule_info::rule (r.target, prereq, r.stem, r.code));
      }
  }
};

static phase<inference> thisphase ("inference", "reparse_vars");


static enum inference::state &
operator |= (enum inference::state &a, enum inference::state b)
{
  return a = (enum inference::state) (int (a) | int (b));
}

static std::string
make_target (node_vec const &list, bool allow_wildcards = true)
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
          if (t.tok != TK_FN_PERCENT || allow_wildcards)
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
#if EXPERIMENTAL_WILDCARD_CODE
    return tok != TK_FILENAME && tok != TK_FN_PERCENT;
#else
    return tok != TK_FILENAME;
#endif
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
  symbol_type type;
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
}

void
inference::visit (t_rule &n)
{
  state = S_TARGET;
  n.target ()->accept (*this);
  state = S_PREREQ;
  n.prereq ()->accept (*this);
  state = S_NONE;

  engine.add_rule (target, prereq, n.code ());
  target.clear ();
  prereq.clear ();
}


void
inference::visit (t_target_definition &n)
{
  engine.add_file (id (n.name ()));
  visitor::visit (n);
}
