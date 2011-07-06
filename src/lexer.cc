#include "lexer.h"
#include "lexer/pimpl.h"
#include "rule_init.h"
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

lexer::lexer ()
  : impl (new pimpl ())
{
  yylex_init (&yyscanner);
  yyset_extra (this, yyscanner);
  yyset_in (NULL, yyscanner);
}

lexer::~lexer ()
{
  yylex_destroy (yyscanner);
}

void
lexer::init (int init, bool alternative)
{
  impl->init = init;
  impl->alternative = alternative;
}

int
lexer::INIT (int init, bool alternative)
{
  switch (init)
    {
    case r_filename:
      push_state (yy::FILENAME);
      if (alternative)
        push_state (yy::MULTIFILE);
      return R_FILENAME;
    }
  throw std::invalid_argument ("invalid rule init");
}

int
lexer::next (YYSTYPE *yylval, YYLTYPE *yylloc)
{
  if (impl->init)
    {
      int init = 0;
      std::swap (init, impl->init);
      
      bool alternative = false;
      std::swap (alternative, impl->alternative);

      return INIT (init, alternative);
    }
  int tok = lex (yylval, yylloc);
#if LEXER_VERBOSE
  if (tok)
    printf ("%-16s: \"%s\"\n", tokname (tok), yylval->token->string.c_str ());
#endif

  impl->T.next ();

  return tok;
}

void
lexer::lloc (YYLTYPE *yylloc, int &lineno, int &column, char const *text, int leng)
{
  assert (yylloc != NULL);
  assert (lineno >= 1);
  assert (column >= 0);
  assert (leng >= 1);
  assert (UINT_MAX - column - leng > INT_MAX);

  if (column == 0)
    column = 1;

  yylloc->file = current_file ();
  yylloc->first_line = lineno;
  yylloc->first_column = column;

  for (char const *p = text; p != text + leng; ++p)
    if (*p == '\n')
      {
        lineno++;
        column = 1;
      }
    else
      column++;

  yylloc->last_line = lineno;
  yylloc->last_column = column;

  impl->loc = yylloc;
}
