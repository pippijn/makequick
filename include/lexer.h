#pragma once

#include "ylcode.h"

#include <memory>
#include <vector>

#define LEXER_BENCH 0

struct lexer
{
  lexer (std::string const &base, std::vector<std::string> const &files);
  ~lexer ();

  bool close_file ();

  int lex (YYSTYPE *yylval, YYLTYPE *yylloc);
  int next (YYSTYPE *yylval, YYLTYPE *yylloc);
  int wrap ();
  void lloc (YYLTYPE *yylloc, int lineno, int column, int leng);

  static char const *STRSTATE (int state);
  static char const *strstate (int state);
  int state () const;
  void push_state (int state);
  void pop_state ();


  void *yyscanner;
  YYLTYPE *loc;

  std::string const base;
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator et;

  struct pimpl;
  std::auto_ptr<pimpl> const impl;

#if LEXER_BENCH
  std::vector<std::string>::const_iterator it0;
#endif
};
