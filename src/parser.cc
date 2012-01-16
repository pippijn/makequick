#include "parser.h"

#include "annotations/error_log.h"
#include "exception.h"

#include <boost/filesystem/path.hpp>

parser::parser (lexer &lex, error_log &errors)
  : lex (lex)
  , errors (errors)
  , doc (0)
{
}

parser::~parser ()
{
}


node_ptr
parser::operator () ()
{
  extern int yyparse (parser *parse);
  yyparse (this);
  return doc;
}


void
yyerror (YYLTYPE const *llocp, parser *parser, char const *msg)
{
#if 0
  printf ("%s:%d:%d: error: \"%s\"\n",
          llocp->file ? llocp->file->c_str () : "<unknown>",
          llocp->first_line,
          llocp->first_column,
          msg);
#endif
  tokens::token_ptr tok = new tokens::token (*llocp, TK_ERROR, "invalid character");
  parser->errors.add<syntax_error> (tok, msg);
}
