#include "lexer.h"
#include "lexer/pimpl.h"
#include "lexer/util.h"
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
    case yy::INITIAL     : return "INITIAL";
    case yy::INITWS      : return "INITWS";
    case yy::EXCLUDE     : return "EXCLUDE";
    case yy::FILENAME    : return "FILENAME";
    case yy::FLAGS       : return "FLAGS";
    case yy::IMPORT      : return "IMPORT";
    case yy::LINK        : return "LINK";
    case yy::MULTIFILE   : return "MULTIFILE";
    case yy::RULE_CODE   : return "RULE_CODE";
    case yy::RULE_INIT   : return "RULE_INIT";
    case yy::RULE_LINE   : return "RULE_LINE";
    case yy::RULE_LINES  : return "RULE_LINES";
    case yy::VARDECL_CODE: return "VARDECL_CODE";
    case yy::VARDECL_INIT: return "VARDECL_INIT";
    case yy::VARDECL_LINE: return "VARDECL_LINE";
    case yy::VARDECL_NAME: return "VARDECL_NAME";
    case yy::VARDECL     : return "VARDECL";
    case yy::VAR_INIT    : return "VAR_INIT";
    case yy::VAR_RBODY   : return "VAR_RBODY";
    case yy::VAR_SQBODY  : return "VAR_SQBODY";
    default              : return "<unknown>";
    }
}

char const *
lexer::strstate (int state)
{
  switch (state)
    {
    case yy::INITIAL     : return "in initial state";
    case yy::INITWS      : return "in whitespace before rule";
    case yy::EXCLUDE     : return "in sources exclude";
    case yy::FILENAME    : return "in filename";
    case yy::FLAGS       : return "in tool flags";
    case yy::IMPORT      : return "in sources import";
    case yy::LINK        : return "in link section";
    case yy::MULTIFILE   : return "in multi-rule wildcard";
    case yy::RULE_CODE   : return "in rule code";
    case yy::RULE_INIT   : return "in rule declaration";
    case yy::RULE_LINE   : return "in rule line continuation";
    case yy::RULE_LINES  : return "in rule line";
    case yy::VARDECL_CODE: return "in variable declaration value";
    case yy::VARDECL_INIT: return "in whitespace before variable declaration";
    case yy::VARDECL_LINE: return "in single line variable declaration value";
    case yy::VARDECL_NAME: return "in variable declaration name";
    case yy::VARDECL     : return "in single line variable declaration name";
    case yy::VAR_INIT    : return "after $";
    case yy::VAR_RBODY   : return "in variable body";
    case yy::VAR_SQBODY  : return "in gvar body";
    default              : return "<unknown>";
    }
}

lexer::lexer (char const *name)
  : impl (new pimpl (name))
{
  yylex_init (&yyscanner);
  yyset_extra (this, yyscanner);
  yyset_in (NULL, yyscanner);
}

lexer::~lexer ()
{
  yylex_destroy (yyscanner);
}

int
lexer::next (YYSTYPE *yylval, YYLTYPE *yylloc)
{
  int tok = impl->get_deferred (yylval, yylloc);
  if (tok == 0)
    tok = lex (yylval, yylloc);

  impl->profiler.next ();

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

  yylloc->file = &current_file ();
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
  impl->profiler.bytes += leng;
}

YYLTYPE const &
lexer::current_location () const
{
  return *impl->loc;
}


int
lexer::pimpl::get_deferred (YYSTYPE *lval, YYLTYPE *lloc)
{
  if (deferred.empty ())
    return 0;

  deferred_token const &token = deferred.back ();
  int tok = token.token;
  lval->token = token.text.empty () ? NULL : new tokens::token (token.lloc, token.token, token.text);
  *lloc = token.lloc;
  deferred.pop_back ();

  return tok;
}


#define LEXER_VERBOSE 0

void
lexer::pimpl::verbose (int tok, char const *type, YYLTYPE const *lloc, char const *text, int leng)
{
#if LEXER_VERBOSE
  printf ("[%d:%d-%d:%d]: %s <<%s>> (%s)\n",
          lloc->first_line,
          lloc->first_column,
          lloc->last_line,
          lloc->last_column,
	  tokname (tok),
          escaped (text, leng).c_str (),
          type);
#endif
}


void
lexer::pimpl::push_keyword (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng)
{
  verbose (tok, "deferred keyword", lloc, text, leng);

  assert (this->text.empty ());
  deferred.push_back (deferred_token (tok, *lloc));
}

int
lexer::pimpl::make_keyword (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng)
{
  verbose (tok, "immediate keyword", lloc, text, leng);

  assert (this->text.empty ());
  lval->token = NULL;
  return tok;
}


void
lexer::pimpl::push_token (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng)
{
  verbose (tok, "deferred token", lloc, text, leng);

  deferred.push_back (deferred_token (tok, *lloc, text, leng));
}

int
lexer::pimpl::make_token (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng)
{
  std::string token_text = this->text.empty ()
                         ? std::string (text, leng)
			 : move (this->text);
  verbose (tok, "immediate token", lloc, token_text.data (), token_text.length ());

  assert (!token_text.empty ());
  lval->token = new tokens::token (*lloc, tok, token_text);
  return tok;
}
