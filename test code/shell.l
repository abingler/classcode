%{
#include <stdio.h>
#include "y.tab.h"

%}
%%

bye					return EXIT;
cd					return CHANGE_DIR;
setenv				return SET_ENV;
printenv			return UNSET_ENV;
unsetenv			return PRINT_ENV;
ls					return LIST_DIR;
alias				return ALIAS;
unalias				return UNALIAS;

[ a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;
["$"\{\}~a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;
\n         			return NEW_LINE;       
[ \t]+                  /* ignore whitespace */;
%%
