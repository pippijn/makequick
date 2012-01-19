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
inference_engine::prerequisite::file_t<boost::regex>::matches (fs::path const &file) const
{
  return regex_match (file.native (), data);
}

template<>
inline std::string
inference_engine::prerequisite::file_t<boost::regex>::stem (fs::path const &file) const
{
  boost::smatch matches;
  regex_match (file.native (), matches, data);
  return matches.str (1);
}
