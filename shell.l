%{
#include <stdio.h>
#include "y.tab.h"

%}
%%

bye					return BYE;
cd					return CD;
setenv				return SETENV;
printenv			return UNSETENV;
unsetenv			return PRINTENV;
ls					return LS;
alias				return ALIAS;
unalias				return UNALIAS;

[a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;
["$"\{\}~a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;
\n         			return NEW_LINE;       
[ \t]+                  /* ignore whitespace */;
%%
