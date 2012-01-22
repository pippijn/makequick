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
  typedef std::string::const_iterator iterator;
  typedef std::string::const_reverse_iterator reverse_iterator;

  std::string const &fname = file.native ();

  iterator p   = data.wc.begin ();
  iterator end = data.wc.end ();

  iterator f = fname.begin ();

  while (p != end)
    {
      // when we find a wildcard, we start looking from the back
      if (*p == '%')
        {
          reverse_iterator rp   = data.wc.rbegin ();
          reverse_iterator rend = rp + (end - p) - 1;

          reverse_iterator rf = fname.rbegin ();

          while (rp != rend)
            {
              if (*rf != *rp)
                return false;

              ++rf;
              ++rp;
            }

          stem.assign (f, fname.begin () + (fname.rend () - rf));
          return true;
        }

      if (*f != *p)
        return false;

      ++f;
      ++p;
    }

  return f == fname.end ();
}
