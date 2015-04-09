%{
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "main.c"



void yyerror(const char *str) /*print any errors*/
{
        fprintf(stderr,"error: %s at line %d\n",str,yylineno);
}
 
int yywrap() /*somthing to do with yyin/yyout dont know yet*/
{
        return 1;
} 



%}


%token CD BYE PRINTENV SETENV UNSETENV NEWLINE ALIAS UNALIAS LS

%union
{
        int number;
        char* string;
        void* linkedlist;
        arg_node* arg_n;
}


%left CHANGE_DIR ALIAS WORD
%token <string> WORD
%token <arg_n> ARGS
%type <arg_n> arg_list
//%type <linkedlist> cmd
//%type <string> arg
%%

commands:
		| commands command 	{printf("%s> ",getenv("PWD"));}
		| commands arg_list NEWLINE{commandBlock($2);printf("%s> ",getenv("PWD"));}

command:
		| NEWLINE /* ignore new line*/
		| bye  NEWLINE
		
		| setEnv NEWLINE
		| printEnv NEWLINE
		| unsetEnv NEWLINE
		| alias NEWLINE
		| unalias NEWLINE
		;


bye:
	BYE
		{
			printf("Exiting the shell now...\n");
			exit(0);
		};

cd: 
	CD	
		{
			chdir(getenv("HOME"));  /*move to home*/
			setenv("PWD", getenv("HOME"), 1);  /*update PWD with home*/
		}
	| CD WORD
		{
			$2 = insertEnv($2); /*extract env from word*/
			chdir($2);			/*change dir*/
			char pwd[4096];
			getcwd(pwd, sizeof(pwd)); /*copy absolute pathname to pwd[]*/
			setenv("PWD", pwd, 1); /*update PWD*/
		};
		
setEnv:
	SETENV WORD WORD
		{
			char* envName = insertEnv($<string>2);/*extract word1*/
			char* envVal = insertEnv($<string>3);/*extract word2*/
			int result = setenv(envName, envVal, 1);
			if(result == -1)
				printf("Failed to set variable %s to %s.\n", envName, envVal);
		};
printEnv:
	PRINTENV
		{		
			extern char **environ;	
			int i=0;
			while(environ[i])
				printf("%s\n", environ[i++]);
			char* path = getenv("PATH");
			printf("%s> ",path);
		}
unsetEnv:
	UNSETENV WORD 
		{
			char* name = $<string>2;
			if(getenv(name))
				unsetenv(name);\
			else
				printf("Variable %s does not exist.\n", name);
		}

/*ls:
	LS
		{
			ls();
		}
*/
alias:
	ALIAS
		{
			printAliasList(aliasHead);/*prints list*/
		}
	| ALIAS WORD WORD
		{ 
			push(&aliasHead, $2, $3); /*add new Alias*/

		}
			
unalias:
	UNALIAS WORD
		{
			removeByAlias(&aliasHead, $2);
		}

arg_list:
    WORD arg_list { $$ = malloc(sizeof(arg_node));
                    $$->next = $2;
                    $$->arg_val = $1;}
    |
    ARGS arg_list {  $$ = $1;
                     arg_node* current = $1;
                     while (current->next != NULL) current = current->next;
                     current->next = $2;
                     //printf("%s\n",$1 );
                 }
    |
    ARGS          { $$ = $1; 
    				//printf("%s\n",$1 );
    				}

    |
    WORD          { $$ = malloc(sizeof(arg_node));
                    $$->next = NULL;
                    $$->arg_val = $1;
                    //printf("%s\n",$1 ); 
                }
    ;
%%

