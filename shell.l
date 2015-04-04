%{
#include <stdio.h>
#include "y.tab.h"

%}

%option yylineno

%%

bye					return BYE;
cd					return CD;
setenv				return SETENV;
printenv			return PRINTENV;
unsetenv			return UNSETENV;
ls					return LS;
alias				return ALIAS;
unalias				return UNALIAS;

[a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;

\n         			return NEW_LINE;       
[ \t]+                  /* ignore whitespace */;
%%
