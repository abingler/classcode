#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "node.h"

#define HOME getenv("HOME")
#define PWD getenv("PWD")
#define copystring(a,b) strcpy((a=(char *)malloc(strlen(b)+1)),b)

extern int yylineno;
extern char** environ;

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
    printf("I make the shell\n");
    printf("wat do\n");

    while(1){   
        printf("%s> ",PWD);
        yyparse();
    }
    return 0;
} 

void ls(){
    int process;
    char *const argv[2] = {"/bin/ls", NULL};
    process = fork();
    if(process > 0)     /* parent */
        wait((int*)0); //Casting zero to an int pointer?
    else if(process == 0)   /* child */
    {
        execve( "/bin/ls", argv, environ ); //Execute commands
        //execlp("ls", "ls", "--color=auto",(char *) NULL ); /*search currentNode direct*/
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
    arg_node* currentNode = args->next; //currentNode equals arg after cd
    int evenNodes = 0;
    int validCheck;
    char* location;
    while(currentNode != NULL && evenNodes!= 1){
        currentNode = currentNode->next;
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
        //printf("args->arg_val = %s \n",args->arg_val);
        //printf("args->next->arg_val = %s \n",args->next->arg_val);
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

void push(node_t** head, char* alias, char* val) { /*add new node to linked list*/
    node_t* currentNode = *head; /*define currentNode as pointing to head*/
    node_t* newNode = malloc(sizeof(node_t)); /*make space for new node*/
    newNode->alias = alias; /*define properties of new node*/
    newNode->val = val;
    newNode->next = NULL; /*new node should be at the end of the list*/
    if (currentNode != NULL)/*if there is alredy a node in the list (not an empty list)*/
    {
        while (currentNode->next != NULL && strcmp(currentNode->alias, alias) != 0) /*while their are more nodes in the list and the alias of the currentNode node != to the new alias*/
        {
            currentNode = currentNode->next; /*iterate through the list*/
        }
        if (strcmp(currentNode->alias, alias) == 0) /*if the alias you tried to add alredy exists*/
        {
            currentNode->val = val; /*update the existing alias */
            free(newNode);/*release the new node we dont need it*/
            printf("Alias has been updated \n");
            return;
        }
        currentNode->next = newNode; /*append new node to list*/
    }
    else
    {
        *head = newNode; /*place new node at the head of the empty list*/
    }
    
}

void printAliasList(node_t* head) /*print alias list duh*/
{
    node_t* currentNode = head; /*define the passed in head as the currentNode node*/
    while (currentNode != NULL)/*while there are nodes in the list*/
    {
        printf("alias %s='%s'\n", currentNode->alias, currentNode->val); /*print info for currentNode node*/
        currentNode = currentNode->next;/*go to next node*/
    }
}
char* retrieveVal(node_t* head, char* alias)/*search the list and return the value of a given alias */
{
    node_t* currentNode = head;
    while (currentNode != NULL) /*while not at the end of list*/
    {
        if (strcmp(currentNode->alias, alias) == 0)/*if match found*/
        {
            return currentNode->val; /*return the val*/
        }
        currentNode = currentNode->next;/*else keep looking*/
    }
    return NULL; /*no match found*/
}

char* aliasReplace(char* alias) 
{
    char* val = retrieveVal(aliasHead, alias);/*look for alias and return matching value*/
    if (val != NULL) return val; /*if the alias exists return value*/
    return alias; /*else return the original input*/
}

int removeByAlias(node_t** head, char * alias) { /*search for a node with a matching alias and remove it*/
    node_t* currentNode = *head; /*define start of list*/
    node_t* prev = NULL; /*track previous node to repair list*/
    while (1) {/*search through list untill..*/
        if (currentNode == NULL) return -1; /*if end of the list is reached with out a match return -1 for err*/
        if (strcmp(currentNode->val, alias) == 0) break;/*break if match is found*/
        prev = currentNode; /*iterate through list while tracking previous node*/
        currentNode = currentNode->next;
    }
    if (currentNode == *head) *head = currentNode->next; /*id the first node is a match make the second node the new head*/
    if (prev != NULL) prev->next = currentNode->next;/*make the previous node point to the node after the deleted node assuming that a previous node exists*/
    free(currentNode); /*delete currentNode node*/
    return 0;
}

void alias(arg_node* args)
{
    arg_node* currentNode = args->next;
    int n = 0;
    while (currentNode != NULL && n != 2)
    {
        n++;
        currentNode = currentNode->next;
    }
    if (n == 2)
    {
        char* arg_alias = args->next->arg_val;
        char* arg_value = args->next->next->arg_val;
        push(&aliasHead, arg_alias, arg_value);
    }
    else if (n == 0)
    {
        printAliasList(aliasHead);
    }
    else
    {
        fprintf(stderr, "error at line %d: incorrect number of args for alias\n", yylineno);
    }
}
void unalias(arg_node* args)
{
    if (args->next != NULL) {
        printf("no next arg\n");
        removeByAlias(&aliasHead, args->next->arg_val);}
    else fprintf(stderr, "error at line %d: too few args for unalias\n", yylineno);
}
void bye()
{
    printf("Exiting the shell now. Goodbye.\n");
    exit(0);
}

void setEnv(arg_node* args)
{
    arg_node* currentNode = args->next;
    if(currentNode->next != NULL && currentNode->next->next == NULL){//2 and only 2 vars
        char* envName = insertEnv(currentNode->arg_val);/*extract word1*/
        char* envVal = insertEnv(currentNode->next->arg_val);/*extract word2*/
        int result = setenv(envName, envVal, 1);
        if(result == -1){
            printf("Failed to set variable %s to %s.\n", envName, envVal);
        }
    }
    else{
        printf("printenv requires 2 variables \n");
    }

};

void printEnv()
{       
            extern char **environ;  
            int i=0;
            while(environ[i])
                printf("%s\n", environ[i++]);
            char* path = getenv("PATH");
            printf("%s> \n",path);
}

void unsetEnv(arg_node* args){
            arg_node* currentNode = args->next;
            char* name = currentNode->arg_val;
            if(getenv(name))
                unsetenv(name);\
            else
                printf("Variable %s does not exist.\n", name);
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
    arg_node* currentNode = head;
    token = strtok(NULL, delimiter); 
    while (token != NULL)
    {
          currentNode->next = malloc(sizeof(arg_node));
          currentNode = currentNode->next;
          currentNode->arg_val = token;
          currentNode->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return head;
}


aliasArgReplace(arg_node* args){
    int nestedAliasLoop = 0;
    int aliasLoop = 0; //guard against infinite expansion
    arg_node* original = args; //first
    while(nestedAliasLoop<100){
        aliasLoop =0;
        while(args->arg_val != aliasReplace(args->arg_val) && aliasLoop < 100) //where an alias exists
            {
                args->arg_val = aliasReplace(args->arg_val);
                /*printf("debug1a args->arg_val = %s\n",args->arg_val );
                if(args->next != NULL){
                    args = args->next;
                    printf("debug1b args->arg_val = %s\n",args->arg_val );
                }*/
                aliasLoop++;
            }
            if (aliasLoop == 100 || aliasLoop == 0) break; //haveing over 100 alias in args is unlikly most likly a loop
            if (hasWhitespace(args->arg_val) && !whitespaceOnly(args->arg_val)){ //if spaces exist in alias
                args = splitToTokens(args->arg_val, " \t"); //break it into tokens about the spaces
                arg_node* currentNode = args; //define the currentNode nose
                while (currentNode->next != NULL) currentNode = currentNode->next; //move to the next node while it exists
                currentNode->next = original->next;// reset currentNode node -> next for next loop
                free(original);
            }
            else break;//no nested alias            
            nestedAliasLoop++;
    }
    if (nestedAliasLoop != 100 && aliasLoop != 100) { 
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


commandBlock(arg_node* args)
{

    /*Variable decs */
    int outRedirects = 0;
    int pipesFound = 0;
    char* outputRed = "";
    char* inputRed;
    int j = 0;
    int pipeBuffer[50][2]; //SET MAX PIPES

    arg_node* tempNode = args;
    arg_node* currentNode = args;
    while(args->next!=NULL){
    args = args->next;
    args = aliasArgReplace(args);
    }
    args = tempNode; //Why don't we reuse this
    args = aliasArgReplace(args);

    currentNode = args;
    while (currentNode != NULL)
    {
        printf("%s\n",currentNode->arg_val);
        currentNode = currentNode->next;
    }

    if (args == NULL) return;
    const char* Commands[8] = {"bye","ls","cd","alias","unalias","setenv","printenv","unsetenv"};
    int i;
    for(i = 0; i< 8; i++){
        if (strcmp(args->arg_val, Commands[i]) == 0){
            switch (i){
                case 0:
                    bye();
                //case 1:
                //    ls(); //Pass in argument if being written to file
                //    return;
                case 2:
                    cd(args);
                    return;
                case 3:
                    alias(args);
                    return;
                case 4:
                    unalias(args);
                    return;
                case 5:
                    setEnv(args);
                    return;
                case 6:
                    printEnv(); //Pass in argument if being written to file
                    return;
                case 7:
                    unsetEnv(args);
                    return;

                }
            }
    }

    /*IO redirection by Andrew B */
    /* 
    1. Need to throw error if output for file makes no sense or if no file given
    2. Handle case for "< file"
    */    


    /*NOTES TO DELETE 
        tempNode1 -> current
        tempNode2 -> h
        tempNext -> nextNode
    */


    arg_node* tempNode1 = args; //Move these to the top?
    arg_node* tempNode2 = args;
    
    printf("You made it this far 1\n");

    while(tempNode1 != NULL){ //Run through args to find some pipes
        if(strcmp(tempNode1->arg_val, "|") == 0){ //Did we find a "|"
            pipesFound = pipesFound + 1; //pipesFound++; C being weird
        }
        tempNode1 = tempNode1->next; //Move to the next node
    }


    arg_node** commandTable = malloc(sizeof(arg_node*)*(pipesFound + 1)); //Pipes sepearate commands in table 
    /* Each pipe is a complete command, thus pipe I/O works */

    tempNode1 = args; //Reset 
    i = 0; //i is unchanged

    /*
        [ls] -> ["|"] -> [wc] -> [-l]

    */

    printf("Found %d pipe(s) \n", pipesFound);

    /*Need to predict piping */
    while(tempNode1->next != NULL){ //current != NULL //Possibly merge this with previous while loop?
        printf("Looped\n");
        arg_node* tempNext = tempNode1->next; //AndrewH won't this mess it up?

        if(strcmp(tempNode1->next->arg_val, "|") == 0){ //Next value is string
            commandTable[i] = tempNode2;
            tempNode2 = tempNode1->next->next; //Grab set of commands before pipes
            tempNode1->next = NULL;
            i++;
        }
        tempNode1 = tempNext;
        printf("Loop and tempNode1->next = %s\n", tempNode1->next);
    } //ONly seg faults if | has no end


    //BE CAREFUL Got a seg fault because i was trying to find arg_value of a null

     if(tempNode2 == NULL){ //Handle the DUMB IDIOT USER putting a pipe at the end of the line
        printf("error at line %d: entered pipe as last command\nYOU'RE SO DUMB\n", yylineno);
        free(commandTable); //This is what self commenting code looks like
        return;
    } 

    commandTable[i] = tempNode2; //Save last command to the table

    /*TO DO handle wildcarding */
    /*TO DO handle path stuff */

    if(pipesFound > 50){ //SET MAX PIPES
        pipesFound = 50;
    }
    //HOLY SHIT I FOUND OUT HOW TO PIPE! gotta connect an array of two together
    //pipe(array[2], int flags);
    
    for(j = 0; j < pipesFound; j++){
        pipe(pipeBuffer[j]); //Connect arrays together
    }

    //Wait for completion?
    printf("Done\n");

    for(i = 0; i < pipesFound + 1; i++){
        if(i == pipesFound){ 
            printf("Test\n");
            //Last pipe or 0 pipes
            //TO DO Set dynamically determine args list size 
            //  TO DO File I/O stuff
            //  
            int numArgs = 1;
            char *argv[ numArgs + 1 ]; //2 arguments
            argv[numArgs] = NULL;
            char *point1;
            int k = 0;
            tempNode1 = commandTable[i];
            while(tempNode1 != NULL){
                point1 = tempNode1->next;
                if(k < numArgs-1){
                    argv[k] = point1; //Load arguments into the table
                }
                tempNode1 = tempNode1 ->next;
                k++;
                // TO DO: Set up File I/O and & symbols

            }
            printf("Test2\n");
             //Execute individual processes
            int child = fork();
            if(child == 0){ //fork success
                //TO DO: Handle file I/O
                if(pipesFound > 0){ //On the last pipe
                    dup2(pipeBuffer[i-1][0], STDIN_FILENO); //Pipe that shit in
                    for(j = 0; j < pipesFound; j++){
                        close(pipeBuffer[j][0]); //Close all our pipes
                        close(pipeBuffer[j][0]);
                    }
                }
                //commandTable[i]->arg_val = "/bin/ls";   
                //char *const argv[2] = {"/bin/ls", NULL};
                printf("Whats in the command table %s \n", commandTable[i]->arg_val);
                execve(commandTable[i]->arg_val, argv, environ);
                //perror("execve"); //Print the error
                //_exit(EXIT_FAILURE);
            }

        }
        printf("Looped\n");
    }

    printf("We're done here\n");

    return;


}


