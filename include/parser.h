#pragma once

#include "lexer.h"
#include "node.h"

namespace annotations
{
  struct error_log;
}

struct parser
{
  parser (lexer &lex, annotations::error_log &errors);
  ~parser ();

  node_ptr operator () ();

  lexer &lex;
  annotations::error_log &errors;
  node_ptr doc;
};
