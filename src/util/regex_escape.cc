#include "util/regex_escape.h"

void
regex_escape (std::string &regex, std::string const &wc)
{
  foreach (char c, wc)
   switch (c)
     {
     case '(':
     case ')':
     case '.':
       regex += '\\';
     default:
       regex += c;
       break;
     }
}

std::string
regex_escape (std::string const &wc)
{
  std::string regex;
  regex_escape (regex, wc);
  return regex;
}
