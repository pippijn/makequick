struct lexer::pimpl
{
  std::string text;

  YYLTYPE *loc;
  lex_timer T;
  int init;
  bool alternative;
  tokens::token *curtok;

  pimpl (char const *name)
    : T (name)
  {
  }

  template<short Tok> bool is_variable_token ();
  template<short Tok> int make_token (YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);
};
