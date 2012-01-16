#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "colours.h"
#include "foreach.h"
#include "util/create_file_list.h"
#include "util/regex_escape.h"

#include <numeric>

#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

struct resolve_wildcards
  : visitor
{
  void visit (t_filename &n);
  void visit (t_sources_members &n);

  bool in_sources;
  file_list const &files;
  error_log &errors;

  std::vector<fs::path> source_files;
  generic_node_vec wildcards;

  resolve_wildcards (annotation_map &annots)
    : in_sources (false)
    , files (annots.get ("files"))
    , errors (annots.get ("errors"))
  {
  }
};

static phase<resolve_wildcards> thisphase ("resolve_wildcards", "resolve_sources");


//Copy_if was dropped from the standard library by accident.
template<typename In, typename Out, typename Pred>
Out
copy_if (In first, In last, Out res, Pred Pr)
{
  while (first != last)
    {
      if (Pr (*first))
        *res++ = *first;
      ++first;
    }
  return res;
}

void
resolve_wildcards::visit (t_filename &n)
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
        regex_escape (regex, t.string);
        break;
      }
    return regex;
  }

  int in_alt;
};

struct regex_matcher
{
  explicit regex_matcher (boost::regex const &re, std::vector<fs::path> const &source_files, node_ptr const &node, error_log &errors)
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
  error_log &errors;
};

void
resolve_wildcards::visit (t_sources_members &n)
{
  if (!wildcards.empty ())
    throw std::runtime_error ("found wildcards somewhere outside sources");

  in_sources = true;
  resume_list ();
  in_sources = false;

  //printf ("%lu wildcards, %lu source files\n", wildcards.size (), source_files.size ());

  if (!wildcards.empty ())
    {
      std::string regex = "(";
      foreach (generic_node_ptr const &wc, wildcards)
        {
          if (wc->list.front ()->as<token> ().tok == TK_FN_SLASH)
            regex_escape (regex, files.base.native ());
          else
            regex_escape (regex, wc->loc.file->parent_path ().native () + "/");
          regex += accumulate (wc->list.begin (),
                               wc->list.end (),
                               std::string (),
                               translate_wildcard ());
          if (wc != wildcards.back ())
            regex += '|';
        }
      wildcards.clear ();
      regex += ")";

      copy_if (files.begin, files.end,
               back_inserter (source_files),
               regex_matcher (boost::regex (regex), source_files, &n, errors));
    }

  n.list.clear ();

  create_file_list (source_files, n.list, n.loc);
  source_files.clear ();
}
