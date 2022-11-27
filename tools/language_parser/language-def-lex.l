%option yylineno
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "textutil.h"
#include "language-defs-builder.h"
#include "language-def-yacc.tab.h"

_Static_assert(sizeof(long) >= 4, "Need 32-bit longs at least for this grammar to make sense");
int l_err(const char*);
char *lex_error_msg;

char *strbuf = NULL;
size_t strbuf_len = 0;
size_t strbuf_alloc = 0;

void put_strbuf(const char *s) {
  size_t len = strlen(s);
  if (strbuf_len + len + 1 > strbuf_alloc) {
    strbuf_alloc = (strbuf_len + len + 1) * 2;
    strbuf = realloc(strbuf, strbuf_alloc);
  }
  memcpy(strbuf + strbuf_len, s, len + 1);
  strbuf_len += len;
  strbuf[strbuf_len] = '\0';
}

%}

%x QUOTED_STRING
%%

\" BEGIN(QUOTED_STRING);

<QUOTED_STRING>{
  \\n put_strbuf("\n");
  \\\\ put_strbuf("\\");
  \\\" put_strbuf("\"");
  [^\\\"]+ put_strbuf(yytext);
  \" {
    BEGIN(INITIAL);
    yylval.strval = strbuf == NULL ? strdup("") : strbuf;
    strbuf = NULL;
    strbuf_len = 0;
    strbuf_alloc = 0;
    return T_string;
  }
}

[ \t] ; /* ignore whitespace */

\n { return yytext[0]; }

[\[\]()=:+*@!] return yytext[0];

#.*/\n ; /* ignore comments */

(0[Xx]|\$)[[:xdigit:]]+ {
    char *end;
    char *start = yytext;
    if (start[0] == '$') start++;
    else start += 2;
    unsigned long val = strtoul(start, &end, 16);
    if (end != &yytext[yyleng]) return l_err("Internal error: cannot parse hex integer");
    if (val == ULONG_MAX && errno == ERANGE) return l_err("Oversize hexadecimal integer literal");
    yylval.intval = val;
    return T_integer;
}

0[Bb][01]+ {
    char *end;
    unsigned long val = strtoul(yytext + 2, &end, 2);
    if (end != &yytext[yyleng]) return l_err("Internal error: cannot parse binary integer");
    if (val == ULONG_MAX && errno == ERANGE) return l_err("Oversize binary integer literal");
    yylval.intval = val;
    return T_integer;
}

[[:digit:]]+ {
    char *end;
    unsigned long val = strtoul(yytext, &end, 10);
    if (end != &yytext[yyleng]) return l_err("Internal error: cannot parse decimal integer");
    if (val == ULONG_MAX && errno == ERANGE) return l_err("Oversize decimal integer literal");
    yylval.intval = val;
    return T_integer;
}

[[:alpha:].][[:alnum:]._]* { yylval.strval = strdup(yytext); return T_identifier; }

. { return l_err("Unexpected character"); }

%%

int yywrap() { return 1; }

int l_err(const char*s) {
    fprintf(stderr, "%s on line %d for input: %s\n", s, yylineno, yytext);
    return T_error;
}
