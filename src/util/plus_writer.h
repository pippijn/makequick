#include <tr1/unordered_set>

struct plus_writer
{
  struct target_pair
    : std::pair<std::string, std::string>
  {
    target_pair (std::string const &dest, std::string const &kind)
      : pair (dest, kind)
    {
    }

    char const *dest () const { return first.c_str (); }
    char const *kind () const { return second.c_str (); }

    struct hash
    {
      size_t operator () (target_pair const &p) const
      {
        std::tr1::hash<std::string> hash;
        return hash (p.first) ^ hash (p.second);
      }
    };
  };

  std::tr1::unordered_set<target_pair, target_pair::hash> seen;

  char const *plus (target_pair const &pair)
  {
    if (seen.find (pair) != seen.end ())
      return "+";
    seen.insert (pair);
    return "";
  }

  char const *plus (std::string const &dest, std::string const &kind)
  {
    return plus (target_pair (dest, kind));
  }

  void plus (FILE *out, target_pair const &pair)
  {
    if (pair.first.empty ())
      fprintf (out, "%s %s= ", pair.kind (), plus (pair));
    else
      fprintf (out, "%s_%s %s= ", pair.dest (), pair.kind (), plus (pair));
  }

  void plus (FILE *out, std::string const &dest, std::string const &kind)
  {
    plus (out, target_pair (dest, kind));
  }
};
