#pragma once

#include "util/timer.h"
#include "ylcode.h"

#include <memory>
#include <vector>

#include <boost/filesystem_fwd.hpp>

#define LEXER_VERBOSE 0
#define PARSER_BENCH 0

struct lexer
{
  lexer (char const *name);
  ~lexer ();

  int INIT (int init, bool alternative);
  void init (int init, bool alternative);

  int lex (YYSTYPE *yylval, YYLTYPE *yylloc);
  int next (YYSTYPE *yylval, YYLTYPE *yylloc);
  void lloc (YYLTYPE *yylloc, int &lineno, int &column, char const *text, int leng);

  virtual int wrap () = 0;
  virtual fs::path const *current_file () const = 0;

  static char const *STRSTATE (int state);
  static char const *strstate (int state);
  int state () const;
  void push_state (int state);
  void pop_state ();

  tokens::token const *curtok () const;

  void *yyscanner;

  struct pimpl;
  std::auto_ptr<pimpl> const impl;
};
