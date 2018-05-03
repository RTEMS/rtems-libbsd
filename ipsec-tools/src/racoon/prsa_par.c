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
#define yyparse    racoonprsaparse
#endif /* yyparse */

#ifndef yylex
#define yylex      racoonprsalex
#endif /* yylex */

#ifndef yyerror
#define yyerror    racoonprsaerror
#endif /* yyerror */

#ifndef yychar
#define yychar     racoonprsachar
#endif /* yychar */

#ifndef yyval
#define yyval      racoonprsaval
#endif /* yyval */

#ifndef yylval
#define yylval     racoonprsalval
#endif /* yylval */

#ifndef yydebug
#define yydebug    racoonprsadebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    racoonprsanerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  racoonprsaerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      racoonprsalhs
#endif /* yylhs */

#ifndef yylen
#define yylen      racoonprsalen
#endif /* yylen */

#ifndef yydefred
#define yydefred   racoonprsadefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    racoonprsadgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   racoonprsasindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   racoonprsarindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   racoonprsagindex
#endif /* yygindex */

#ifndef yytable
#define yytable    racoonprsatable
#endif /* yytable */

#ifndef yycheck
#define yycheck    racoonprsacheck
#endif /* yycheck */

#ifndef yyname
#define yyname     racoonprsaname
#endif /* yyname */

#ifndef yyrule
#define yyrule     racoonprsarule
#endif /* yyrule */
#define YYPREFIX "racoonprsa"

#define YYPURE 0

#line 6 "../../ipsec-tools/src/racoon/prsa_par.y"
/*
 * Copyright (C) 2004 SuSE Linux AG, Nuernberg, Germany.
 * Contributed by: Michal Ludvig <mludvig@suse.cz>, SUSE Labs
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* This file contains a parser for FreeS/WAN-style ipsec.secrets RSA keys. */

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <unistd.h>

#include <openssl/bn.h>
#include <openssl/rsa.h>

#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "oakley.h"
#include "isakmp_var.h"
#include "handler.h"
#include "crypto_openssl.h"
#include "sockmisc.h"
#include "rsalist.h"
#ifdef __rtems__
#define prsaparse yyparse
#define prsaerror yyerror
#define prsain racoonprsain
#define prsawrap racoonprsawrap
#endif /* __rtems__ */

extern void prsaerror(const char *str, ...);
extern int prsawrap (void);
extern int prsalex (void);

extern char *prsatext;
extern int prsa_cur_lineno;
extern char *prsa_cur_fname;
extern FILE *prsain;

int prsa_cur_lineno = 0;
char *prsa_cur_fname = NULL;
struct genlist *prsa_cur_list = NULL;
enum rsa_key_type prsa_cur_type = RSA_TYPE_ANY;

static RSA *rsa_cur;

void
prsaerror(const char *s, ...)
{
	char fmt[512];

	va_list ap;
#ifdef HAVE_STDARG_H
	va_start(ap, s);
#else
	va_start(ap);
#endif
	snprintf(fmt, sizeof(fmt), "%s:%d: %s",
		prsa_cur_fname, prsa_cur_lineno, s);
	plogv(LLV_ERROR, LOCATION, NULL, fmt, ap);
	va_end(ap);
}

void
prsawarning(const char *s, ...)
{
	char fmt[512];

	va_list ap;
#ifdef HAVE_STDARG_H
	va_start(ap, s);
#else
	va_start(ap);
#endif
	snprintf(fmt, sizeof(fmt), "%s:%d: %s",
		prsa_cur_fname, prsa_cur_lineno, s);
	plogv(LLV_WARNING, LOCATION, NULL, fmt, ap);
	va_end(ap);
}

