/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20160324

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0

#ifndef yyparse
#define yyparse    __libipsecyyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      __libipsecyylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    __libipsecyyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     __libipsecyychar
#endif /* yychar */

#ifndef yyval
#define yyval      __libipsecyyval
#endif /* yyval */

#ifndef yylval
#define yylval     __libipsecyylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    __libipsecyydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    __libipsecyynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  __libipsecyyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      __libipsecyylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      __libipsecyylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   __libipsecyydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    __libipsecyydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   __libipsecyysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   __libipsecyyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   __libipsecyygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    __libipsecyytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    __libipsecyycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     __libipsecyyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     __libipsecyyrule
#endif /* yyrule */
#define YYPREFIX "__libipsecyy"

#define YYPURE 0

#line 52 "../../freebsd/lib/libipsec/policy_parse.y"
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netipsec/ipsec.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include "ipsec_strerror.h"

#define ATOX(c) \
  (isdigit(c) ? (c - '0') : (isupper(c) ? (c - 'A' + 10) : (c - 'a' + 10) ))

static caddr_t pbuf = NULL;		/* sadb_x_policy buffer */
static int tlen = 0;			/* total length of pbuf */
static int offset = 0;			/* offset of pbuf */
static int p_dir, p_type, p_protocol, p_mode, p_level, p_reqid;
static struct sockaddr *p_src = NULL;
static struct sockaddr *p_dst = NULL;

struct _val;
extern void yyerror(char *msg);
static struct sockaddr *parse_sockaddr(struct _val *buf);
static int rule_check(void);
static int init_x_policy(void);
static int set_x_request(struct sockaddr *src, struct sockaddr *dst);
static int set_sockaddr(struct sockaddr *addr);
static void policy_parse_request_init(void);
static caddr_t policy_parse(char *msg, int msglen);

extern void __policy__strbuffer__init__(char *msg);
extern void __policy__strbuffer__free__(void);
extern int yylex(void);

extern char *__libipsecyytext;	/*XXX*/

