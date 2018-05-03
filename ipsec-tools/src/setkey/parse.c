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
#define yyparse    setkeyyyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      setkeyyylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    setkeyyyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     setkeyyychar
#endif /* yychar */

#ifndef yyval
#define yyval      setkeyyyval
#endif /* yyval */

#ifndef yylval
#define yylval     setkeyyylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    setkeyyydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    setkeyyynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  setkeyyyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      setkeyyylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      setkeyyylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   setkeyyydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    setkeyyydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   setkeyyysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   setkeyyyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   setkeyyygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    setkeyyytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    setkeyyycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     setkeyyyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     setkeyyyrule
#endif /* yyrule */
#define YYPREFIX "setkeyyy"

#define YYPURE 0

#line 35 "../../ipsec-tools/src/setkey/parse.y"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <net/pfkeyv2.h>
#include PATH_IPSEC_H
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "libpfkey.h"
#include "vchar.h"
#include "extern.h"

#define DEFAULT_NATT_PORT	4500

#ifndef UDP_ENCAP_ESPINUDP
#define UDP_ENCAP_ESPINUDP	2
#endif

#define ATOX(c) \
  (isdigit((int)c) ? (c - '0') : \
    (isupper((int)c) ? (c - 'A' + 10) : (c - 'a' + 10)))

#ifndef __rtems__
u_int32_t p_spi;
u_int p_ext, p_alg_enc, p_alg_auth, p_replay, p_mode;
u_int32_t p_reqid;
u_int p_key_enc_len, p_key_auth_len;
const char *p_key_enc;
const char *p_key_auth;
time_t p_lt_hard, p_lt_soft;
size_t p_lb_hard, p_lb_soft;
#else /* __rtems__ */
static u_int32_t p_spi;
static u_int p_ext, p_alg_enc, p_alg_auth, p_replay, p_mode;
static u_int32_t p_reqid;
static u_int p_key_enc_len, p_key_auth_len;
static const char *p_key_enc;
static const char *p_key_auth;
static time_t p_lt_hard, p_lt_soft;
static size_t p_lb_hard, p_lb_soft;
#endif /* __rtems__ */

struct security_ctx {
	u_int8_t doi;
	u_int8_t alg;
	u_int16_t len;
	char *buf;
};

static struct security_ctx sec_ctx;

static u_int p_natt_type;
static struct addrinfo * p_natt_oa = NULL;

static int p_aiflags = 0, p_aifamily = PF_UNSPEC;

static struct addrinfo *parse_addr __P((char *, char *));
static int fix_portstr __P((int, vchar_t *, vchar_t *, vchar_t *));
static int setvarbuf __P((char *, int *, struct sadb_ext *, int, 
    const void *, int));
void parse_init __P((void));
void free_buffer __P((void));

int setkeymsg0 __P((struct sadb_msg *, unsigned int, unsigned int, size_t));
static int setkeymsg_spdaddr __P((unsigned int, unsigned int, vchar_t *,
	struct addrinfo *, int, struct addrinfo *, int));
static int setkeymsg_spdaddr_tag __P((unsigned int, char *, vchar_t *));
static int setkeymsg_addr __P((unsigned int, unsigned int,
	struct addrinfo *, struct addrinfo *, int));