int
prsawrap()
{
	return 1;
} 
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 136 "../../ipsec-tools/src/racoon/prsa_par.y"
typedef union {
	BIGNUM *bn;
	RSA *rsa;
	char *chr;
	long num;
	struct netaddr *naddr;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 246 "racoonprsa.tab.c"

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

#define COLON 257
#define HEX 258
#define OBRACE 259
#define EBRACE 260
#define TAG_RSA 261
#define TAG_PUB 262
#define TAG_PSK 263
#define MODULUS 264
#define PUBLIC_EXPONENT 265
#define PRIVATE_EXPONENT 266
#define PRIME1 267
#define PRIME2 268
#define EXPONENT1 269
#define EXPONENT2 270
#define COEFFICIENT 271
#define ADDR4 272
#define ADDR6 273
#define ADDRANY 274
#define SLASH 275
#define NUMBER 276
#define BASE64 277
#define YYERRCODE 256
typedef int YYINT;
static const YYINT racoonprsalhs[] = {                   -1,
    0,    0,    6,    6,    6,    1,    1,    1,    5,    5,
    5,    3,    4,    2,    2,    7,    7,    8,    8,    8,
    8,    8,    8,    8,    8,
};
static const YYINT racoonprsalen[] = {                    2,
    2,    1,    4,    3,    2,    4,    2,    2,    1,    1,
    1,    2,    2,    0,    2,    2,    1,    3,    3,    3,
    3,    3,    3,    3,    3,
};
static const YYINT racoonprsadefred[] = {                 0,
    0,    0,    0,   11,    0,    9,   10,    0,    2,    0,
    0,    5,    0,   12,   13,    1,    0,    0,    0,    8,
    7,   15,    4,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   17,    3,    0,    0,    0,    0,    0,
    0,    0,    0,    6,   16,   18,   19,   20,   21,   22,
   23,   24,   25,
};
static const YYINT racoonprsadgoto[] = {                  5,
   12,   14,    6,    7,    8,    9,   33,   34,
};
static const YYINT racoonprsasindex[] = {              -256,
 -255, -272, -272,    0, -256,    0,    0, -252,    0, -248,
 -258,    0, -266,    0,    0,    0, -255, -253, -226,    0,
    0,    0,    0, -255, -245, -244, -243, -242, -230, -229,
 -228, -211, -234,    0,    0, -210, -209, -208, -207, -206,
 -205, -204, -203,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
static const YYINT racoonprsarindex[] = {                 0,
    0, -249, -249,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
static const YYINT racoonprsagindex[] = {                 0,
  -15,   44,    0,    0,   48,   52,    0,   25,
};
#define YYTABLESIZE 58
static const YYINT racoonprsatable[] = {                 20,
    1,   23,   13,   24,   17,   10,   11,   14,   35,   22,
   19,   36,   37,   38,   39,    2,    3,    4,   21,    2,
    3,    4,   14,   14,   14,   44,   40,   41,   42,   25,
   26,   27,   28,   29,   30,   31,   32,   25,   26,   27,
   28,   29,   30,   31,   32,   43,   15,   46,   47,   48,
   49,   50,   51,   52,   53,   18,   16,   45,
};
static const YYINT racoonprsacheck[] = {                258,
  257,   17,  275,  257,  257,  261,  262,  257,   24,  276,
  259,  257,  257,  257,  257,  272,  273,  274,  277,  272,
  273,  274,  272,  273,  274,  260,  257,  257,  257,  264,
  265,  266,  267,  268,  269,  270,  271,  264,  265,  266,
  267,  268,  269,  270,  271,  257,    3,  258,  258,  258,
  258,  258,  258,  258,  258,    8,    5,   33,
};
#define YYFINAL 5
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 277
#define YYUNDFTOKEN 288
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const racoonprsaname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"COLON","HEX","OBRACE","EBRACE",
"TAG_RSA","TAG_PUB","TAG_PSK","MODULUS","PUBLIC_EXPONENT","PRIVATE_EXPONENT",
"PRIME1","PRIME2","EXPONENT1","EXPONENT2","COEFFICIENT","ADDR4","ADDR6",
"ADDRANY","SLASH","NUMBER","BASE64",0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const racoonprsarule[] = {
"$accept : statements",
"statements : statements statement",
"statements : statement",
"statement : addr addr COLON rsa_statement",
"statement : addr COLON rsa_statement",
"statement : COLON rsa_statement",
"rsa_statement : TAG_RSA OBRACE params EBRACE",
"rsa_statement : TAG_PUB BASE64",
"rsa_statement : TAG_PUB HEX",
"addr : addr4",
"addr : addr6",
"addr : ADDRANY",
"addr4 : ADDR4 prefix",
"addr6 : ADDR6 prefix",
"prefix :",
"prefix : SLASH NUMBER",
"params : params param",
"params : param",
"param : MODULUS COLON HEX",
"param : PUBLIC_EXPONENT COLON HEX",
"param : PRIVATE_EXPONENT COLON HEX",
"param : PRIME1 COLON HEX",
"param : PRIME2 COLON HEX",
"param : EXPONENT1 COLON HEX",
"param : EXPONENT2 COLON HEX",
"param : COEFFICIENT COLON HEX",

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
#line 327 "../../ipsec-tools/src/racoon/prsa_par.y"

int prsaparse(void);

int
prsa_parse_file(struct genlist *list, char *fname, enum rsa_key_type type)
{
	FILE *fp = NULL;
	int ret;
	
	if (!fname)
		return -1;
	if (type == RSA_TYPE_PRIVATE) {
		struct stat st;
		if (stat(fname, &st) < 0)
			return -1;
		if (st.st_mode & (S_IRWXG | S_IRWXO)) {
			plog(LLV_ERROR, LOCATION, NULL,
				"Too slack permissions on private key '%s'\n", 
				fname);
			plog(LLV_ERROR, LOCATION, NULL,
				"Should be at most 0600, now is 0%o\n",
				st.st_mode & 0777);
			return -1;
		}
	}
	fp = fopen(fname, "r");
	if (!fp)
		return -1;
	prsain = fp;
	prsa_cur_lineno = 1;
	prsa_cur_fname = fname;
	prsa_cur_list = list;
	prsa_cur_type = type;
	rsa_cur = RSA_new();
	ret = prsaparse();
	if (rsa_cur) {
		RSA_free(rsa_cur);
		rsa_cur = NULL;
	}
	fclose (fp);
	prsain = NULL;
	return ret;
}
#line 488 "racoonprsa.tab.c"

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
case 3:
#line 167 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		rsa_key_insert(prsa_cur_list, yystack.l_mark[-3].naddr, yystack.l_mark[-2].naddr, yystack.l_mark[0].rsa);
	}
break;
case 4:
#line 171 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		rsa_key_insert(prsa_cur_list, NULL, yystack.l_mark[-2].naddr, yystack.l_mark[0].rsa);
	}
break;
case 5:
#line 175 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		rsa_key_insert(prsa_cur_list, NULL, NULL, yystack.l_mark[0].rsa);
	}
break;
case 6:
#line 182 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		if (prsa_cur_type == RSA_TYPE_PUBLIC) {
			prsawarning("Using private key for public key purpose.\n");
			if (!rsa_cur->n || !rsa_cur->e) {
				prsaerror("Incomplete key. Mandatory parameters are missing!\n");
				YYABORT;
			}
		}
		else {
			if (!rsa_cur->n || !rsa_cur->e || !rsa_cur->d) {
				prsaerror("Incomplete key. Mandatory parameters are missing!\n");
				YYABORT;
			}
			if (!rsa_cur->p || !rsa_cur->q || !rsa_cur->dmp1
			    || !rsa_cur->dmq1 || !rsa_cur->iqmp) {
				if (rsa_cur->p) BN_clear_free(rsa_cur->p);
				if (rsa_cur->q) BN_clear_free(rsa_cur->q);
				if (rsa_cur->dmp1) BN_clear_free(rsa_cur->dmp1);
				if (rsa_cur->dmq1) BN_clear_free(rsa_cur->dmq1);
				if (rsa_cur->iqmp) BN_clear_free(rsa_cur->iqmp);

				rsa_cur->p = NULL;
				rsa_cur->q = NULL;
				rsa_cur->dmp1 = NULL;
				rsa_cur->dmq1 = NULL;
				rsa_cur->iqmp = NULL;
			}
		}
		yyval.rsa = rsa_cur;
		rsa_cur = RSA_new();
	}
