/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY__NSYY_NSYY_TAB_H_INCLUDED
# define YY__NSYY_NSYY_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int _nsyydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NL = 258,
     SUCCESS = 259,
     UNAVAIL = 260,
     NOTFOUND = 261,
     TRYAGAIN = 262,
     RETURN = 263,
     CONTINUE = 264,
     ERRORTOKEN = 265,
     STRING = 266
   };
#endif
/* Tokens.  */
#define NL 258
#define SUCCESS 259
#define UNAVAIL 260
#define NOTFOUND 261
#define TRYAGAIN 262
#define RETURN 263
#define CONTINUE 264
#define ERRORTOKEN 265
#define STRING 266



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 58 "freebsd/lib/libc/net/nsparser.y"

	char *str;
	int   mapval;


/* Line 2058 of yacc.c  */
#line 85 "_nsyy.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE _nsyylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int _nsyyparse (void *YYPARSE_PARAM);
#else
int _nsyyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int _nsyyparse (void);
#else
int _nsyyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY__NSYY_NSYY_TAB_H_INCLUDED  */
