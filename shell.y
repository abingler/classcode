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



%}


%token BYE NEWLINE

%union
{
        int number;
        char* string;
        void* linkedlist;
        argNode* argN;
}



%token <string> WORD
%token <argN> ARGS
%type <argN> argList
//%type <linkedlist> cmd
//%type <string> arg
%%

commands:
        | commands command  {printf("%s> ",getenv("PWD"));}
        | commands argList NEWLINE{commandBlock($2);printf("%s> ",getenv("PWD"));}

command:/*commands moved to commandBlock in main.c*/
         NEWLINE /* ignore new line*/
        ;

argList:
    WORD argList { $$ = malloc(sizeof(argNode));
                    $$->next = $2;
                    $$->argVal = $1;}
    |
    ARGS argList {  $$ = $1;
                     argNode* current = $1;
                     while (current->next != NULL) current = current->next;
                     current->next = $2;
                     //printf("%s\n",$1 );
                 }
    |
    ARGS          { $$ = $1; 
                    //printf("%s\n",$1 );
                    }

    |
    WORD          { $$ = malloc(sizeof(argNode));
                    $$->next = NULL;
                    $$->argVal = $1;
                    //printf("%s\n",$1 ); 
                }
    ;
%%


/*bye:
    BYE
        {
            printf("Exiting the shell now...\n");
            exit(0);
        };
*/

//cd: 
//  CD  
//      {
//          chdir(getenv("HOME"));  /*move to home*/
//          setenv("PWD", getenv("HOME"), 1);  /*update PWD with home*/
//      }
//  | CD WORD
//      {
//          $2 = insertEnv($2); /*extract env from word*/
//          chdir($2);          /*change dir*/
//          char pwd[4096];
//          getcwd(pwd, sizeof(pwd)); /*copy absolute pathname to pwd[]*/
//          setenv("PWD", pwd, 1); /*update PWD*/
//      };
/*      
setEnv:
    SETENV WORD WORD
        {
            char* envName = insertEnv($<string>2);/*extract word1
            char* envVal = insertEnv($<string>3);/*extract word2
            int result = setenv(envName, envVal, 1);
            if(result == -1)
                printf("Failed to set variable %s to %s.\n", envName, envVal);
        };
printEnv:
    PRINTENV
        {       
            extern char **environ;  
            int i=0;
            while(environ[i])
                printf("%s\n", environ[i++]);
            char* path = getenv("PATH");
            printf("%s> ",path);
        }

unsetEnv:
    UNSETENV WORD 
        {
            char* name = $<string>2;
            if(getenv(name))
                unsetenv(name);\
            else
                printf("Variable %s does not exist.\n", name);
        }
*/
/*ls:
    LS
        {
            ls();
        }

alias:
    ALIAS
        {
            printAliasList(aliasHead);/*prints list
        }
    | ALIAS WORD WORD
        { 
            push(&aliasHead, $2, $3); /*add new Alias

        }
            
unalias:
    UNALIAS WORD
        {
            removeByAlias(&aliasHead, $2);
        }
*/

