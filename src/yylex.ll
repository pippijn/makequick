%{
#include "lexer.h"

#define SELF static_cast<lexer *> (yyget_extra (yyscanner))

#define YY_USER_ACTION					\
  {							\
    SELF->lloc (yylloc, yylineno, yycolumn, yyleng);	\
    if (*yytext == '\n')				\
      yycolumn = 1;					\
    else						\
      yycolumn += yyleng;				\
  }

#define Return(TOK)					\
  do {							\
    yylval->token					\
      = new nodes::token (TOK, yytext, yyleng);		\
    return TOK;						\
  } while (0)						\

%}

%option prefix="yy"
%option header-file="yylex.h"
%option bison-locations
%option reentrant
%option yylineno
%option nounput noinput nounistd
%option never-interactive

WS	[ \t\v\n\r]
ID	[^ \t\v\n\r]

%%
"#".*					{ }
{WS}+					{ }

"if"					{ Return (TK_STRING); }
"cppflags"				{ Return (TK_STRING); }
"extra_dist"				{ Return (TK_STRING); }
"link"					{ Return (TK_STRING); }
"build"					{ Return (TK_STRING); }
"custom"				{ Return (TK_STRING); }
"program"				{ Return (TK_STRING); }
"library"				{ Return (TK_STRING); }
"rule"					{ Return (TK_STRING); }
"sources"				{ Return (TK_STRING); }
"nodist_sources"			{ Return (TK_STRING); }

"{"					{ Return (TK_STRING); }
"}"					{ Return (TK_STRING); }
"->"					{ Return (TK_STRING); }

\"(\\.|[^\\"])*\"			{ Return (TK_STRING); }
\'(\\.|[^\\'])*\'			{ Return (TK_STRING); }

{ID}+					{ Return (TK_STRING); }
%%

int
yywrap (yyscan_t yyscanner)
{
  return SELF->wrap ();
}