#line 97 "../../freebsd/lib/libipsec/policy_parse.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	u_int num;
	struct _val {
		int len;
		char *buf;
	} val;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 160 "__libipsecyy.tab.c"

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
#define ACTION 258
#define PROTOCOL 259
#define MODE 260
#define LEVEL 261
#define LEVEL_SPECIFY 262
#define IPADDRESS 263
#define ME 264
#define ANY 265
#define SLASH 266
#define HYPHEN 267
#define YYERRCODE 256
typedef int YYINT;
static const YYINT __libipsecyylhs[] = {                 -1,
    2,    0,    0,    1,    1,    3,    3,    3,    3,    3,
    3,    3,    3,    4,    5,    7,    7,    8,    6,    6,
    6,
};
static const YYINT __libipsecyylen[] = {                  2,
    0,    4,    1,    0,    2,    7,    6,    5,    4,    6,
    3,    2,    1,    1,    1,    1,    1,    0,    4,    3,
    3,
};
static const YYINT __libipsecyydefred[] = {               0,
    0,    0,    1,    4,    0,   14,    5,    0,    0,   15,
    0,    0,   18,    0,    0,    0,    0,    0,    0,    0,
   16,   17,   10,    0,    0,   20,   21,    6,   19,
};
static const YYINT __libipsecyydgoto[] = {                2,
    5,    4,    7,    8,   11,   17,   23,   18,
};
static const YYINT __libipsecyysindex[] = {            -257,
 -245,    0,    0,    0, -244,    0,    0, -252, -243,    0,
 -248, -256,    0, -251, -247, -250, -242, -246, -240, -241,
    0,    0,    0, -250, -237,    0,    0,    0,    0,
};
static const YYINT __libipsecyyrindex[] = {               0,
   19,    0,    0,    0,   22,    0,    0,    1,    2,    0,
    3,    4,    0,    0,    0,    0,    5,    0,    0,    0,
    0,    0,    0,    6,    0,    0,    0,    0,    0,
};
static const YYINT __libipsecyygindex[] = {               0,
    0,    0,    0,    0,    0,    0,    7,    0,
};
#define YYTABLESIZE 265
static const YYINT __libipsecyytable[] = {                1,
   13,   12,   11,    9,    8,    7,   13,   14,   15,   16,
   21,   22,    3,    9,    6,   19,   10,   12,    3,   20,
   25,    2,   27,   24,   26,   29,    0,    0,    0,    0,
   28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   13,
   12,   11,    9,    8,    7,
};
static const YYINT __libipsecyycheck[] = {              257,
    0,    0,    0,    0,    0,    0,  263,  264,  265,  266,
  261,  262,  258,  266,  259,  267,  260,  266,    0,  267,
  267,    0,  264,  266,  265,  263,   -1,   -1,   -1,   -1,
   24,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
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
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  259,
  259,  259,  259,  259,  259,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#define YYUNDFTOKEN 278
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const __libipsecyyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DIR","ACTION","PROTOCOL","MODE",
"LEVEL","LEVEL_SPECIFY","IPADDRESS","ME","ANY","SLASH","HYPHEN",0,0,0,0,0,0,0,0,
0,0,"illegal-symbol",
};
static const char *const __libipsecyyrule[] = {
"$accept : policy_spec",
"$$1 :",
"policy_spec : DIR ACTION $$1 rules",
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
"$$2 :",
"addresses : IPADDRESS $$2 HYPHEN IPADDRESS",
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
#line 212 "../../freebsd/lib/libipsec/policy_parse.y"

void
yyerror(msg)
	char *msg;
{
	fprintf(stderr, "libipsec: %s while parsing \"%s\"\n",
		msg, __libipsecyytext);

	return;
}

static struct sockaddr *
parse_sockaddr(buf)
	struct _val *buf;
{
	struct addrinfo hints, *res;
	char *serv = NULL;
	int error;
	struct sockaddr *newaddr = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_flags = AI_NUMERICHOST;
	error = getaddrinfo(buf->buf, serv, &hints, &res);
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

	newaddr = malloc(res->ai_addr->sa_len);
	if (newaddr == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		freeaddrinfo(res);
		return NULL;
	}
	memcpy(newaddr, res->ai_addr, res->ai_addr->sa_len);

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

	tlen = sizeof(struct sadb_x_policy);

	pbuf = malloc(tlen);
	if (pbuf == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
	memset(pbuf, 0, tlen);
	p = (struct sadb_x_policy *)pbuf;
	p->sadb_x_policy_len = 0;	/* must update later */
	p->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
	p->sadb_x_policy_type = p_type;
	p->sadb_x_policy_dir = p_dir;
	p->sadb_x_policy_id = 0;

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

	reqlen = sizeof(*p)
		+ (src ? src->sa_len : 0)
		+ (dst ? dst->sa_len : 0);
	tlen += reqlen;		/* increment to total length */

	pbuf = realloc(pbuf, tlen);
	if (pbuf == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
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

	memcpy(&pbuf[offset], addr, addr->sa_len);

	offset += addr->sa_len;

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

static caddr_t
policy_parse(msg, msglen)
	char *msg;
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

caddr_t
ipsec_set_policy(msg, msglen)
	char *msg;
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

#line 604 "__libipsecyy.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
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
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
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
            yys = yyname[YYTRANSLATE(yychar)];
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
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 115 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[-1].num;
			p_type = yystack.l_mark[0].num;

			if (init_x_policy())
				return -1;
		}
break;
case 3:
#line 124 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_dir = yystack.l_mark[0].num;
			p_type = 0;	/* ignored it by kernel */

			if (init_x_policy())
				return -1;
		}
break;
case 5:
#line 135 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			if (rule_check() < 0)
				return -1;

			if (set_x_request(p_src, p_dst) < 0)
				return -1;

			policy_parse_request_init();
		}
break;
case 12:
#line 153 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
break;
case 13:
#line 157 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
break;
case 14:
#line 164 "../../freebsd/lib/libipsec/policy_parse.y"
	{ p_protocol = yystack.l_mark[0].num; }
break;
case 15:
#line 168 "../../freebsd/lib/libipsec/policy_parse.y"
	{ p_mode = yystack.l_mark[0].num; }
break;
case 16:
#line 172 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_level = yystack.l_mark[0].num;
			p_reqid = 0;
		}
break;
case 17:
#line 176 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_level = IPSEC_LEVEL_UNIQUE;
			p_reqid = atol(yystack.l_mark[0].val.buf);	/* atol() is good. */
		}
break;
case 18:
#line 183 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_src = parse_sockaddr(&yystack.l_mark[0].val);
			if (p_src == NULL)
				return -1;
		}
break;
case 19:
#line 189 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			p_dst = parse_sockaddr(&yystack.l_mark[0].val);
			if (p_dst == NULL)
				return -1;
		}
break;
case 20:
#line 194 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			if (p_dir != IPSEC_DIR_OUTBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
break;
case 21:
#line 200 "../../freebsd/lib/libipsec/policy_parse.y"
	{
			if (p_dir != IPSEC_DIR_INBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
break;
#line 908 "__libipsecyy.tab.c"
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
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
        goto yyoverflow;
    }
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
