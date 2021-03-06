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


int main(){         
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
char* concatenate(char* dest, char* src);

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
        fprintf(stderr, "error on line %d, 'HOME' == NULL\n", yylineno); 
        }
    }
    else{
        //printf("args->argVal = %s \n",args->argVal);
        //printf("args->next->argVal = %s \n",args->next->argVal);
        location = args->next->argVal;          /*change dir*/
        //args = args->next;
        //location = args->argVal;
        if(location[0] == '~'){
            location++;//move past '~''
            location = concatenate(getenv("HOME"), location);//append location to home path
        }
    }
    validCheck = chdir(location); //attempt to change path and check if its valid
    if (validCheck != 0) fprintf(stderr, "error on line %d, %s does not exist here\n", yylineno, location);
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
    if (currentNode != NULL){/*if there is alredy a node in the list (not an empty list)*/
        while (currentNode->next != NULL && strcmp(currentNode->alias, alias) != 0){ /*while their are more nodes in the list and the alias of the currentNode node != to the new alias*/
            currentNode = currentNode->next; /*iterate through the list*/
        }
        if (strcmp(currentNode->alias, alias) == 0){ /*if the alias you tried to add alredy exists*/
            currentNode->val = val; /*update the existing alias */
            free(newNode);/*release the new node we dont need it*/
            printf("Alias has been updated \n");
            return;
        }
        currentNode->next = newNode; /*append new node to list*/
    }
    else{
        *head = newNode; /*place new node at the head of the empty list*/
    }    
}

void printAliasList(nodeT* head){ /*print alias list*/
    nodeT* currentNode = head; /*define the passed in head as the currentNode node*/
    while (currentNode != NULL){/*while there are nodes in the list*/
        printf("alias %s='%s'\n", currentNode->alias, currentNode->val); /*print info for currentNode node*/
        currentNode = currentNode->next;/*go to next node*/
    }
}
char* retrieveVal(nodeT* head, char* alias){/*search the list and return the value of a given alias */
    nodeT* currentNode = head;
    while (currentNode != NULL){ /*while not at the end of list*/
        if (strcmp(currentNode->alias, alias) == 0){/*if match found*/
            return currentNode->val; /*return the val*/
        }
        currentNode = currentNode->next;/*else keep looking*/
    }
    return NULL; /*no match found*/
}

char* aliasReplace(char* alias){
    char* val = retrieveVal(aliasHead, alias);/*look for alias and return matching value*/
    if (val != NULL) return val; /*if the alias exists return value*/
    return alias; /*else return the original input*/
}

