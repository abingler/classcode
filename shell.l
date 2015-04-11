%{
#include <stdio.h>
#include "node.h"
#include "y.tab.h"


%}

%option yylineno

%%

<<<<<<< HEAD
[a-zA-Z0-9\/.-]+ 		{	yylval.string = strdup(yytext);
							yylval.string = insertEnv(yylval.string);
							replaceEscape(yylval.string);
							return WORD;
						}

[^ \t\r\n\v\f;]*        {
							char* words=strdup(yytext);
                        	words=insertEnv(words);
                        	replaceEscape(words);
                        	if (hasWhitespace(words))
=======
[a-zA-Z0-9\/.|-]+ 		{ //One or more copies of a-z or A-Z or 0-9 or \ or / or . or - (This is a character class, so only ^ or - between chars has any effect)
							yylval.string=strdup(yytext); return WORD; 
						}

[^ \f\n\r\t\v;]*        { //Zero or more copies of anything except, " \f\n\r\t\v"
							char* text=strdup(yytext);
                        	text=insertEnv(text);
                        	replaceEscape(text);
                        	if (hasWhitespace(text))
>>>>>>> origin/master
                        	{
                        		yylval.arg_n = splitToTokens(words, " \t");
                        	    return ARGS;
                        	}
                        }

;         				return NEWLINE;
\n         				return NEWLINE;       
[ \t]+                  /* ignore whitespace */;
%%
