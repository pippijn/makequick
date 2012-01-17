#include "parseutil.h"
#include "parser.h"

#include "file_lexer.h"
#include "string_lexer.h"

static node_ptr
parse (lexer &lex, error_log &errors)
{
  return parser (lex, errors) ();
}


node_ptr
parse_files (file_list const &files, error_log &errors)
{
  file_lexer lex (files);
  return parse (lex, errors);
}

node_ptr
parse_string (std::string const &s, error_log &errors)
{
  string_lexer lex (s);
  return parse (lex, errors);
}
