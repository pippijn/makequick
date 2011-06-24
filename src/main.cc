#include "config.h"

#include "lexer.h"
#include "parser.h"
#include "phases.h"
#include "sighandler.h"

#include <clocale>
#include <cstdlib>

#if LEXER_BENCH
#include <sys/time.h>
#endif

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#define ONESHOT 0
#define LEXER_TEST 0
#define PARSER_BENCH 0

namespace fs = boost::filesystem;

static void
collect (fs::path const &path, std::vector<std::string> &files)
{
  if (is_directory (path))
    {
      std::for_each (fs::directory_iterator (path),
		     fs::directory_iterator (),
                     boost::bind (collect, _1, boost::ref (files)));
    }
  if (path.extension () == ".mq")
    files.push_back (path.string ());
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

  std::vector<std::string> files;
  collect (path, files);
  std::sort (files.begin (), files.end ());

#if 0
  std::copy (files.begin (), files.end (), std::ostream_iterator<std::string> (std::cout, "\n"));
#endif

#if PARSER_BENCH
  for (int i = 0; i < 10; i++)
    {
      lexer lex (path.string (), files);
      parser parse (lex);
      parse ();
    }
  return EXIT_SUCCESS;
#endif

  lexer lex (path.string (), files);
#if LEXER_TEST
  YYSTYPE yylval;
  YYLTYPE yylloc;
  while (int token = lex.next (&yylval, &yylloc))
    {
    }
  return EXIT_SUCCESS;
#endif

#if LEXER_BENCH
  int i = 0;
  timeval start;
  gettimeofday (&start, 0);
  double max_per_sec = 0;
  YYSTYPE yylval;
  YYLTYPE yylloc;
  while (int token = lex.next (&yylval, &yylloc))
    {
      if (should_terminate)
        return EXIT_FAILURE;
      if (++i == 1000000)
        {
          timeval now, sub;
          gettimeofday (&now, 0);
          timersub (&now, &start, &sub);
          double per_sec = i / (sub.tv_sec + double (sub.tv_usec) / 1000000);
          max_per_sec = std::max (per_sec, max_per_sec);
          printf ("%d tokens in %ld.%06ld seconds (%.0f tokens per second, max=%.0f)\n", i, sub.tv_sec, sub.tv_usec,
                  per_sec, max_per_sec);
          i = 0;
          start = now;
#if ONESHOT
          return EXIT_SUCCESS;
#endif
        }
    }
#endif

  parser parse (lex);

  if (node_ptr doc = parse ())
    {
      phases::run ("audit", doc);
      phases::run ("xml", doc);
    }
  else
    {
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
catch (std::exception const &e)
{
  printf ("error: %s\n", e.what ());
  return EXIT_FAILURE;
}
