#pragma once

#include <string>

struct shstr
{
  static std::string const &intern (char const *text, size_t leng);

  shstr (char const *text, size_t leng)
    : s (intern (text, leng))
  {
  }

  char const *c_str () const { return s.c_str (); }

  std::string const &s;
};
