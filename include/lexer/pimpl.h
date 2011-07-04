struct lexer::pimpl
{
  typedef std::vector<fs::path const *> file_vec;

  std::string text;

  file_vec::const_iterator it;
  file_vec::const_iterator et;


  template<short Tok> bool is_variable_token ();
  template<short Tok> int make_token (YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);

  pimpl (file_vec const &files)
    : it (files.begin ())
    , et (files.end ())
  {
  }
};
