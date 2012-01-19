template inference_engine::prerequisite::file_t<std::string>::file_t (std::string const &data);

template<>
inline void
inference_engine::prerequisite::file_t<std::string>::print () const
{
  std::cout << '"' << data << '"';
}

template<>
inline bool
inference_engine::prerequisite::file_t<std::string>::final () const
{
  return true;
}

template<>
inline bool
inference_engine::prerequisite::file_t<std::string>::matches (fs::path const &file) const
{
  return file == data;
}

template<>
inline bool
inference_engine::prerequisite::file_t<std::string>::stem (fs::path const &file, std::string &stem) const
{
  std::string const &str = native (file);
  if (str == data)
    {
      stem = str;
      return true;
    }
  return false;
}
