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
    if (strchr (yytext, '\n'))				\
      yycolumn = 0;					\
    else						\
      yycolumn += yyleng;				\
  }

#define Return(TOK)					\
  return impl->make_token<TOK> (yylval, yylloc, yytext, yyleng)

#define PUSH(STATE)	push_state (STATE)
#define POP()		pop_state ()

#define YY_DECL int lexer::lex (YYSTYPE *yylval_param, YYLTYPE *yylloc_param)

#define BACKTRACK(N) do {		\
  yyless (N);				\
  yycolumn -= yyleng - N;		\
} while (0)
%}

%option prefix="yy"
%option header-file="yylex.h"
%option bison-locations
%option reentrant
/*%option ecs full 8bit*/
%option yylineno stack
%option nounput noinput nounistd
%option nodefault
%option never-interactive

%x VAR_INIT VAR_RBODY VAR_SQBODY
%x RULE_INIT RULE_CODE RULE_LINE
%x FILENAME MULTIFILE SOURCES LINK VARDECL FLAGS

/* Whitespace */
SPACE	[ \t\v]
WS	[ \t\v\n\r]
NWS	[^ \t\v\n\r]
FLAG	[^ \t\v\n\r{}]
/* Filenames */
FN	[^ \t\v\n\r{}/%*.:]
FNSTART	([./*%{]|"**")
/* Identifiers */
ID	[a-zA-Z_-][a-zA-Z0-9_-]*
/* Rule start */
MULTI	"{"[^}\n\r]+"}"
RLSTART	((({FNSTART}|{MULTI})({NWS}|{MULTI}))+|{ID})":"
/* Numbers */
DIGIT	[0-9]
INTEGER	{DIGIT}+

%%
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
"config_header:"			{ Return (KW_CONFIG_HEADER); }
"contact:"				{ Return (KW_CONTACT); }
"cppflags"				{ Return (KW_CPPFLAGS); }
"define"				{ Return (KW_DEFINE); }
"error"					{ Return (KW_ERROR); }
"exclude"				{ Return (KW_EXCLUDE); }
"extra_dist"				{ Return (KW_EXTRA_DIST); }
"functions"				{ Return (KW_FUNCTIONS); }
"global"				{ Return (KW_GLOBAL); }
"header:"				{ Return (KW_HEADER); }
"headers"				{ Return (KW_HEADERS); }
"library"				{ Return (KW_LIBRARY); }
"link"					{ Return (KW_LINK); }
"nodist_sources"			{ Return (KW_NODIST_SOURCES); }
"notfound:"				{ Return (KW_NOTFOUND); }
"options"				{ Return (KW_OPTIONS); }
"program"				{ Return (KW_PROGRAM); }
"project"				{ Return (KW_PROJECT); }
"section"				{ Return (KW_SECTION); }
"sizeof"				{ Return (KW_SIZEOF); }
"sources"				{ Return (KW_SOURCES); }
"symbol:"				{ Return (KW_SYMBOL); }
"template"				{ Return (KW_TEMPLATE); }
"version:"				{ Return (KW_VERSION); }

<INITIAL>{
	{WS}+				{ }
	{NWS}+{FNSTART}			{ PUSH (FILENAME); BACKTRACK (0); }
	{NWS}+{RLSTART}			{ PUSH (RULE_INIT); PUSH (FILENAME); BACKTRACK (0); }
	{FNSTART}{NWS}			{ PUSH (FILENAME); BACKTRACK (0); }
	{RLSTART}			{ PUSH (RULE_INIT); PUSH (FILENAME); BACKTRACK (0); }
	{ID}				{ Return (TK_IDENTIFIER); }
	"("				{ Return (TK_LBRACK); }
	")"				{ Return (TK_RBRACK); }
	"{"				{ Return (TK_LBRACE); }
	"}"				{ Return (TK_RBRACE); }
	"="				{ Return (TK_EQUALS); }
	":"				{ Return (TK_COLON); }
	"->"				{ Return (TK_ARROW); }
	"=>"				{ Return (TK_DARROW); }
	'(\\.|[^'])*'			{ Return (TK_STRING); }
}

