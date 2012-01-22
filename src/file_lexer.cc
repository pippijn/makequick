#include "file_lexer.h"
#include "annotations/file_list.h"
#include "lexer/pimpl.h"
#include "yystate.h"

#include <cassert>
#include <climits>
#include <stdexcept>

#include <boost/filesystem/path.hpp>

file_lexer::file_lexer (file_list const &files)
  : lexer ("parsing")
  , files (files)
  , cur (files.begin)
{
  if (wrap ())
    throw std::invalid_argument ("no source files found");
}

file_lexer::~file_lexer ()
{
  close_file ();
}

bool
file_lexer::close_file ()
{
  if (FILE *oldfh = yyget_in (yyscanner))
    {
      fclose (oldfh);
      yyset_in (NULL, yyscanner);
      return true;
    }
  return false;
}

static bool
is_rule_file (fs::path const &file)
{
  fs::path const &filename = file.filename ();
  return filename == "Rules.mq"
      || filename == "configure.mq";
}

int
file_lexer::wrap ()
{
  if (state () != yy::INITIAL)
    {
      std::string msg = "end of file ";
      msg += strstate (state ());
      yyerror (&current_location (), 0, msg.c_str ());
    }

  if (close_file ())
    yyset_lineno (1, yyscanner);

  while (cur != files.end && !is_rule_file (*cur))
    ++cur;

  if (cur == files.end)
    return 1;

  FILE *fh = fopen (files.absolute (*cur).c_str (), "r");
  if (!fh)
    throw std::runtime_error ("Could not open " + cur->native () + " for reading");
  ++cur;

  yyset_in (fh, yyscanner);
  return 0;
}

fs::path const &
file_lexer::current_file () const
{
  return cur[-1];
}
