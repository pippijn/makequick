static std::string
move (std::string &s)
{
  std::string r = s;
  s.clear ();
  return r;
}

static std::string
escaped (char const *text, int leng)
{
  std::string s;

  while (leng--)
    switch (char c = *text++)
      {
      case '\t': s += "\\t"; break;
      case '\n': s += "\\n"; break;
      case '\v': s += "\\v"; break;
      case '\f': s += "\\f"; break;
      default:   s += c;     break;
      }

  return s;
}


#define YY_USER_ACTION		lloc (yylloc, yylineno, yycolumn, yytext, yyleng);

#define MakeKeyword(TOK)	impl->make_keyword (TOK, yylval, yylloc, yytext, yyleng)
#define PushKeyword(TOK)	impl->push_keyword (TOK, yylval, yylloc, yytext, yyleng)
#define RetKeyword(TOK)		return MakeKeyword (TOK)

#define MakeToken(TOK)		impl->make_token (TOK, yylval, yylloc, yytext, yyleng)
#define PushToken(TOK)		impl->push_token (TOK, yylval, yylloc, yytext, yyleng)
#define RetToken(TOK)		return MakeToken (TOK)

#define PUSH(STATE)		push_state (STATE)
#define POP()			pop_state ()
#define SWITCH(STATE)		switch_state (STATE)

#define BACKTRACK(N) do {		\
  yycolumn -= yyleng - N;		\
  impl->profiler.bytes -= yyleng - N;	\
  yyless (N);				\
} while (0)

#define YY_DECL int lexer::lex (YYSTYPE *yylval_param, YYLTYPE *yylloc_param)
