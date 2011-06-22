#include "lexer.h"
#include "lexer/pimpl.h"
#include "yystate.h"

#include <cassert>
#include <climits>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

static char const *
strstate (int state)
{
  switch (state)
    {
    case yy::INITIAL    : return "in initial state";
    case yy::VAR_INIT   : return "after $";
    case yy::VAR_RBODY  : return "in variable body";
    case yy::VAR_SQBODY : return "in gvar body";
    case yy::RULE_INIT  : return "in rule declaration";
    case yy::RULE_CODE  : return "in rule code";
    case yy::RULE_LINE  : return "in rule line";
    case yy::FILENAME   : return "in filename";
    case yy::MULTIFILE  : return "in multi-rule wildcard";
    case yy::SOURCES    : return "in sources block";
    default             : return "<unknown>";
    }
}

lexer::lexer (std::string const &base, std::vector<std::string> const &files)
  : loc (0)
  , it (files.begin ())
  , et (files.end ())
  , base (base)
#if LEXER_TEST
  , it0 (it)
#endif
  , impl (new pimpl)
{
  yylex_init (&lex);
  yyset_extra (this, lex);
  yyset_in (NULL, lex);

  wrap ();
}

lexer::~lexer ()
{
  close_file ();
  yylex_destroy (lex);
}

bool
lexer::close_file ()
{
  if (FILE *oldfh = yyget_in (lex))
    {
      fclose (oldfh);
      yyset_in (NULL, lex);
      return true;
    }
  return false;
}

int
lexer::next (YYSTYPE *yylval, YYLTYPE *yylloc)
{
  int tok = yylex (yylval, yylloc, lex);
  if (tok)
    printf ("%-16s: \"%s\"\n", tokname (tok - 255), yylval->token->string.c_str ());
  return tok;
}

int
lexer::wrap ()
{
  if (state () != yy::INITIAL)
    {
      std::string msg = "end of file ";
      msg += strstate (state ());
      yyerror (loc, 0, msg.c_str ());
    }

  if (close_file ())
    yyset_lineno (1, lex);

  if (it == et)
    {
#if !LEXER_TEST
      return 1;
#else
      it = it0;
#endif
    }

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

  loc = yylloc;
}
