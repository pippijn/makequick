struct lexer::pimpl
{
  struct deferred_token
  {
    int token;
    YYLTYPE lloc;
    std::string text;

    deferred_token (short token, YYLTYPE const &lloc, char const *text, int leng)
      : token (token)
      , lloc (lloc)
      , text (text, leng)
    {
    }

    deferred_token (short token, YYLTYPE const &lloc)
      : token (token)
      , lloc (lloc)
    {
    }
  };

  std::string text;
  std::vector<deferred_token> deferred;

  YYLTYPE *loc;
  lex_timer profiler;
  int indent;

  pimpl (char const *name)
    : profiler (name)
  {
  }

  template<short Tok> bool is_variable_token ();
  void push_keyword (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);
  int  make_keyword (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);
  void push_token (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);
  int  make_token (int tok, YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng);

  int get_deferred (YYSTYPE *lval, YYLTYPE *lloc);

private:
  static void verbose (int tok, char const *type, YYLTYPE const *lloc, char const *text, int leng);
};
