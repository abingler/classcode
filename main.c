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

node_t* alias_head; /*points to the head of the linked list*/


int main()
{         
	alias_head = NULL;
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



commandBlock(arg_node* args)
{
	args = aliasArgReplace(args);
	/*while (args != NULL)
        {
            printf("%s\n",args->arg_str);
            args = args->next;
        }*/
    const char* Commands[1] = {"ls",};
    int i;
    for(i = 0; i< 1; i++){
        if (strcmp(args->arg_str, Commands[i]) == 0){
            switch (i){
                case 0:
                    ls();
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
    char* val = retrieve_val(alias_head, alias);/*look for alias and return matching value*/
    if (val != NULL) return val; /*if the alias exists return value*/
    return alias; /*else return the original input*/
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


aliasArgReplace(arg_node* args){
	arg_node* original = args; //first
	int nestedAliasCount = 0;
	int aliasCount = 0; //guard against infinite expansion
	while(nestedAliasCount<100){
		while(args->arg_str != alias_replace(args->arg_str) && aliasCount < 100) //where an alias exists
        	{
        		args->arg_str = alias_replace(args->arg_str);
       		 	aliasCount++;
        	}
        	if (aliasCount == 100 || aliasCount == 0) break; //haveing over 100 alias in args is unlikly most likly a loop
        	if (hasWhitespace(args->arg_str) && !whitespaceOnly(args->arg_str)) //if spaces exist in alias
        	{
        		args = split_to_tokens(args->arg_str, " \t"); //break it into tokens about the spaces
        	    arg_node* currentNode = args; //define the current nose
        	    while (currentNode->next != NULL) currentNode = currentNode->next; //move to the next node while it exists
        	    currentNode->next = original->next;// reset current node -> next for next loop
        	    free(original);
        	}
        	else break;//no nested alias
        	nestedAliasCount++;
    	}
		if (nestedAliasCount != 100 && aliasCount != 100) return args; //function succsefull
		else
		{
        fprintf(stderr, "on line %d: infinite alias error occured\n", yylineno);
        arg_node* prev = NULL;//empty args list and free nodes
        while (args != NULL)
        {
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
            printf("Alias has been updated \n");
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


