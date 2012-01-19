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
inference_engine::prerequisite::file_t<wildcard>::stem (fs::path const &file, std::string &stem) const
{
  std::string::const_iterator p = data.wc.begin ();
  std::string::const_iterator end = data.wc.end ();

  char const *f = file.c_str ();
  char const *stem_begin = f;
  char const *stem_end = f;
  while (p != end)
    {
      switch (char c = *p)
        {
        case '%':
          // TODO: fails to match moo.cow.c with %.c
          c = *++p;
          stem_begin = f;
          while (*f && *f != c)
            f++;
          stem_end = f;
          break;
        default:
          if (*f != c)
            return false;
          break;
        }
      ++f;
      ++p;
    }

  stem.assign (stem_begin, stem_end);

  return !*f;
}
