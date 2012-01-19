#include "config.h"

#include "annotations/output_file.h"
#include "annotations/file_list.h"
#include "annotations/error_log.h"
#include "annotation_map.h"
#include "parseutil.h"
#include "phases.h"
#include "sighandler.h"
#include "util/foreach.h"
#include "util/timer.h"

#include <cstdio>
#include <fstream>
#include <typeinfo>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

static char const *const all_phases[] = {
  "audit",
  "insert_target_syms",
  "inheritance",
  "remove_templates",
  "default_prereq",
  "default_destdir",
  "insert_vardecl_syms",
  "insert_varadd_syms",
  "expand_vars",
  "flatten_vars",
  "reparse_vars",
  "multirule",
  "inference",
  "flatten_filenames",
  "resolve_sources",
  "resolve_wildcards",
  "infer_target_objects",
  "instantiate_rules",
  "squarevars",
  //"merge_blocks",
  "sx",
};

static fs::path
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
  if (is_directory (path) && path.filename ().c_str ()[0] != '.')
    for_each (fs::directory_iterator (path),
              fs::directory_iterator (),
              bind (collect, _1, boost::ref (files)));
  else if (is_regular_file (path))
    files.push_back (fs::path (path));
}

struct unbase
{
  unbase (fs::path const &base)
    : base (base.native ())
  {
  }

  void operator () (fs::path &path)
  {
    std::string const &n = path.native ();
    assert (n.substr (0, base.length ()) == base);
    path = n.substr (base.length () + 1);
  }

  std::string base;
};

static void
load_store (node_ptr &doc, bool text = false)
{
  node::compress_hash ();
  {
    timer T ("store");
    printf ("%%%% storing %lu nodes\n", node::node_count ());
    std::ofstream os ("nodes.bin");
    node::store (os, doc, text);
  }
  doc = 0;
  {
    timer T ("load");
    std::ifstream is ("nodes.bin");
    doc = node::load (is, text);
    printf ("%%%% loaded %lu nodes\n", node::node_count ());
  }
}

static void
usage ()
{
  puts ("usage: "PACKAGE_TARNAME" [--phase...] <srcdir>");
  puts ("  available phases:");
  phases::print ();
}

static void
SX (node &n)
{
  phases::run ("sx", &n);
}

int
main (int argc, char *argv[])
try
{
  char const *const mem_low = (char const *)sbrk (0);

  ++argv;

  if (!argv[0] || !strcmp (argv[0], "--help"))
    {
      usage ();
      return argv[0] ? EXIT_SUCCESS : EXIT_FAILURE;
    }

  if (!strcmp (argv[0], "--version"))
    {
      puts (PACKAGE_STRING);
      return EXIT_SUCCESS;
    }

  std::vector<char const *> to_run;
  while (argv[0] && !strncmp (argv[0], "--", 2))
    {
      to_run.push_back (argv[0] + 2);
      ++argv;
    }

  if (!argv[0])
    {
      usage ();
      return EXIT_FAILURE;
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

  std::vector<fs::path> filevec;
  filevec.push_back (path / "Rules.mq");
  filevec.push_back (path / "configure.mq");
  collect (path / "extra", filevec);
  collect (path / "include", filevec);
  collect (path / "src", filevec);
  collect (path / "testsuite", filevec);
  for_each (filevec.begin (), filevec.end (), unbase (path));
  sort (filevec.begin (), filevec.end ());
  std::auto_ptr<file_list> files (new file_list (path, filevec.begin (), filevec.end ()));

#if 0
  copy (files->begin, files->end, std::ostream_iterator<fs::path> (std::cout, "\n"));
  return EXIT_SUCCESS;
#endif

  std::auto_ptr<error_log> errors (new error_log);

  if (node_ptr doc = parse_files (*files, *errors))
    {
      //load_store (doc);

      annotation_map annots;
      annots.put ("files", files.release ());
#if 0
      annots.put ("output", new output_file ((path / ".project" / "makepp.am").c_str (), "configure.out"));
#endif
      annots.put ("errors", errors.release ());

      error_log &errors = annots.get ("errors");

      if (to_run.empty ())
        to_run.assign (all_phases, all_phases + sizeof all_phases / sizeof *all_phases);

      try
      {
        if (!to_run.empty ())
          foreach (char const *phase, to_run)
            phases::run (phase, doc, annots);
        else
          phases::run (doc, annots);
      }
      catch (...)
      {
        errors.print (path, argv[0]);
        throw;
      }

      errors.print (path, argv[0]);
      if (errors.has_errors ())
        return EXIT_FAILURE;

      if (to_run.empty ())
        {
          //phases::run ("print", doc, annots);
          //phases::run ("xml", doc, annots);
          phases::run ("sx", doc, annots);
        }
    }
  else
    {
      errors->print (path, argv[0]);
      return EXIT_FAILURE;
    }

  char const *const mem_high = (char const *)sbrk (0);
  printf ("%%%% memory used: %ld MiB\n", (mem_high - mem_low) / 1024 / 1024);

  return should_terminate;
}
catch (std::exception const &e)
{
  printf ("\e[1;31m%%%% runtime error (T=%s)\e[0m: %s\n", typeid (e).name (), e.what ());
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