static int setkeymsg_add __P((unsigned int, unsigned int,
	struct addrinfo *, struct addrinfo *));
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 121 "../../ipsec-tools/src/setkey/parse.y"
typedef union {
	int num;
	unsigned long ulnum;
	vchar_t val;
	struct addrinfo *res;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 200 "setkeyyy.tab.c"

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

#define EOT 257
#define SLASH 258
#define BLCL 259
#define ELCL 260
#define ADD 261
#define GET 262
#define DELETE 263
#define DELETEALL 264
#define FLUSH 265
#define DUMP 266
#define EXIT 267
#define PR_ESP 268
#define PR_AH 269
#define PR_IPCOMP 270
#define PR_ESPUDP 271
#define PR_TCP 272
#define F_PROTOCOL 273
#define F_AUTH 274
#define F_ENC 275
#define F_REPLAY 276
#define F_COMP 277
#define F_RAWCPI 278
#define F_MODE 279
#define MODE 280
#define F_REQID 281
#define F_EXT 282
#define EXTENSION 283
#define NOCYCLICSEQ 284
#define ALG_AUTH 285
#define ALG_AUTH_NOKEY 286
#define ALG_ENC 287
#define ALG_ENC_NOKEY 288
#define ALG_ENC_DESDERIV 289
#define ALG_ENC_DES32IV 290
#define ALG_ENC_OLD 291
#define ALG_COMP 292
#define F_LIFETIME_HARD 293
#define F_LIFETIME_SOFT 294
#define F_LIFEBYTE_HARD 295
#define F_LIFEBYTE_SOFT 296
#define DECSTRING 297
#define QUOTEDSTRING 298
#define HEXSTRING 299
#define STRING 300
#define ANY 301
#define SPDADD 302
#define SPDUPDATE 303
#define SPDDELETE 304
#define SPDDUMP 305
#define SPDFLUSH 306
#define F_POLICY 307
#define PL_REQUESTS 308
#define F_AIFLAGS 309
#define TAGGED 310
#define SECURITY_CTX 311
#define YYERRCODE 256
typedef int YYINT;
static const YYINT setkeyyylhs[] = {                     -1,
    0,    0,   11,   11,   11,   11,   11,   11,   11,   11,
   11,   11,   11,   11,   12,   14,   15,   13,   16,   17,
    2,    2,    2,    2,    2,    2,    2,   25,   25,   27,
   27,   27,   28,   28,   29,   30,   30,   31,   31,   31,
   31,   31,   32,   32,    5,    5,   26,   26,   33,   33,
   33,   33,   33,   33,   33,   33,   33,   33,   33,   19,
   19,   20,   20,   21,   22,   23,   24,   24,   35,    9,
   10,   10,    1,    1,    4,    4,    4,    4,    3,    3,
    3,    3,    7,    7,   34,   34,    8,    6,   18,
};
static const YYINT setkeyyylen[] = {                      2,
    0,    2,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    9,    8,    6,    8,    3,    3,
    0,    1,    1,    1,    1,    2,    1,    1,    1,    1,
    1,    1,    4,    2,    2,    2,    3,    1,    2,    1,
    2,    2,    2,    1,    1,    1,    0,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    4,   13,
    5,   13,    5,   13,    2,    2,    0,    2,    1,    1,
    1,    2,    0,    2,    0,    3,    3,    3,    1,    1,
    1,    1,    0,    1,    0,    4,    2,    1,    2,
};
static const YYINT setkeyyydefred[] = {                   1,
    0,   67,   67,   67,   67,    0,    0,    0,    0,    0,
   67,    0,    0,    2,    3,    4,    5,    6,    7,    8,
    9,   10,   11,   12,   13,   14,    0,    0,    0,    0,
   22,   23,   24,    0,   27,    0,    0,   89,    0,    0,
    0,    0,    0,   65,   66,    0,   69,    0,   68,    0,
    0,   70,    0,   26,   19,   20,    0,    0,    0,    0,
    0,    0,   72,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   88,   87,   61,   74,    0,   63,    0,    0,   77,
   78,   76,   28,   29,   47,   47,   47,   17,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   30,   31,   32,   48,   18,   16,    0,    0,    0,    0,
   44,   35,    0,   38,    0,    0,   40,    0,   54,    0,
   51,   52,   53,   49,   50,   55,   56,   57,   58,    0,
   15,   81,   79,   82,   80,    0,    0,    0,   45,   46,
   43,   39,   41,   42,    0,   37,    0,   84,    0,    0,
    0,   33,   59,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   60,   62,   64,   86,
};
static const YYINT setkeyyydgoto[] = {                    1,
   71,   36,  156,   63,  161,   83,  169,   69,   53,   48,
   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
   24,   25,   26,   27,   95,  102,  120,  121,  122,  123,
  138,  132,  124,  175,   49,
};
static const YYINT setkeyyysindex[] = {                   0,
 -251,    0,    0,    0,    0, -128, -128, -250, -267, -253,
    0, -227, -169,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -292, -292, -292, -277,
    0,    0,    0, -187,    0, -129, -122,    0, -233, -214,
 -133, -185, -180,    0,    0, -103,    0, -134,    0, -134,
 -134,    0, -187,    0,    0,    0, -140,  -90, -140,  -90,
  -90, -147,    0, -128, -128, -128, -128, -139,  -87, -126,
 -103,  -85, -103, -103,  -86,  -84,  -83, -210, -210, -210,
  -82,    0,    0,    0,    0, -127,    0, -121, -120,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -90,  -90,
  -90, -197, -255, -248, -103, -103, -103, -280, -142, -119,
 -111, -276, -115, -264, -114, -113, -112, -110, -109,  -71,
    0,    0,    0,    0,    0,    0, -191, -191, -191, -249,
    0,    0, -249,    0, -249, -249,    0,  -81,    0,  -89,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -107,
    0,    0,    0,    0,    0, -108, -108, -108,    0,    0,
    0,    0,    0,    0, -280,    0, -104,    0, -116, -116,
 -116,    0,    0, -106, -140, -140, -140, -101,  -60,  -58,
  -57, -100,    0,    0,    0,    0,
};
static const YYINT setkeyyyrindex[] = {                   0,
    0,    0,    0,    0,    0,  -55,  -55,    0, -173, -173,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -235,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -178,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -256,    0, -256,
 -256,    0,    0, -192, -192, -192,  -55,    0,    0,    0,
  -99,    0,  -99,  -99,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -230, -230,
 -230,    0,    0,    0, -189, -189, -189,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -54,    0,  -53,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -177, -177, -177,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -102, -102,
 -102,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
static const YYINT setkeyyygindex[] = {                   0,
  -25,   -6,  -56,   -5,   22,    0,    2,  -59,  -16,   75,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  128,   72,   65,    0,    0,    0,    0,
    0,   41,    0,   -7,    0,
};
#define YYTABLESIZE 206
static const YYINT setkeyyytable[] = {                   72,
   37,  125,   73,  141,  130,  131,   38,   46,  126,    2,
    3,    4,    5,    6,    7,    8,   47,   54,  144,  145,
  110,   25,   52,  112,  142,  113,  114,  110,   73,   44,
  112,   47,  113,  114,   73,   74,   67,  115,  116,  117,
  118,   73,   39,   73,  115,  116,  117,  118,  159,  160,
    9,   10,   11,   12,   13,  119,   41,   78,   79,   80,
   81,   25,  119,   25,   57,   86,   73,   88,   89,   73,
   73,  157,  158,  105,  106,  107,  108,  109,  110,  111,
  152,  112,   75,  113,  114,   58,   93,   45,   94,   71,
   71,   71,   71,   71,   47,  115,  116,  117,  118,  127,
  128,  129,   50,   51,   21,  153,   21,   75,  154,  155,
   75,   75,   52,  119,   60,  179,  180,  181,   71,   61,
   71,   71,   64,   47,   65,   66,   67,   55,   47,   83,
   28,   29,   30,   83,   56,   67,   40,   42,   43,   31,
   32,   33,   34,   35,  133,  134,  135,  136,  137,   75,
   96,   97,   76,   77,  162,   62,  163,  164,  170,  171,
  103,  104,  176,  177,   59,   46,   68,   70,   82,   84,
   85,   87,   99,   90,   98,   91,   92,  139,  100,  101,
  140,  143,  146,  147,  148,  151,  149,  150,  166,  167,
  178,  168,  165,  173,  174,  182,  183,  186,  184,  185,
   75,   21,   34,   36,   85,  172,
};
static const YYINT setkeyyycheck[] = {                   59,
    7,  257,  259,  280,  285,  286,  257,  300,  257,  261,
  262,  263,  264,  265,  266,  267,  309,   34,  283,  284,
  276,  257,  300,  279,  301,  281,  282,  276,  259,  257,
  279,  309,  281,  282,   60,   61,   53,  293,  294,  295,
  296,  272,  310,  300,  293,  294,  295,  296,  298,  299,
  302,  303,  304,  305,  306,  311,  310,   64,   65,   66,
   67,  297,  311,  299,  298,   71,  297,   73,   74,  300,
  301,  128,  129,   99,  100,  101,  274,  275,  276,  277,
  272,  279,  272,  281,  282,  300,  297,  257,  299,  268,
  269,  270,  271,  272,  309,  293,  294,  295,  296,  105,
  106,  107,   28,   29,  297,  297,  299,  297,  300,  301,
  300,  301,  300,  311,  300,  175,  176,  177,  297,  300,
  299,  300,   48,  309,   50,   51,  300,  257,  309,  307,
    3,    4,    5,  311,  257,  309,    9,   10,   11,  268,
  269,  270,  271,  272,  287,  288,  289,  290,  291,  297,
   79,   80,  300,  301,  133,  259,  135,  136,  157,  158,
   96,   97,  170,  171,  298,  300,  307,  258,  308,  257,
  297,  257,  300,  260,  257,  260,  260,  297,  300,  300,
  292,  297,  297,  297,  297,  257,  297,  297,  278,  297,
  297,  300,  274,  298,  311,  297,  257,  298,  257,  257,
  300,  257,  257,  257,  307,  165,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 311
#define YYUNDFTOKEN 349
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const setkeyyyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"EOT","SLASH","BLCL","ELCL","ADD",
"GET","DELETE","DELETEALL","FLUSH","DUMP","EXIT","PR_ESP","PR_AH","PR_IPCOMP",
"PR_ESPUDP","PR_TCP","F_PROTOCOL","F_AUTH","F_ENC","F_REPLAY","F_COMP",
"F_RAWCPI","F_MODE","MODE","F_REQID","F_EXT","EXTENSION","NOCYCLICSEQ",
"ALG_AUTH","ALG_AUTH_NOKEY","ALG_ENC","ALG_ENC_NOKEY","ALG_ENC_DESDERIV",
"ALG_ENC_DES32IV","ALG_ENC_OLD","ALG_COMP","F_LIFETIME_HARD","F_LIFETIME_SOFT",
"F_LIFEBYTE_HARD","F_LIFEBYTE_SOFT","DECSTRING","QUOTEDSTRING","HEXSTRING",
"STRING","ANY","SPDADD","SPDUPDATE","SPDDELETE","SPDDUMP","SPDFLUSH","F_POLICY",
"PL_REQUESTS","F_AIFLAGS","TAGGED","SECURITY_CTX",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const setkeyyyrule[] = {
"$accept : commands",
"commands :",
"commands : commands command",
"command : add_command",
"command : get_command",
"command : delete_command",
"command : deleteall_command",
"command : flush_command",
"command : dump_command",
"command : exit_command",
"command : spdadd_command",
"command : spdupdate_command",
"command : spddelete_command",
"command : spddump_command",
"command : spdflush_command",
"add_command : ADD ipaddropts ipandport ipandport protocol_spec spi extension_spec algorithm_spec EOT",
"delete_command : DELETE ipaddropts ipandport ipandport protocol_spec spi extension_spec EOT",
"deleteall_command : DELETEALL ipaddropts ipaddr ipaddr protocol_spec EOT",
"get_command : GET ipaddropts ipandport ipandport protocol_spec spi extension_spec EOT",
"flush_command : FLUSH protocol_spec EOT",
"dump_command : DUMP protocol_spec EOT",
"protocol_spec :",
"protocol_spec : PR_ESP",
"protocol_spec : PR_AH",
"protocol_spec : PR_IPCOMP",
"protocol_spec : PR_ESPUDP",
"protocol_spec : PR_ESPUDP ipaddr",
"protocol_spec : PR_TCP",
"spi : DECSTRING",
"spi : HEXSTRING",
"algorithm_spec : esp_spec",
"algorithm_spec : ah_spec",
"algorithm_spec : ipcomp_spec",
"esp_spec : F_ENC enc_alg F_AUTH auth_alg",
"esp_spec : F_ENC enc_alg",
"ah_spec : F_AUTH auth_alg",
"ipcomp_spec : F_COMP ALG_COMP",
"ipcomp_spec : F_COMP ALG_COMP F_RAWCPI",
"enc_alg : ALG_ENC_NOKEY",
"enc_alg : ALG_ENC key_string",
"enc_alg : ALG_ENC_OLD",
"enc_alg : ALG_ENC_DESDERIV key_string",
"enc_alg : ALG_ENC_DES32IV key_string",
"auth_alg : ALG_AUTH key_string",
"auth_alg : ALG_AUTH_NOKEY",
"key_string : QUOTEDSTRING",
"key_string : HEXSTRING",
"extension_spec :",
"extension_spec : extension_spec extension",
"extension : F_EXT EXTENSION",
"extension : F_EXT NOCYCLICSEQ",
"extension : F_MODE MODE",
"extension : F_MODE ANY",
"extension : F_REQID DECSTRING",
"extension : F_REPLAY DECSTRING",
"extension : F_LIFETIME_HARD DECSTRING",
"extension : F_LIFETIME_SOFT DECSTRING",
"extension : F_LIFEBYTE_HARD DECSTRING",
"extension : F_LIFEBYTE_SOFT DECSTRING",
"extension : SECURITY_CTX DECSTRING DECSTRING QUOTEDSTRING",
"spdadd_command : SPDADD ipaddropts STRING prefix portstr STRING prefix portstr upper_spec upper_misc_spec context_spec policy_spec EOT",
"spdadd_command : SPDADD TAGGED QUOTEDSTRING policy_spec EOT",
"spdupdate_command : SPDUPDATE ipaddropts STRING prefix portstr STRING prefix portstr upper_spec upper_misc_spec context_spec policy_spec EOT",
"spdupdate_command : SPDUPDATE TAGGED QUOTEDSTRING policy_spec EOT",
"spddelete_command : SPDDELETE ipaddropts STRING prefix portstr STRING prefix portstr upper_spec upper_misc_spec context_spec policy_spec EOT",
"spddump_command : SPDDUMP EOT",
"spdflush_command : SPDFLUSH EOT",
"ipaddropts :",
"ipaddropts : ipaddropts ipaddropt",
"ipaddropt : F_AIFLAGS",
"ipaddr : STRING",
"ipandport : STRING",
"ipandport : STRING portstr",
"prefix :",
"prefix : SLASH DECSTRING",
"portstr :",
"portstr : BLCL ANY ELCL",
"portstr : BLCL DECSTRING ELCL",
"portstr : BLCL STRING ELCL",
"upper_spec : DECSTRING",
"upper_spec : ANY",
"upper_spec : PR_TCP",
"upper_spec : STRING",
"upper_misc_spec :",
"upper_misc_spec : STRING",
"context_spec :",
"context_spec : SECURITY_CTX DECSTRING DECSTRING QUOTEDSTRING",
"policy_spec : F_POLICY policy_requests",
"policy_requests : PL_REQUESTS",
"exit_command : EXIT EOT",

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
#line 919 "../../ipsec-tools/src/setkey/parse.y"

int
setkeymsg0(msg, type, satype, l)
	struct sadb_msg *msg;
	unsigned int type;
	unsigned int satype;
	size_t l;
{

	msg->sadb_msg_version = PF_KEY_V2;
	msg->sadb_msg_type = type;
	msg->sadb_msg_errno = 0;
	msg->sadb_msg_satype = satype;
	msg->sadb_msg_reserved = 0;
	msg->sadb_msg_seq = 0;
	msg->sadb_msg_pid = getpid();
	msg->sadb_msg_len = PFKEY_UNIT64(l);
	return 0;
}

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_spdaddr(type, upper, policy, srcs, splen, dsts, dplen)
	unsigned int type;
	unsigned int upper;
	vchar_t *policy;
	struct addrinfo *srcs;
	int splen;
	struct addrinfo *dsts;
	int dplen;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;
	struct sadb_x_policy *sp;
#ifdef HAVE_POLICY_FWD
	struct sadb_x_ipsecrequest *ps = NULL;
	int saved_level, saved_id = 0;
#endif

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, SADB_SATYPE_UNSPEC, 0);
	l = sizeof(struct sadb_msg);

	sp = (struct sadb_x_policy*) (buf + l);
	memcpy(buf + l, policy->buf, policy->len);
	l += policy->len;

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = upper;
			m_addr.sadb_address_prefixlen =
			    (splen >= 0 ? splen : plen);
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = upper;
			m_addr.sadb_address_prefixlen =
			    (dplen >= 0 ? dplen : plen);
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);
#ifdef SADB_X_EXT_SEC_CTX
			/* Add security context label */
			if (sec_ctx.doi) {
				struct sadb_x_sec_ctx m_sec_ctx;
				u_int slen = sizeof(struct sadb_x_sec_ctx);

				memset(&m_sec_ctx, 0, slen);

				m_sec_ctx.sadb_x_sec_len =
				PFKEY_UNIT64(slen + PFKEY_ALIGN8(sec_ctx.len));

				m_sec_ctx.sadb_x_sec_exttype = 
					SADB_X_EXT_SEC_CTX;
				m_sec_ctx.sadb_x_ctx_len = sec_ctx.len;/*bytes*/
				m_sec_ctx.sadb_x_ctx_doi = sec_ctx.doi;
				m_sec_ctx.sadb_x_ctx_alg = sec_ctx.alg;
				setvarbuf(buf, &l, 
					  (struct sadb_ext *)&m_sec_ctx, slen, 
					  (caddr_t)sec_ctx.buf, sec_ctx.len);
			}
#endif
			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

#ifdef HAVE_POLICY_FWD
			/* create extra call for FWD policy */
			if (f_rfcmode && sp->sadb_x_policy_dir == IPSEC_DIR_INBOUND) {
				sp->sadb_x_policy_dir = IPSEC_DIR_FWD;
				ps = (struct sadb_x_ipsecrequest*) (sp+1);

				/* if request level is unique, change it to
				 * require for fwd policy */
				/* XXX: currently, only first policy is updated
				 * only. Update following too... */
				saved_level = ps->sadb_x_ipsecrequest_level;
				if (saved_level == IPSEC_LEVEL_UNIQUE) {
					saved_id = ps->sadb_x_ipsecrequest_reqid;
					ps->sadb_x_ipsecrequest_reqid=0;
					ps->sadb_x_ipsecrequest_level=IPSEC_LEVEL_REQUIRE;
				}

				sendkeymsg(buf, l);
				/* restoring for next message */
				sp->sadb_x_policy_dir = IPSEC_DIR_INBOUND;
				if (saved_level == IPSEC_LEVEL_UNIQUE) {
					ps->sadb_x_ipsecrequest_reqid = saved_id;
					ps->sadb_x_ipsecrequest_level = saved_level;
				}
			}
#endif

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

static int
setkeymsg_spdaddr_tag(type, tag, policy)
	unsigned int type;
	char *tag;
	vchar_t *policy;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0;
#ifdef SADB_X_EXT_TAG
	struct sadb_x_tag m_tag;
#endif
	int n;

	msg = (struct sadb_msg *)buf;

	/* fix up length afterwards */
	setkeymsg0(msg, type, SADB_SATYPE_UNSPEC, 0);
	l = sizeof(struct sadb_msg);

	memcpy(buf + l, policy->buf, policy->len);
	l += policy->len;

	l0 = l;
	n = 0;

#ifdef SADB_X_EXT_TAG
	memset(&m_tag, 0, sizeof(m_tag));
	m_tag.sadb_x_tag_len = PFKEY_UNIT64(sizeof(m_tag));
	m_tag.sadb_x_tag_exttype = SADB_X_EXT_TAG;
	if (strlcpy(m_tag.sadb_x_tag_name, tag,
	    sizeof(m_tag.sadb_x_tag_name)) >= sizeof(m_tag.sadb_x_tag_name))
		return -1;
	memcpy(buf + l, &m_tag, sizeof(m_tag));
	l += sizeof(m_tag);
#endif

	msg->sadb_msg_len = PFKEY_UNIT64(l);

	sendkeymsg(buf, l);

	return 0;
}

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_addr(type, satype, srcs, dsts, no_spi)
	unsigned int type;
	unsigned int satype;
	struct addrinfo *srcs;
	struct addrinfo *dsts;
	int no_spi;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0, len;
	struct sadb_sa m_sa;
	struct sadb_x_sa2 m_sa2;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, satype, 0);
	l = sizeof(struct sadb_msg);

	if (!no_spi) {
		len = sizeof(struct sadb_sa);
		m_sa.sadb_sa_len = PFKEY_UNIT64(len);
		m_sa.sadb_sa_exttype = SADB_EXT_SA;
		m_sa.sadb_sa_spi = htonl(p_spi);
		m_sa.sadb_sa_replay = p_replay;
		m_sa.sadb_sa_state = 0;
		m_sa.sadb_sa_auth = p_alg_auth;
		m_sa.sadb_sa_encrypt = p_alg_enc;
		m_sa.sadb_sa_flags = p_ext;

		memcpy(buf + l, &m_sa, len);
		l += len;

		len = sizeof(struct sadb_x_sa2);
		m_sa2.sadb_x_sa2_len = PFKEY_UNIT64(len);
		m_sa2.sadb_x_sa2_exttype = SADB_X_EXT_SA2;
		m_sa2.sadb_x_sa2_mode = p_mode;
		m_sa2.sadb_x_sa2_reqid = p_reqid;

		memcpy(buf + l, &m_sa2, len);
		l += len;
	}

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);

			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

