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

#define YY_USER_ACTION					\
  {							\
    lloc (yylloc, yylineno, yycolumn, yyleng);		\
    if (*yytext == '\n')				\
      yycolumn = 1;					\
    else						\
      yycolumn += yyleng;				\
  }

#define APPEND() impl->text.append (yytext, yyleng)

#define Return(TOK)					\
  do {							\
    if (!impl->text.empty ())				\
      yylval->token					\
        = new nodes::token (TOK, move (impl->text));	\
    else						\
      yylval->token					\
        = new nodes::token (TOK, yytext, yyleng);	\
    return TOK;						\
  } while (0)						\

#define PUSH(STATE)	push_state (STATE)
#define POP()		pop_state ()

#define YY_DECL int lexer::lex (YYSTYPE *yylval_param, YYLTYPE *yylloc_param)
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
SPACE	[ \t\v]
WS	[ \t\v\n\r]
NWS	[^ \t\v\n\r]
/* Filenames */
FN	[^ \t\v\n\r{}/%*.:]
FNSTART	([./*%{]|"**")
/* Identifiers */
ID	[a-zA-Z_-][a-zA-Z0-9_-]*

%%
<*>"#".*				{ }

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
	{SPACE}+			{ }
	{NWS}+{FNSTART}			{ PUSH (FILENAME); yyless (0); }
	{NWS}+{FNSTART}{NWS}+":"	{ PUSH (RULE_INIT); PUSH (FILENAME); yyless (0); }
	{FNSTART}{NWS}			{ PUSH (FILENAME); yyless (0); }
	{FNSTART}{NWS}+":"		{ PUSH (RULE_INIT); PUSH (FILENAME); yyless (0); }
	{ID}				{ Return (TK_IDENTIFIER); }
	"{"				{ Return (TK_LBRACE); }
	"}"				{ Return (TK_RBRACE); }
}

<FILENAME>{
	{WS}+				{ POP (); Return (TK_WHITESPACE); }
	{FN}+				{ Return (TK_FILENAME); }
	"{"				{ PUSH (MULTIFILE); Return (TK_FILENAME); }
	{FNSTART}			{ Return (TK_FILENAME); }
	":"				{ POP (); yyless (0); }
}
<MULTIFILE>{
	{WS}+				{ }
	{FN}+				{ Return (TK_FILENAME); }
	"}"				{ POP (); Return (TK_FILENAME); }
}

<RULE_INIT>{
	{WS}+				{ }
	":"				{ Return (TK_COLON); }
	"{"				{ POP (); yyless (0); }
	{NWS}				{ PUSH (FILENAME); yyless (0); }
}

<RULE_CODE>{
	"}"				{ Return (TK_RBRACE); }
	^\t{2}				{ PUSH (RULE_LINE); }
}

<RULE_LINE>{
	\n				{ }
	[^\n\t]+			{ Return (TK_CODE); }
	^\t{1}"}"			{ POP (); yyless (1); Return (TK_WHITESPACE); }
	^\t{2}				{ Return (TK_WHITESPACE); }
	^\t{3}				{ }
}


<*>.					{ printf (">>> in state %s\n", STRSTATE (state ())); yyerror (yylloc, 0, yytext); }
%%

#define SELF static_cast<lexer *> (yyget_extra (yyscanner))

int
yywrap (yyscan_t yyscanner)
{
  return SELF->wrap ();
}

int
lexer::state () const
{
  yyguts_t *yyg = (yyguts_t *)yyscanner;
  return YY_START;
}

void
lexer::push_state (int state)
{
  printf ("PUSH (%s)\n", STRSTATE (state));
  yy_push_state (state, yyscanner);
}

void
lexer::pop_state ()
{
  printf ("POP (%s)\n", STRSTATE (state ()));
  yy_pop_state (yyscanner);
}
