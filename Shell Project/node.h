#ifndef NODE_H
#define NODE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
typedef struct argsNode {
    char* argVal;
    struct argsNode * next;
} argNode;
#endif