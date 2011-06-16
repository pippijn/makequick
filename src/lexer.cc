#include "lexer.h"

#include <cassert>
#include <climits>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

lexer::lexer (std::string const &base, std::vector<std::string> const &files)
  : it (files.begin ())
  , et (files.end ())
  , base (base)
{
  yylex_init (&lex);
  yyset_extra (this, lex);

  wrap ();
}

lexer::~lexer ()
{
  yylex_destroy (lex);
}

int
lexer::wrap ()
{
  if (FILE *oldfh = yyget_in (lex))
    {
      fclose (oldfh);
      yyset_lineno (1, lex);
    }

  if (it == et)
    return 1;

  FILE *fh = fopen (it->c_str (), "r");
  if (!fh)
    throw std::runtime_error ("Could not open " + *it + " for reading");
  ++it;

  yyset_in (fh, lex);
  return 0;
}

void
lexer::lloc (YYLTYPE *yylloc, int lineno, int column, int leng)
{
  assert (yylloc != NULL);
  assert (lineno >= 1);
  assert (column >= 0);
  assert (leng >= 1);
  assert (UINT_MAX - column - leng > INT_MAX);

  if (column == 0)
    column = 1;

  yylloc->file = &it[-1];
  yylloc->first_line = lineno;
  yylloc->first_column = column;
  yylloc->last_column = column + leng;
  yylloc->last_line = lineno;
}