int removeByAlias(nodeT** head, char * alias){ /*search for a node with a matching alias and remove it*/
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

void alias(argNode* args){
    argNode* currentNode = args->next;
    int n = 0;
    while (currentNode != NULL && n != 2){
        n++;
        currentNode = currentNode->next;
    }
    if (n == 2){
        char* argAlias = args->next->argVal;
        char* argValue = args->next->next->argVal;
        push(&aliasHead, argAlias, argValue);
    }
    else if (n == 0){
        printAliasList(aliasHead);
    }
    else{
        fprintf(stderr, "error on line %d, incorrect number of args for alias\n", yylineno);
    }
}
void unalias(argNode* args)
{
    if (args->next != NULL) {
        //printf("no next arg\n");
        removeByAlias(&aliasHead, args->next->argVal);}
    else fprintf(stderr, "error on line %d, too few args for unalias\n", yylineno);
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
            if(getenv(name)) unsetenv(name);
            else printf("Variable %s does not exist.\n", name);
}

argNode* stringTok(char* string, char* delimiter){
    char* token;
    char* tmp = strdup(string); //DO NOT MODIFY string. It's a POINTER
    token = strtok(tmp, delimiter); //Returns pointer to the next token


    argNode* point = malloc(sizeof(argNode));//Create new node
    point->next = NULL;


    if (token != NULL) point->argVal = token; //Until we reach the end of the tokens
    else point->argVal = tmp; //Nothing to tokenize
    argNode* save = point; //Save head of the Linked List


    token = strtok(NULL, delimiter);//Grab next token
    while (token != NULL){ //Fill out nodes with remaining tokens
          point->next = malloc(sizeof(argNode));
          point = point->next;
          point->argVal = token;
          point->next = NULL;  
          token = strtok(NULL, delimiter); 
    }
    return save; //Return the head
}


argNode* aliasArgReplace(argNode* args){
    int nestedAliasLoop = 0;
    int aliasLoop = 0; //guard against infinite expansion
    argNode* original = args; //first
    while(nestedAliasLoop<100){
        aliasLoop =0;
        while(args->argVal != aliasReplace(args->argVal) && aliasLoop < 100){ //where an alias exists
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
                args = stringTok(args->argVal, " \t"); //break it into tokens about the spaces
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
    else{
        fprintf(stderr, "on line %d: infinite alias error occured\n", yylineno);
        argNode* prev = NULL;//empty args list and free nodes
        while (args != NULL){
            prev = args;
            args = args->next;
            free(prev);
        }
        return NULL;
    }
}//



/*string handling functions*/

char* concatenate(char* dest, char* src){
    char* concat = malloc(strlen(dest)+strlen(src)+1);
    strcpy(concat, dest);
    strcat(concat, src); //Append src to dest
    return concat;
}


int characterCheck(char* string, char chara){  ///Check string for character chara
    int i;
    for (i = 0; i < strlen(string); i++){
        if (string[i] == chara) return 1;
    }
    return 0;
}

char *replace(char *str, char *orig, char * rep){ /*replace string with new substring*/
    static char buffer[4096];
    char *p;
    if(!(p = strstr(str, orig))) return str; /*is orig in str*/
    
    strncpy(buffer, str, p-str); /*copy char from str start to orig into buffer*/
    buffer[p-str] = '\0';
    
    sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
    
    return buffer;
}

char *insertEnv(char* input){ /*function extrats env variable*/
    char * envvar = input;
    int i;
    int validFlag = 0;
    int start;
    int end;
    for (i = 0; i < strlen(envvar); i++){ /*iterate through input*/
        if(envvar[i] == '$') start = i;
        if(envvar[i] == '{' && i == start+1) validFlag = 1;
        if(envvar[i] == '}' && validFlag){
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

void replaceEscape(char* str){
    char* pRead = str;
    char* pWrite = str;
    while (*pRead) {
        *pWrite = *pRead++;
        pWrite += (*pWrite != '\\' || *(pWrite + 1) == '\\');
    }
    *pWrite = '\0';
}

int hasWhitespace(char* string){
    int i;
    for (i = 0; i < strlen(string); i++){
        if (string[i] == '\t' || string[i] == ' ') return 1;
    }
    return 0;
}

int whitespaceOnly(char* string){
    int i;
    for (i = 0; i < strlen(string); i++){
        if (string[i] != '\t' && string[i] != ' ') return 0;
    }
    return 1;
}




void commandBlock(argNode* args){
    argNode* tempNode = args;
    argNode* currentNode = args;
    while(args->next!=NULL){
    args = args->next;
    args = aliasArgReplace(args);
    }
    args = tempNode; //Why don't we reuse this
    args = aliasArgReplace(args);

    

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
    int pipeBuffer[50][2];
    int n;


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


    for (val = 0; val < pipesFound + 1; val++){ //For each group of commands

        argNode* list = commandTable[val];

        if ( !characterCheck(commandTable[val]->argVal, '/') ){ //If we do not have the '/' character ?No path to bin?
            char* path = getenv("PATH"); //Grab that path
            char* callCmd;
            int check = 0;

            argNode* tokedPath = stringTok(path, ":"); //Tokenize Paths

            while (tokedPath != NULL && check == 0){ //
                char* temp = concatenate(tokedPath->argVal, "/"); //Append / 
                callCmd = concatenate(temp, commandTable[val]->argVal); //Append command (We want /bin/ls for example)

                 if(access(callCmd, F_OK ) != -1 ){ //Can we access this area (Will not always work)
                    check = 1;
                    commandTable[val]->argVal = callCmd;
                    //printf("Call command is %s\n", callCmd);
                }
                else free(callCmd);


                //commandTable[val]->argVal = callCmd;
                tokedPath = tokedPath->next;

                free(temp); //Overflow problems
            }
            if (check == 0){ //No access
                fprintf(stderr, "error on line %d, no command %s\n", yylineno, commandTable[val]->argVal);
                return;
            }
        }
        else{ //If we already have our path setup
            if( access( commandTable[val]->argVal, F_OK|X_OK ) != 0 ){ //No access
                fprintf(stderr, "error on line %d, no command %s\n", yylineno, commandTable[val]->argVal);
                return;
            }
        }
    }


    //FPIPING!
    if (pipesFound > 50) pipesFound = 50; //Max of 50 pipes
    for (n = 0; n < pipesFound; n++){
        pipe(pipeBuffer[n]);//Data written to the write end of the pipe is buffered by the kernel until it is read from the read end of the pipe
    }


    int ampersand = 1;
    for (val = 0; val < pipesFound + 1; val++) //For every val
    {

       /* if(pipesFound == 0){

        } */

        if(val == pipesFound ) {//If we have zero or max pipes
            char* inputRed = ""; //These are used to determine if we have a new file //char* inputRed;
            char* outputRed = "";
            char* errorRed = "";
            argNode * current = commandTable[val]; //
            int count = 0;
            while (current != NULL){
                if (strcmp(current->argVal, "|") != 0 && strcmp(current->argVal, "<") != 0 && strcmp(current->argVal, ">>") != 0 &&
                    strcmp(current->argVal, ">") != 0 && (current->argVal[0]!='2' && current->argVal[1]!='>') &&
                    strcmp(current->argVal, "&") != 0){
                        count++;
                        current = current->next; 
                }
                else break;
            }
            int numArgs = count+1; //How many args do we have. name change?

            int haveError = 0;
            char* thisArg;
            int i = 0;

            char *argv[ numArgs +1 ];
            argNode* list = commandTable[val]; //Grab current head of command

            while(list != NULL){ //Follow each value in the list
                thisArg = list->argVal;

                if (i<numArgs-1){ //Grab arguments first
                argv[i] = thisArg;
                } 
                list = list->next;
                i++;
                if (strcmp(thisArg, "<") == 0) { //INPUT REDIRECTION HERE
                    if(list == NULL){ //No file given
                        fprintf(stderr, "error on line %d, need input file\n", yylineno );
                        return;
                    }
                    printf("Input file is %s\n", list->argVal); //Next after < should be a command
                    inputRed = list->argVal; //Grab input
                    list = list->next; //Update to next list
                    i++;
                } 
                else if (strcmp(thisArg, ">") == 0 || 
                    strcmp(thisArg, ">>") == 0){ //OUTPUT REDIRECTION HERE
                    if (list == NULL){
                        fprintf(stderr, "error on line %d, need output file>\n", yylineno );
                        return;
                    }
                    outputRed = list->argVal; //Grab output
                    list = list->next;
                    i++;
                } 
                else if (strcmp(thisArg, "2>$1") == 0) { //ERROR REDIRECTION HERE
                    haveError = 1;
                } 
                else if (thisArg[0]=='2' && thisArg[1]=='>') { //ERROR TO FILE HERE
                    int k = 0;
                    char errf[strlen(thisArg) - 2];
                    for(k = 0; k < strlen(thisArg)-2; k++){
                        errf[k] = thisArg[k+2];
                    }
                    errorRed = concatenate("", errf); //con-cat-e-nate errf to ""
                } 
                else if (thisArg[0]=='&') { //& WAIT IN BACKGROUND HERE
                    ampersand = 0;
                }
            }
            argv[numArgs-1] = NULL; //Add NULL to end of argv or IT DOES NOT WORK

            int child = fork(); //FORK THIS DAMN CHILD
            if(child == 0 ){
                if (haveError == 1) { //Simple error to ourput
                    dup2(fileno(stdout), fileno(stderr)); //Redirect from one std to another
                }
                else if(errorRed != ""){ //Error handling
                    FILE *errorFile = fopen(errorRed, "a+");
                    dup2(fileno(errorFile), STDERR_FILENO); //Redirect from one file to std
                    fclose(errorFile);
                } 
                if(outputRed != ""){
                    FILE *outputFile = fopen(outputRed, "a+");
                    dup2(fileno(outputFile), STDOUT_FILENO);
                    fclose(outputFile);
                }
                if(inputRed != ""){
                    //printf("You have changed file input\n");
                    FILE *inputFile = fopen(inputRed, "a+");//Change input
                    dup2(fileno(inputFile), STDIN_FILENO); 

                    fclose(inputFile);
                }
                else if (pipesFound > 0){ //If there is no change to input, then route STDIN to piping
                    dup2(pipeBuffer[val-1][0], STDIN_FILENO); 
                    for (n = 0; n < pipesFound; n++){ //Apparantly we need to close all the pipes or it just won't output
                        close(pipeBuffer[n][0]); 
                        close(pipeBuffer[n][1]);
                    } 
                } 
                int g = 0;
                //for(g = 0; g < numArgs+1; g++){
                //    printf("Value at argv[%d] is: %s", g, argv[g]);
                //}
                execve(commandTable[val]->argVal, argv, environ );
                _exit(EXIT_FAILURE); //Terminate calling process
            }
        }










        else if(val == 0){ //Very first value with piping
            argNode * current = commandTable[val];
            int count = 0;
            while (current != NULL){
                if (strcmp(current->argVal, ">") != 0 && strcmp(current->argVal, ">>") != 0 && strcmp(current->argVal, "<") != 0 &&
                    strcmp(current->argVal, "|") != 0 && (current->argVal[0]!='2' && current->argVal[1]!='>') &&
                    strcmp(current->argVal, "&") != 0){
                        count++;
                        current = current->next; 
                }
                else break;
            }
            int numArgs = count; //Number of args we have in the current commandTable list

            //DON'T NEED TO WORRY ABOUT FILE I/O HERE

            char *argv[numArgs+1];
            int i;
            argNode* list = commandTable[val];
            for (i = 0; i < numArgs; i++){ //Fill up argv
                argv[i] = list->argVal;
                list = list->next;
            }
            argv[numArgs] = NULL;


            int child = fork(); //Sub programs
            if ( child == 0 )
            {
                dup2(pipeBuffer[val][1], STDOUT_FILENO); //Route output through pipes
                for (n = 0; n < pipesFound; n++)
                {
                    close(pipeBuffer[n][0]);
                    close(pipeBuffer[n][1]);
                }
                /*int g = 0;
                for(g = 0; g < numArgs+1; g++){
                    printf("Value at argv[%d] is: %s", g, argv[g]);
                } */
                execve(commandTable[val]->argVal, argv, environ);
                _exit(EXIT_FAILURE); //Terminate calling process
            }
        }










        //else if( val != 0 && val < pipesFound){
        else{
            argNode * current = commandTable[val];
            int count = 0;
            while (current != NULL){
                if (strcmp(current->argVal, ">") != 0 && strcmp(current->argVal, ">>") != 0 && strcmp(current->argVal, "<") != 0 &&
                    strcmp(current->argVal, "|") != 0 && (current->argVal[0]!='2' && current->argVal[1]!='>') &&
                    strcmp(current->argVal, "&") != 0){
                        count++;
                        current = current->next; 
                }
                else break;
            }
            int numArgs = count;


            char *argv[numArgs+1];
            int i;
            argNode* list = commandTable[val];
            for (i = 0; i < numArgs; i++) //Fill out argv
            {
                argv[i] = list->argVal;
                list = list->next;
            }

            argv[numArgs] = NULL;
            int child = fork();
            if ( child == 0 )
            {
                dup2(pipeBuffer[val-1][0], STDIN_FILENO); //Pipe input from previous commands
                dup2(pipeBuffer[val][1], STDOUT_FILENO); //Pipe output from current commands
                for (n = 0; n < pipesFound; n++)
                {
                    close(pipeBuffer[n][0]);
                    close(pipeBuffer[n][1]);
                }
                /* int g = 0;
                for(g = 0; g < numArgs+1; g++){
                    printf("Value at argv[%d] is: %s", g, argv[g]);
                } */
                execve( commandTable[val]->argVal, argv, environ );
                _exit(EXIT_FAILURE); //Terminate calling process
            }
        }
    }
    for (n = 0; n < pipesFound; n++)
    {
        close(pipeBuffer[n][0]);
        close(pipeBuffer[n][1]);
    } 
    if (ampersand)
    {
        while ( wait() > 0 ) {};
    }


}