#ifdef SADB_X_EXT_NAT_T_TYPE
static u_int16_t get_port (struct addrinfo *addr)
{
	struct sockaddr *s = addr->ai_addr;
	u_int16_t port = 0;

	switch (s->sa_family) {
	case AF_INET:
	  {
		struct sockaddr_in *sin4 = (struct sockaddr_in *)s;
		port = ntohs(sin4->sin_port);
		break;
	  }
	case AF_INET6:
	  {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)s;
		port = ntohs(sin6->sin6_port);
		break;
	  }
	}

	if (port == 0)
		port = DEFAULT_NATT_PORT;

	return port;
}
#endif

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_add(type, satype, srcs, dsts)
	unsigned int type;
	unsigned int satype;
	struct addrinfo *srcs;
	struct addrinfo *dsts;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0, len;
	struct sadb_sa m_sa;
	struct sadb_x_sa2 m_sa2;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, satype, 0);
	l = sizeof(struct sadb_msg);

	/* set encryption algorithm, if present. */
	if (satype != SADB_X_SATYPE_IPCOMP && p_key_enc) {
		union {
			struct sadb_key key;
			struct sadb_ext ext;
		} m;

		m.key.sadb_key_len =
			PFKEY_UNIT64(sizeof(m.key)
				   + PFKEY_ALIGN8(p_key_enc_len));
		m.key.sadb_key_exttype = SADB_EXT_KEY_ENCRYPT;
		m.key.sadb_key_bits = p_key_enc_len * 8;
		m.key.sadb_key_reserved = 0;

		setvarbuf(buf, &l, &m.ext, sizeof(m.key),
			p_key_enc, p_key_enc_len);
	}

	/* set authentication algorithm, if present. */
	if (p_key_auth) {
		union {
			struct sadb_key key;
			struct sadb_ext ext;
		} m;

		m.key.sadb_key_len =
			PFKEY_UNIT64(sizeof(m.key)
				   + PFKEY_ALIGN8(p_key_auth_len));
		m.key.sadb_key_exttype = SADB_EXT_KEY_AUTH;
		m.key.sadb_key_bits = p_key_auth_len * 8;
		m.key.sadb_key_reserved = 0;

		setvarbuf(buf, &l, &m.ext, sizeof(m.key),
			p_key_auth, p_key_auth_len);
	}

	/* set lifetime for HARD */
	if (p_lt_hard != 0 || p_lb_hard != 0) {
		struct sadb_lifetime m_lt;
		u_int slen = sizeof(struct sadb_lifetime);

		m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
		m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
		m_lt.sadb_lifetime_allocations = 0;
		m_lt.sadb_lifetime_bytes = p_lb_hard;
		m_lt.sadb_lifetime_addtime = p_lt_hard;
		m_lt.sadb_lifetime_usetime = 0;

		memcpy(buf + l, &m_lt, slen);
		l += slen;
	}

	/* set lifetime for SOFT */
	if (p_lt_soft != 0 || p_lb_soft != 0) {
		struct sadb_lifetime m_lt;
		u_int slen = sizeof(struct sadb_lifetime);

		m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
		m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
		m_lt.sadb_lifetime_allocations = 0;
		m_lt.sadb_lifetime_bytes = p_lb_soft;
		m_lt.sadb_lifetime_addtime = p_lt_soft;
		m_lt.sadb_lifetime_usetime = 0;

		memcpy(buf + l, &m_lt, slen);
		l += slen;
	}

