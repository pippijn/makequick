#include "lexer.h"
#include "lexer/pimpl.h"
#include "yystate.h"

#include <cassert>
#include <climits>
#include <stdexcept>

#include <boost/filesystem/path.hpp>

char const *
lexer::STRSTATE (int state)
{
  switch (state)
    {
    case yy::INITIAL    : return "INITIAL";
    case yy::VAR_INIT   : return "VAR_INIT";
    case yy::VAR_RBODY  : return "VAR_RBODY";
    case yy::VAR_SQBODY : return "VAR_SQBODY";
    case yy::RULE_INIT  : return "RULE_INIT";
    case yy::RULE_CODE  : return "RULE_CODE";
    case yy::RULE_LINE  : return "RULE_LINE";
    case yy::FILENAME   : return "FILENAME";
    case yy::MULTIFILE  : return "MULTIFILE";
    case yy::SOURCES    : return "SOURCES";
    case yy::LINK       : return "LINK";
    case yy::VARDECL    : return "VARDECL";
    case yy::FLAGS      : return "FLAGS";
    default             : return "<unknown>";
    }
}

char const *
lexer::strstate (int state)
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
    case yy::LINK       : return "in link section";
    case yy::VARDECL    : return "in variable declaration";
    case yy::FLAGS      : return "in tool flags";
    default             : return "<unknown>";
    }
}

lexer::lexer (std::vector<fs::path> const &files)
  : loc (0)
  , impl (new pimpl (files))
{
  yylex_init (&yyscanner);
  yyset_extra (this, yyscanner);
  yyset_in (NULL, yyscanner);

  if (wrap ())
    throw std::invalid_argument ("no source files found");
}

lexer::~lexer ()
{
  close_file ();
  yylex_destroy (yyscanner);
}

bool
lexer::close_file ()
{
  if (FILE *oldfh = yyget_in (yyscanner))
    {
      fclose (oldfh);
      yyset_in (NULL, yyscanner);
      return true;
    }
  return false;
}

#include "timing.h"

static timer T;

int
lexer::next (YYSTYPE *yylval, YYLTYPE *yylloc)
{
  int tok = lex (yylval, yylloc);
#if LEXER_VERBOSE
  if (tok)
    printf ("%-16s: \"%s\"\n", tokname (tok), yylval->token->string.c_str ());
#endif

  T.next ();

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
    yyset_lineno (1, yyscanner);

  if (impl->it == impl->et)
    return 1;

  FILE *fh = fopen (impl->it->c_str (), "r");
  if (!fh)
    throw std::runtime_error ("Could not open " + impl->it->string () + " for reading");
  ++impl->it;

  yyset_in (fh, yyscanner);
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

  column++;

  yylloc->file = &impl->it[-1];
  yylloc->first_line = lineno;
  yylloc->first_column = column;
  yylloc->last_line = lineno;
  yylloc->last_column = column + leng;

  loc = yylloc;
}
