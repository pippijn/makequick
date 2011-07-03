#pragma once

#include "ylcode.h"

#include <memory>
#include <vector>

#include <boost/filesystem_fwd.hpp>

#define LEXER_VERBOSE 0
#define PARSER_BENCH 0

#if EXTERN_TEMPLATE
extern template class std::vector<fs::path>;
#endif

struct lexer
{
  lexer (std::vector<fs::path> const &files);
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

  struct pimpl;
  std::auto_ptr<pimpl> const impl;
};
