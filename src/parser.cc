#include "parser.h"

#include "exception.h"

#include <boost/filesystem/path.hpp>

parser::parser (lexer &lex)
  : lex (lex)
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
  printf ("%s:%d:%d: error: \"%s\"\n",
          llocp->file ? llocp->file->c_str () : "<unknown>",
          llocp->first_line,
          llocp->first_column,
          msg);
  throw syntax_error (parser->lex.curtok (), msg);
}
