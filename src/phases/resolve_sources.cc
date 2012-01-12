#include "phase.h"

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

struct resolve_sources
  : visitor
{
  void visit (t_filename &n);
  void visit (t_sources_members &n);
#if 0
  void visit (t_toplevel_declarations &n)
  {
    visitor::visit (n);
    annotation_map annots;
    phases::run ("print", &n, annots);
  }
#endif

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

          if (!try_resolve (files, rule_info.files, t.mutable_string, n.loc.file->parent_path ()))
            if (!try_resolve (files, rule_info.files, t.mutable_string, files.base))
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
