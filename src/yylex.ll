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

#define PUSH(STATE)	yy_push_state (STATE, yyscanner)
#define POP()		yy_pop_state (yyscanner)

%}

%option prefix="yy"
%option header-file="yylex.h"
%option bison-locations
%option reentrant
/*%option ecs full 8bit*/
%option yylineno stack
%option nounput noinput nounistd
%option never-interactive

%x VAR_INIT
%x VAR_RBODY VAR_SQBODY
%x RULE
%x STRING

WS	[ \t\v\n\r]
ID	[a-zA-Z_-][a-zA-Z0-9_-]*
DIGIT	[0-9]

%%
"#".*					{ }
{WS}+					{ }

"if"					{ Return (TK_KEYWORD); }
"alignof"				{ Return (TK_KEYWORD); }
"arg_enable"				{ Return (TK_KEYWORD); }
"arg_with"				{ Return (TK_KEYWORD); }
"c_bigendian"				{ Return (TK_KEYWORD); }
"c_charset"				{ Return (TK_KEYWORD); }
"c_enum_fwdecl"				{ Return (TK_KEYWORD); }
"c_float_format"			{ Return (TK_KEYWORD); }
"c_late_expansion"			{ Return (TK_KEYWORD); }
"c_stdint_h"				{ Return (TK_KEYWORD); }
"c_token_paste"				{ Return (TK_KEYWORD); }
"c_typeof"				{ Return (TK_KEYWORD); }
"cflags"				{ Return (TK_KEYWORD); }
"config_header"				{ Return (TK_KEYWORD); }
"contact"				{ Return (TK_KEYWORD); }
"cppflags"				{ Return (TK_KEYWORD); }
"define"				{ Return (TK_KEYWORD); }
"exclude"				{ Return (TK_KEYWORD); }
"extra_dist"				{ Return (TK_KEYWORD); }
"functions"				{ Return (TK_KEYWORD); }
"header"				{ Return (TK_KEYWORD); }
"headers"				{ Return (TK_KEYWORD); }
"library"				{ Return (TK_KEYWORD); }
"link"					{ Return (TK_KEYWORD); }
"nodist_sources"			{ Return (KW_NODIST_SOURCES); }
"options"				{ Return (TK_KEYWORD); }
"program"				{ Return (TK_KEYWORD); }
"project"				{ Return (TK_KEYWORD); }
"section"				{ Return (TK_KEYWORD); }
"sizeof"				{ Return (TK_KEYWORD); }
"sources"				{ Return (KW_SOURCES); }
"symbol"				{ Return (TK_KEYWORD); }
"template"				{ Return (TK_KEYWORD); }
"version"				{ Return (TK_KEYWORD); }

<INITIAL,STRING>{
	"$"				{ PUSH (VAR_INIT); Return (TK_OPERATOR); }
	"->"				{ Return (TK_OPERATOR); }
	"=>"				{ Return (TK_OPERATOR); }
	":"				{ Return (TK_OPERATOR); }
	"<"				{ Return (TK_OPERATOR); }
	"@"				{ Return (TK_OPERATOR); }
	"*"				{ Return (TK_OPERATOR); }
	"["				{ Return (TK_OPERATOR); }
	"]"				{ Return (TK_OPERATOR); }
	"("				{ Return (TK_OPERATOR); }
	")"				{ Return (TK_OPERATOR); }
	"{"				{ Return (TK_OPERATOR); }
	"}"				{ Return (TK_OPERATOR); }
	"="				{ Return (TK_OPERATOR); }

	\'(\\.|[^\\'])*\'		{ Return (TK_STRING); }

	/* rules start */
	"%"				{ PUSH (RULE); Return (TK_OPERATOR); }
}

<RULE>{
	"/"				{ Return (TK_OPERATOR); }
	"."				{ Return (TK_OPERATOR); }
	"{"				{ Return (TK_OPERATOR); }
	"}"				{ POP (); Return (TK_OPERATOR); }
}

<VAR_INIT>{
	{WS}				{ yyerror (yylloc, 0, "whitespace after $"); }
	"("				{ BEGIN (VAR_RBODY); Return (TK_OPERATOR); }
	"["				{ BEGIN (VAR_SQBODY); Return (TK_OPERATOR); }
	"<"				{ POP (); Return (TK_OPERATOR); }
	"@"				{ POP (); Return (TK_OPERATOR); }
	"*"				{ POP (); Return (TK_OPERATOR); }
}
<VAR_RBODY>{
	{WS}				{ }
	"."				{ Return (TK_OPERATOR); }
	":"				{ Return (TK_OPERATOR); }
	")"				{ POP (); Return (TK_OPERATOR); }
	{ID}				{ Return (TK_IDENTIFIER); }
}
<VAR_SQBODY>{
	"]"				{ POP (); Return (TK_OPERATOR); }
	{ID}				{ Return (TK_IDENTIFIER); }
}

\"					{ PUSH (STRING); Return (TK_STRING); }
<STRING>{
	\\\"				{ Return (TK_STRING); }
	\"				{ POP (); Return (TK_STRING); }
	{WS}+				{ Return (TK_STRING); }
}


<INITIAL,STRING>{
	{ID}				{ Return (TK_IDENTIFIER); }
	{DIGIT}+			{ Return (TK_INTEGER); }
}

<*>.					{ yyerror (yylloc, 0, yytext); }
%%

int
yywrap (yyscan_t yyscanner)
{
  return SELF->wrap ();
}

int
lexer::cond ()
{
  yyguts_t *yyg = (yyguts_t *)lex;
  return YY_START;
}