#ifdef SADB_X_EXT_SEC_CTX
	/* Add security context label */
	if (sec_ctx.doi) {
		struct sadb_x_sec_ctx m_sec_ctx;
		u_int slen = sizeof(struct sadb_x_sec_ctx);

		memset(&m_sec_ctx, 0, slen);

		m_sec_ctx.sadb_x_sec_len = PFKEY_UNIT64(slen +
					PFKEY_ALIGN8(sec_ctx.len));
		m_sec_ctx.sadb_x_sec_exttype = SADB_X_EXT_SEC_CTX;
		m_sec_ctx.sadb_x_ctx_len = sec_ctx.len; /* bytes */
		m_sec_ctx.sadb_x_ctx_doi = sec_ctx.doi;
		m_sec_ctx.sadb_x_ctx_alg = sec_ctx.alg;
		setvarbuf(buf, &l, (struct sadb_ext *)&m_sec_ctx, slen,
			  (caddr_t)sec_ctx.buf, sec_ctx.len); 
	}
#endif

	len = sizeof(struct sadb_sa);
	m_sa.sadb_sa_len = PFKEY_UNIT64(len);
	m_sa.sadb_sa_exttype = SADB_EXT_SA;
	m_sa.sadb_sa_spi = htonl(p_spi);
	m_sa.sadb_sa_replay = p_replay;
	m_sa.sadb_sa_state = 0;
	m_sa.sadb_sa_auth = p_alg_auth;
	m_sa.sadb_sa_encrypt = p_alg_enc;
	m_sa.sadb_sa_flags = p_ext;

	memcpy(buf + l, &m_sa, len);
	l += len;

	len = sizeof(struct sadb_x_sa2);
	m_sa2.sadb_x_sa2_len = PFKEY_UNIT64(len);
	m_sa2.sadb_x_sa2_exttype = SADB_X_EXT_SA2;
	m_sa2.sadb_x_sa2_mode = p_mode;
	m_sa2.sadb_x_sa2_reqid = p_reqid;

	memcpy(buf + l, &m_sa2, len);
	l += len;

