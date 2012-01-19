template inference_engine::prerequisite::file_t<boost::regex>::file_t (boost::regex const &data);

template<>
inline void
inference_engine::prerequisite::file_t<boost::regex>::print () const
{
  std::cout << '{' << data << '}';
}

template<>
inline bool
inference_engine::prerequisite::file_t<boost::regex>::final () const
{
  return false;
}

template<>
inline bool
inference_engine::prerequisite::file_t<boost::regex>::stem (fs::path const &file, std::string &stem) const
{
  boost::smatch matches;
  bool matched = regex_match (file.native (), matches, data);
  if (matched)
    stem = matches.str (1);
  return matched;
}
