#pragma once

#include <vector>

#include <sys/time.h>

struct timer
{
  timer (char const *name);
  ~timer ();

  char const *name;
  timeval start;
};

struct lex_timer
  : timer
{
  typedef std::vector<timeval> timervec;

  lex_timer ();
  ~lex_timer ();

  void next ();

  timervec times;
  timeval prev;
};
