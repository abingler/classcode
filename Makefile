all: shell

shell: y.tab.o lex.yy.o main.o
	cc lex.yy.o y.tab.o -o shell

main.o: main.c lex.yy.c y.tab.c
	gcc -c main.c

lex.yy.o: lex.yy.c
	cc -c lex.yy.c

y.tab.o: y.tab.c
	cc -c y.tab.c

lex.yy.c: shell.l 
	lex shell.l

y.tab.c: shell.y 
	yacc -d shell.y

clean:
	rm *.o
	rm lex.yy.c
	rm y.tab.c
	rm y.tab.h 