#pragma once

#include "lexer.h"

struct string_lexer
  : lexer
{
  string_lexer (std::string const &s);
  ~string_lexer ();

  virtual int wrap ();
  virtual fs::path const &current_file () const;

  std::string str;
};
