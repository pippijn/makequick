#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


template<typename ForwardIterator>
static bool
is_sorted (ForwardIterator it, ForwardIterator et)
{
  ForwardIterator prev = it;
  while (++it != et)
    {
      if (*it < *prev)
        return false;
      prev = it;
    }
  return true;
}

struct resolve_sources_fs
  : symbol_visitor
{
  void visit (t_template &n) { /* ignore */ }
  void visit (t_filename &n);
  void visit (t_sources_members &n);

  bool in_sources;
  file_list const &files;
  error_log &errors;

  resolve_sources_fs (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , in_sources (false)
    , files (annots.get ("files"))
    , errors (annots.get ("errors"))
  {
    assert (is_sorted (files.begin, files.end));
  }
};

static phase<resolve_sources_fs> thisphase ("resolve_sources_fs", "inference");


static bool
try_resolve (file_list const &files,
             std::string &file, fs::path const &dir = fs::path ())
{
  fs::path const &path = file[0] == '/' ? file.c_str () + 1 : dir / file;
#if 0
  std::cout << "looking for " << path << "\n";
#endif
  if (binary_search (files.begin, files.end, path))
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
resolve_sources_fs::visit (t_filename &n)
{
  if (in_sources)
    {
      // only plain files, no wildcards
      if (n.size () == 1)
        {
          token &t = n[0]->as<token> ();
          fs::path file (t.string);
#if 0
          std::cout << "resolving " << file << "\n";
#endif

          fs::path CURDIR = proper_loc (n).file->parent_path ();
          if (generic_node_ptr curdir = symtab.lookup (T_VARIABLE, "CURDIR"))
            CURDIR = fs::path (id ((*curdir)[0]));

          bool absolute = t.string[0] == '/';
          std::string resolved = t.string;

          bool found = false;
          if (absolute)
            {
              resolved = resolved.substr (1);
              found = try_resolve (files, resolved);
            }
          else
            {
              found = try_resolve (files, resolved, CURDIR)
                   || try_resolve (files, resolved);
            }

          if (found)
            t.mutable_string = resolved;
        }
    }
}


void
resolve_sources_fs::visit (t_sources_members &n)
{
  in_sources = true;
  resume_list ();
  in_sources = false;
}
