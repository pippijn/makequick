#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "annotations/rule_info.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

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

struct resolve_sources
  : symbol_visitor
{
  void visit (t_filename &n);
  void visit (t_sources_members &n);

  bool in_sources;
  file_list const &files;
  struct rule_info const &rule_info;
  error_log &errors;

  resolve_sources (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , in_sources (false)
    , files (annots.get ("files"))
    , rule_info (annots.get ("rule_info"))
    , errors (annots.get ("errors"))
  {
    assert (is_sorted (files.begin, files.end));
    assert (is_sorted (rule_info.files.begin (), rule_info.files.end ()));
  }
};

static phase<resolve_sources> thisphase ("resolve_sources", "inference");


static bool
try_resolve (file_list const &files,
             std::vector<fs::path> const &buildable,
             std::string &file, fs::path const &dir = fs::path ())
{
  fs::path const &path = file[0] == '/' ? file.c_str () + 1 : dir / file;
#if 0
  std::cout << "looking for " << path << "\n";
#endif
  if (exists (path) || binary_search (buildable.begin (), buildable.end (), path))
    {
      file = (dir / file).native ();
      return true;
    }

  return false;
}

static location
proper_loc (node &n)
{
  if (n.loc.file->c_str ()[0] != '<')
    return n.loc;
  assert (n.parent ());
  return proper_loc (*n.parent ());
}

void
resolve_sources::visit (t_filename &n)
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

          fs::path CURDIR = proper_loc (n).file->parent_path ();
          if (generic_node_ptr curdir = symtab.lookup (T_VARIABLE, "CURDIR"))
            CURDIR = (*curdir)[0]->as<token> ().string;

          bool absolute = t.mutable_string[0] == '/';

          bool found = false;
          if (absolute)
            {
              t.mutable_string = t.string.substr (1);
              found = try_resolve (files, rule_info.files, t.mutable_string);
            }
          else
            {
              found = try_resolve (files, rule_info.files, t.mutable_string, CURDIR)
                   || try_resolve (files, rule_info.files, t.mutable_string);
            }

          if (!found)
            errors.add<semantic_error> (&n, "could not resolve file: " + C::filename (t.string),
                                        "not in file system and no rule to build it");
        }
    }
}


void
resolve_sources::visit (t_sources_members &n)
{
  in_sources = true;
  resume_list ();
  in_sources = false;
}
