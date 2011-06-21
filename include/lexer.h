#pragma once

#include "ylcode.h"

#include <vector>

#define LEXER_TEST 0

struct lexer
{
  lexer (std::string const &base, std::vector<std::string> const &files);
  ~lexer ();

  bool close_file ();

  int next (YYSTYPE *yylval, YYLTYPE *yylloc);
  int cond ();
  int wrap ();
  void lloc (YYLTYPE *yylloc, int lineno, int column, int leng);

  void *lex;
  YYLTYPE *loc;

  std::string const base;
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator et;

#if LEXER_TEST
  std::vector<std::string>::const_iterator it0;
#endif
};
