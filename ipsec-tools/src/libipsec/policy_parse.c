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
#define yyparse    __libipsecparse
#endif /* yyparse */

#ifndef yylex
#define yylex      __libipseclex
#endif /* yylex */

#ifndef yyerror
#define yyerror    __libipsecerror
#endif /* yyerror */

#ifndef yychar
#define yychar     __libipsecchar
#endif /* yychar */

#ifndef yyval
#define yyval      __libipsecval
#endif /* yyval */

#ifndef yylval
#define yylval     __libipseclval
#endif /* yylval */

#ifndef yydebug
#define yydebug    __libipsecdebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    __libipsecnerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  __libipsecerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      __libipseclhs
#endif /* yylhs */

#ifndef yylen
#define yylen      __libipseclen
#endif /* yylen */

#ifndef yydefred
#define yydefred   __libipsecdefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    __libipsecdgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   __libipsecsindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   __libipsecrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   __libipsecgindex
#endif /* yygindex */

#ifndef yytable
#define yytable    __libipsectable
#endif /* yytable */

#ifndef yycheck
#define yycheck    __libipseccheck
#endif /* yycheck */

#ifndef yyname
#define yyname     __libipsecname
#endif /* yyname */

#ifndef yyrule
#define yyrule     __libipsecrule
#endif /* yyrule */
#define YYPREFIX "__libipsec"

#define YYPURE 0

#line 66 "../../ipsec-tools/src/libipsec/policy_parse.y"
#ifdef __rtems__
#include <machine/rtems-bsd-user-space.h>
#endif /* __rtems__ */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include PATH_IPSEC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include <errno.h>

#include "config.h"

#include "ipsec_strerror.h"
#include "libpfkey.h"

#ifndef INT32_MAX
#define INT32_MAX	(0xffffffff)
#endif

#ifndef INT32_MIN
#define INT32_MIN	(-INT32_MAX-1)
#endif

#define ATOX(c) \
  (isdigit(c) ? (c - '0') : (isupper(c) ? (c - 'A' + 10) : (c - 'a' + 10) ))

static u_int8_t *pbuf = NULL;		/* sadb_x_policy buffer */
static int tlen = 0;			/* total length of pbuf */
static int offset = 0;			/* offset of pbuf */
static int p_dir, p_type, p_protocol, p_mode, p_level, p_reqid;
static u_int32_t p_priority = 0;
static long p_priority_offset = 0;
static struct sockaddr *p_src = NULL;
static struct sockaddr *p_dst = NULL;

struct _val;
extern void yyerror __P((char *msg));
static struct sockaddr *parse_sockaddr __P((struct _val *addrbuf,
    struct _val *portbuf));
static int rule_check __P((void));
static int init_x_policy __P((void));
static int set_x_request __P((struct sockaddr *, struct sockaddr *));
static int set_sockaddr __P((struct sockaddr *));
static void policy_parse_request_init __P((void));
static void *policy_parse __P((const char *, int));

extern void __policy__strbuffer__init__ __P((const char *));
extern void __policy__strbuffer__free__ __P((void));
extern int yyparse __P((void));
extern int yylex __P((void));

extern char *__libipsectext;	/*XXX*/