#ifdef SADB_X_EXT_NAT_T_TYPE
	if (p_natt_type) {
		struct sadb_x_nat_t_type natt_type;

		len = sizeof(struct sadb_x_nat_t_type);
		memset(&natt_type, 0, len);
		natt_type.sadb_x_nat_t_type_len = PFKEY_UNIT64(len);
		natt_type.sadb_x_nat_t_type_exttype = SADB_X_EXT_NAT_T_TYPE;
		natt_type.sadb_x_nat_t_type_type = p_natt_type;

		memcpy(buf + l, &natt_type, len);
		l += len;

		if (p_natt_oa) {
			sa = p_natt_oa->ai_addr;
			switch (sa->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				return -1;
			}
			salen = sysdep_sa_len(sa);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_X_EXT_NAT_T_OA;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);
		}
	}
#endif

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), sa, salen);

#ifdef SADB_X_EXT_NAT_T_TYPE
			if (p_natt_type) {
				struct sadb_x_nat_t_port natt_port;

				/* NATT_SPORT */
				len = sizeof(struct sadb_x_nat_t_port);
				memset(&natt_port, 0, len);
				natt_port.sadb_x_nat_t_port_len = PFKEY_UNIT64(len);
				natt_port.sadb_x_nat_t_port_exttype =
					SADB_X_EXT_NAT_T_SPORT;
				natt_port.sadb_x_nat_t_port_port = htons(get_port(s));
				
				memcpy(buf + l, &natt_port, len);
				l += len;

				/* NATT_DPORT */
				natt_port.sadb_x_nat_t_port_exttype =
					SADB_X_EXT_NAT_T_DPORT;
				natt_port.sadb_x_nat_t_port_port = htons(get_port(d));
				
				memcpy(buf + l, &natt_port, len);
				l += len;
			}
#endif
			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

static struct addrinfo *
parse_addr(host, port)
	char *host;
	char *port;
{
	struct addrinfo hints, *res = NULL;
	int error;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = p_aifamily;
	hints.ai_socktype = SOCK_DGRAM;		/*dummy*/
	hints.ai_protocol = IPPROTO_UDP;	/*dummy*/
	hints.ai_flags = p_aiflags;
	error = getaddrinfo(host, port, &hints, &res);
	if (error != 0) {
		yyerror(gai_strerror(error));
		return NULL;
	}
	return res;
}

static int
fix_portstr(ulproto, spec, sport, dport)
	int ulproto;
	vchar_t *spec, *sport, *dport;
{
	char sp[16], dp[16];
	int a, b, c, d;
	unsigned long u;

	if (spec->buf == NULL)
		return 0;

	switch (ulproto) {
	case IPPROTO_ICMP:
	case IPPROTO_ICMPV6:
	case IPPROTO_MH:
		if (sscanf(spec->buf, "%d,%d", &a, &b) == 2) {
			sprintf(sp, "%d", a);
			sprintf(dp, "%d", b);
		} else if (sscanf(spec->buf, "%d", &a) == 1) {
			sprintf(sp, "%d", a);
		} else {
			yyerror("invalid an upper layer protocol spec");
			return -1;
		}
		break;
	case IPPROTO_GRE:
		if (sscanf(spec->buf, "%d.%d.%d.%d", &a, &b, &c, &d) == 4) {
			sprintf(sp, "%d", (a << 8) + b);
			sprintf(dp, "%d", (c << 8) + d);
		} else if (sscanf(spec->buf, "%lu", &u) == 1) {
			sprintf(sp, "%d", (int) (u >> 16));
			sprintf(dp, "%d", (int) (u & 0xffff));
		} else {
			yyerror("invalid an upper layer protocol spec");
			return -1;
		}
		break;
	}

	free(sport->buf);
	sport->buf = strdup(sp);
	if (!sport->buf) {
		yyerror("insufficient memory");
		return -1;
	}
	sport->len = strlen(sport->buf);

	free(dport->buf);
	dport->buf = strdup(dp);
	if (!dport->buf) {
		yyerror("insufficient memory");
		return -1;
	}
	dport->len = strlen(dport->buf);

	return 0;
}

