#pragma once

#include "lexer.h"
#include "node.h"

struct error_log;

struct parser
{
  parser (lexer &lex, error_log &errors);
  ~parser ();

  node_ptr operator () ();

  lexer &lex;
  error_log &errors;
  node_ptr doc;
};