#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 132 "../../ipsec-tools/src/libipsec/policy_parse.y"
typedef union {
	u_int num;
	u_int32_t num32;
	struct _val {
		int len;
		char *buf;
	} val;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 182 "__libipsec.tab.c"

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

#define DIR 257
#define PRIORITY 258
#define PLUS 259
#define PRIO_BASE 260
#define PRIO_OFFSET 261
#define ACTION 262
#define PROTOCOL 263
#define MODE 264
#define LEVEL 265
#define LEVEL_SPECIFY 266
#define IPADDRESS 267
#define PORT 268
#define ME 269
#define ANY 270
#define SLASH 271
#define HYPHEN 272
#define YYERRCODE 256
typedef int YYINT;
static const YYINT __libipseclhs[] = {                   -1,
    2,    0,    3,    0,    4,    0,    5,    0,    6,    0,
    7,    0,    0,    1,    1,    8,    8,    8,    8,    8,
    8,    8,    8,    9,   10,   12,   12,   13,   11,   14,
   11,   11,   11,
};
static const YYINT __libipseclen[] = {                    2,
    0,    4,    0,    6,    0,    7,    0,    6,    0,    8,
    0,    8,    1,    0,    2,    7,    6,    5,    4,    6,
    3,    2,    1,    1,    1,    1,    1,    0,    4,    0,
    6,    3,    3,
};
static const YYINT __libipsecdefred[] = {                 0,
    0,    0,    0,    1,    0,    0,    0,   14,    0,    7,
    0,    3,    0,    0,    0,   14,    0,   14,    5,   24,
   15,    0,    9,    0,   11,    0,   14,    0,   14,   14,
    0,   25,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   30,    0,    0,    0,   26,   27,   20,    0,    0,
    0,   32,   33,   16,    0,   29,    0,   31,
};
static const YYINT __libipsecdgoto[] = {                  2,
   14,    8,   18,   27,   16,   29,   30,   21,   22,   33,
   41,   48,   43,   50,
};
static const YYINT __libipsecsindex[] = {              -257,
 -250,    0, -246,    0, -249, -251, -245,    0, -244,    0,
 -239,    0, -243, -231, -238,    0, -229,    0,    0,    0,
    0, -237,    0, -231,    0, -231,    0, -228,    0,    0,
 -231,    0, -236, -231, -231, -242, -230, -233, -232, -235,
 -234,    0, -227, -226, -223,    0,    0,    0, -235, -225,
 -224,    0,    0,    0, -219,    0, -218,    0,
};
static const YYINT __libipsecrindex[] = {                 0,
   41,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   42,    0,    0,    0,    0,    0,    0,
    0,    1,    0,   49,    0,   51,    0,    2,    0,    0,
   52,    0,    3,   53,   54,    4, -217,    0,    0,    0,
    5,    0,    0,    0,    0,    0,    0,    0,    6,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT __libipsecgindex[] = {                 0,
   -9,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    7,    0,    0,
};
#define YYTABLESIZE 269
static const YYINT __libipsectable[] = {                  1,
   23,   22,   21,   19,   18,   17,   24,    3,   26,    9,
   12,    4,   10,    5,    6,   13,   15,   31,   19,   34,
   35,   17,   11,   23,   37,    7,   38,   39,   40,   46,
   47,   20,   25,   28,   36,   32,   49,   42,   44,   45,
   13,    2,   56,   52,   51,   53,   55,   57,    8,   58,
    4,    6,   10,   12,   28,   54,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   23,   22,   21,   19,   18,   17,
};
static const YYINT __libipseccheck[] = {                257,
    0,    0,    0,    0,    0,    0,   16,  258,   18,  259,
  262,  262,  262,  260,  261,  261,  261,   27,  262,   29,
   30,  261,  272,  262,  267,  272,  269,  270,  271,  265,
  266,  263,  262,  271,  271,  264,  271,  268,  272,  272,
    0,    0,  267,  270,  272,  269,  272,  267,    0,  268,
    0,    0,    0,    0,  272,   49,   -1,   -1,   -1,   -1,
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
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  263,  263,  263,  263,  263,  263,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 272
#define YYUNDFTOKEN 289
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const __libipsecname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DIR","PRIORITY","PLUS",
"PRIO_BASE","PRIO_OFFSET","ACTION","PROTOCOL","MODE","LEVEL","LEVEL_SPECIFY",
"IPADDRESS","PORT","ME","ANY","SLASH","HYPHEN",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"illegal-symbol",
};
static const char *const __libipsecrule[] = {
"$accept : policy_spec",
"$$1 :",
"policy_spec : DIR ACTION $$1 rules",
"$$2 :",
"policy_spec : DIR PRIORITY PRIO_OFFSET ACTION $$2 rules",
"$$3 :",
"policy_spec : DIR PRIORITY HYPHEN PRIO_OFFSET ACTION $$3 rules",
"$$4 :",
"policy_spec : DIR PRIORITY PRIO_BASE ACTION $$4 rules",
"$$5 :",
"policy_spec : DIR PRIORITY PRIO_BASE PLUS PRIO_OFFSET ACTION $$5 rules",
"$$6 :",
"policy_spec : DIR PRIORITY PRIO_BASE HYPHEN PRIO_OFFSET ACTION $$6 rules",
"policy_spec : DIR",
"rules :",
"rules : rules rule",
"rule : protocol SLASH mode SLASH addresses SLASH level",
"rule : protocol SLASH mode SLASH addresses SLASH",
"rule : protocol SLASH mode SLASH addresses",
"rule : protocol SLASH mode SLASH",
"rule : protocol SLASH mode SLASH SLASH level",
"rule : protocol SLASH mode",
"rule : protocol SLASH",
"rule : protocol",
"protocol : PROTOCOL",
"mode : MODE",
"level : LEVEL",
"level : LEVEL_SPECIFY",
"$$7 :",
"addresses : IPADDRESS $$7 HYPHEN IPADDRESS",
"$$8 :",
"addresses : IPADDRESS PORT $$8 HYPHEN IPADDRESS PORT",
"addresses : ME HYPHEN ANY",
"addresses : ANY HYPHEN ME",

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
#line 366 "../../ipsec-tools/src/libipsec/policy_parse.y"

void
yyerror(msg)
	char *msg;
{
	fprintf(stderr, "libipsec: %s while parsing \"%s\"\n",
		msg, __libipsectext);

	return;
}

static struct sockaddr *
parse_sockaddr(addrbuf, portbuf)
	struct _val *addrbuf;
	struct _val *portbuf;
{
	struct addrinfo hints, *res;
	char *addr;
	char *serv = NULL;
	int error;
	struct sockaddr *newaddr = NULL;

	if ((addr = malloc(addrbuf->len + 1)) == NULL) {
		yyerror("malloc failed");
		__ipsec_set_strerror(strerror(errno));
		return NULL;
	}

	if (portbuf && ((serv = malloc(portbuf->len + 1)) == NULL)) {
		free(addr);
		yyerror("malloc failed");
		__ipsec_set_strerror(strerror(errno));
		return NULL;
	}

	strncpy(addr, addrbuf->buf, addrbuf->len);
	addr[addrbuf->len] = '\0';

	if (portbuf) {
		strncpy(serv, portbuf->buf, portbuf->len);
		serv[portbuf->len] = '\0';
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_flags = AI_NUMERICHOST;
	hints.ai_socktype = SOCK_DGRAM;
	error = getaddrinfo(addr, serv, &hints, &res);
	free(addr);
	if (serv != NULL)
		free(serv);
	if (error != 0) {
		yyerror("invalid IP address");
		__ipsec_set_strerror(gai_strerror(error));
		return NULL;
	}

	if (res->ai_addr == NULL) {
		yyerror("invalid IP address");
		__ipsec_set_strerror(gai_strerror(error));
		return NULL;
	}

	newaddr = malloc(res->ai_addrlen);
	if (newaddr == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		freeaddrinfo(res);
		return NULL;
	}
	memcpy(newaddr, res->ai_addr, res->ai_addrlen);

	freeaddrinfo(res);

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return newaddr;
}

static int
rule_check()
{
	if (p_type == IPSEC_POLICY_IPSEC) {
		if (p_protocol == IPPROTO_IP) {
			__ipsec_errcode = EIPSEC_NO_PROTO;
			return -1;
		}

		if (p_mode != IPSEC_MODE_TRANSPORT
		 && p_mode != IPSEC_MODE_TUNNEL) {
			__ipsec_errcode = EIPSEC_INVAL_MODE;
			return -1;
		}

		if (p_src == NULL && p_dst == NULL) {
			 if (p_mode != IPSEC_MODE_TRANSPORT) {
				__ipsec_errcode = EIPSEC_INVAL_ADDRESS;
				return -1;
			}
		}
		else if (p_src->sa_family != p_dst->sa_family) {
			__ipsec_errcode = EIPSEC_FAMILY_MISMATCH;
			return -1;
		}
	}

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
init_x_policy()
{
	struct sadb_x_policy *p;

	if (pbuf) {
		free(pbuf);
		tlen = 0;
	}
	pbuf = malloc(sizeof(struct sadb_x_policy));
	if (pbuf == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
	tlen = sizeof(struct sadb_x_policy);

	memset(pbuf, 0, tlen);
	p = (struct sadb_x_policy *)pbuf;
	p->sadb_x_policy_len = 0;	/* must update later */
	p->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
	p->sadb_x_policy_type = p_type;
	p->sadb_x_policy_dir = p_dir;
	p->sadb_x_policy_id = 0;
#ifdef HAVE_PFKEY_POLICY_PRIORITY
	p->sadb_x_policy_priority = p_priority;
#else
    /* fail if given a priority and libipsec was not compiled with 
	   priority support */
	if (p_priority != 0)
	{
		__ipsec_errcode = EIPSEC_PRIORITY_NOT_COMPILED;
		return -1;
	}
#endif

	offset = tlen;

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
set_x_request(src, dst)
	struct sockaddr *src, *dst;
{
	struct sadb_x_ipsecrequest *p;
	int reqlen;
	u_int8_t *n;

	reqlen = sizeof(*p)
		+ (src ? sysdep_sa_len(src) : 0)
		+ (dst ? sysdep_sa_len(dst) : 0);
	tlen += reqlen;		/* increment to total length */

	n = realloc(pbuf, tlen);
	if (n == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
	pbuf = n;

	p = (struct sadb_x_ipsecrequest *)&pbuf[offset];
	p->sadb_x_ipsecrequest_len = reqlen;
	p->sadb_x_ipsecrequest_proto = p_protocol;
	p->sadb_x_ipsecrequest_mode = p_mode;
	p->sadb_x_ipsecrequest_level = p_level;
	p->sadb_x_ipsecrequest_reqid = p_reqid;
	offset += sizeof(*p);

	if (set_sockaddr(src) || set_sockaddr(dst))
		return -1;

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
set_sockaddr(addr)
	struct sockaddr *addr;
{
	if (addr == NULL) {
		__ipsec_errcode = EIPSEC_NO_ERROR;
		return 0;
	}

	/* tlen has already incremented */

	memcpy(&pbuf[offset], addr, sysdep_sa_len(addr));

	offset += sysdep_sa_len(addr);

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static void
policy_parse_request_init()
{
	p_protocol = IPPROTO_IP;
	p_mode = IPSEC_MODE_ANY;
	p_level = IPSEC_LEVEL_DEFAULT;
	p_reqid = 0;
	if (p_src != NULL) {
		free(p_src);
		p_src = NULL;
	}
	if (p_dst != NULL) {
		free(p_dst);
		p_dst = NULL;
	}

	return;
}

static void *
policy_parse(msg, msglen)
	const char *msg;
	int msglen;
{
	int error;

	pbuf = NULL;
	tlen = 0;

	/* initialize */
	p_dir = IPSEC_DIR_INVALID;
	p_type = IPSEC_POLICY_DISCARD;
	policy_parse_request_init();
	__policy__strbuffer__init__(msg);

	error = yyparse();	/* it must be set errcode. */
	__policy__strbuffer__free__();
	#ifdef __rtems__
	/* This frees the p_src and p_dst buffers. */
	policy_parse_request_init();
	#endif /* __rtems__ */

	if (error) {
		if (pbuf != NULL)
			free(pbuf);
		return NULL;
	}

	/* update total length */
	((struct sadb_x_policy *)pbuf)->sadb_x_policy_len = PFKEY_UNIT64(tlen);

	__ipsec_errcode = EIPSEC_NO_ERROR;

	return pbuf;
}

ipsec_policy_t
ipsec_set_policy(msg, msglen)
	__ipsec_const char *msg;
	int msglen;
{
	caddr_t policy;

	policy = policy_parse(msg, msglen);
	if (policy == NULL) {
		if (__ipsec_errcode == EIPSEC_NO_ERROR)
			__ipsec_errcode = EIPSEC_INVAL_ARGUMENT;
		return NULL;
	}

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return policy;
}
#ifdef __rtems__

void
ipsec_free_policy(ipsec_policy_t buf)
{
	free(buf);
}
#endif /* __rtems__ */
#line 714 "__libipsec.tab.c"

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
case 1:
#line 154 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-1].num;
			p_type = yystack.l_mark[0].num;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			p_priority = PRIORITY_DEFAULT;
#else
			p_priority = 0;
#endif

			if (init_x_policy())
				return -1;
		}
break;
case 3:
#line 169 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-3].num;
			p_type = yystack.l_mark[0].num;
			p_priority_offset = -atol(yystack.l_mark[-1].val.buf);

			errno = 0;
			if (errno != 0 || p_priority_offset < INT32_MIN)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_OFFSET;
				return -1;
			}

			p_priority = PRIORITY_DEFAULT + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
break;
case 5:
#line 188 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-4].num;
			p_type = yystack.l_mark[0].num;

			errno = 0;
			p_priority_offset = atol(yystack.l_mark[-1].val.buf);

			if (errno != 0 || p_priority_offset > INT32_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_OFFSET;
				return -1;
			}

			/* negative input value means lower priority, therefore higher
			   actual value so that is closer to the end of the list */
			p_priority = PRIORITY_DEFAULT + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
break;
case 7:
#line 210 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-3].num;
			p_type = yystack.l_mark[0].num;

			p_priority = yystack.l_mark[-1].num32;

			if (init_x_policy())
				return -1;
		}
break;
case 9:
#line 221 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-5].num;
			p_type = yystack.l_mark[0].num;

			errno = 0;
			p_priority_offset = atol(yystack.l_mark[-1].val.buf);

			if (errno != 0 || p_priority_offset > PRIORITY_OFFSET_NEGATIVE_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_BASE_OFFSET;
				return -1;
			}

			/* adding value means higher priority, therefore lower
			   actual value so that is closer to the beginning of the list */
			p_priority = yystack.l_mark[-3].num32 - (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
break;
case 11:
#line 243 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-5].num;
			p_type = yystack.l_mark[0].num;

			errno = 0;
			p_priority_offset = atol(yystack.l_mark[-1].val.buf);

			if (errno != 0 || p_priority_offset > PRIORITY_OFFSET_POSITIVE_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_BASE_OFFSET;
				return -1;
			}

			/* subtracting value means lower priority, therefore higher
			   actual value so that is closer to the end of the list */
			p_priority = yystack.l_mark[-3].num32 + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
break;
case 13:
#line 265 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[0].num;
			p_type = 0;	/* ignored it by kernel */

			p_priority = 0;

			if (init_x_policy())
				return -1;
		}
break;
case 15:
#line 278 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			if (rule_check() < 0)
				return -1;

			if (set_x_request(p_src, p_dst) < 0)
				return -1;

			policy_parse_request_init();
		}
break;
case 22:
#line 296 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
break;
case 23:
#line 300 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
break;
case 24:
#line 307 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{ p_protocol = yystack.l_mark[0].num; }
break;
case 25:
#line 311 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{ p_mode = yystack.l_mark[0].num; }
break;
case 26:
#line 315 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_level = yystack.l_mark[0].num;
			p_reqid = 0;
		}
break;
case 27:
#line 319 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_level = IPSEC_LEVEL_UNIQUE;
			p_reqid = atol(yystack.l_mark[0].val.buf);	/* atol() is good. */
		}
break;
case 28:
#line 326 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_src = parse_sockaddr(&yystack.l_mark[0].val, NULL);
			if (p_src == NULL)
				return -1;
		}
break;
case 29:
#line 332 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dst = parse_sockaddr(&yystack.l_mark[0].val, NULL);
			if (p_dst == NULL)
				return -1;
		}
break;
case 30:
#line 337 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_src = parse_sockaddr(&yystack.l_mark[-1].val, &yystack.l_mark[0].val);
			if (p_src == NULL)
				return -1;
		}
break;
case 31:
#line 343 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			p_dst = parse_sockaddr(&yystack.l_mark[-1].val, &yystack.l_mark[0].val);
			if (p_dst == NULL)
				return -1;
		}
break;
case 32:
#line 348 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			if (p_dir != IPSEC_DIR_OUTBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
break;
case 33:
#line 354 "../../ipsec-tools/src/libipsec/policy_parse.y"
	{
			if (p_dir != IPSEC_DIR_INBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
break;
#line 1140 "__libipsec.tab.c"
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
