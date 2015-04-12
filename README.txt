By Andrew Hall, and Andrew Bingler

Our code is in the shell.l,shell.y,main.c, and node.h files. 
Most of our functions are called in the commandBlock() function in the main.c file.
our lex 

working content:
created built in commands: cd,setenv,unsetenv,printenv,alias, and unalias
args list and command table
nested aliasing substitution
enviromental character substitution
piping and I/O redirection and file IO working (issues on mac?)
'&' backround implementation 
partial tilda expansion (works for cd command, ex: cd ~/xinu-dev)

basic error handling with line number printing
