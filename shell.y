%{
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "main.c"

#define HOME getenv("HOME")
#define PWD getenv("PWD")


void yyerror(const char *str) /*print any errors*/
{
        fprintf(stderr,"error: %s at line %d\n",str,yylineno);
}
 
int yywrap() /*somthing to do with yyin/yyout dont know yet*/
{
        return 1;
} 

%}


%token CD BYE PRINTENV SETENV UNSETENV NEW_LINE ALIAS UNALIAS LS

%union
{
        int number;
        char* string;
        void* linkedlist;
}


%left CHANGE_DIR ALIAS WORD
%token <string> WORD
//%type <linkedlist> arg_list
//%type <linkedlist> cmd
//%type <string> arg
%%

commands:
		| commands command 	{printf("%s> ",getenv("PWD"));}; /*print the current working dir*/

command:
		| NEW_LINE /* ignore new line*/
		| bye  NEW_LINE
		| cd  NEW_LINE
		| set_env NEW_LINE
		| print_env NEW_LINE
		| unset_env NEW_LINE
		| ls NEW_LINE
		| alias NEW_LINE
		| unalias NEW_LINE
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
			$2 = insert_env($2); /*extract env from word*/
			chdir($2);			/*change dir*/
			char pwd[4096];
			getcwd(pwd, sizeof(pwd)); /*copy absolute pathname to pwd[]*/
			setenv("PWD", pwd, 1); /*update PWD*/
		};
		
set_env:
	SETENV WORD WORD
		{
			char* envName = insert_env($<string>2);/*extract word1*/
			char* envVal = insert_env($<string>3);/*extract word2*/
			int result = setenv(envName, envVal, 1);
			if(result == -1)
				printf("Failed to set variable %s to %s.\n", envName, envVal);
		};
print_env:
	PRINTENV
		{		
			extern char **environ;	
			int i=0;
			while(environ[i])
				printf("%s\n", environ[i++]);
			char* path = getenv("PATH");
			printf("%s> ",path);
		}
unset_env:
	UNSETENV WORD 
		{
			char* name = $<string>2;
			if(getenv(name))
				unsetenv(name);\
			else
				printf("Variable %s does not exist.\n", name);
		}

ls:
	LS
		{
			ls();
		}

alias:
	ALIAS
		{
			print_alias_list(alias_head);/*prints list*/
		}
	| ALIAS WORD WORD
		{ 
			if(retrieve_val( alias_head,$2) != NULL){
				remove_by_alias(&alias_head, $2); //remove existing alias
			}
			push(&alias_head, $2, $3); /*add new Alias*/

		}
			
unalias:
	UNALIAS WORD
		{
			remove_by_alias(&alias_head, $2);
		}


%%