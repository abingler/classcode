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
} nodeT;

nodeT* aliasHead; /*points to the head of the linked list*/


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

/* void ls(){
    int process;
    char *const argv[2] = {"/bin/ls", NULL};
    process = fork();
    if(process > 0)     
        wait((int*)0); //Casting zero to an int pointer?
    else if(process == 0)   
    {
        execve( "/bin/ls", argv, environ ); //Execute commands
        //execlp("ls", "ls", "--color=auto",(char *) NULL ); 
        exit(1);
    }
    else if(process == -1)      
    {
        fprintf(stderr, "Can't fork!\n");
        exit(2);
    }
} */

char * insertEnv(char* input);

void cd(argNode* args){
    argNode* currentNode = args->next; //currentNode equals arg after cd
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
        //printf("args->argVal = %s \n",args->argVal);
        //printf("args->next->argVal = %s \n",args->next->argVal);
        location = args->next->argVal;          /*change dir*/
        args = args->next;
        location = args->argVal;
    }
    validCheck = chdir(location); //attempt to change path and check if its valid
    if (validCheck != 0) fprintf(stderr, "error at line %d:'%s' does not exist here\n", yylineno, location);
    char pwd[4096];
    getcwd(pwd, sizeof(pwd)); /*copy absolute pathname to pwd[]*/
    setenv("PWD", pwd, 1);
}

