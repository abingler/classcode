all: shell

shell: y.tab.o lex.yy.o main.c
	cc lex.yy.o y.tab.o -o shell

lex.yy.o: lex.yy.c
	cc -c lex.yy.c

y.tab.o: y.tab.c
	cc -c y.tab.c

lex.yy.c: shell.l
	lex shell.l

y.tab.c: shell.y
	yacc -d shell.y

main.o: main.c
	gcc main.c

clean:
	rm *.o
	rm lex.yy.c
	rm y.tab.c
	rm y.tab.h 
	rm shell