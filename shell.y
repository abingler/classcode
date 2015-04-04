%{
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"

#define HOME getenv("HOME")
#define PWD getenv("PWD")
#define copystring(a,b) strcpy((a=(char *)malloc(strlen(b)+1)),b)

extern int yylineno;

void yyerror(const char *str) /*print any errors*/
{
        fprintf(stderr,"error: %s at line %d\n",str,yylineno);
}
 
int yywrap() /*somthing to do with yyin/yyout dont know yet*/
{
        return 1;
} 

typedef struct node { /*all nodes in alias linked list need a name value and pointer to the next node*/
	char* alias;
	char* val;
	struct node* next;
} node_t;

node_t* alias_head; /*points to the head of the linked list*/


int main()
{         
	alias_head = NULL;
	printf("hello I am computer\n");
	printf("I make am shell\n");
	printf("wat do\n");

	while(1){	
		printf("%s$ ",PWD);
		yyparse();
	}
	return 0;
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

void ls(){
	int process;
	process = fork();
	if(process > 0)		/* parent */
		wait((int*)0);
	else if(process == 0)	/* child */
	{
		execlp("ls", "ls", "--color=auto",(char *) NULL ); /*search current direct*/
		exit(1);
	}
	else if(process == -1)		/* can't create a new process */
	{
		fprintf(stderr, "Can't fork!\n");
		exit(2);
	}
}

/*Alias Linked List*/

void push(node_t** head, char* alias, char* val) { /*add new node to linked list*/
    node_t* current = *head; /*define current as pointing to head*/
    node_t* newNode = malloc(sizeof(node_t)); /*make space for new node*/
    newNode->alias = alias; /*define properties of new node*/
    newNode->val = val;
    newNode->next = NULL; /*new node should be at the end of the list*/
    if (current != NULL)/*if there is alredy a node in the list (not an empty list)*/
    {
        while (current->next != NULL && strcmp(current->alias, alias) != 0) /*while their are more nodes in the list and the alias of the current node != to the new alias*/
        {
            current = current->next; /*iterate through the list*/
        }
        if (strcmp(current->alias, alias) == 0) /*if the alias you tried to add alredy exists*/
        {
            current->val = val; /*update the existing alias */
            free(newNode);/*release the new node we dont need it*/
            return;
        }
        current->next = newNode; /*append new node to list*/
    }
    else
    {
        *head = newNode; /*place new node at the head of the empty list*/
    }
    
}

void print_alias_list(node_t* head) /*print alias list duh*/
{
    node_t* current = head; /*define the passed in head as the current node*/
    while (current != NULL)/*while there are nodes in the list*/
    {
        printf("alias %s='%s'\n", current->alias, current->val); /*print info for current node*/
        current = current->next;/*go to next node*/
    }
}

int remove_by_alias(node_t** head, char * alias) { /*search for a node with a matching alias and remove it*/
    node_t* current = *head; /*define start of list*/
    node_t* prev = NULL; /*track previous node to repair list*/
    while (1) {/*search through list untill..*/
        if (current == NULL) return -1; /*if end of the list is reached with out a match return -1 for err*/
        if (strcmp(current->alias, alias) == 0) break;/*break if match is found*/
        prev = current; /*iterate through list while tracking previous node*/
        current = current->next;
    }
    if (current == *head) *head = current->next; /*id the first node is a match make the second node the new head*/
    if (prev != NULL) prev->next = current->next;/*make the previous node point to the node after the deleted node assuming that a previous node exists*/
    free(current); /*delete current node*/
    return 0;
}

char* retrieve_val(node_t* head, char* alias)/*search the list and return the value of a given alias */
{
    node_t* current = head;
    while (current != NULL) /*while not at the end of list*/
    {
        if (strcmp(current->alias, alias) == 0)/*if match found*/
        {
            return current->val; /*return the val*/
        }
        current = current->next;/*else keep looking*/
    }
    return NULL; /*no match found*/
}


char* alias_replace(char* alias) 
{
    char* val = retrieve_val(alias_head, alias);/*look for alias and return matching value*/
    if (val != NULL) return val; /*if the alias exists return value*/
    return alias; /*else return the original input*/
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
		| commands command 	{printf("%s$ ",getenv("PWD"));}; /*print the current working dir*/

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
			printf("%s$ ",path);
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