%{
#include "lexer.h"
#include "lexer/pimpl.h"

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

#define APPEND() SELF->impl->text.append (yytext, yyleng)

#define Return(TOK)					\
  do {							\
    if (!SELF->impl->text.empty ())			\
      yylval->token					\
        = new nodes::token (TOK,			\
            move (SELF->impl->text));			\
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
%x RULE_INIT RULE_CODE RULE_LINE
%x FILENAME MULTIFILE SOURCES LINK

/* Whitespace */
WS	[ \t\v\n\r]
/* Filenames */
FN	[^ \t\v\n\r{}/%*.:]
/* Identifiers */
ID	[a-zA-Z_-][a-zA-Z0-9_-]*

%%
<*>"#".*				{ }
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
"export"				{ Return (KW_EXPORT); }
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

<INITIAL>{
	{ID}				{ Return (TK_IDENTIFIER); }
	"{"				{ Return (TK_LBRACE); }
	"}"				{ Return (TK_RBRACE); }
}

<SOURCES>{
	/* Expect filenames */
	{WS}+				{ }
	{FN}+				{ PUSH (FILENAME); Return (TK_FILENAME); }
	("."|"/"|"*"|"**"|"%")		{ PUSH (FILENAME); Return (TK_FILENAME); }
	"{"				{ PUSH (FILENAME); PUSH (MULTIFILE); Return (TK_FILENAME); }
	"}"				{ Return (TK_RBRACE); }
}
<FILENAME>{
	{WS}+				{ POP (); Return (TK_WHITESPACE); }
	{FN}+				{ Return (TK_FILENAME); }
	("."|"/"|"*"|"**"|"%")		{ Return (TK_FILENAME); }
	"{"				{ PUSH (MULTIFILE); Return (TK_FILENAME); }
	":"				{ POP (); yyless (0); }
}
<MULTIFILE>{
	{WS}+				{ }
	{FN}+				{ Return (TK_FILENAME); }
	"}"				{ POP (); Return (TK_FILENAME); }
}


<RULE_INIT>{
	/* Expect filename pattern followed by ":" and then more filename patterns */
	{WS}+				{ }
	"%"				{ PUSH (FILENAME); Return (TK_FILENAME); }
	{FN}+				{ PUSH (FILENAME); Return (TK_FILENAME); }
	":"				{ Return (TK_COLON); }
	"{"\n*				{ Return (TK_LBRACE); }
	"}"				{ Return (TK_RBRACE); }
}

<RULE_CODE>{
	^\t{2}"}"			{ Return (TK_RBRACE); }
	^\t{3}				{ PUSH (RULE_LINE); Return (TK_WHITESPACE); }
	^\t{4}				{ PUSH (RULE_LINE); }
}
<RULE_LINE>{
	[^$\n\r]+			{ Return (TK_CODE); }
	"$"				{ APPEND (); PUSH (VAR_INIT); }
	\n				{ POP (); }
}

<LINK>{
	{WS}+				{ }
	[a-z][a-z_]*			{ Return (TK_INT_LIB); }
	-l{FN}+				{ Return (TK_EXT_LIB); }
	"}"				{ Return (TK_RBRACE); }
}

<VAR_INIT>{
	"<"				{ APPEND (); POP (); Return (TK_VAR); }
	"@"				{ APPEND (); POP (); Return (TK_VAR); }
	"*"				{ APPEND (); POP (); Return (TK_VAR); }
}

<*>.					{ printf ("in state %d\n", YY_START); yyerror (yylloc, 0, yytext); }
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
