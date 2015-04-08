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

/*string handling functions*/

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


int has_whitespace(char* string)
{
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        if (string[i] == '\t' || string[i] == ' ') return 1;
    }
    return 0;
}
void replace_escape(char* str)
{
    char* p_read = str;
    char* p_write = str;
    while (*p_read) {
        *p_write = *p_read++;
        p_write += (*p_write != '\\' || *(p_write + 1) == '\\');
    }
    *p_write = '\0';
}

arg_node* split_to_tokens(char* string, char* delimiter)
{
    char* token;
    char* tmp = strdup(string);
    token = strtok(tmp, delimiter);
    arg_node* head = malloc(sizeof(arg_node));
    head->next = NULL;
    if (token != NULL)
    {
        head->arg_str = token;
    }
    else
    {
        head->arg_str = tmp;
    }
    arg_node* current = head;
    token = strtok(NULL, delimiter); 
    while (token != NULL)
    {
          current->next = malloc(sizeof(arg_node));
          current = current->next;
          current->arg_str = token;
          current->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return head;
}

%}


%token CD BYE PRINTENV SETENV UNSETENV NEW_LINE ALIAS UNALIAS LS

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
		| commands arg_list {//printf("%s> ",getenv("PWD"));} /*print the current working dir*/
	}

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

arg_list:
    WORD arg_list { $$ = malloc(sizeof(arg_node));
                    $$->next = $2;
                    $$->arg_str = $1;
                    printf("%s\n",$1 );
                	}
    |
    ARGS arg_list {  $$ = $1;
                     arg_node* current = $1;
                     while (current->next != NULL) current = current->next;
                     current->next = $2;
                     printf("%s\n",$1 );
                 }
    |
    ARGS          { $$ = $1; 
    				printf("%s\n",$1 );
    				}

    |
    WORD          { $$ = malloc(sizeof(arg_node));
                    $$->next = NULL;
                    $$->arg_str = $1;
                    printf("%s\n",$1 ); 
                }
    ;
%%