break;
case 7:
#line 214 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		if (prsa_cur_type == RSA_TYPE_PRIVATE) {
			prsaerror("Public key in private-key file!\n");
			YYABORT;
		}
		yyval.rsa = base64_pubkey2rsa(yystack.l_mark[0].chr);
		free(yystack.l_mark[0].chr);
	}
break;
case 8:
#line 223 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		if (prsa_cur_type == RSA_TYPE_PRIVATE) {
			prsaerror("Public key in private-key file!\n");
			YYABORT;
		}
		yyval.rsa = bignum_pubkey2rsa(yystack.l_mark[0].bn);
	}
break;
case 11:
#line 236 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		yyval.naddr = NULL;
	}
break;
case 12:
#line 243 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		int err;
		struct sockaddr_in *sap;
		struct addrinfo hints, *res;
		
		if (yystack.l_mark[0].num == -1) yystack.l_mark[0].num = 32;
		if (yystack.l_mark[0].num < 0 || yystack.l_mark[0].num > 32) {
			prsaerror ("Invalid IPv4 prefix\n");
			YYABORT;
		}
		yyval.naddr = calloc (sizeof(struct netaddr), 1);
		yyval.naddr->prefix = yystack.l_mark[0].num;
		sap = (struct sockaddr_in *)(&yyval.naddr->sa);
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_flags = AI_NUMERICHOST;
		err = getaddrinfo(yystack.l_mark[-1].chr, NULL, &hints, &res);
		if (err < 0) {
			prsaerror("getaddrinfo(%s): %s\n", yystack.l_mark[-1].chr, gai_strerror(err));
			YYABORT;
		}
		memcpy(sap, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);
		free(yystack.l_mark[-1].chr);
	}