<FILENAME>{
	{WS}+				{ POP (); Return (TK_WHITESPACE); }
	{FN}+				{ Return (TK_FILENAME); }
	"{"				{ PUSH (MULTIFILE); Return (TK_FILENAME); }
	{FNSTART}			{ Return (TK_FILENAME); }
	":"				{ POP (); BACKTRACK (0); }
}
<MULTIFILE>{
	{WS}+				{ }
	{FN}+				{ Return (TK_FILENAME); }
	"}"				{ POP (); Return (TK_FILENAME); }
}

<RULE_INIT>{
	{WS}+				{ }
	":"				{ Return (TK_COLON); }
	";"				{ POP (); Return (TK_SEMICOLON); }
	"{"				{ POP (); BACKTRACK (0); }
	{NWS}				{ PUSH (FILENAME); BACKTRACK (0); }
}

<RULE_CODE>{
	\n				{ }
	"}"				{ Return (TK_RBRACE); }
	^\t{2}				{ PUSH (RULE_LINE); }
}

<RULE_LINE>{
	\n				{ }
	^\t{3}				{ }
	^\t{2}				{ Return (TK_WHITESPACE); }
	^\t{1}"}"			{ POP (); BACKTRACK (1); Return (TK_WHITESPACE); }
	[^\n\t$]+			{ Return (TK_CODE); }
	"$"				{ PUSH (VAR_INIT); Return (TK_DOLLAR); }
}

<VAR_INIT>{
	[@*<]				{ POP (); Return (TK_SHORTVAR); }
	{INTEGER}			{ POP (); Return (TK_INTEGER); }
	"("				{ BEGIN (VAR_RBODY); Return (TK_LBRACK); }
	"["				{ BEGIN (VAR_SQBODY); Return (TK_LSQBRACK); }
}
<VAR_RBODY>{
	{WS}+				{ }
	{ID}				{ Return (TK_IDENTIFIER); }
	"."				{ Return (TK_DOT); }
	":"				{ Return (TK_COLON); }
	")"				{ POP (); Return (TK_RBRACK); }
}
<VAR_SQBODY>{
	{ID}				{ Return (TK_IDENTIFIER); }
	"]"				{ POP (); Return (TK_RSQBRACK); }
}

<LINK>{
	{WS}+				{ }
	-l{ID}				{ Return (TK_EXT_LIB); }
	{ID}				{ Return (TK_INT_LIB); }
	"}"				{ Return (TK_RBRACE); }
}

<VARDECL>{
	\n				{ }
	^\t{2}				{ }
	^\t{1}				{ Return (TK_WHITESPACE); }
	[^\n\t$]+			{ Return (TK_CODE); }
	"$"				{ PUSH (VAR_INIT); Return (TK_DOLLAR); }
}

<FLAGS>{
	{WS}+				{ }
	{FLAG}+				{ Return (TK_FLAG); }
	"}"				{ Return (TK_RBRACE); }
}


<*>"#".*				{ }

<*>(.|\n)				{ printf (">>> in state %s\n", STRSTATE (state ())); yyerror (yylloc, 0, yytext); }
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
#if LEXER_VERBOSE
  printf ("PUSH (%s)\n", STRSTATE (state));
#endif
  yy_push_state (state, yyscanner);
}

void
lexer::pop_state ()
{
#if LEXER_VERBOSE
  printf ("POP (%s)\n", STRSTATE (state ()));
#endif
  yy_pop_state (yyscanner);
}

template<short Tok>
int
lexer::pimpl::make_token (YYSTYPE *lval, YYLTYPE const *lloc, char const *text, int leng)
{
  if (!this->text.empty ())
    lval->token = new tokens::token (*lloc, Tok, move (this->text));
  else
    lval->token = new tokens::token (*lloc, Tok, text, leng);
  return Tok;
}
