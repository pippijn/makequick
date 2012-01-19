typedef inference_engine::wildcard wildcard;

template inference_engine::prerequisite::file_t<wildcard>::file_t (wildcard const &data);

template<>
inline void
inference_engine::prerequisite::file_t<wildcard>::print () const
{
  std::cout << '{' << data.wc << '}';
}

template<>
inline bool
inference_engine::prerequisite::file_t<wildcard>::final () const
{
  return false;
}

template<>
inline bool
inference_engine::prerequisite::file_t<wildcard>::matches (fs::path const &file) const
{
  assert (!"unimplemented");
}

template<>
inline bool
inference_engine::prerequisite::file_t<wildcard>::stem (fs::path const &file, std::string &stem) const
{
  assert (!"unimplemented");
}
