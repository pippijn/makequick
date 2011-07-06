#include "lexer.h"
#include "lexer/pimpl.h"
#include "yystate.h"

#include <cassert>
#include <climits>
#include <stdexcept>

#include <boost/filesystem/path.hpp>

file_lexer::file_lexer (std::vector<fs::path const *> const &files)
  : it (files.begin ())
  , et (files.end ())
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

int
file_lexer::wrap ()
{
  if (state () != yy::INITIAL)
    {
      std::string msg = "end of file ";
      msg += strstate (state ());
      yyerror (impl->loc, 0, msg.c_str ());
    }

  if (close_file ())
    yyset_lineno (1, yyscanner);

  if (it == et)
    return 1;

  FILE *fh = fopen ((*it)->c_str (), "r");
  if (!fh)
    throw std::runtime_error ("Could not open " + (*it)->string () + " for reading");
  ++it;

  yyset_in (fh, yyscanner);
  return 0;
}

fs::path const *
file_lexer::current_file () const
{
  return it[-1];
}
