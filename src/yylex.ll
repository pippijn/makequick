%{
#include "lexer.h"

static std::string
move (std::string &s)
{
  std::string r = s;
  s.clear ();
  return r;
}

#define SELF static_cast<lexer *> (yyget_extra (yyscanner))

#define YY_USER_ACTION					\
  {							\
    SELF->lloc (yylloc, yylineno, yycolumn, yyleng);	\
    if (*yytext == '\n')				\
      yycolumn = 1;					\
    else						\
      yycolumn += yyleng;				\
  }

#define APPEND() SELF->text.append (yytext, yyleng)

#define Return(TOK)					\
  do {							\
    if (!SELF->text.empty ())				\
      yylval->token					\
        = new nodes::token (TOK, move (SELF->text));	\
    else						\
      yylval->token					\
        = new nodes::token (TOK, yytext, yyleng);	\
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
%x RULE RULE_INIT RULE_CODE
%x STRING FILENAME MULTIFILE SOURCES

/* Whitespace */
WS	[ \t\v\n\r]
/* Filenames */
FN	[^ \t\v\n\r{}]
/* Identifiers */
ID	[a-zA-Z_-][a-zA-Z0-9_-]*
/* Numbers */
DIGIT	[0-9]

%%
"#".*					{ }
{WS}+					{ }

"if"					{ Return (KW_IF); }
"alignof"				{ Return (KW_ALIGNOF); }
"arg_enable"				{ Return (KW_ARG_ENABLE); }
"arg_with"				{ Return (KW_ARG_WITH); }
"c_bigendian"				{ Return (KW_C_BIGENDIAN); }
"c_charset"				{ Return (KW_C_CHARSET); }
"c_enum_fwdecl"				{ Return (KW_C_ENUM_FWDECL); }
"c_float_format"			{ Return (KW_C_FLOAT_FORMAT); }
"c_late_expansion"			{ Return (KW_C_LATE_EXPANSION); }
"c_stdint_h"				{ Return (KW_C_STDINT_H); }
"c_token_paste"				{ Return (KW_C_TOKEN_PASTE); }
"c_typeof"				{ Return (KW_C_TYPEOF); }
"cflags"				{ Return (KW_CFLAGS); }
"config_header"				{ Return (KW_CONFIG_HEADER); }
"contact"				{ Return (KW_CONTACT); }
"cppflags"				{ Return (KW_CPPFLAGS); }
"define"				{ Return (KW_DEFINE); }
"exclude"				{ Return (KW_EXCLUDE); }
"extra_dist"				{ Return (KW_EXTRA_DIST); }
"functions"				{ Return (KW_FUNCTIONS); }
"header"				{ Return (KW_HEADER); }
"headers"				{ Return (KW_HEADERS); }
"library"				{ Return (KW_LIBRARY); }
"link"					{ Return (KW_LINK); }
"nodist_sources"			{ Return (KW_NODIST_SOURCES); }
"options"				{ Return (KW_OPTIONS); }
"program"				{ Return (KW_PROGRAM); }
"project"				{ Return (KW_PROJECT); }
"section"				{ Return (KW_SECTION); }
"sizeof"				{ Return (KW_SIZEOF); }
"sources"				{ Return (KW_SOURCES); }
"symbol"				{ Return (KW_SYMBOL); }
"template"				{ Return (KW_TEMPLATE); }
"version"				{ Return (KW_VERSION); }

{FN}+":"				{ PUSH (RULE_INIT); Return (TK_FILENAME); }
<RULE_INIT>{
	{WS}+				{ }
	{FN}+				{ POP (); Return (TK_FILENAME); }
}
<RULE_CODE>{
	[^}]+				{ Return (TK_STRING); }
	"}"				{ Return (TK_RBRACE); }
}

<INITIAL,STRING>{
	"$"				{ PUSH (VAR_INIT); Return (TK_OPERATOR); }
	"->"				{ Return (TK_OPERATOR); }
	"=>"				{ Return (TK_OPERATOR); }
	":"				{ Return (TK_OPERATOR); }
	"{"				{ Return (TK_LBRACE); }
	"}"				{ Return (TK_RBRACE); }
	"="				{ Return (TK_OPERATOR); }

	\'(\\.|[^\\'])*\'		{ Return (TK_STRING); }

	/* rules start */
	"%"				{ PUSH (RULE); Return (TK_OPERATOR); }
}

<SOURCES>{
	{WS}+				{ }
	{FN}+				{ APPEND (); PUSH (FILENAME); }
	"{"				{ yyless (0); PUSH (FILENAME); }
	"}"				{ Return (TK_RBRACE); }
}
<FILENAME>{
	{WS}+				{ POP (); Return (TK_FILENAME); }
	{FN}+				{ APPEND (); }
	"{"				{ PUSH (MULTIFILE); APPEND (); }
}
<MULTIFILE>{
	{WS}+				{ SELF->text.append (" ", 1); }
	{FN}+				{ APPEND (); }
	"}"				{ APPEND (); POP (); }
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
lexer::state () const
{
  yyguts_t *yyg = (yyguts_t *)lex;
  return YY_START;
}

void
lexer::push_state (int cond)
{
  yy_push_state (cond, lex);
}

void
lexer::pop_state ()
{
  yy_pop_state (lex);
}
