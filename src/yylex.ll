%{
#include "lexer.h"
#include "lexer/pimpl.h"
#include "lexer/util.h"
%}

%option prefix="yy"
%option header-file="/tmp/yylex.h"
%option bison-locations
%option reentrant
%option ecs full 8bit
%option stack
%option nounput noinput nounistd
%option nodefault
%option never-interactive

%x INITWS
%x VAR_INIT VAR_RBODY VAR_SQBODY
%x RULE_INIT RULE_CODE RULE_LINES RULE_LINE
%x VARDECL_INIT VARDECL_NAME VARDECL_CODE VARDECL VARDECL_LINE
%x FILENAME RULE_FILENAME MULTIFILE IMPORT EXCLUDE LINK FLAGS

/* Whitespace */
SPACE	[ \t\v\f]
WS	[ \t\v\f\n\r]
IGNORED	({WS}+|"#".*)+
/* Tool flags */
FLAG	[^ \t\v\f\n\r}#$]
/* Filenames */
FN	[^ \t\v\f\n\r{}%*:;?$#<>!]
/* Identifiers */
ID	[a-zA-Z_-][a-zA-Z0-9_-]*
NAME	[a-zA-Z_-][a-zA-Z0-9_/.-]*
UC	[A-Z][A-Z0-9_]*
/* Variable declaration */
VDSTART	{UC}+{WS}*("+"?)"="
SVAR	[@*<^+$]
/* Rule start */
MULTI	"{"[^}\n\r]+"}"
RLSTART	.+":".+[;{]
/* Numbers */
DIGIT	[0-9]
INTEGER	{DIGIT}+
/* Strings */
SSTRING	'(\\.|[^\\'])+'
DSTRING	\"(\\.|[^\\"])+\"
BSTRING	`(\\.|[^\\`])+`
STRING	({SSTRING}|{DSTRING}|{BSTRING})

%%
"c_bigendian"				{ RetToken (TK_AC_CHECK); }
"c_charset"				{ RetToken (TK_AC_CHECK); }
"c_enum_fwdecl"				{ RetToken (TK_AC_CHECK); }
"c_float_format"			{ RetToken (TK_AC_CHECK); }
"c_late_expansion"			{ RetToken (TK_AC_CHECK); }
"c_stdint_h"				{ RetToken (TK_AC_CHECK); }
"c_token_paste"				{ RetToken (TK_AC_CHECK); }
"c_typeof"				{ RetToken (TK_AC_CHECK); }

"alignof"				{ RetKeyword (KW_ALIGNOF); }
"arg_enable"				{ RetKeyword (KW_ARG_ENABLE); }
"built_sources"				{ RetKeyword (KW_BUILT_SOURCES); }
"config_header:"			{ RetKeyword (KW_CONFIG_HEADER); }
"contact:"				{ RetKeyword (KW_CONTACT); }
"data"					{ RetKeyword (KW_DATA); }
"define"				{ RetKeyword (KW_DEFINE); }
"dir"					{ RetKeyword (KW_DIR); }
"error"					{ RetKeyword (KW_ERROR); }
"exclude"				{ RetKeyword (KW_EXCLUDE); }
"extern"				{ RetKeyword (KW_EXTERN); }
"extra_dist"				{ RetKeyword (KW_EXTRA_DIST); }
"functions"				{ RetKeyword (KW_FUNCTIONS); }
"global"				{ RetKeyword (KW_GLOBAL); }
"header:"				{ RetKeyword (KW_HEADER); }
"headers"				{ RetKeyword (KW_HEADERS); }
"if"					{ RetKeyword (KW_IF); }
"library"				{ RetKeyword (KW_LIBRARY); }
"link"					{ RetKeyword (KW_LINK); }
"test"					{ RetKeyword (KW_TEST); }
"log_compiler"				{ RetKeyword (KW_LOG_COMPILER); }
"mans"					{ RetKeyword (KW_MANS); }
"nodist_sources"			{ RetKeyword (KW_NODIST_SOURCES); }
"notfound:"				{ RetKeyword (KW_NOTFOUND); }
"options"				{ RetKeyword (KW_OPTIONS); }
"program"				{ RetKeyword (KW_PROGRAM); }
"project"				{ RetKeyword (KW_PROJECT); }
"section"				{ RetKeyword (KW_SECTION); }
"sizeof"				{ RetKeyword (KW_SIZEOF); }
"sources"				{ RetKeyword (KW_SOURCES); }
"symbol:"				{ RetKeyword (KW_SYMBOL); }
"template"				{ RetKeyword (KW_TEMPLATE); }
"version:"				{ RetKeyword (KW_VERSION); }

<INITWS>{
	{WS}+				{ /* ignore initial whitespace */ }
	.				{ POP (); BACKTRACK (0); }
}

<*>{
	"$"				{ PUSH (VAR_INIT); RetKeyword (TK_DOLLAR); }
}
<VAR_INIT>{
	{SVAR}				{ POP (); RetToken (TK_SHORTVAR); }
	"("{SVAR}D")"			{ POP (); RetToken (TK_SHORTVAR); }
	"("{SVAR}F")"			{ POP (); RetToken (TK_SHORTVAR); }
	{INTEGER}			{ POP (); RetToken (TK_INTEGER); }
	"("				{ SWITCH (VAR_RBODY); RetKeyword (TK_LBRACK); }
	"["				{ SWITCH (VAR_SQBODY); RetKeyword (TK_LSQBRACK); }
}
<VAR_RBODY>{
	{WS}+				{ }
	{ID}				{ RetToken (TK_IDENTIFIER); }
	"."				{ RetKeyword (TK_DOT); }
	":"				{ RetKeyword (TK_COLON); }
	")"				{ POP (); RetKeyword (TK_RBRACK); }
}
<VAR_SQBODY>{
	{ID}				{ RetToken (TK_IDENTIFIER); }
	"]"				{ POP (); RetKeyword (TK_RSQBRACK); }
}


<INITIAL>{
	{WS}				{ }
	{ID}"flags"			{ RetToken (TK_FLAGS_ID); }
	{NAME}				{ RetToken (TK_IDENTIFIER); }
	^{SPACE}+{RLSTART}$		{ PUSH (RULE_INIT); PUSH (INITWS); BACKTRACK (0); }
	^{RLSTART}$			{ PUSH (RULE_INIT); BACKTRACK (0); }
	^{SPACE}*{VDSTART}.+$		{ PUSH (VARDECL); BACKTRACK (0); }
	^{SPACE}*{VDSTART}$		{ PUSH (VARDECL_INIT); BACKTRACK (0); }
	"("				{ RetKeyword (TK_LBRACK); }
	")"				{ RetKeyword (TK_RBRACK); }
	"{"				{ RetKeyword (TK_LBRACE); }
	"}"				{ RetKeyword (TK_RBRACE); }
	"="				{ RetKeyword (TK_EQUALS); }
	"<"				{ RetKeyword (TK_LESS); }
	"->"				{ RetKeyword (TK_ARROW); }
	"=>"				{ RetKeyword (TK_DARROW); }

	{STRING}			{ RetToken (TK_STRING); }
}

<RULE_INIT>{
	{WS}+				{ RetKeyword (TK_WHITESPACE); }
	":"				{ PushKeyword (TK_COLON); RetKeyword (TK_WHITESPACE); }
	";"				{ POP (); RetKeyword (TK_SEMICOLON); }
	"{\n"				{ SWITCH (RULE_CODE); RetKeyword (TK_LBRACE); }
	.				{ PUSH (FILENAME); BACKTRACK (0); }
}
<RULE_CODE>{
	^\t{1}				{ impl->indent = 1; SWITCH (RULE_LINES); BACKTRACK (0); }
	^\t{2}				{ impl->indent = 2; SWITCH (RULE_LINES); BACKTRACK (0); }
	^\t{3}				{ impl->indent = 3; SWITCH (RULE_LINES); BACKTRACK (0); }
}
<RULE_LINES>{
	\t{1}[^\t]			{ if (impl->indent < 1) RetToken (TK_ERROR); PUSH (RULE_LINE); BACKTRACK (1); }
	\t{2}[^\t]			{ if (impl->indent < 2) RetToken (TK_ERROR); PUSH (RULE_LINE); BACKTRACK (2); }
	\t{3}[^\t]			{ if (impl->indent < 3) RetToken (TK_ERROR); PUSH (RULE_LINE); BACKTRACK (3); }
	"}"				{ POP (); RetKeyword (TK_RBRACE); }
}
<RULE_LINE>{
	[^\n\t$]+			{ RetToken (TK_CODE); }
	\n\t{1}				{ if (impl->indent >= 1) { BACKTRACK (1); POP (); } }
	\n\t{2}				{ if (impl->indent >= 2) { BACKTRACK (2); POP (); } }
	\n\t{3}				{ if (impl->indent >= 3) { BACKTRACK (3); POP (); } }
	\n"}"				{ if (impl->indent == 1) { BACKTRACK (1); POP (); } }
	\n\t{1}"}"			{ if (impl->indent >= 2) { BACKTRACK (2); POP (); } }
	\n\t{2}"}"			{ if (impl->indent >= 3) { BACKTRACK (3); POP (); } }
}

<FILENAME>{
	{IGNORED}			{ RetKeyword (TK_WHITESPACE); }
	{FN}+				{ RetToken (TK_FILENAME); }
	"%"				{ RetToken (TK_FN_PERCENT); }
	"%%"				{ RetToken (TK_FN_PERPERCENT); }
	"?"				{ RetToken (TK_FN_QMARK); }
	"*"				{ RetToken (TK_FN_STAR); }
	"**"				{ RetToken (TK_FN_STARSTAR); }
	"{".				{ PUSH (MULTIFILE); BACKTRACK (1); RetToken (TK_FN_LBRACE); }
	"sources"{SPACE}*"("		{ PUSH (IMPORT); BACKTRACK (0); }
	"exclude"{SPACE}*"{"		{ PUSH (EXCLUDE); BACKTRACK (0); }
	.				{ POP (); BACKTRACK (0); }
}
<MULTIFILE>{
	{SPACE}+			{ }
	{FN}+				{ RetToken (TK_FILENAME); }
	"}"				{ POP (); RetToken (TK_FN_RBRACE); }
}

<IMPORT>{
	{SPACE}				{ }
	"sources"			{ RetKeyword (KW_SOURCES); }
	"("				{ RetKeyword (TK_LBRACK); }
	{ID}				{ RetToken (TK_IDENTIFIER); }
	")"				{ POP (); RetKeyword (TK_RBRACK); }
}
<EXCLUDE>{
	{SPACE}				{ }
	"exclude"			{ RetKeyword (KW_EXCLUDE); }
	"{"				{ RetKeyword (TK_LBRACE); }
	"}"				{ POP (); RetKeyword (TK_RBRACE); }
}

<FLAGS>{
	{IGNORED}			{ RetKeyword (TK_WHITESPACE); }
	{FLAG}+				{ RetToken (TK_FLAG); }
	"}"				{ RetKeyword (TK_RBRACE); }
}

<VARDECL>{
	{ID}				{ RetToken (TK_IDENTIFIER); }
	{SPACE}+			{ }
	"="				{ SWITCH (VARDECL_LINE); RetKeyword (TK_EQUALS); }
	"+="				{ SWITCH (VARDECL_LINE); RetKeyword (TK_PLUSEQ); }
}
<VARDECL_LINE>{
	[^\n$]+				{ RetToken (TK_CODE); }
	\n				{ POP (); RetKeyword (TK_WHITESPACE); }
}

<VARDECL_INIT>{
	{SPACE}*[A-Z]			{ impl->indent = yyleng; SWITCH (VARDECL_NAME); BACKTRACK (yyleng - 1); }
}
<VARDECL_NAME>{
	{ID}				{ RetToken (TK_IDENTIFIER); }
	{SPACE}*"="			{ RetKeyword (TK_EQUALS); }
	\n\t				{ SWITCH (VARDECL_CODE); BACKTRACK (0); }
}
<VARDECL_CODE>{
	\n\t*				{ if (yyleng - 1 < impl->indent) { POP (); BACKTRACK (0); RetKeyword (TK_WHITESPACE); } }
	.+				{ RetToken (TK_CODE); }
}

<LINK>{
	{WS}+				{ }
	-l{ID}				{ RetToken (TK_EXT_LIB); }
	{ID}				{ RetToken (TK_INT_LIB); }
	"}"				{ RetKeyword (TK_RBRACE); }
}


<*>"#".*				{ }

<*>(.|\n)				{ RetToken (TK_ERROR); }
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

#define STATE_VERBOSE 0

void
lexer::push_state (int state)
{
#if STATE_VERBOSE
  printf ("PUSH (%s -> %s)\n", STRSTATE (this->state ()), STRSTATE (state));
#endif
  yy_push_state (state, yyscanner);
}

void
lexer::switch_state (int state)
{
  yyguts_t *yyg = (yyguts_t *)yyscanner;
#if STATE_VERBOSE
  printf ("SWITCH (%s -> %s)\n", STRSTATE (this->state ()), STRSTATE (state));
#endif
  BEGIN (state);
}

void
lexer::pop_state ()
{
  int state = this->state ();
  yy_pop_state (yyscanner);
#if STATE_VERBOSE
  printf ("POP (%s -> %s)\n", STRSTATE (state), STRSTATE (this->state ()));
#endif
}
