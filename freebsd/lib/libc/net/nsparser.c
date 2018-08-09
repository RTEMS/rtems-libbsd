/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20170201

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0

#ifndef yyparse
#define yyparse    _nsyyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      _nsyylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    _nsyyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     _nsyychar
#endif /* yychar */

#ifndef yyval
#define yyval      _nsyyval
#endif /* yyval */

#ifndef yylval
#define yylval     _nsyylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    _nsyydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    _nsyynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  _nsyyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      _nsyylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      _nsyylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   _nsyydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    _nsyydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   _nsyysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   _nsyyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   _nsyygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    _nsyytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    _nsyycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     _nsyyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     _nsyyrule
#endif /* yyrule */
#define YYPREFIX "_nsyy"

#define YYPURE 0

#line 2 "nsparser.y"
/*	$NetBSD: nsparser.y,v 1.3 1999/01/25 00:16:18 lukem Exp $	*/

/*-
 * SPDX-License-Identifier: BSD-2-Clause-NetBSD
 *
 * Copyright (c) 1997, 1998, 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Luke Mewburn.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "namespace.h"
#define _NS_PRIVATE
#include <nsswitch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "un-namespace.h"

static	void	_nsaddsrctomap(const char *);

static	ns_dbt		curdbt;
static	ns_src		cursrc;
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 53 "nsparser.y"
typedef union {
	char *str;
	int   mapval;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 163 "nsparser.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define NL 257
#define SUCCESS 258
#define UNAVAIL 259
#define NOTFOUND 260
#define TRYAGAIN 261
#define RETURN 262
#define CONTINUE 263
#define ERRORTOKEN 264
#define STRING 265
#define YYERRCODE 256
typedef int YYINT;
static const YYINT _nsyylhs[] = {                        -1,
    0,    0,    3,    3,    4,    4,    4,    4,    5,    6,
    6,    7,    9,    7,    8,    8,   10,    1,    1,    1,
    1,    2,    2,
};
static const YYINT _nsyylen[] = {                         2,
    0,    1,    1,    2,    1,    3,    4,    2,    1,    1,
    2,    1,    0,    5,    1,    2,    3,    1,    1,    1,
    1,    1,    1,
};
static const YYINT _nsyydefred[] = {                      0,
    0,    5,    9,    0,    0,    3,    0,    8,    4,    0,
    6,    0,    0,   10,   13,    7,   11,    0,   18,   19,
   20,   21,    0,    0,   15,    0,   14,   16,   22,   23,
   17,
};
static const YYINT _nsyydgoto[] = {                       4,
   23,   31,    5,    6,    7,   13,   14,   24,   18,   25,
};
static const YYINT _nsyysindex[] = {                   -255,
 -249,    0,    0,    0, -255,    0,  -49,    0,    0, -254,
    0,  -73, -253,    0,    0,    0,    0, -244,    0,    0,
    0,    0,  -42,  -93,    0, -256,    0,    0,    0,    0,
    0,
};
static const YYINT _nsyyrindex[] = {                     20,
    0,    0,    0,    0,   21,    0,    0,    0,    0,    0,
    0, -252,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
static const YYINT _nsyygindex[] = {                      0,
    0,    0,    0,   17,    0,    0,   10,    0,    0,    1,
};
#define YYTABLESIZE 168
static const YYINT _nsyytable[] = {                      27,
    1,    2,   11,   16,   12,   29,   30,    8,   10,    3,
   12,   12,   12,   19,   20,   21,   22,   15,   26,    1,
    2,    9,   17,    0,   28,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   19,   20,   21,   22,
};
static const YYINT _nsyycheck[] = {                      93,
  256,  257,  257,  257,  257,  262,  263,  257,   58,  265,
  265,  265,  265,  258,  259,  260,  261,   91,   61,    0,
    0,    5,   13,   -1,   24,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  258,  259,  260,  261,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 265
#define YYUNDFTOKEN 278
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const _nsyyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,"'='",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NL","SUCCESS",
"UNAVAIL","NOTFOUND","TRYAGAIN","RETURN","CONTINUE","ERRORTOKEN","STRING",0,0,0,
0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const _nsyyrule[] = {
"$accept : File",
"File :",
"File : Lines",
"Lines : Entry",
"Lines : Lines Entry",
"Entry : NL",
"Entry : Database ':' NL",
"Entry : Database ':' Srclist NL",
"Entry : error NL",
"Database : STRING",
"Srclist : Item",
"Srclist : Srclist Item",
"Item : STRING",
"$$1 :",
"Item : STRING '[' $$1 Criteria ']'",
"Criteria : Criterion",
"Criteria : Criteria Criterion",
"Criterion : Status '=' Action",
"Status : SUCCESS",
"Status : UNAVAIL",
"Status : NOTFOUND",
"Status : TRYAGAIN",
"Action : RETURN",
"Action : CONTINUE",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 148 "nsparser.y"

static void
_nsaddsrctomap(const char *elem)
{
	int		i, lineno;
	extern int	_nsyylineno;
	extern char  	_nsyytext[];

	lineno = _nsyylineno - (_nsyytext[0] == '\n' ? 1 : 0);
	if (curdbt.srclistsize > 0) {
		if (((strcasecmp(elem, NSSRC_COMPAT) == 0) &&
		    (strcasecmp(curdbt.srclist[0].name, NSSRC_CACHE) != 0)) ||
		    (strcasecmp(curdbt.srclist[0].name, NSSRC_COMPAT) == 0)) {
			syslog(LOG_ERR,
	    "NSSWITCH(nsparser): %s line %d: 'compat' used with sources, other than 'cache'",
			    _PATH_NS_CONF, lineno);
			free((void*)elem);
			return;
		}
	}
	for (i = 0; i < curdbt.srclistsize; i++) {
		if (strcasecmp(curdbt.srclist[i].name, elem) == 0) {
			syslog(LOG_ERR,
		       "NSSWITCH(nsparser): %s line %d: duplicate source '%s'",
			    _PATH_NS_CONF, lineno, elem);
			free((void*)elem);
			return;
		}
	}
	cursrc.name = elem;
	_nsdbtaddsrc(&curdbt, &cursrc);
}
#line 395 "nsparser.c"

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yym = 0;
    yyn = 0;
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 6:
#line 81 "nsparser.y"
	{
			free((char*)curdbt.name);
		}
break;
case 7:
#line 85 "nsparser.y"
	{
			_nsdbtput(&curdbt);
		}
break;
case 8:
#line 89 "nsparser.y"
	{
			yyerrok;
		}
break;
case 9:
#line 96 "nsparser.y"
	{
			curdbt.name = yylval.str;
			curdbt.srclist = NULL;
			curdbt.srclistsize = 0;
		}
break;
case 12:
#line 110 "nsparser.y"
	{
			cursrc.flags = NS_TERMINATE;
			_nsaddsrctomap(yystack.l_mark[0].str);
		}
break;
case 13:
#line 114 "nsparser.y"
	{ cursrc.flags = NS_SUCCESS; }
break;
case 14:
#line 115 "nsparser.y"
	{
			_nsaddsrctomap(yystack.l_mark[-4].str);
		}
break;
case 17:
#line 127 "nsparser.y"
	{
			if (yystack.l_mark[0].mapval)	     /* if action == RETURN set RETURN bit */
				cursrc.flags |= yystack.l_mark[-2].mapval;  
			else	     /* else unset it */
				cursrc.flags &= ~yystack.l_mark[-2].mapval;
		}
break;
case 18:
#line 136 "nsparser.y"
	{ yyval.mapval = NS_SUCCESS; }
break;
case 19:
#line 137 "nsparser.y"
	{ yyval.mapval = NS_UNAVAIL; }
break;
case 20:
#line 138 "nsparser.y"
	{ yyval.mapval = NS_NOTFOUND; }
break;
case 21:
#line 139 "nsparser.y"
	{ yyval.mapval = NS_TRYAGAIN; }
break;
case 22:
#line 143 "nsparser.y"
	{ yyval.mapval = NS_ACTION_RETURN; }
break;
case 23:
#line 144 "nsparser.y"
	{ yyval.mapval = NS_ACTION_CONTINUE; }
break;
#line 670 "nsparser.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