void push(nodeT** head, char* alias, char* val) { /*add new node to linked list*/
    nodeT* currentNode = *head; /*define currentNode as pointing to head*/
    nodeT* newNode = malloc(sizeof(nodeT)); /*make space for new node*/
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

void printAliasList(nodeT* head) /*print alias list duh*/
{
    nodeT* currentNode = head; /*define the passed in head as the currentNode node*/
    while (currentNode != NULL)/*while there are nodes in the list*/
    {
        printf("alias %s='%s'\n", currentNode->alias, currentNode->val); /*print info for currentNode node*/
        currentNode = currentNode->next;/*go to next node*/
    }
}
char* retrieveVal(nodeT* head, char* alias)/*search the list and return the value of a given alias */
{
    nodeT* currentNode = head;
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

int removeByAlias(nodeT** head, char * alias) { /*search for a node with a matching alias and remove it*/
    nodeT* currentNode = *head; /*define start of list*/
    nodeT* prev = NULL; /*track previous node to repair list*/
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

void alias(argNode* args)
{
    argNode* currentNode = args->next;
    int n = 0;
    while (currentNode != NULL && n != 2)
    {
        n++;
        currentNode = currentNode->next;
    }
    if (n == 2)
    {
        char* argAlias = args->next->argVal;
        char* argValue = args->next->next->argVal;
        push(&aliasHead, argAlias, argValue);
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
void unalias(argNode* args)
{
    if (args->next != NULL) {
        printf("no next arg\n");
        removeByAlias(&aliasHead, args->next->argVal);}
    else fprintf(stderr, "error at line %d: too few args for unalias\n", yylineno);
}
void bye()
{
    printf("Exiting the shell now. Goodbye.\n");
    exit(0);
}


void setEnv(argNode* args)
{
    argNode* currentNode = args->next;
    if(currentNode->next != NULL && currentNode->next->next == NULL){//2 and only 2 vars
        char* envName = insertEnv(currentNode->argVal);/*extract word1*/
        char* envVal = insertEnv(currentNode->next->argVal);/*extract word2*/
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

void unsetEnv(argNode* args){
            argNode* currentNode = args->next;
            char* name = currentNode->argVal;
            if(getenv(name))
                unsetenv(name);\
            else
                printf("Variable %s does not exist.\n", name);
}

argNode* splitToTokens(char* string, char* delimiter)
{
    char* token;
    char* tmp = strdup(string);
    token = strtok(tmp, delimiter);
    argNode* head = malloc(sizeof(argNode));
    head->next = NULL;
    if (token != NULL)
    {
        head->argVal = token;
    }
    else
    {
        head->argVal = tmp;
    }
    argNode* currentNode = head;
    token = strtok(NULL, delimiter); 
    while (token != NULL)
    {
          currentNode->next = malloc(sizeof(argNode));
          currentNode = currentNode->next;
          currentNode->argVal = token;
          currentNode->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return head;
}


aliasArgReplace(argNode* args){
    int nestedAliasLoop = 0;
    int aliasLoop = 0; //guard against infinite expansion
    argNode* original = args; //first
    while(nestedAliasLoop<100){
        aliasLoop =0;
        while(args->argVal != aliasReplace(args->argVal) && aliasLoop < 100) //where an alias exists
            {
                args->argVal = aliasReplace(args->argVal);
                /*printf("debug1a args->argVal = %s\n",args->argVal );
                if(args->next != NULL){
                    args = args->next;
                    printf("debug1b args->argVal = %s\n",args->argVal );
                }*/
                aliasLoop++;
            }
            if (aliasLoop == 100 || aliasLoop == 0) break; //haveing over 100 alias in args is unlikly most likly a loop
            if (hasWhitespace(args->argVal) && !whitespaceOnly(args->argVal)){ //if spaces exist in alias
                args = splitToTokens(args->argVal, " \t"); //break it into tokens about the spaces
                argNode* currentNode = args; //define the currentNode nose
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
        argNode* prev = NULL;//empty args list and free nodes
        while (args != NULL){
                prev = args;
                args = args->next;
                free(prev);
        }
        return NULL;
    }
}



/*string handling functions*/

char* concat(char* string1, char* string2)
{
    char* result = malloc(strlen(string1)+strlen(string2)+1);
    strcpy(result, string1);
    strcat(result, string2);
    return result;
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

char * insertEnv(char* input){ /*function extrats env variable*/
    char * envvar = input;
    int i;
    int validFlag = 0;
    int start;
    int end;
    for (i = 0; i < strlen(envvar); i++) /*iterate through input*/
    {
        if(envvar[i] == '$') start = i;
        if(envvar[i] == '{' && i == start+1) validFlag = 1;
        if(envvar[i] == '}' && validFlag)
        {
            char subbuf[4096];
            memcpy(subbuf, &envvar[start], i-start+1);
            subbuf[i-start+1] = '\0';

            char * var; /*extrat var from ${var}*/
            copystring(var, subbuf);
            var = var + 2;              //get rid of ${
            var[i-start-2] = '\0';          //get rid of ending }
            
            envvar = replace(envvar, subbuf, getenv(var));
        }
    
    }
    return envvar;
}

void replaceEscape(char* str)
{
    char* pRead = str;
    char* pWrite = str;
    while (*pRead) {
        *pWrite = *pRead++;
        pWrite += (*pWrite != '\\' || *(pWrite + 1) == '\\');
    }
    *pWrite = '\0';
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


/*New Functions ****************************************************************************/
int has_character(char* string, char ch)
{
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        if (string[i] == ch) return 1;
    }
    return 0;
}

argNode* split_to_tokens(char* string, char* delimiter)
{
    char* token;
    char* tmp = strdup(string);
    token = strtok(tmp, delimiter);
    argNode* head = malloc(sizeof(argNode));
    head->next = NULL;
    if (token != NULL)
    {
        head->argVal = token;
    }
    else
    {
        head->argVal = tmp;
    }
    argNode* current = head;
    token = strtok(NULL, delimiter); 
    while (token != NULL)
    {
          current->next = malloc(sizeof(argNode));
          current = current->next;
          current->argVal = token;
          current->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return head;
}

int get_args_list_size(argNode * head)
{
    argNode * current = head;
    int counter = 0;
    while (current != NULL)
    {
        if (strcmp(current->argVal, ">") != 0 &&
            strcmp(current->argVal, ">>") != 0 &&
            strcmp(current->argVal, "<") != 0 &&
            strcmp(current->argVal, "|") != 0 &&
            (current->argVal[0]!='2' && current->argVal[1]!='>') &&
            strcmp(current->argVal, "&") != 0) {
                counter++;
                current = current->next; 
        }
        else break;
    }
    return counter;
}


/*END New Functions *************************************************************************/


commandBlock(argNode* args){


    argNode* tempNode = args;
    argNode* currentNode = args;
    while(args->next!=NULL){
    args = args->next;
    args = aliasArgReplace(args);
    }
    args = tempNode; //Why don't we reuse this
    args = aliasArgReplace(args);

    currentNode = args;
    
    while (currentNode != NULL)
    {
        printf("%s\n",currentNode->argVal);
        currentNode = currentNode->next;
    }

    if (args == NULL) return;
    const char* Commands[8] = {"bye","ls","cd","alias","unalias","setenv","printenv","unsetenv"};
    int i;
    for(i = 0; i< 8; i++){
        if (strcmp(args->argVal, Commands[i]) == 0){
            switch (i){
                case 0:
                    bye();
               // case 1:
               //     ls(); //Pass in argument if being written to file
               //     return;
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

    //Variables for CommandTable (Should be at top of commandBlock?)
    argNode* list = args;
    int pipesFound = 0;
    argNode* temp = args;
    int val = 0;
    int test = 0;


    while (list != NULL){ //Determine pipes found
        if (strcmp(list->argVal, "|") == 0){
            pipesFound++;
        }
        list = list->next;
    }
 
    argNode** commandTable = malloc( //Create Comand Table
        sizeof(argNode*)*(pipesFound+1)); //+1 for 0 pipes case


    list = args;
    while (list->next != NULL){ //Parse the command table out
        argNode* node2 = list->next; 
        if (strcmp(list->next->argVal, "|") == 0)
        {
            commandTable[val++] = temp; //Grabbing the head of each set of arguments
            temp = list->next->next;
            list->next = NULL; //Break the command apart from the rest
        }
        list = node2;
    }

    //Handle the DUMB IDIOT USER 
    if (temp == NULL)
    {
        fprintf(stderr, "error on line: %d, user placed pipe as last command\n", yylineno);
        free(commandTable);
        return;
    }
    commandTable[val] = temp;


    /* //TESTING COMMANDTABLE
    for(test = 0; test <= val; test++){
        printf("Value in commandTable[%d] is: %s\n", test, commandTable[test]->argVal);
    } */



    for (val = 0; val < pipesFound + 1; val++) //For each group of commands
    {
        argNode* list = commandTable[val];
       /* while (list != NULL)
        {
            argNode* original = list->next;
            if (has_character(list->argVal, '*') || has_character(list->argVal, '?')) //Do we have any wildcard characters here //WILDCARDING
            {
               glob_t globbuf; //Getcha globbins
               if (glob(list->argVal, 0, NULL, &globbuf) == 0)
               {

                  size_t i;
                  argNode* iter = list;
                  for (i = 0; i < globbuf.gl_pathc; i++)
                  {
                    iter->argVal = strdup(globbuf.gl_pathv[i]);
                    if (i != globbuf.gl_pathc - 1)
                    {
                      iter->next = malloc(sizeof(argNode));
                      iter = iter->next;
                    }
                  }
                  iter->next = original;
                  globfree(&globbuf);
                }
            }
            list = original;
        } */
        if ( !has_character(commandTable[val]->argVal, '/') ) //If we do not have the '/' character ?No path to bin?
        {
            char* path = getenv("PATH");
            argNode* paths = split_to_tokens(path, ":");
            argNode* list_path = paths;
            char* fname;
            int found = 0;
            while (list_path != NULL && found == 0)
            {
                char* temp = concat(list_path->argVal, "/");
                fname = concat(temp, commandTable[val]->argVal);
                free(temp);
                if( access( fname, F_OK ) != -1 )
                {
                    found = 1;
                    commandTable[val]->argVal = fname;
                }
                else
                {
                    free(fname);
                }
                list_path = list_path->next;
            }
            if (found == 0)
            {
                fprintf(stderr, "error at line %d: command '%s' not found\n", yylineno, commandTable[val]->argVal);
                return;
            }
        }
        else
        {
            if( access( commandTable[val]->argVal, F_OK|X_OK ) != 0 )
            {
                fprintf(stderr, "error at line %d: command '%s' not found\n", yylineno, commandTable[val]->argVal);
                return;
            }
        }
    }
    if (pipesFound > 200) pipesFound = 200;
    int pipeBuffer[200][2];
    int n;
    for (n = 0; n < pipesFound; n++)
    {
        pipe(pipeBuffer[n]);

    }
    int wait_for_comp = 1;
    for (val = 0; val < pipesFound + 1; val++)
    {
        if ( val == pipesFound ) {
            int arg_size = get_args_list_size(commandTable[val])+1;
            char *argv[ arg_size+1 ];
            char* input_file = "";
            char* output_file = "";
            char* err_file = "";
            int errisstdout = 0;
            char* curr_arg;
            int i = 0;
            argNode* list = commandTable[val];
            while(list != NULL) {
                curr_arg = list->argVal;
                if (i<arg_size-1) {
                argv[i] = curr_arg;} //get args before >,<,|,etc
                list = list->next;
                i++;
                if (strcmp(curr_arg, ">") == 0 || strcmp(curr_arg, ">>") == 0) { //new file for output
                    if (list == NULL)
                    {
                        fprintf(stderr, "error at line %d: no output file specified after >\n", yylineno );
                        return;
                    }
                    output_file = list->argVal;
                    list = list->next;
                    i++;
                } else if (strcmp(curr_arg, "<") == 0) {//new file for input
                    if (list == NULL)
                    {
                        fprintf(stderr, "error at line %d: no input file specified after <\n", yylineno );
                        return;
                    }
                    input_file = list->argVal;
                    list = list->next;
                    i++;
                } else if (strcmp(curr_arg, "2>$1") == 0) {
                    //set std err to std out
                    errisstdout = 1;
                } else if (curr_arg[0]=='2' && curr_arg[1]=='>') {
                    //set std err to another file
                    int k = 0;
                    char errf[strlen(curr_arg) - 2];
                    for(k = 0; k < strlen(curr_arg)-2; k++) {
                        errf[k] = curr_arg[k+2];
                    }
                    err_file = concat("", errf);
                } else if (curr_arg[0]=='&') {
                    //perform in background
                    wait_for_comp = 0;
                }
            }
            argv[arg_size-1] = NULL; //null terminated bruh

            int childPID = fork();
            if ( childPID == 0 ) {
                //child process
                if (input_file != "") {
                    FILE *fp_in = fopen(input_file, "a+");
                    dup2(fileno(fp_in), STDIN_FILENO);
                    fclose(fp_in);
                }
                else if (pipesFound > 0)
                {
                    dup2(pipeBuffer[val-1][0], STDIN_FILENO);
                    for (n = 0; n < pipesFound; n++)
                    {
                        close(pipeBuffer[n][0]);
                        close(pipeBuffer[n][1]);
                    }
                }
                if (err_file != "") {
                    FILE *fp_err = fopen(err_file, "a+");
                    dup2(fileno(fp_err), STDERR_FILENO);
                    fclose(fp_err);
                } else if (errisstdout == 1) {
                    dup2(fileno(stdout), fileno(stderr));
                }
                if (output_file != "") {
                    FILE *fp_out = fopen(output_file, "a+");
                    dup2(fileno(fp_out), STDOUT_FILENO);
                    fclose(fp_out);
                }
                int g = 0;
                for(g = 0; g < arg_size+1; g++){
                    printf("Value at argv[%d] is: %s", g, argv[g]);
                }
                execve( commandTable[val]->argVal, argv, environ );
                perror("execve");
                _exit(EXIT_FAILURE);
            }
        }
        else if ( val == 0 )
        {
            int arg_size = get_args_list_size(commandTable[val]);
            char *argv[arg_size+1];
            int i;
            argNode* list = commandTable[val];
            for (i = 0; i < arg_size; i++)
            {
                argv[i] = list->argVal;
                list = list->next;
            }
            argv[arg_size] = NULL;
            int childPID = fork();
            if ( childPID == 0 )
            {
                dup2(pipeBuffer[val][1], STDOUT_FILENO);
                for (n = 0; n < pipesFound; n++)
                {
                    close(pipeBuffer[n][0]);
                    close(pipeBuffer[n][1]);
                }
                int g = 0;
                for(g = 0; g < arg_size+1; g++){
                    printf("Value at argv[%d] is: %s", g, argv[g]);
                }
                execve( commandTable[val]->argVal, argv, environ );
                perror("execve");
                _exit(EXIT_FAILURE);
            }
        }
        else
        {
            int arg_size = get_args_list_size(commandTable[val]);
            char *argv[arg_size+1];
            int i;
            argNode* list = commandTable[val];
            for (i = 0; i < arg_size; i++)
            {
                argv[i] = list->argVal;
                list = list->next;
            }
            argv[arg_size] = NULL;
            int childPID = fork();
            if ( childPID == 0 )
            {
                dup2(pipeBuffer[val-1][0], STDIN_FILENO);
                dup2(pipeBuffer[val][1], STDOUT_FILENO);
                for (n = 0; n < pipesFound; n++)
                {
                    close(pipeBuffer[n][0]);
                    close(pipeBuffer[n][1]);
                }
                int g = 0;
                for(g = 0; g < arg_size+1; g++){
                    printf("Value at argv[%d] is: %s", g, argv[g]);
                }
                execve( commandTable[val]->argVal, argv, environ );
                perror("execve");
                _exit(EXIT_FAILURE);
            }
        }
    }
    for (n = 0; n < pipesFound; n++)
    {
        close(pipeBuffer[n][0]);
        close(pipeBuffer[n][1]);
    }
    if (wait_for_comp)
    {
        while ( wait() > 0 ) {};
    }


}


