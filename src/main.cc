#include "config.h"

#include "lexer.h"
#include "parser.h"
#include "nopvisitor.h"

#include <clocale>
#include <cstdlib>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

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

#if 0
  std::copy (files.begin (), files.end (), std::ostream_iterator<std::string> (std::cout, "\n"));
#endif

  lexer lex (path.string (), files);
  parser parse (lex);

  if (node_ptr doc = parse ())
    {
      nopvisitor nop;
      doc->accept (nop);
    }
  else
    {
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
