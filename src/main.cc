#include "config.h"

#include "algorithm/grep.h"
#include "annotations/file_list.h"
#include "annotations/error_log.h"
#include "annotation_map.h"
#include "foreach.h"
#include "parseutil.h"
#include "phases.h"
#include "sighandler.h"

#include <cstdio>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

fs::path
resolve (fs::path const &p)
{
  fs::path result;

  for (fs::path::iterator it = p.begin ();
       it != p.end ();
       ++it)
    {
      if (*it == "..")
        {
          // /a/b/.. is not necessarily /a if b is a symbolic link
          if (is_symlink (result))
            result /= *it;
          // /a/b/../.. is not /a/b/.. under most circumstances
          // We can end up with ..s in our result because of symbolic links
          else if (result.filename () == "..")
            result /= *it;
          // Otherwise it should be safe to resolve the parent
          else
            result = result.parent_path ();
        }
      else if (*it == ".")
        {
          // Ignore
        }
      else
        // Just cat other path entries
        result /= *it;
    }
  return result;
}

static void
collect (fs::path const &path, std::vector<fs::path> &files)
{
  if (is_directory (path))
    for_each (fs::directory_iterator (path),
              fs::directory_iterator (),
              boost::bind (collect, _1, boost::ref (files)));
  else if (is_regular_file (path))
    files.push_back (path);
}

struct base_sort
{
  base_sort (fs::path const &base)
    : offset (base.native ().length () + 1)
  {
  }

  bool operator () (fs::path const &a, fs::path const &b)
  {
#if 1
    return a < b;
#else
    return strcmp (a.c_str () + offset, b.c_str () + offset) < 0;
#endif
  }

  size_t const offset;
};

int
main (int argc, char *argv[])
try
{
  ++argv;

  if (!argv[0] || !strcmp (argv[0], "--help"))
    {
      puts ("usage: "PACKAGE_TARNAME" [phases...] <srcdir>");
      puts ("  available phases:");
      phases::print ();
      return argv[0] ? EXIT_SUCCESS : EXIT_FAILURE;
    }

  if (!strcmp (argv[0], "--version"))
    {
      puts (PACKAGE_STRING);
      return EXIT_SUCCESS;
    }

  std::vector<char const *> to_run;
  while (!strncmp (argv[0], "--", 2))
    {
      to_run.push_back (argv[0] + 2);
      ++argv;
    }

  fs::path const path (resolve (absolute (fs::path (argv[0]))));
  if (!exists (path))
    {
      puts ("path does not exist");
      return EXIT_FAILURE;
    }

  if (!is_directory (path))
    {
      puts ("path is not a directory");
      return EXIT_FAILURE;
    }

  std::vector<fs::path> files;
  collect (path, files);
  sort (files.begin (), files.end (), base_sort (path));

#if 0
  copy (files.begin (), files.end (), std::ostream_iterator<fs::path> (std::cout, "\n"));
  return EXIT_SUCCESS;
#endif

  if (node_ptr doc = parse_files (files))
    {
      using namespace annotations;

      annotation_map annots;
      annots.put ("files", new file_list (path, files.begin (), files.end ()));
      annots.put ("errors", new error_log);

      if (!to_run.empty ())
        foreach (char const *phase, to_run)
          phases::run (phase, doc, annots);
      else
        {
          phases::run (doc, annots);
        }

      error_log &errors = annots.get ("errors");
      errors.print (path, argv[0]);
      if (errors.has_errors ())
        return EXIT_FAILURE;

      if (to_run.empty ())
        phases::run ("xml", doc, annots);
    }
  else
    return EXIT_FAILURE;

  return should_terminate;
}
catch (std::exception const &e)
{
  printf ("\e[1;31m%%%% runtime error\e[0m: %s\n", e.what ());
  return EXIT_FAILURE;
}
catch (int i)
{
  printf ("\e[1;31m%%%% runtime error\e[0m: %d\n", i);
  return EXIT_FAILURE;
}
catch (char const *s)
{
  printf ("\e[1;31m%%%% runtime error\e[0m: %s\n", s);
  return EXIT_FAILURE;
}
