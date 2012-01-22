static inline std::string
uc (std::string const &s)
{
  std::string uc;
  uc.reserve (s.size ());
  transform (s.begin (), s.end (), back_inserter (uc), toupper);
  return uc;
}
