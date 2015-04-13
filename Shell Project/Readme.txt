By Andrew Hall, and Andrew Bingler

Our code is in the shell.l, shell.y, main.c, and node.h files. 
Most of our functions are in the main.c file.
All lex parsing is in shell.l
All yacc parsing is in shell.y
substitution is handled in lex file
makefile is included.

HOW TO RUN:
To compile, run "make clean; make"
Then ./shell to run the shell
Please note: If you are reading an input file to the shell to run commands, add the command bye and either a ";" or newline to the end of the file, or else it may simply run in an infinite loop

Not working:
file name completion
Wildcard Matching
Some of ~ expansion

Working:
created built in commands: cd, setenv, unsetenv, printenv, alias, and unalias
args list and command table
nested aliasing substitution
enviromental character substitution
piping and I/O redirection and file IO working (issues on mac?)
'&' backround implementation 
partial ~ expansion (works for cd command, ex: cd ~/xinu-dev)
basic error handling with line number printing