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




commandBlock(arg_node* args)
{

    /*Variable decs */
    int outRedirects = 0;
    char* outputRed = "";
    char* inputRed;

    arg_node* tempNode = args;
    arg_node* currentNode = args;
    while(args->next!=NULL){
    args = args->next;
    args = aliasArgReplace(args);
    }
    args = tempNode;
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
                case 1:
                    ls(); //Pass in argument if being written to file
                    return;
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

    arg_node* tempNode1 = args; 

    while(tempNode1 != NULL){ //Check for >
        if(tempNode1->next->arg_val == ">"){ //If node after current has >
            if(tempNode1->next->next == NULL){
                fprintf(stderr, "error at line %d: no output file specified after >\n", yylineno );
                return;
            }
            outputRed = tempNode1->next->next->arg_val;
        }
    }

    if(outputRed != ""){
        FILE *outputfile = fopen(outputRed, "a+");
        dup2(fileno(outputfile), STDOUT_FILENO);
        fclose(outputfile);
    }

    ls();    
    
}



