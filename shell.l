%{
#include <stdio.h>
#include "node.h"
#include "y.tab.h"


%}

%option yylineno

%%

bye					return BYE;
cd					return CD;
setenv				return SETENV;
printenv			return PRINTENV;
unsetenv			return UNSETENV;
alias				return ALIAS;
unalias				return UNALIAS;

[a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;

[^ \t\r\n\v\f;]*        {
							char* text=strdup(yytext);
                        	text=insert_env(text);
                        	replace_escape(text);
                        	if (hasWhitespace(text))
                        	{
                        		yylval.arg_n = split_to_tokens(text, " \t");
                        	    return ARGS;
                        	}
                     	}

;         			return NEW_LINE;
\n         			return NEW_LINE;       
[ \t]+                  /* ignore whitespace */;
%%