break;
case 13:
#line 272 "../../ipsec-tools/src/racoon/prsa_par.y"
	{
		int err;
		struct sockaddr_in6 *sap;
		struct addrinfo hints, *res;
		
		if (yystack.l_mark[0].num == -1) yystack.l_mark[0].num = 128;
		if (yystack.l_mark[0].num < 0 || yystack.l_mark[0].num > 128) {
			prsaerror ("Invalid IPv6 prefix\n");
			YYABORT;
		}
		yyval.naddr = calloc (sizeof(struct netaddr), 1);
		yyval.naddr->prefix = yystack.l_mark[0].num;
		sap = (struct sockaddr_in6 *)(&yyval.naddr->sa);
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET6;
		hints.ai_flags = AI_NUMERICHOST;
		err = getaddrinfo(yystack.l_mark[-1].chr, NULL, &hints, &res);
		if (err < 0) {
			prsaerror("getaddrinfo(%s): %s\n", yystack.l_mark[-1].chr, gai_strerror(err));
			YYABORT;
		}
		memcpy(sap, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);
		free(yystack.l_mark[-1].chr);
	}
break;
case 14:
#line 300 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ yyval.num = -1; }
break;
case 15:
#line 301 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 18:
#line 310 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->n) rsa_cur->n = yystack.l_mark[0].bn; else { prsaerror ("Modulus already defined\n"); YYABORT; } }
break;
case 19:
#line 312 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->e) rsa_cur->e = yystack.l_mark[0].bn; else { prsaerror ("PublicExponent already defined\n"); YYABORT; } }
break;
case 20:
#line 314 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->d) rsa_cur->d = yystack.l_mark[0].bn; else { prsaerror ("PrivateExponent already defined\n"); YYABORT; } }
break;
case 21:
#line 316 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->p) rsa_cur->p = yystack.l_mark[0].bn; else { prsaerror ("Prime1 already defined\n"); YYABORT; } }
break;
case 22:
#line 318 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->q) rsa_cur->q = yystack.l_mark[0].bn; else { prsaerror ("Prime2 already defined\n"); YYABORT; } }
break;
case 23:
#line 320 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->dmp1) rsa_cur->dmp1 = yystack.l_mark[0].bn; else { prsaerror ("Exponent1 already defined\n"); YYABORT; } }
break;
case 24:
#line 322 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->dmq1) rsa_cur->dmq1 = yystack.l_mark[0].bn; else { prsaerror ("Exponent2 already defined\n"); YYABORT; } }
break;
case 25:
#line 324 "../../ipsec-tools/src/racoon/prsa_par.y"
	{ if (!rsa_cur->iqmp) rsa_cur->iqmp = yystack.l_mark[0].bn; else { prsaerror ("Coefficient already defined\n"); YYABORT; } }
break;
#line 862 "racoonprsa.tab.c"
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
