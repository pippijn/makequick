#include <typeinfo>
#include "phase.h"

#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "foreach.h"
#include "util/inference_engine.h"
#include "util/regex_escape.h"

#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/home/phoenix.hpp>

using annotations::file_list;
using annotations::rule_info;

struct inference
  : visitor
{
  void visit (t_filename &n);
  void visit (t_rule &n);

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
#if 1
    engine.print ();
    if (!std::uncaught_exception ())
      throw "inference test";
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
  }
};

static phase<inference> thisphase ("inference", "concat_sources", "resolve_vars", "multirule");


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



void
inference::visit (t_filename &n)
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
