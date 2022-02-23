%option yylineno
%{
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "language-defs-builder.h"
#include "language-def-yacc.tab.h"

_Static_assert(sizeof(long) >= 4, "Need 32-bit longs at least for this grammar to make sense");
int l_err(const char*);
char *lex_error_msg;

%}

%%

[ \t] ; /* ignore whitespace */

\"[^"\n]*\" {
    char *found = strdup(&yytext[1]);
    found[yyleng-2] = '\0';
    yylval.strval = found;
    return T_string;
}

\n { return yytext[0]; }

[\[\]()=:+*@!] return yytext[0];

#.*/\n ; /* ignore comments */

(0[Xx]|\$)[[:xdigit:]]+ {
    char *end;
    char *start = yytext;
    if (start[0] == '$') start++;
    else start += 2;
    unsigned long val = strtoul(start, &end, 16);
    if (end != &yytext[yyleng]) return l_err("Internal error: cannot parse integer");
    if (val == ULONG_MAX && errno == ERANGE) return l_err("Oversize hexadecimal integer literal");
    yylval.intval = val;
    return T_integer;
}

[[:digit:]]+ {
    char *end;
    unsigned long val = strtoul(yytext, &end, 10);
    if (end != &yytext[yyleng]) return l_err("Internal error: cannot parse integer");
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
