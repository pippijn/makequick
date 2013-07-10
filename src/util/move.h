#pragma once

static inline std::string
move (std::string &s)
{
  std::string r = s;
  s.clear ();
  return r;
}
