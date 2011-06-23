struct lexer::pimpl
{
  std::string text;

  template<short Tok> bool is_variable_token ();
  template<short Tok> int make_token (YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);
};
