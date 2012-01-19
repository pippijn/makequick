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
  std::string::const_iterator it = data.wc.begin ();
  std::string::const_iterator et = data.wc.end ();

  // TODO: finish this
  char const *f = file.c_str ();
  while (it != et)
    {
      switch (char c = *it)
        {
        case '%':
          {
            char next = *++it;
            while (*f != next && *f != '\0')
              f++;
            if (*f != next)
              return false;
            break;
          }
        default:
          if (*f != c)
            return false;
          break;
        }
      ++f;
      ++it;
    }

  return *f == '\0';
}

template<>
inline std::string
inference_engine::prerequisite::file_t<wildcard>::stem (fs::path const &file) const
{
  std::string stem;

  std::string::const_iterator it = data.wc.begin ();
  std::string::const_iterator et = data.wc.end ();

  char const *f = file.c_str ();
  while (it != et)
    {
      switch (char c = *it)
        {
        case '%':
          {
            char next = *++it;
            while (*f != next && *f != '\0')
              stem += *f++;
            assert (*f == next);
            break;
          }
        default:
          assert (*f == c);
          break;
        }
      ++f;
      ++it;
    }

  return stem;
}
