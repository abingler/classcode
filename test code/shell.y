%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HOME getenv("HOME")
#define PWD getenv("PWD")

void yyerror(const char *str) /*print any errors*/
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap() /*somthing to do with yyin/yyout dont know yet*/
{
        return 1;
} 

int main()
{
	//alias_list = create_linked_list();
	printf("hello I am computer\n");
	printf("I make an shell\n");
	printf("wat do\n");	
	printf("%s$ ",PWD);
	yyparse();
	return 0;
} 

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

char *replace(char *str, char *orig, char * rep) /*replace string with new substring*/
{
	static char buffer[4096];
	char *p;
	if(!(p = strstr(str, orig))) return str; /*is orig in str*/
	
	strncpy(buffer, str, p-str); /*copy char from str start to orig into buffer*/
	buffer[p-str] = '\0';
	
	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
	
	return buffer;
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
			char* envName = insert_env($<string>2);/*extract word1*/
			char* envVal = insert_env($<string>3);/*extract word2*/
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
				printf("%s\n", env[i++]);
			char* path = getenv("PATH");
		};

unset_env:
	UNSET_ENV WORD
		{
			char* name = $<string>2;
			if(getenv(name)) /*checks if valid env before unseting it*/
				unsetenv(name);
			else
				printf("No variable named %s.\n", name);
		};

alias://the rest is dependant on datastructures so left undone
	ALIAS 
		{
			/*print current aliass*/
		}
		| ALIAS WORD WORD
		{
			/*add new alias $2 =$3*/
		};


arg: WORD{$$=$1;}
%%