#include "config.h"

#include "annotations/file_list.h"
#include "annotations/error_log.h"
#include "lexer.h"
#include "parser.h"
#include "phases.h"
#include "sighandler.h"

static void
collect (fs::path const &path, std::vector<fs::path> &files)
{
  using boost::ref;
  if (is_directory (path))
    {
      std::for_each (fs::directory_iterator (path),
		     fs::directory_iterator (),
                     boost::bind (collect, _1, ref (files)));
    }
  else if (is_regular_file (path))
    files.push_back (path);
}

int
main (int argc, char *argv[])
try
{
  if (argc < 2)
    return EXIT_FAILURE;

  if (!strcmp (argv[1], "--version"))
    {
      puts (PACKAGE_NAME " v" PACKAGE_VERSION);
      return EXIT_SUCCESS;
    }

  if (!strcmp (argv[1], "--help"))
    {
      puts ("usage: jmlc <jmlfile>");
      return EXIT_SUCCESS;
    }

  fs::path const path (argv[1]);
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
  std::sort (files.begin (), files.end ());

#if 0
  std::copy (files.begin (), files.end (), std::ostream_iterator<fs::path> (std::cout, "\n"));
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

  lexer lex (files);
  parser parse (lex);

  if (node_ptr doc = parse ())
    {
      using namespace annotations;

      annotation_map annots;
      annots.put ("files", new file_list (path, files.begin (), files.end ()));
      error_log &errors = annots.put ("errors", new error_log);
      phases::run (doc, annots);
      {
        int count = 0;
        foreach (semantic_error const &e, errors.log)
          {
            if (count++ > 10)
              {
                puts ("too many diagnostics; exiting");
                break;
              }
            puts (e.what ());
          }
      }
      if (!errors.has_errors ())
        phases::run ("xml", doc, annots);
      else
        return EXIT_FAILURE;
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
