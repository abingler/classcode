%{
#include <stdio.h>
#include "node.h"
#include "y.tab.h"


%}

%option yylineno

%%


[a-zA-Z0-9\/\$\{\}\~/.|><&*?-]+ 		{	yylval.string = strdup(yytext);
							yylval.string = (char*) insertEnv(yylval.string);
							replaceEscape(yylval.string);
							return WORD;
						}

[^ \t\r\n\v\f;]*        {//Zero or more copies of anything except, " \f\n\r\t\v"
							char* words=strdup(yytext);
                        	words= (char*) insertEnv(words);
                        	replaceEscape(words);
                        	if (hasWhitespace(words)){
                        		yylval.argN = (argNode*) stringTok(words, " \t");
                        	    return ARGS;
                        	}
                        	
                        }


;         				return NEWLINE;
\n         				return NEWLINE;       
[ \t]+                  /* ignore whitespace */;
%%
