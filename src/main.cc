#include "config.h"

#include "algorithm/grep.h"
#include "annotations/file_list.h"
#include "annotations/error_log.h"
#include "annotation_map.h"
#include "foreach.h"
#include "lexer.h"
#include "parser.h"
#include "phases.h"
#include "sighandler.h"

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

static bool
is_rule_file (fs::path const &file)
{
  return file.extension () == ".mq";
}

int
main (int argc, char *argv[])
try
{
  if (!argv[1] || !strcmp (argv[1], "--help"))
    {
      puts ("usage: "PACKAGE_TARNAME" <srcdir>");
      return argv[1] ? EXIT_SUCCESS : EXIT_FAILURE;
    }

  if (!strcmp (argv[1], "--version"))
    {
      puts (PACKAGE_STRING);
      return EXIT_SUCCESS;
    }

  fs::path const path (resolve (absolute (fs::path (argv[1]))));
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
  sort (files.begin (), files.end ());

#if 0
  copy (files.begin (), files.end (), std::ostream_iterator<fs::path> (std::cout, "\n"));
  return EXIT_SUCCESS;
#endif

#if PARSER_BENCH
  for (int i = 0; i < 20; i++)
    {
      lexer lex (files);
      parser parse (lex);
      parse ();
      if (should_terminate)
        return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
#endif

  std::vector<fs::path> rule_files;
  grep (files.begin (), files.end (), back_inserter (rule_files), is_rule_file);
  lexer lex (rule_files);
  parser parse (lex);

  if (node_ptr doc = parse ())
    {
      using namespace annotations;

      annotation_map annots;
      annots.put ("files", new file_list (path, files.begin (), files.end ()));
      annots.put ("errors", new error_log);

      phases::run (doc, annots);

      error_log &errors = annots.get ("errors");
      errors.print (path, argv[1]);
      if (errors.has_errors ())
        return EXIT_FAILURE;

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