static int
setvarbuf(buf, off, ebuf, elen, vbuf, vlen)
	char *buf;
	int *off;
	struct sadb_ext *ebuf;
	int elen;
	const void *vbuf;
	int vlen;
{
	memset(buf + *off, 0, PFKEY_UNUNIT64(ebuf->sadb_ext_len));
	memcpy(buf + *off, (caddr_t)ebuf, elen);
	memcpy(buf + *off + elen, vbuf, vlen);
	(*off) += PFKEY_ALIGN8(elen + vlen);

	return 0;
}

void
parse_init()
{
	p_spi = 0;

	p_ext = SADB_X_EXT_CYCSEQ;
	p_alg_enc = SADB_EALG_NONE;
	p_alg_auth = SADB_AALG_NONE;
	p_mode = IPSEC_MODE_ANY;
	p_reqid = 0;
	p_replay = 0;
	p_key_enc_len = p_key_auth_len = 0;
	p_key_enc = p_key_auth = 0;
	p_lt_hard = p_lt_soft = 0;
	p_lb_hard = p_lb_soft = 0;

	memset(&sec_ctx, 0, sizeof(struct security_ctx));

	p_aiflags = 0;
	p_aifamily = PF_UNSPEC;

	/* Clear out any natt OA information */
	if (p_natt_oa)
		freeaddrinfo (p_natt_oa);
	p_natt_oa = NULL;
	p_natt_type = 0;

	return;
}

void
free_buffer()
{
	/* we got tons of memory leaks in the parser anyways, leave them */

	return;
}
#line 1353 "setkeyyy.tab.c"

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
case 2:
#line 165 "../../ipsec-tools/src/setkey/parse.y"
	{
			free_buffer();
			parse_init();
		}
break;
case 15:
#line 190 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;

			status = setkeymsg_add(SADB_ADD, yystack.l_mark[-4].num, yystack.l_mark[-6].res, yystack.l_mark[-5].res);
			if (status < 0)
				return -1;
		}
break;
case 16:
#line 202 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;

			if (yystack.l_mark[-5].res->ai_next || yystack.l_mark[-4].res->ai_next) {
				yyerror("multiple address specified");
				return -1;
			}
			if (p_mode != IPSEC_MODE_ANY)
				yyerror("WARNING: mode is obsolete");

			status = setkeymsg_addr(SADB_DELETE, yystack.l_mark[-3].num, yystack.l_mark[-5].res, yystack.l_mark[-4].res, 0);
			if (status < 0)
				return -1;
		}
break;
case 17:
#line 221 "../../ipsec-tools/src/setkey/parse.y"
	{
#ifndef __linux__
			if (setkeymsg_addr(SADB_DELETE, yystack.l_mark[-1].num, yystack.l_mark[-3].res, yystack.l_mark[-2].res, 1) < 0)
				return -1;
#else /* __linux__ */
			/* linux strictly adheres to RFC2367, and returns
			 * an error if we send an SADB_DELETE request without
			 * an SPI. Therefore, we must first retrieve a list
			 * of SPIs for all matching SADB entries, and then
			 * delete each one separately. */
			u_int32_t *spi;
			int i, n;

			spi = sendkeymsg_spigrep(yystack.l_mark[-1].num, yystack.l_mark[-3].res, yystack.l_mark[-2].res, &n);
			for (i = 0; i < n; i++) {
				p_spi = spi[i];
				if (setkeymsg_addr(SADB_DELETE,
							yystack.l_mark[-1].num, yystack.l_mark[-3].res, yystack.l_mark[-2].res, 0) < 0)
					return -1;
			}
			free(spi);
#endif /* __linux__ */
		}
break;
case 18:
#line 249 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;

			if (p_mode != IPSEC_MODE_ANY)
				yyerror("WARNING: mode is obsolete");

			status = setkeymsg_addr(SADB_GET, yystack.l_mark[-3].num, yystack.l_mark[-5].res, yystack.l_mark[-4].res, 0);
			if (status < 0)
				return -1;
		}
break;
case 19:
#line 264 "../../ipsec-tools/src/setkey/parse.y"
	{
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_FLUSH, yystack.l_mark[-1].num, sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
break;
case 20:
#line 274 "../../ipsec-tools/src/setkey/parse.y"
	{
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_DUMP, yystack.l_mark[-1].num, sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
break;
case 21:
#line 283 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_SATYPE_UNSPEC;
		}
break;
case 22:
#line 287 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_SATYPE_ESP;
			if (yystack.l_mark[0].num == 1)
				p_ext |= SADB_X_EXT_OLD;
			else
				p_ext &= ~SADB_X_EXT_OLD;
		}
break;
case 23:
#line 295 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_SATYPE_AH;
			if (yystack.l_mark[0].num == 1)
				p_ext |= SADB_X_EXT_OLD;
			else
				p_ext &= ~SADB_X_EXT_OLD;
		}
break;
case 24:
#line 303 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_X_SATYPE_IPCOMP;
		}
break;
case 25:
#line 307 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_SATYPE_ESP;
			p_ext &= ~SADB_X_EXT_OLD;
			p_natt_oa = 0;
			p_natt_type = UDP_ENCAP_ESPINUDP;
		}
break;
case 26:
#line 314 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.num = SADB_SATYPE_ESP;
			p_ext &= ~SADB_X_EXT_OLD;
			p_natt_oa = yystack.l_mark[0].res;
			p_natt_type = UDP_ENCAP_ESPINUDP;
		}
break;
case 27:
#line 321 "../../ipsec-tools/src/setkey/parse.y"
	{
#ifdef SADB_X_SATYPE_TCPSIGNATURE
			yyval.num = SADB_X_SATYPE_TCPSIGNATURE;
#endif
		}
break;
case 28:
#line 329 "../../ipsec-tools/src/setkey/parse.y"
	{ p_spi = yystack.l_mark[0].ulnum; }
break;
case 29:
#line 331 "../../ipsec-tools/src/setkey/parse.y"
	{
			char *ep;
			unsigned long v;

			ep = NULL;
			v = strtoul(yystack.l_mark[0].val.buf, &ep, 16);
			if (!ep || *ep) {
				yyerror("invalid SPI");
				return -1;
			}
			if (v & ~0xffffffff) {
				yyerror("SPI too big.");
				return -1;
			}

			p_spi = v;
		}
break;
case 36:
#line 367 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[0].num;
		}
break;
case 37:
#line 375 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[-1].num;
			p_ext |= SADB_X_EXT_RAWCPI;
		}
