%{
#include <stdio.h>
#include "node.h"
#include "y.tab.h"


%}

%option yylineno

%%

[a-zA-Z0-9\/.-]+ yylval.string=strdup(yytext); return WORD;

[^ \t\r\n\v\f;]*        {
							char* text=strdup(yytext);
                        	text=insertEnv(text);
                        	replaceEscape(text);
                        	if (hasWhitespace(text))
                        	{
                        		yylval.arg_n = splitToTokens(text, " \t");
                        	    return ARGS;
                        	}
                     	}

;         			return NEWLINE;
\n         			return NEWLINE;       
[ \t]+                  /* ignore whitespace */;
%%
