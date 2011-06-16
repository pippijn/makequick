#pragma once

#include "ylcode.h"

#include <vector>

struct lexer
{
  lexer (std::string const &base, std::vector<std::string> const &files);
  ~lexer ();

  int wrap ();
  void lloc (YYLTYPE *yylloc, int lineno, int column, int leng);

  void *lex;

  std::string const base;
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator et;
};