break;
case 38:
#line 386 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[0].num;

			p_key_enc_len = 0;
			p_key_enc = "";
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
break;
case 39:
#line 401 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[-1].num;

			p_key_enc_len = yystack.l_mark[0].val.len;
			p_key_enc = yystack.l_mark[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
break;
case 40:
#line 416 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			yyerror("WARNING: obsolete algorithm");
			p_alg_enc = yystack.l_mark[0].num;

			p_key_enc_len = 0;
			p_key_enc = "";
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
break;
case 41:
#line 433 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[-1].num;
			if (p_ext & SADB_X_EXT_OLD) {
				yyerror("algorithm mismatched");
				return -1;
			}
			p_ext |= SADB_X_EXT_DERIV;

			p_key_enc_len = yystack.l_mark[0].val.len;
			p_key_enc = yystack.l_mark[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
break;
case 42:
#line 454 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yystack.l_mark[-1].num;
			if (!(p_ext & SADB_X_EXT_OLD)) {
				yyerror("algorithm mismatched");
				return -1;
			}
			p_ext |= SADB_X_EXT_IV4B;

			p_key_enc_len = yystack.l_mark[0].val.len;
			p_key_enc = yystack.l_mark[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
break;
case 43:
#line 477 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_auth = yystack.l_mark[-1].num;

			p_key_auth_len = yystack.l_mark[0].val.len;
			p_key_auth = yystack.l_mark[0].val.buf;
#ifdef SADB_X_AALG_TCP_MD5
			if (p_alg_auth == SADB_X_AALG_TCP_MD5) {
				if ((p_key_auth_len < 1) || 
				    (p_key_auth_len > 80))
					return -1;
			} else 
#endif
			{
				if (ipsec_check_keylen(SADB_EXT_SUPPORTED_AUTH,
				    p_alg_auth, 
				    PFKEY_UNUNIT64(p_key_auth_len)) < 0) {
					yyerror(ipsec_strerror());
					return -1;
				}
			}
		}
break;
case 44:
#line 502 "../../ipsec-tools/src/setkey/parse.y"
	{
			if (yystack.l_mark[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_auth = yystack.l_mark[0].num;

			p_key_auth_len = 0;
			p_key_auth = NULL;
		}
break;
case 45:
#line 516 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val = yystack.l_mark[0].val;
		}
break;
case 46:
#line 520 "../../ipsec-tools/src/setkey/parse.y"
	{
			caddr_t pp_key;
			caddr_t bp;
			caddr_t yp = yystack.l_mark[0].val.buf;
			int l;

			l = strlen(yp) % 2 + strlen(yp) / 2;
			if ((pp_key = malloc(l)) == 0) {
				yyerror("not enough core");
				return -1;
			}
			memset(pp_key, 0, l);

			bp = pp_key;
			if (strlen(yp) % 2) {
				*bp = ATOX(yp[0]);
				yp++, bp++;
			}
			while (*yp) {
				*bp = (ATOX(yp[0]) << 4) | ATOX(yp[1]);
				yp += 2, bp++;
			}

			yyval.val.len = l;
			yyval.val.buf = pp_key;
		}
break;
case 49:
#line 554 "../../ipsec-tools/src/setkey/parse.y"
	{ p_ext |= yystack.l_mark[0].num; }
break;
case 50:
#line 555 "../../ipsec-tools/src/setkey/parse.y"
	{ p_ext &= ~SADB_X_EXT_CYCSEQ; }
break;
case 51:
#line 556 "../../ipsec-tools/src/setkey/parse.y"
	{ p_mode = yystack.l_mark[0].num; }
break;
case 52:
#line 557 "../../ipsec-tools/src/setkey/parse.y"
	{ p_mode = IPSEC_MODE_ANY; }
break;
case 53:
#line 558 "../../ipsec-tools/src/setkey/parse.y"
	{ p_reqid = yystack.l_mark[0].ulnum; }
break;
case 54:
#line 560 "../../ipsec-tools/src/setkey/parse.y"
	{
			if ((p_ext & SADB_X_EXT_OLD) != 0) {
				yyerror("replay prevention cannot be used with "
				    "ah/esp-old");
				return -1;
			}
			p_replay = yystack.l_mark[0].ulnum;
		}
break;
case 55:
#line 568 "../../ipsec-tools/src/setkey/parse.y"
	{ p_lt_hard = yystack.l_mark[0].ulnum; }
break;
case 56:
#line 569 "../../ipsec-tools/src/setkey/parse.y"
	{ p_lt_soft = yystack.l_mark[0].ulnum; }
break;
case 57:
#line 570 "../../ipsec-tools/src/setkey/parse.y"
	{ p_lb_hard = yystack.l_mark[0].ulnum; }
break;
case 58:
#line 571 "../../ipsec-tools/src/setkey/parse.y"
	{ p_lb_soft = yystack.l_mark[0].ulnum; }
break;
case 59:
#line 572 "../../ipsec-tools/src/setkey/parse.y"
	{
		sec_ctx.doi = yystack.l_mark[-2].ulnum;
		sec_ctx.alg = yystack.l_mark[-1].ulnum;
		sec_ctx.len = yystack.l_mark[0].val.len+1;
		sec_ctx.buf = yystack.l_mark[0].val.buf;
	}
break;
case 60:
#line 585 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;
			struct addrinfo *src, *dst;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			last_msg_type = SADB_X_SPDADD;
#endif

			/* fixed port fields if ulp is icmp */
			if (fix_portstr(yystack.l_mark[-4].num, &yystack.l_mark[-3].val, &yystack.l_mark[-8].val, &yystack.l_mark[-5].val))
				return -1;

			src = parse_addr(yystack.l_mark[-10].val.buf, yystack.l_mark[-8].val.buf);
			dst = parse_addr(yystack.l_mark[-7].val.buf, yystack.l_mark[-5].val.buf);
			if (!src || !dst) {
				/* yyerror is already called */
				return -1;
			}
			if (src->ai_next || dst->ai_next) {
				yyerror("multiple address specified");
				freeaddrinfo(src);
				freeaddrinfo(dst);
				return -1;
			}

			status = setkeymsg_spdaddr(SADB_X_SPDADD, yystack.l_mark[-4].num, &yystack.l_mark[-1].val,
			    src, yystack.l_mark[-9].num, dst, yystack.l_mark[-6].num);
			freeaddrinfo(src);
			freeaddrinfo(dst);
			if (status < 0)
				return -1;
		}
break;
case 61:
#line 618 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;

			status = setkeymsg_spdaddr_tag(SADB_X_SPDADD,
			    yystack.l_mark[-2].val.buf, &yystack.l_mark[-1].val);
			if (status < 0)
				return -1;
		}
break;
case 62:
#line 631 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;
			struct addrinfo *src, *dst;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			last_msg_type = SADB_X_SPDUPDATE;
#endif

			/* fixed port fields if ulp is icmp */
			if (fix_portstr(yystack.l_mark[-4].num, &yystack.l_mark[-3].val, &yystack.l_mark[-8].val, &yystack.l_mark[-5].val))
				return -1;

			src = parse_addr(yystack.l_mark[-10].val.buf, yystack.l_mark[-8].val.buf);
			dst = parse_addr(yystack.l_mark[-7].val.buf, yystack.l_mark[-5].val.buf);
			if (!src || !dst) {
				/* yyerror is already called */
				return -1;
			}
			if (src->ai_next || dst->ai_next) {
				yyerror("multiple address specified");
				freeaddrinfo(src);
				freeaddrinfo(dst);
				return -1;
			}

			status = setkeymsg_spdaddr(SADB_X_SPDUPDATE, yystack.l_mark[-4].num, &yystack.l_mark[-1].val,
			    src, yystack.l_mark[-9].num, dst, yystack.l_mark[-6].num);
			freeaddrinfo(src);
			freeaddrinfo(dst);
			if (status < 0)
				return -1;
		}
