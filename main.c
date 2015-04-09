#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"

#define HOME getenv("HOME")
#define PWD getenv("PWD")
#define copystring(a,b) strcpy((a=(char *)malloc(strlen(b)+1)),b)

extern int yylineno;

typedef struct node { /*all nodes in alias linked list need a name value and pointer to the next node*/
	char* alias;
	char* val;
	struct node* next;
} node_t;

node_t* aliasHead; /*points to the head of the linked list*/


int main()
{         
	aliasHead = NULL;
	printf("hello I am computer\n");
	printf("I make am shell\n");
	printf("wat do\n");

	while(1){	
		printf("%s> ",PWD);
		yyparse();
	}
	return 0;
} 

void ls(){
    int process;
    process = fork();
    if(process > 0)     /* parent */
        wait((int*)0); //Casting zero to an int pointer?
    else if(process == 0)   /* child */
    {
        execlp("ls", "ls", "--color=auto",(char *) NULL ); /*search current direct*/
        exit(1);
    }
    else if(process == -1)      /* can't create a new process */
    {
        fprintf(stderr, "Can't fork!\n");
        exit(2);
    }
}

char * insertEnv(char* input);

void cd(arg_node* args){
    arg_node* current = args->next; //current equals arg after cd
    int evenNodes = 0;
    int validCheck;
    char* location;
    while(current != NULL && evenNodes!= 1){
        current = current->next;
        evenNodes++;
    }   
    if (evenNodes == 0){ //if no args go to home
        if (getenv("HOME") != NULL){
            location = (getenv("HOME"));  /*move to home*/
        }
        else{
        fprintf(stderr, "error at line %d: 'HOME' == NULL\n", yylineno); 
        }
    }
    else{
        printf("args->arg_val = %s \n",args->arg_val);
        printf("args->next->arg_val = %s \n",args->next->arg_val);
        //location = args->next->arg_val;          /*change dir*/
        args = args->next;
        location = args->arg_val;
    }
    validCheck = chdir(location); //attempt to change path and check if its valid
    if (validCheck != 0) fprintf(stderr, "error at line %d:'%s' does not exist here\n", yylineno, location);
    char pwd[4096];
    getcwd(pwd, sizeof(pwd)); /*copy absolute pathname to pwd[]*/
    setenv("PWD", pwd, 1);
}

commandBlock(arg_node* args)
{
    arg_node* current = args;
    while (current != NULL)
    {
        printf("%s\n",current->arg_val);
        current = current->next;
    }
	args = aliasArgReplace(args);
    current = args;
    while (current != NULL)
    {
        printf("%s\n",current->arg_val);
        current = current->next;
    }

    if (args == NULL) return;
    const char* Commands[2] = {"ls","cd"};
    int i;
    for(i = 0; i< 2; i++){
        if (strcmp(args->arg_val, Commands[i]) == 0){
            switch (i){
                case 0:
                    ls();
                    printf("debug: exit ls function \n");
                    return;
                case 1:
                    cd(args);
                    printf("debug: exit cd function \n");
                    return;

                }
            }
    }
    

    
    
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
    char* val = retrieve_val(aliasHead, alias);/*look for alias and return matching value*/
    if (val != NULL) return val; /*if the alias exists return value*/
    return alias; /*else return the original input*/
}

arg_node* splitToTokens(char* string, char* delimiter)
{
    char* token;
    char* tmp = strdup(string);
    token = strtok(tmp, delimiter);
    arg_node* head = malloc(sizeof(arg_node));
    head->next = NULL;
    if (token != NULL)
    {
        head->arg_val = token;
    }
    else
    {
        head->arg_val = tmp;
    }
    arg_node* current = head;
    token = strtok(NULL, delimiter); 
    while (token != NULL)
    {
          current->next = malloc(sizeof(arg_node));
          current = current->next;
          current->arg_val = token;
          current->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return head;
}


aliasArgReplace(arg_node* args){
	int n = 0;
	int n2 = 0; //guard against infinite expansion
    arg_node* original = args; //first
    arg_node* currentNode = args;
	while(n<100){
        n2 =0;
        printf("debug: alias_replace(args->arg_val) = %s\n",alias_replace(args->arg_val));
        while(args->arg_val != alias_replace(args->arg_val) && n2 < 100) //where an alias exists
        	{
        		args->arg_val = alias_replace(args->arg_val);
                printf("debug1a args->arg_val = %s\n",args->arg_val );
                if(args->next != NULL){
                    args = args->next;
                    printf("debug1b args->arg_val = %s\n",args->arg_val );
                }
       		 	n2++;
        	}
            printf("debug2 before break, n2 = %d\n",n2);
            printf("debug3 before break, n = %d\n",n);
        	if (n2 == 100 || n2 == 0) break; //haveing over 100 alias in args is unlikly most likly a loop
            /*
        	if (hasWhitespace(args->arg_val) && !whitespaceOnly(args->arg_val)){ //if spaces exist in alias
        		args = splitToTokens(args->arg_val, " \t"); //break it into tokens about the spaces
        	    arg_node* currentNode = args; //define the current nose
        	    while (currentNode->next != NULL) currentNode = currentNode->next; //move to the next node while it exists
        	    currentNode->next = original->next;// reset current node -> next for next loop
        	    free(original);
        	}
        	else break;//no nested alias
            printf("debug no break args->arg_val = %s\n",args->arg_val );
            printf("debug2 args->arg_val = %s\n",args->arg_val );
        	*/
        n++;
    }
    printf("debug2 after break, n2 = %d\n",n2);
    printf("debug3 after break, n = %d\n",n);   
	if (n != 100 && n2 != 100) { 
        printf("debug4 inside if args->arg_val = %s\n",args->arg_val ); 
        args->next = original->next;
        //free(original);
        printf("debug4 inside if args->arg_val = %s\n",args->arg_val ); 
        return args;
    } //function succsefull
	else
	{
        fprintf(stderr, "on line %d: infinite alias error occured\n", yylineno);
        arg_node* prev = NULL;//empty args list and free nodes
        while (args != NULL){
                prev = args;
                args = args->next;
                free(prev);
        }
        return NULL;
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
            printf("Alias %s has been updated \n", alias);
            return;
        }
        current->next = newNode; /*append new node to list*/
    }
    else
    {
        *head = newNode; /*place new node at the head of the empty list*/
    }
    
}

void printAliasList(node_t* head) /*print alias list duh*/
{
    node_t* current = head; /*define the passed in head as the current node*/
    while (current != NULL)/*while there are nodes in the list*/
    {
        printf("alias %s='%s'\n", current->alias, current->val); /*print info for current node*/
        current = current->next;/*go to next node*/
    }
}

int removeByAlias(node_t** head, char * alias) { /*search for a node with a matching alias and remove it*/
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

char * insertEnv(char* input){ /*function extrats env variable*/
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
            var = var + 2;              //get rid of ${
            var[i-start-2] = '\0';          //get rid of ending }
            
            s = replace(s, subbuf, getenv(var));
        }
    
    }
    return s;
}

void replaceEscape(char* str)
{
    char* p_read = str;
    char* p_write = str;
    while (*p_read) {
        *p_write = *p_read++;
        p_write += (*p_write != '\\' || *(p_write + 1) == '\\');
    }
    *p_write = '\0';
}

int hasWhitespace(char* string)
{
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        if (string[i] == '\t' || string[i] == ' ') return 1;
    }
    return 0;
}

int whitespaceOnly(char* string)
{
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        if (string[i] != '\t' && string[i] != ' ') return 0;
    }
    return 1;
}
