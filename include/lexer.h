#pragma once

#include "util/timer.h"
#include "ylcode.h"

#include <memory>
#include <vector>

#include <boost/filesystem_fwd.hpp>

#define PARSER_BENCH 0

extern "C" int yywrap (yyscan_t yyscanner);

struct lexer
{
  lexer (char const *name);
  ~lexer ();

  int next (YYSTYPE *yylval, YYLTYPE *yylloc);

  virtual fs::path const &current_file () const = 0;

  tokens::token const *curtok () const;

protected:
  static char const *STRSTATE (int state);
  static char const *strstate (int state);
  int state () const;

  YYLTYPE const &current_location () const;

  void *yyscanner;

private:
  friend int yyparse (parser *parse);
  void push_state (int state);
  void switch_state (int state);
  void pop_state ();

  void lloc (YYLTYPE *yylloc, int &lineno, int &column, char const *text, int leng);
  int lex (YYSTYPE *yylval, YYLTYPE *yylloc);

  friend int yywrap (yyscan_t yyscanner);
  virtual int wrap () = 0;

  struct pimpl;
  std::auto_ptr<pimpl> const impl;
};
