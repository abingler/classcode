
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CD = 258,
     LS = 259,
     BYE = 260,
     NUMBER = 261,
     PRINT_ENV = 262,
     FRONTSLASH = 263,
     PERIOD = 264,
     LESSTHAN = 265,
     GREATERTHAN = 266,
     PIPE = 267,
     DOUBLEQUOTE = 268,
     BACKSLASH = 269,
     AMPERSAND = 270,
     SET_ENV = 271,
     UNSET_ENV = 272,
     WORD = 273
   };
#endif
/* Tokens.  */
#define CD 258
#define LS 259
#define BYE 260
#define NUMBER 261
#define PRINT_ENV 262
#define FRONTSLASH 263
#define PERIOD 264
#define LESSTHAN 265
#define GREATERTHAN 266
#define PIPE 267
#define DOUBLEQUOTE 268
#define BACKSLASH 269
#define AMPERSAND 270
#define SET_ENV 271
#define UNSET_ENV 272
#define WORD 273




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 23 "exampleshell.y"

        int number;
        char* string;



/* Line 1676 of yacc.c  */
#line 95 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


