%{
#include <stdio.h>
#include "node.h"
#include "y.tab.h"


%}

%option yylineno

%%

[a-zA-Z0-9\/.-]+ 		{ //One or more copies of a-z or A-Z or 0-9 or \ or / or . or - (This is a character class, so only ^ or - between chars has any effect)
							yylval.string=strdup(yytext); return WORD; 
						}

[^ \f\n\r\t\v;]*        { //Zero or more copies of anything except, " \f\n\r\t\v"
							char* text=strdup(yytext);
                        	text=insertEnv(text);
                        	replaceEscape(text);
                        	if (hasWhitespace(text))
                        	{
                        		yylval.arg_n = splitToTokens(text, " \t");
                        	    return ARGS;
                        	}
                     	}

;         				return NEWLINE;
\n         				return NEWLINE;       
[ \t]+                  /* ignore whitespace */;
%%
