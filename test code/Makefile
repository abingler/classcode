all: exampleshell

exampleshell: y.tab.o lex.yy.o
	cc lex.yy.o y.tab.o -o exampleshell

lex.yy.o: lex.yy.c
	cc -c lex.yy.c

y.tab.o: y.tab.c
	cc -c y.tab.c

lex.yy.c: exampleshell.l
	lex exampleshell.l

y.tab.c: exampleshell.y
	yacc -d exampleshell.y
