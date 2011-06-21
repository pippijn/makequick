#include "shstr.h"

#include <tr1/unordered_set>

typedef std::tr1::unordered_set<std::string> HT;

static HT pool;

std::string const &
shstr::intern (char const *text, size_t leng)
{
  std::string const s (text, leng);

  HT::const_iterator found = pool.find (s);
  if (found == pool.end ())
    found = pool.insert (s).first;

  return *found;
}
