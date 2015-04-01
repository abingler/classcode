%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HOME getenv("HOME")
#define PWD getenv("PWD")

char * insert_env(char* input){ /*function extrats env variable*/
	char * s = input;
	int i;
	int validFlag = 0;
	int start;
	int end;
	for (i = 0; i < strlen(s); i++) /*iterate through input*/
	{
		if(s[i] == '$') start = i;
		if(s[i] == '{' && i == start+1) validFlag = 1;
		if(s[i] == '}' && validFlag)
		{
			char subbuf[4096];
			memcpy(subbuf, &s[start], i-start+1);
			subbuf[i-start+1] = '\0';

			char * var; /*extrat var from ${var}*/
			copystring(var, subbuf);
			var = var + 2; 				//get rid of ${
			var[i-start-2] = '\0';  		//get rid of ending }
			
			s = replace(s, subbuf, getenv(var));
		}
	
	}
	return s;
}
%}


%token CHANGE_DIR BYE PRINT_ENV SET_ENV UNSET_ENV NEW_LINE ALIAS UNALIAS

%union
{
        int number;
        char* string;
        void* linkedlist;
}


%left CHANGE_DIR ALIAS WORD
%token <string> WORD
%type <linkedlist> arg_list
%type <linkedlist> cmd
%type <string> arg
%%

commands:
		| commands command 	{printf("%s$ ",getenv("PWD"));}; /*print the current working dir*/

command:
		| NEW_LINE {} /* ignore new line*/
		| bye  NEW_LINE
		| cd  NEW_LINE
		| setenv NEW_LINE
		| printenv NEW_LINE
		| unsetenv NEW_LINE
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
	SET_ENV WORD WORD
		{
			char* envname = insert_env($<string>2);/*extract word1*/
			char* envval = insert_env($<string>3);/*extract word2*/
			int result = setenv(envname, envval, 1);
			if(result == -1)
				printf("Failed to set variable %s to %s.\n", envname, envval);
		};
print_env:
	PRINT_ENV
		{
			extern char **env;
			int i=0;
			while(env[i])
				printf("%s\n", envi[i++]);
			char* path = getenv("PATH");
		}