break;
case 63:
#line 664 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;

			status = setkeymsg_spdaddr_tag(SADB_X_SPDUPDATE,
			    yystack.l_mark[-2].val.buf, &yystack.l_mark[-1].val);
			if (status < 0)
				return -1;
		}
break;
case 64:
#line 676 "../../ipsec-tools/src/setkey/parse.y"
	{
			int status;
			struct addrinfo *src, *dst;

			/* fixed port fields if ulp is icmp */
			if (fix_portstr(yystack.l_mark[-4].num, &yystack.l_mark[-3].val, &yystack.l_mark[-8].val, &yystack.l_mark[-5].val))
				return -1;

			src = parse_addr(yystack.l_mark[-10].val.buf, yystack.l_mark[-8].val.buf);
			dst = parse_addr(yystack.l_mark[-7].val.buf, yystack.l_mark[-5].val.buf);
			if (!src || !dst) {
				/* yyerror is already called */
				return -1;
			}
			if (src->ai_next || dst->ai_next) {
				yyerror("multiple address specified");
				freeaddrinfo(src);
				freeaddrinfo(dst);
				return -1;
			}

			status = setkeymsg_spdaddr(SADB_X_SPDDELETE, yystack.l_mark[-4].num, &yystack.l_mark[-1].val,
			    src, yystack.l_mark[-9].num, dst, yystack.l_mark[-6].num);
			freeaddrinfo(src);
			freeaddrinfo(dst);
			if (status < 0)
				return -1;
		}
break;
case 65:
#line 708 "../../ipsec-tools/src/setkey/parse.y"
	{
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_X_SPDDUMP, SADB_SATYPE_UNSPEC,
			    sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
break;
case 66:
#line 719 "../../ipsec-tools/src/setkey/parse.y"
	{
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_X_SPDFLUSH, SADB_SATYPE_UNSPEC,
			    sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
break;
case 69:
#line 734 "../../ipsec-tools/src/setkey/parse.y"
	{
			char *p;

			for (p = yystack.l_mark[0].val.buf + 1; *p; p++)
				switch (*p) {
				case '4':
					p_aifamily = AF_INET;
					break;
#ifdef INET6
				case '6':
					p_aifamily = AF_INET6;
					break;
#endif
				case 'n':
					p_aiflags = AI_NUMERICHOST;
					break;
				default:
					yyerror("invalid flag");
					return -1;
				}
		}
break;
case 70:
#line 759 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.res = parse_addr(yystack.l_mark[0].val.buf, NULL);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
break;
case 71:
#line 770 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.res = parse_addr(yystack.l_mark[0].val.buf, NULL);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
break;
case 72:
#line 778 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.res = parse_addr(yystack.l_mark[-1].val.buf, yystack.l_mark[0].val.buf);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
break;
case 73:
#line 788 "../../ipsec-tools/src/setkey/parse.y"
	{ yyval.num = -1; }
break;
case 74:
#line 789 "../../ipsec-tools/src/setkey/parse.y"
	{ yyval.num = yystack.l_mark[0].ulnum; }
break;
case 75:
#line 794 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val.buf = strdup("0");
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
break;
case 76:
#line 803 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val.buf = strdup("0");
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
break;
case 77:
#line 812 "../../ipsec-tools/src/setkey/parse.y"
	{
			char buf[20];
			snprintf(buf, sizeof(buf), "%lu", yystack.l_mark[-1].ulnum);
			yyval.val.buf = strdup(buf);
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
break;
case 78:
#line 823 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val = yystack.l_mark[-1].val;
		}
break;
case 79:
#line 829 "../../ipsec-tools/src/setkey/parse.y"
	{ yyval.num = yystack.l_mark[0].ulnum; }
break;
case 80:
#line 830 "../../ipsec-tools/src/setkey/parse.y"
	{ yyval.num = IPSEC_ULPROTO_ANY; }
break;
case 81:
#line 831 "../../ipsec-tools/src/setkey/parse.y"
	{ 
				yyval.num = IPPROTO_TCP; 
			}
break;
case 82:
#line 835 "../../ipsec-tools/src/setkey/parse.y"
	{
			struct protoent *ent;

			ent = getprotobyname(yystack.l_mark[0].val.buf);
			if (ent)
				yyval.num = ent->p_proto;
			else {
				if (strcmp("icmp6", yystack.l_mark[0].val.buf) == 0) {
					yyval.num = IPPROTO_ICMPV6;
				} else if(strcmp("ip4", yystack.l_mark[0].val.buf) == 0) {
					yyval.num = IPPROTO_IPV4;
				} else {
					yyerror("invalid upper layer protocol");
					return -1;
				}
			}
			endprotoent();
		}
break;
case 83:
#line 857 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val.buf = NULL;
			yyval.val.len = 0;
		}
break;
case 84:
#line 862 "../../ipsec-tools/src/setkey/parse.y"
	{
			yyval.val.buf = strdup(yystack.l_mark[0].val.buf);
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
break;
case 86:
#line 874 "../../ipsec-tools/src/setkey/parse.y"
	{
			sec_ctx.doi = yystack.l_mark[-2].ulnum;
			sec_ctx.alg = yystack.l_mark[-1].ulnum;
			sec_ctx.len = yystack.l_mark[0].val.len+1;
			sec_ctx.buf = yystack.l_mark[0].val.buf;
		}
break;
case 87:
#line 884 "../../ipsec-tools/src/setkey/parse.y"
	{
			char *policy;
#ifdef HAVE_PFKEY_POLICY_PRIORITY
			struct sadb_x_policy *xpl;
#endif

			policy = ipsec_set_policy(yystack.l_mark[0].val.buf, yystack.l_mark[0].val.len);
			if (policy == NULL) {
				yyerror(ipsec_strerror());
				return -1;
			}

			yyval.val.buf = policy;
			yyval.val.len = ipsec_get_policylen(policy);

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			xpl = (struct sadb_x_policy *) yyval.val.buf;
			last_priority = xpl->sadb_x_policy_priority;
#endif
		}
break;
case 88:
#line 907 "../../ipsec-tools/src/setkey/parse.y"
	{ yyval.val = yystack.l_mark[0].val; }
break;
case 89:
#line 913 "../../ipsec-tools/src/setkey/parse.y"
	{
			exit_now = 1;
			YYACCEPT;
		}
break;
#line 2317 "setkeyyy.tab.c"
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
