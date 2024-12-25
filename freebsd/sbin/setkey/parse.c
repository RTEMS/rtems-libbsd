/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar    1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20230201

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

#line 35 "../../freebsd/sbin/setkey/parse.y"
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <net/route.h>
#include <netinet/in.h>
#include <net/pfkeyv2.h>
#include <netipsec/key_var.h>
#include <netipsec/ipsec.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>

#include "libpfkey.h"
#include "vchar.h"

#define ATOX(c) \
  (isdigit(c) ? (c - '0') : (isupper(c) ? (c - 'A' + 10) : (c - 'a' + 10)))

#ifndef __rtems__
u_int32_t p_spi;
u_int p_ext, p_alg_enc, p_alg_auth, p_replay, p_mode;
u_int32_t p_reqid;
u_int p_key_enc_len, p_key_auth_len;
caddr_t p_key_enc, p_key_auth;
time_t p_lt_hard, p_lt_soft;
u_int p_natt_type;
struct addrinfo *p_natt_oai, *p_natt_oar;
int p_natt_sport, p_natt_dport;
int p_natt_fraglen;
bool esn;
vchar_t p_hwif;
#else /* __rtems__ */
static u_int32_t p_spi;
static u_int p_ext, p_alg_enc, p_alg_auth, p_replay, p_mode;
static u_int32_t p_reqid;
static u_int p_key_enc_len, p_key_auth_len;
static caddr_t p_key_enc, p_key_auth;
static time_t p_lt_hard, p_lt_soft;
static u_int p_natt_type;
static struct addrinfo *p_natt_oai, *p_natt_oar;
static int p_natt_sport, p_natt_dport;
static int p_natt_fraglen;
static bool esn;
static vchar_t p_hwif;
#endif /* __rtems__ */

static int p_aiflags = 0, p_aifamily = PF_UNSPEC;

static struct addrinfo *parse_addr(char *, char *);
static int fix_portstr(vchar_t *, vchar_t *, vchar_t *);
static int setvarbuf(char *, int *, struct sadb_ext *, int, caddr_t, int);
void parse_init(void);
void free_buffer(void);

int setkeymsg0(struct sadb_msg *, unsigned int, unsigned int, size_t);
static int setkeymsg_spdaddr(unsigned int, unsigned int, vchar_t *,
        struct addrinfo *, int, struct addrinfo *, int);
static int setkeymsg_addr(unsigned int, unsigned int,
        struct addrinfo *, struct addrinfo *, int);
static int setkeymsg_add(unsigned int, unsigned int,
        struct addrinfo *, struct addrinfo *);
extern int setkeymsg(char *, size_t *);
extern int sendkeymsg(char *, size_t);

extern int yylex(void);
extern void yyfatal(const char *);
extern void yyerror(const char *);
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 113 "../../freebsd/sbin/setkey/parse.y"
typedef union {
        int num;
        unsigned long ulnum;
        vchar_t val;
        struct addrinfo *res;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 192 "setkeyyy.tab.c"

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
#define PR_ESP 267
#define PR_AH 268
#define PR_IPCOMP 269
#define PR_TCP 270
#define F_PROTOCOL 271
#define F_AUTH 272
#define F_ENC 273
#define F_REPLAY 274
#define F_COMP 275
#define F_RAWCPI 276
#define F_MODE 277
#define MODE 278
#define F_REQID 279
#define F_EXT 280
#define EXTENSION 281
#define NOCYCLICSEQ 282
#define ALG_AUTH 283
#define ALG_AUTH_NOKEY 284
#define ALG_ENC 285
#define ALG_ENC_NOKEY 286
#define ALG_ENC_DESDERIV 287
#define ALG_ENC_DES32IV 288
#define ALG_ENC_OLD 289
#define ALG_ENC_SALT 290
#define ALG_COMP 291
#define F_LIFETIME_HARD 292
#define F_LIFETIME_SOFT 293
#define DECSTRING 294
#define QUOTEDSTRING 295
#define HEXSTRING 296
#define STRING 297
#define ANY 298
#define SPDADD 299
#define SPDDELETE 300
#define SPDDUMP 301
#define SPDFLUSH 302
#define F_POLICY 303
#define PL_REQUESTS 304
#define F_AIFLAGS 305
#define F_NATT 306
#define F_NATT_MTU 307
#define F_ESN 308
#define F_HWIF 309
#define TAGGED 310
#define YYERRCODE 256
typedef int YYINT;
static const YYINT setkeyyylhs[] = {                     -1,
    0,    0,   10,   10,   10,   10,   10,   10,   10,   10,
   10,   10,   11,   13,   14,   12,   15,   16,    2,    2,
    2,    2,    2,   22,   22,   24,   24,   24,   25,   25,
   26,   27,   27,   28,   28,   28,   28,   28,   28,   29,
   29,    5,    5,   23,   23,   30,   30,   30,   30,   30,
   30,   30,   30,   30,   30,   30,   30,   17,   17,   18,
   19,   20,   21,   21,   31,   31,   32,    9,    1,    1,
    4,    4,    4,    4,    3,    3,    3,    3,    3,    7,
    7,    8,    6,
};
static const YYINT setkeyyylen[] = {                      2,
    0,    2,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    9,    8,    6,    8,    3,    3,    0,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    4,    2,
    2,    2,    3,    1,    2,    1,    2,    2,    2,    2,
    1,    1,    1,    0,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    9,    2,    1,    2,   13,    5,   12,
    2,    2,    0,    2,    0,    2,    1,    1,    0,    2,
    0,    3,    3,    3,    1,    1,    1,    1,    1,    0,
    1,    2,    1,
};
static const YYINT setkeyyydefred[] = {                   1,
    0,   63,   63,   63,   63,    0,    0,    0,   63,    0,
    0,    2,    3,    4,    5,    6,    7,    8,    9,   10,
   11,   12,    0,    0,    0,    0,   20,   21,   22,   23,
    0,    0,    0,    0,    0,   61,   62,   68,   67,    0,
   64,    0,    0,    0,   17,   18,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   83,   82,   59,   70,    0,    0,    0,   24,
   25,   44,   44,   44,   15,    0,    0,    0,    0,    0,
    0,    0,    0,   73,   74,   72,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   56,
    0,    0,   26,   27,   28,   45,   16,   14,    0,    0,
    0,   41,   31,    0,   34,    0,    0,   36,    0,    0,
   51,    0,   48,   49,   50,   46,   47,   52,   53,    0,
   55,   57,   13,   78,   77,   75,   79,   76,    0,    0,
   42,   43,   40,   35,   37,   38,   39,    0,   33,    0,
   81,    0,    0,   29,    0,    0,    0,    0,    0,    0,
   60,    0,   66,   58,    0,    0,   54,
};
static const YYINT setkeyyydgoto[] = {                    1,
   57,   31,  139,   68,  143,   64,  152,   55,   40,   12,
   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
   23,   72,   81,  102,  103,  104,  105,  120,  113,  106,
  160,   41,
};
static const YYINT setkeyyysindex[] = {                   0,
 -229,    0,    0,    0,    0, -141, -141, -304,    0, -240,
 -207,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -292, -292, -292, -292,    0,    0,    0,    0,
 -167, -157, -230, -282, -221,    0,    0,    0,    0, -203,
    0, -203, -203, -203,    0,    0, -200, -140, -140, -141,
 -141, -141, -141, -181, -116, -152, -115, -115, -284, -284,
 -284, -114,    0,    0,    0,    0, -173, -150, -149,    0,
    0,    0,    0,    0,    0, -111, -110, -109, -140, -140,
 -211, -253, -249,    0,    0,    0, -115, -115, -198, -174,
 -148, -146, -269, -142, -151, -139, -138, -203, -137,    0,
 -144, -103,    0,    0,    0,    0,    0,    0, -256, -256,
 -163,    0,    0, -163,    0, -163, -163,    0, -163, -113,
    0, -118,    0,    0,    0,    0,    0,    0,    0,  -99,
    0,    0,    0,    0,    0,    0,    0,    0, -136, -136,
    0,    0,    0,    0,    0,    0,    0, -198,    0, -132,
    0, -200, -200,    0,  -97, -145,  -92, -203, -131,  -90,
    0,  -91,    0,    0, -125,  -89,    0,
};
static const YYINT setkeyyyrindex[] = {                   0,
    0,    0,    0,    0,    0,  -87,  -87, -214,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -252, -252, -195,
 -195, -195,  -87,    0,    0,    0, -124, -124,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -192, -192,
    0,    0,    0,    0,    0,    0, -190, -190,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -85,
    0,  -83,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -128, -128,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -116,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT setkeyyygindex[] = {                   0,
   30,   -4,   67,  -36,    3,    0,   36,  -17,  -24,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   84,   77,   66,    0,    0,    0,    0,    0,   31,    0,
    0,    0,
};
#define YYTABLESIZE 179
static const YYINT setkeyyytable[] = {                   42,
   43,   44,   32,  107,   38,   33,   69,  108,  123,   70,
  134,   71,   39,  135,   48,   50,   36,   51,   52,   53,
   91,   69,   39,   93,   91,   94,   95,   93,  124,   94,
   95,    2,    3,    4,    5,    6,    7,  136,   96,   97,
  137,  138,   96,   97,   69,   59,   60,   61,   62,   37,
  109,  110,   98,   99,  100,  101,   98,   99,  100,  101,
   89,   90,   91,   92,   47,   93,   69,   94,   95,    8,
    9,   10,   11,  130,   69,   49,   71,   69,   58,   71,
   96,   97,   63,   39,  111,  112,   24,   25,   26,   45,
   63,   34,   35,   38,   98,   99,  100,  101,   19,   46,
   19,   69,   54,   71,   69,   69,   71,   71,   87,   88,
  114,  115,  116,  117,  118,  119,  144,   56,  145,  146,
   76,  147,   63,   77,   78,   27,   28,   29,   30,  126,
  127,  141,  142,  162,  156,  157,   73,   74,   82,   83,
   65,   66,   75,   67,  122,  121,   79,   80,   84,   85,
   86,  125,  132,  133,  128,  129,  131,  149,  148,  150,
  151,  155,  158,  159,  161,  163,  164,  165,  166,   19,
  167,   30,   71,   32,   80,  153,  140,    0,  154,
};
static const YYINT setkeyyycheck[] = {                   24,
   25,   26,    7,  257,  297,  310,  259,  257,  278,  294,
  267,  296,  305,  270,  297,   40,  257,   42,   43,   44,
  274,   58,  305,  277,  274,  279,  280,  277,  298,  279,
  280,  261,  262,  263,  264,  265,  266,  294,  292,  293,
  297,  298,  292,  293,  297,   50,   51,   52,   53,  257,
   87,   88,  306,  307,  308,  309,  306,  307,  308,  309,
  272,  273,  274,  275,  295,  277,  259,  279,  280,  299,
  300,  301,  302,   98,  267,  297,  267,  270,   49,  270,
  292,  293,  297,  305,  283,  284,    3,    4,    5,  257,
  305,    8,    9,  297,  306,  307,  308,  309,  294,  257,
  296,  294,  303,  294,  297,  298,  297,  298,   79,   80,
  285,  286,  287,  288,  289,  290,  114,  258,  116,  117,
  294,  119,  304,  297,  298,  267,  268,  269,  270,  281,
  282,  295,  296,  158,  152,  153,   60,   61,   73,   74,
  257,  294,  257,  259,  291,  294,  297,  297,  260,  260,
  260,  294,  297,  257,  294,  294,  294,  276,  272,  259,
  297,  294,  260,  309,  257,  297,  257,  259,  294,  257,
  260,  257,  297,  257,  303,  140,  110,   -1,  148,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 310
#define YYUNDFTOKEN 345
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
"GET","DELETE","DELETEALL","FLUSH","DUMP","PR_ESP","PR_AH","PR_IPCOMP","PR_TCP",
"F_PROTOCOL","F_AUTH","F_ENC","F_REPLAY","F_COMP","F_RAWCPI","F_MODE","MODE",
"F_REQID","F_EXT","EXTENSION","NOCYCLICSEQ","ALG_AUTH","ALG_AUTH_NOKEY",
"ALG_ENC","ALG_ENC_NOKEY","ALG_ENC_DESDERIV","ALG_ENC_DES32IV","ALG_ENC_OLD",
"ALG_ENC_SALT","ALG_COMP","F_LIFETIME_HARD","F_LIFETIME_SOFT","DECSTRING",
"QUOTEDSTRING","HEXSTRING","STRING","ANY","SPDADD","SPDDELETE","SPDDUMP",
"SPDFLUSH","F_POLICY","PL_REQUESTS","F_AIFLAGS","F_NATT","F_NATT_MTU","F_ESN",
"F_HWIF","TAGGED",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"illegal-symbol",
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
"command : spdadd_command",
"command : spddelete_command",
"command : spddump_command",
"command : spdflush_command",
"add_command : ADD ipaddropts ipaddr ipaddr protocol_spec spi extension_spec algorithm_spec EOT",
"delete_command : DELETE ipaddropts ipaddr ipaddr protocol_spec spi extension_spec EOT",
"deleteall_command : DELETEALL ipaddropts ipaddr ipaddr protocol_spec EOT",
"get_command : GET ipaddropts ipaddr ipaddr protocol_spec spi extension_spec EOT",
"flush_command : FLUSH protocol_spec EOT",
"dump_command : DUMP protocol_spec EOT",
"protocol_spec :",
"protocol_spec : PR_ESP",
"protocol_spec : PR_AH",
"protocol_spec : PR_IPCOMP",
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
"enc_alg : ALG_ENC_SALT key_string",
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
"extension : F_NATT ipaddr BLCL DECSTRING ELCL ipaddr BLCL DECSTRING ELCL",
"extension : F_NATT_MTU DECSTRING",
"extension : F_ESN",
"extension : F_HWIF STRING",
"spdadd_command : SPDADD ipaddropts STRING prefix portstr STRING prefix portstr upper_spec upper_misc_spec policy_spec spd_hwif EOT",
"spdadd_command : SPDADD TAGGED QUOTEDSTRING policy_spec EOT",
"spddelete_command : SPDDELETE ipaddropts STRING prefix portstr STRING prefix portstr upper_spec upper_misc_spec policy_spec EOT",
"spddump_command : SPDDUMP EOT",
"spdflush_command : SPDFLUSH EOT",
"ipaddropts :",
"ipaddropts : ipaddropts ipaddropt",
"spd_hwif :",
"spd_hwif : F_HWIF STRING",
"ipaddropt : F_AIFLAGS",
"ipaddr : STRING",
"prefix :",
"prefix : SLASH DECSTRING",
"portstr :",
"portstr : BLCL ANY ELCL",
"portstr : BLCL DECSTRING ELCL",
"portstr : BLCL STRING ELCL",
"upper_spec : DECSTRING",
"upper_spec : ANY",
"upper_spec : PR_TCP",
"upper_spec : PR_ESP",
"upper_spec : STRING",
"upper_misc_spec :",
"upper_misc_spec : STRING",
"policy_spec : F_POLICY policy_requests",
"policy_requests : PL_REQUESTS",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

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
#line 828 "../../freebsd/sbin/setkey/parse.y"

int
setkeymsg0(struct sadb_msg *msg, unsigned type, unsigned satype, size_t l)
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

static int
setkeymsg_plen(struct addrinfo *s)
{
        switch (s->ai_addr->sa_family) {
#ifdef INET
        case AF_INET:
                return (sizeof(struct in_addr) << 3);
#endif
#ifdef INET6
        case AF_INET6:
                return (sizeof(struct in6_addr) << 3);
#endif
        default:
                return (-1);
        }
}

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_spdaddr(unsigned type, unsigned upper, vchar_t *policy,
    struct addrinfo *srcs, int splen, struct addrinfo *dsts, int dplen)
{
        struct sadb_msg *msg;
        char buf[BUFSIZ];
        int l, l0;
        struct sadb_address m_addr;
        struct sadb_x_if_hw_offl m_if_hw;
        struct addrinfo *s, *d;
        int n;
        int plen;
        struct sockaddr *sa;
        int salen;

        msg = (struct sadb_msg *)buf;

        if (!srcs || !dsts)
                return -1;

        /* fix up length afterwards */
        setkeymsg0(msg, type, SADB_SATYPE_UNSPEC, 0);
        l = sizeof(struct sadb_msg);

        memcpy(buf + l, policy->buf, policy->len);
        l += policy->len;

        if (p_hwif.len != 0) {
                l0 = sizeof(struct sadb_x_if_hw_offl);
                m_if_hw.sadb_x_if_hw_offl_len = PFKEY_UNIT64(l0);
                m_if_hw.sadb_x_if_hw_offl_exttype = SADB_X_EXT_IF_HW_OFFL;
                m_if_hw.sadb_x_if_hw_offl_flags = 0;
                memset(&m_if_hw.sadb_x_if_hw_offl_if[0], 0,
                    sizeof(m_if_hw.sadb_x_if_hw_offl_if));
                strlcpy(&m_if_hw.sadb_x_if_hw_offl_if[0], p_hwif.buf,
                    sizeof(m_if_hw.sadb_x_if_hw_offl_if));

                memcpy(buf + l, &m_if_hw, l0);
                l += l0;
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
                        plen = setkeymsg_plen(s);
                        if (plen == -1)
                                continue;

                        /* set src */
                        sa = s->ai_addr;
                        salen = s->ai_addr->sa_len;
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
                        salen = d->ai_addr->sa_len;
                        m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                            PFKEY_ALIGN8(salen));
                        m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
                        m_addr.sadb_address_proto = upper;
                        m_addr.sadb_address_prefixlen =
                            (dplen >= 0 ? dplen : plen);
                        m_addr.sadb_address_reserved = 0;

                        setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                            sizeof(m_addr), (caddr_t)sa, salen);

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

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_addr(unsigned type, unsigned satype, struct addrinfo *srcs,
    struct addrinfo *dsts, int no_spi)
{
        struct sadb_msg *msg;
        char buf[BUFSIZ];
        int l, l0, len;
        struct sadb_sa m_sa;
        struct sadb_x_sa2 m_sa2;
        struct sadb_x_sa_replay m_replay;
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
                m_sa.sadb_sa_replay = p_replay > UINT8_MAX ? UINT8_MAX:
                    p_replay;
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

                if (p_replay > UINT8_MAX) {
                        len = sizeof(struct sadb_x_sa_replay);
                        m_replay.sadb_x_sa_replay_len = PFKEY_UNIT64(len);
                        m_replay.sadb_x_sa_replay_exttype =
                            SADB_X_EXT_SA_REPLAY;
                        m_replay.sadb_x_sa_replay_replay = p_replay << 3;

                        memcpy(buf + l, &m_replay, len);
                        l += len;
                }
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
                        plen = setkeymsg_plen(s);
                        if (plen == -1)
                                continue;

                        /* set src */
                        sa = s->ai_addr;
                        salen = s->ai_addr->sa_len;
                        m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                            PFKEY_ALIGN8(salen));
                        m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
                        m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
                        m_addr.sadb_address_prefixlen = plen;
                        m_addr.sadb_address_reserved = 0;

                        setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                            sizeof(m_addr), (caddr_t)sa, salen);

                        /* set dst */
                        sa = d->ai_addr;
                        salen = d->ai_addr->sa_len;
                        m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                            PFKEY_ALIGN8(salen));
                        m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
                        m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
                        m_addr.sadb_address_prefixlen = plen;
                        m_addr.sadb_address_reserved = 0;

                        setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                            sizeof(m_addr), (caddr_t)sa, salen);

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

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_add(unsigned type, unsigned satype, struct addrinfo *srcs,
    struct addrinfo *dsts)
{
        struct sadb_msg *msg;
        char buf[BUFSIZ];
        int l, l0, len;
        struct sadb_sa m_sa;
        struct sadb_x_sa2 m_sa2;
        struct sadb_address m_addr;
        struct sadb_x_sa_replay m_replay;
        struct addrinfo *s, *d;
        struct sadb_x_nat_t_type m_natt_type;
        struct sadb_x_nat_t_port m_natt_port;
        struct sadb_x_nat_t_frag m_natt_frag;
        struct sadb_x_if_hw_offl m_if_hw;
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
                struct sadb_key m_key;

                m_key.sadb_key_len =
                        PFKEY_UNIT64(sizeof(m_key)
                                   + PFKEY_ALIGN8(p_key_enc_len));
                m_key.sadb_key_exttype = SADB_EXT_KEY_ENCRYPT;
                m_key.sadb_key_bits = p_key_enc_len * 8;
                m_key.sadb_key_reserved = 0;

                setvarbuf(buf, &l,
                        (struct sadb_ext *)&m_key, sizeof(m_key),
                        (caddr_t)p_key_enc, p_key_enc_len);
        }

        /* set authentication algorithm, if present. */
        if (p_key_auth) {
                struct sadb_key m_key;

                m_key.sadb_key_len =
                        PFKEY_UNIT64(sizeof(m_key)
                                   + PFKEY_ALIGN8(p_key_auth_len));
                m_key.sadb_key_exttype = SADB_EXT_KEY_AUTH;
                m_key.sadb_key_bits = p_key_auth_len * 8;
                m_key.sadb_key_reserved = 0;

                setvarbuf(buf, &l,
                        (struct sadb_ext *)&m_key, sizeof(m_key),
                        (caddr_t)p_key_auth, p_key_auth_len);
        }

        /* set lifetime for HARD */
        if (p_lt_hard != 0) {
                struct sadb_lifetime m_lt;
                u_int slen = sizeof(struct sadb_lifetime);

                m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
                m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
                m_lt.sadb_lifetime_allocations = 0;
                m_lt.sadb_lifetime_bytes = 0;
                m_lt.sadb_lifetime_addtime = p_lt_hard;
                m_lt.sadb_lifetime_usetime = 0;

                memcpy(buf + l, &m_lt, slen);
                l += slen;
        }

        /* set lifetime for SOFT */
        if (p_lt_soft != 0) {
                struct sadb_lifetime m_lt;
                u_int slen = sizeof(struct sadb_lifetime);

                m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
                m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
                m_lt.sadb_lifetime_allocations = 0;
                m_lt.sadb_lifetime_bytes = 0;
                m_lt.sadb_lifetime_addtime = p_lt_soft;
                m_lt.sadb_lifetime_usetime = 0;

                memcpy(buf + l, &m_lt, slen);
                l += slen;
        }

        len = sizeof(struct sadb_sa);
        m_sa.sadb_sa_len = PFKEY_UNIT64(len);
        m_sa.sadb_sa_exttype = SADB_EXT_SA;
        m_sa.sadb_sa_spi = htonl(p_spi);
        m_sa.sadb_sa_replay = p_replay > UINT8_MAX ? UINT8_MAX: p_replay;
#ifdef __rtems__
        m_sa.sadb_sa_state = 1;
#else /* __rtems__ */
        m_sa.sadb_sa_state = 0;
#endif /* __rtems__ */
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

        if (p_replay > UINT8_MAX) {
                len = sizeof(struct sadb_x_sa_replay);
                m_replay.sadb_x_sa_replay_len = PFKEY_UNIT64(len);
                m_replay.sadb_x_sa_replay_exttype = SADB_X_EXT_SA_REPLAY;
                m_replay.sadb_x_sa_replay_replay = p_replay << 3;

                memcpy(buf + l, &m_replay, len);
                l += len;
        }

        if (p_natt_type != 0) {
                len = sizeof(m_natt_type);
                memset(&m_natt_type, 0, sizeof(m_natt_type));
                m_natt_type.sadb_x_nat_t_type_len = PFKEY_UNIT64(len);
                m_natt_type.sadb_x_nat_t_type_exttype = SADB_X_EXT_NAT_T_TYPE;
                m_natt_type.sadb_x_nat_t_type_type = p_natt_type;
                memcpy(buf + l, &m_natt_type, len);
                l += len;

                memset(&m_addr, 0, sizeof(m_addr));
                m_addr.sadb_address_exttype = SADB_X_EXT_NAT_T_OAI;
                sa = p_natt_oai->ai_addr;
                salen = p_natt_oai->ai_addr->sa_len;
                m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                    PFKEY_ALIGN8(salen));
                m_addr.sadb_address_prefixlen = setkeymsg_plen(p_natt_oai);
                setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                    sizeof(m_addr), (caddr_t)sa, salen);

                len = sizeof(m_natt_port);
                memset(&m_natt_port, 0, sizeof(m_natt_port));
                m_natt_port.sadb_x_nat_t_port_len = PFKEY_UNIT64(len);
                m_natt_port.sadb_x_nat_t_port_exttype = SADB_X_EXT_NAT_T_SPORT;
                m_natt_port.sadb_x_nat_t_port_port = htons(p_natt_sport);
                memcpy(buf + l, &m_natt_port, len);
                l += len;

                memset(&m_addr, 0, sizeof(m_addr));
                m_addr.sadb_address_exttype = SADB_X_EXT_NAT_T_OAR;
                sa = p_natt_oar->ai_addr;
                salen = p_natt_oar->ai_addr->sa_len;
                m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                    PFKEY_ALIGN8(salen));
                m_addr.sadb_address_prefixlen = setkeymsg_plen(p_natt_oar);
                setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                    sizeof(m_addr), (caddr_t)sa, salen);

                len = sizeof(m_natt_port);
                memset(&m_natt_port, 0, sizeof(m_natt_port));
                m_natt_port.sadb_x_nat_t_port_len = PFKEY_UNIT64(len);
                m_natt_port.sadb_x_nat_t_port_exttype = SADB_X_EXT_NAT_T_DPORT;
                m_natt_port.sadb_x_nat_t_port_port = htons(p_natt_dport);
                memcpy(buf + l, &m_natt_port, len);
                l += len;

                if (p_natt_fraglen != -1) {
                        len = sizeof(m_natt_frag);
                        memset(&m_natt_port, 0, sizeof(m_natt_frag));
                        m_natt_frag.sadb_x_nat_t_frag_len = PFKEY_UNIT64(len);
                        m_natt_frag.sadb_x_nat_t_frag_exttype =
                            SADB_X_EXT_NAT_T_FRAG;
                        m_natt_frag.sadb_x_nat_t_frag_fraglen = p_natt_fraglen;
                        memcpy(buf + l, &m_natt_frag, len);
                        l += len;
                }
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
                        plen = setkeymsg_plen(s);
                        if (plen == -1)
                                continue;

                        /* set src */
                        sa = s->ai_addr;
                        salen = s->ai_addr->sa_len;
                        m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                            PFKEY_ALIGN8(salen));
                        m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
                        m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
                        m_addr.sadb_address_prefixlen = plen;
                        m_addr.sadb_address_reserved = 0;

                        setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                            sizeof(m_addr), (caddr_t)sa, salen);

                        /* set dst */
                        sa = d->ai_addr;
                        salen = d->ai_addr->sa_len;
                        m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
                            PFKEY_ALIGN8(salen));
                        m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
                        m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
                        m_addr.sadb_address_prefixlen = plen;
                        m_addr.sadb_address_reserved = 0;

                        setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
                            sizeof(m_addr), (caddr_t)sa, salen);

                        msg->sadb_msg_len = PFKEY_UNIT64(l);

                        sendkeymsg(buf, l);

                        n++;
                }
        }

        if (p_hwif.len != 0) {
                len = sizeof(struct sadb_x_if_hw_offl);
                m_if_hw.sadb_x_if_hw_offl_len = PFKEY_UNIT64(len);
                m_if_hw.sadb_x_if_hw_offl_exttype = SADB_X_EXT_IF_HW_OFFL;
                m_if_hw.sadb_x_if_hw_offl_flags = 0;
                memset(&m_if_hw.sadb_x_if_hw_offl_if[0], 0,
                    sizeof(m_if_hw.sadb_x_if_hw_offl_if));
                strlcpy(&m_if_hw.sadb_x_if_hw_offl_if[0], p_hwif.buf,
                    sizeof(m_if_hw.sadb_x_if_hw_offl_if));

                memcpy(buf + l, &m_if_hw, len);
                l += len;
        }

        if (n == 0)
                return -1;
        else
                return 0;
}

static struct addrinfo *
parse_addr(char *host, char *port)
{
        struct addrinfo hints, *res = NULL;
        int error;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = p_aifamily;
        hints.ai_socktype = SOCK_DGRAM;         /*dummy*/
        hints.ai_protocol = IPPROTO_UDP;        /*dummy*/
        hints.ai_flags = p_aiflags;
        error = getaddrinfo(host, port, &hints, &res);
        if (error != 0) {
                yyerror(gai_strerror(error));
                return NULL;
        }
        return res;
}

static int
fix_portstr(vchar_t *spec, vchar_t *sport, vchar_t *dport)
{
        char *p, *p2;
        u_int l;

        l = 0;
        for (p = spec->buf; *p != ',' && *p != '\0' && l < spec->len; p++, l++)
                ;
        if (*p == '\0') {
                p2 = "0";
        } else {
                if (*p == ',') {
                        *p = '\0';
                        p2 = ++p;
                }
                for (p = p2; *p != '\0' && l < spec->len; p++, l++)
                        ;
                if (*p != '\0' || *p2 == '\0') {
                        yyerror("invalid an upper layer protocol spec");
                        return -1;
                }
        }

        sport->buf = strdup(spec->buf);
        if (!sport->buf) {
                yyerror("insufficient memory");
                return -1;
        }
        sport->len = strlen(sport->buf);
        dport->buf = strdup(p2);
        if (!dport->buf) {
                yyerror("insufficient memory");
                return -1;
        }
        dport->len = strlen(dport->buf);

        return 0;
}

static int
setvarbuf(char *buf, int *off, struct sadb_ext *ebuf, int elen, caddr_t vbuf,
    int vlen)
{
        memset(buf + *off, 0, PFKEY_UNUNIT64(ebuf->sadb_ext_len));
        memcpy(buf + *off, (caddr_t)ebuf, elen);
        memcpy(buf + *off + elen, vbuf, vlen);
        (*off) += PFKEY_ALIGN8(elen + vlen);

        return 0;
}

void
parse_init(void)
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

        p_aiflags = 0;
        p_aifamily = PF_UNSPEC;

        p_natt_type = 0;
        p_natt_oai = p_natt_oar = NULL;
        p_natt_sport = p_natt_dport = 0;
        p_natt_fraglen = -1;

        esn = false;
        p_hwif.len = 0;
        p_hwif.buf = NULL;
}

void
free_buffer(void)
{
        /* we got tons of memory leaks in the parser anyways, leave them */
}
#line 1176 "setkeyyy.tab.c"

#if YYDEBUG
#include <stdio.h>      /* needed for printf */
#endif

#include <stdlib.h>     /* needed for malloc, etc */
#include <string.h>     /* needed for memset */

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
    if (newss == NULL)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == NULL)
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

    if ((yys = getenv("YYDEBUG")) != NULL)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    /* yym is set below */
    /* yyn is set below */
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
#line 158 "../../freebsd/sbin/setkey/parse.y"
        {
                        free_buffer();
                        parse_init();
                }
#line 1381 "setkeyyy.tab.c"
break;
case 13:
#line 181 "../../freebsd/sbin/setkey/parse.y"
        {
                        int status;

                        status = setkeymsg_add(SADB_ADD, yystack.l_mark[-4].num, yystack.l_mark[-6].res, yystack.l_mark[-5].res);
                        if (status < 0)
                                return -1;
                }
#line 1392 "setkeyyy.tab.c"
break;
case 14:
#line 193 "../../freebsd/sbin/setkey/parse.y"
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
#line 1410 "setkeyyy.tab.c"
break;
case 15:
#line 212 "../../freebsd/sbin/setkey/parse.y"
        {
                        int status;

                        status = setkeymsg_addr(SADB_DELETE, yystack.l_mark[-1].num, yystack.l_mark[-3].res, yystack.l_mark[-2].res, 1);
                        if (status < 0)
                                return -1;
                }
#line 1421 "setkeyyy.tab.c"
break;
case 16:
#line 224 "../../freebsd/sbin/setkey/parse.y"
        {
                        int status;

                        if (p_mode != IPSEC_MODE_ANY)
                                yyerror("WARNING: mode is obsolete");

                        status = setkeymsg_addr(SADB_GET, yystack.l_mark[-3].num, yystack.l_mark[-5].res, yystack.l_mark[-4].res, 0);
                        if (status < 0)
                                return -1;
                }
#line 1435 "setkeyyy.tab.c"
break;
case 17:
#line 239 "../../freebsd/sbin/setkey/parse.y"
        {
                        struct sadb_msg msg;
                        setkeymsg0(&msg, SADB_FLUSH, yystack.l_mark[-1].num, sizeof(msg));
                        sendkeymsg((char *)&msg, sizeof(msg));
                }
#line 1444 "setkeyyy.tab.c"
break;
case 18:
#line 249 "../../freebsd/sbin/setkey/parse.y"
        {
                        struct sadb_msg msg;
                        setkeymsg0(&msg, SADB_DUMP, yystack.l_mark[-1].num, sizeof(msg));
                        sendkeymsg((char *)&msg, sizeof(msg));
                }
#line 1453 "setkeyyy.tab.c"
break;
case 19:
#line 258 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.num = SADB_SATYPE_UNSPEC;
                }
#line 1460 "setkeyyy.tab.c"
break;
case 20:
#line 262 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.num = SADB_SATYPE_ESP;
                        if (yystack.l_mark[0].num == 1)
                                p_ext |= SADB_X_EXT_OLD;
                        else
                                p_ext &= ~SADB_X_EXT_OLD;
                }
#line 1471 "setkeyyy.tab.c"
break;
case 21:
#line 270 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.num = SADB_SATYPE_AH;
                        if (yystack.l_mark[0].num == 1)
                                p_ext |= SADB_X_EXT_OLD;
                        else
                                p_ext &= ~SADB_X_EXT_OLD;
                }
#line 1482 "setkeyyy.tab.c"
break;
case 22:
#line 278 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.num = SADB_X_SATYPE_IPCOMP;
                }
#line 1489 "setkeyyy.tab.c"
break;
case 23:
#line 282 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.num = SADB_X_SATYPE_TCPSIGNATURE;
                }
#line 1496 "setkeyyy.tab.c"
break;
case 24:
#line 288 "../../freebsd/sbin/setkey/parse.y"
        { p_spi = yystack.l_mark[0].ulnum; }
#line 1501 "setkeyyy.tab.c"
break;
case 25:
#line 290 "../../freebsd/sbin/setkey/parse.y"
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
#line 1522 "setkeyyy.tab.c"
break;
case 32:
#line 326 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[0].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_enc = yystack.l_mark[0].num;
                }
#line 1533 "setkeyyy.tab.c"
break;
case 33:
#line 334 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[-1].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_enc = yystack.l_mark[-1].num;
                        p_ext |= SADB_X_EXT_RAWCPI;
                }
#line 1545 "setkeyyy.tab.c"
break;
case 34:
#line 345 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[0].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_enc = yystack.l_mark[0].num;

                        p_key_enc_len = 0;
                        p_key_enc = NULL;
                        if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
                            p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
                                yyerror(ipsec_strerror());
                                return -1;
                        }
                }
#line 1564 "setkeyyy.tab.c"
break;
case 35:
#line 360 "../../freebsd/sbin/setkey/parse.y"
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
#line 1583 "setkeyyy.tab.c"
break;
case 36:
#line 375 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[0].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        yyerror("WARNING: obsolete algorithm");
                        p_alg_enc = yystack.l_mark[0].num;

                        p_key_enc_len = 0;
                        p_key_enc = NULL;
                        if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
                            p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
                                yyerror(ipsec_strerror());
                                return -1;
                        }
                }
#line 1603 "setkeyyy.tab.c"
break;
case 37:
#line 392 "../../freebsd/sbin/setkey/parse.y"
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
#line 1627 "setkeyyy.tab.c"
break;
case 38:
#line 413 "../../freebsd/sbin/setkey/parse.y"
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
#line 1651 "setkeyyy.tab.c"
break;
case 39:
#line 434 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[-1].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_enc = yystack.l_mark[-1].num;

                        p_key_enc_len = yystack.l_mark[0].val.len;

                        p_key_enc = yystack.l_mark[0].val.buf;
                        /*
                         * Salted keys include a 4 byte value that is
                         * not part of the key.
                         */
                        if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
                            p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len - 4)) < 0) {
                                yyerror(ipsec_strerror());
                                return -1;
                        }
                }
#line 1675 "setkeyyy.tab.c"
break;
case 40:
#line 457 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[-1].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_auth = yystack.l_mark[-1].num;

                        p_key_auth_len = yystack.l_mark[0].val.len;
                        p_key_auth = yystack.l_mark[0].val.buf;

                        if (p_alg_auth == SADB_X_AALG_TCP_MD5) {
                                if ((p_key_auth_len < 1) || (p_key_auth_len >
                                    80))
                                        return -1;
                        } else if (ipsec_check_keylen(SADB_EXT_SUPPORTED_AUTH,
                            p_alg_auth, PFKEY_UNUNIT64(p_key_auth_len)) < 0) {
                                yyerror(ipsec_strerror());
                                return -1;
                        }
                }
#line 1699 "setkeyyy.tab.c"
break;
case 41:
#line 477 "../../freebsd/sbin/setkey/parse.y"
        {
                        if (yystack.l_mark[0].num < 0) {
                                yyerror("unsupported algorithm");
                                return -1;
                        }
                        p_alg_auth = yystack.l_mark[0].num;

                        p_key_auth_len = 0;
                        p_key_auth = NULL;
                }
#line 1713 "setkeyyy.tab.c"
break;
case 42:
#line 491 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val = yystack.l_mark[0].val;
                }
#line 1720 "setkeyyy.tab.c"
break;
case 43:
#line 495 "../../freebsd/sbin/setkey/parse.y"
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
#line 1750 "setkeyyy.tab.c"
break;
case 46:
#line 529 "../../freebsd/sbin/setkey/parse.y"
        { p_ext |= yystack.l_mark[0].num; }
#line 1755 "setkeyyy.tab.c"
break;
case 47:
#line 530 "../../freebsd/sbin/setkey/parse.y"
        { p_ext &= ~SADB_X_EXT_CYCSEQ; }
#line 1760 "setkeyyy.tab.c"
break;
case 48:
#line 531 "../../freebsd/sbin/setkey/parse.y"
        { p_mode = yystack.l_mark[0].num; }
#line 1765 "setkeyyy.tab.c"
break;
case 49:
#line 532 "../../freebsd/sbin/setkey/parse.y"
        { p_mode = IPSEC_MODE_ANY; }
#line 1770 "setkeyyy.tab.c"
break;
case 50:
#line 533 "../../freebsd/sbin/setkey/parse.y"
        { p_reqid = yystack.l_mark[0].ulnum; }
#line 1775 "setkeyyy.tab.c"
break;
case 51:
#line 535 "../../freebsd/sbin/setkey/parse.y"
        {
                        if ((p_ext & SADB_X_EXT_OLD) != 0) {
                                yyerror("replay prevention cannot be used with "
                                    "ah/esp-old");
                                return -1;
                        }
                        p_replay = yystack.l_mark[0].ulnum;
                        if (p_replay > (UINT32_MAX - 32) >> 3)
                                yyerror("replay window is too large");
                }
#line 1789 "setkeyyy.tab.c"
break;
case 52:
#line 545 "../../freebsd/sbin/setkey/parse.y"
        { p_lt_hard = yystack.l_mark[0].ulnum; }
#line 1794 "setkeyyy.tab.c"
break;
case 53:
#line 546 "../../freebsd/sbin/setkey/parse.y"
        { p_lt_soft = yystack.l_mark[0].ulnum; }
#line 1799 "setkeyyy.tab.c"
break;
case 54:
#line 548 "../../freebsd/sbin/setkey/parse.y"
        {
                        p_natt_type = UDP_ENCAP_ESPINUDP;
                        p_natt_oai = yystack.l_mark[-7].res;
                        p_natt_oar = yystack.l_mark[-3].res;
                        if (p_natt_oai == NULL || p_natt_oar == NULL)
                                return (-1);
                        p_natt_sport = yystack.l_mark[-5].ulnum;
                        p_natt_dport = yystack.l_mark[-1].ulnum;
                }
#line 1812 "setkeyyy.tab.c"
break;
case 55:
#line 558 "../../freebsd/sbin/setkey/parse.y"
        {
                        p_natt_fraglen = yystack.l_mark[0].ulnum;
                }
#line 1819 "setkeyyy.tab.c"
break;
case 56:
#line 562 "../../freebsd/sbin/setkey/parse.y"
        {
                        esn = true;
                        p_ext |= SADB_X_SAFLAGS_ESN;
                }
#line 1827 "setkeyyy.tab.c"
break;
case 57:
#line 567 "../../freebsd/sbin/setkey/parse.y"
        {
                        p_hwif = yystack.l_mark[0].val;
                }
#line 1834 "setkeyyy.tab.c"
break;
case 58:
#line 576 "../../freebsd/sbin/setkey/parse.y"
        {
                        int status;
                        struct addrinfo *src, *dst;

                        /* fixed port fields if ulp is icmpv6 */
                        if (yystack.l_mark[-3].val.buf != NULL) {
                                if (yystack.l_mark[-4].num != IPPROTO_ICMPV6)
                                        return -1;
                                free(yystack.l_mark[-8].val.buf);
                                free(yystack.l_mark[-5].val.buf);
                                if (fix_portstr(&yystack.l_mark[-3].val, &yystack.l_mark[-8].val, &yystack.l_mark[-5].val))
                                        return -1;
                        }

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

                        status = setkeymsg_spdaddr(SADB_X_SPDADD, yystack.l_mark[-4].num, &yystack.l_mark[-2].val,
                            src, yystack.l_mark[-9].num, dst, yystack.l_mark[-6].num);
                        freeaddrinfo(src);
                        freeaddrinfo(dst);
                        if (status < 0)
                                return -1;
                }
#line 1872 "setkeyyy.tab.c"
break;
case 59:
#line 611 "../../freebsd/sbin/setkey/parse.y"
        {
                        return -1;
                }
#line 1879 "setkeyyy.tab.c"
break;
case 60:
#line 618 "../../freebsd/sbin/setkey/parse.y"
        {
                        int status;
                        struct addrinfo *src, *dst;

                        /* fixed port fields if ulp is icmpv6 */
                        if (yystack.l_mark[-2].val.buf != NULL) {
                                if (yystack.l_mark[-3].num != IPPROTO_ICMPV6)
                                        return -1;
                                free(yystack.l_mark[-7].val.buf);
                                free(yystack.l_mark[-4].val.buf);
                                if (fix_portstr(&yystack.l_mark[-2].val, &yystack.l_mark[-7].val, &yystack.l_mark[-4].val))
                                        return -1;
                        }

                        src = parse_addr(yystack.l_mark[-9].val.buf, yystack.l_mark[-7].val.buf);
                        dst = parse_addr(yystack.l_mark[-6].val.buf, yystack.l_mark[-4].val.buf);
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

                        status = setkeymsg_spdaddr(SADB_X_SPDDELETE, yystack.l_mark[-3].num, &yystack.l_mark[-1].val,
                            src, yystack.l_mark[-8].num, dst, yystack.l_mark[-5].num);
                        freeaddrinfo(src);
                        freeaddrinfo(dst);
                        if (status < 0)
                                return -1;
                }
#line 1917 "setkeyyy.tab.c"
break;
case 61:
#line 656 "../../freebsd/sbin/setkey/parse.y"
        {
                        struct sadb_msg msg;
                        setkeymsg0(&msg, SADB_X_SPDDUMP, SADB_SATYPE_UNSPEC,
                            sizeof(msg));
                        sendkeymsg((char *)&msg, sizeof(msg));
                }
#line 1927 "setkeyyy.tab.c"
break;
case 62:
#line 666 "../../freebsd/sbin/setkey/parse.y"
        {
                        struct sadb_msg msg;
                        setkeymsg0(&msg, SADB_X_SPDFLUSH, SADB_SATYPE_UNSPEC,
                            sizeof(msg));
                        sendkeymsg((char *)&msg, sizeof(msg));
                }
#line 1937 "setkeyyy.tab.c"
break;
case 66:
#line 682 "../../freebsd/sbin/setkey/parse.y"
        {
                        p_hwif = yystack.l_mark[0].val;
                }
#line 1944 "setkeyyy.tab.c"
break;
case 67:
#line 689 "../../freebsd/sbin/setkey/parse.y"
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
#line 1969 "setkeyyy.tab.c"
break;
case 68:
#line 714 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.res = parse_addr(yystack.l_mark[0].val.buf, NULL);
                        if (yyval.res == NULL) {
                                /* yyerror already called by parse_addr */
                                return -1;
                        }
                }
#line 1980 "setkeyyy.tab.c"
break;
case 69:
#line 724 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = -1; }
#line 1985 "setkeyyy.tab.c"
break;
case 70:
#line 725 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = yystack.l_mark[0].ulnum; }
#line 1990 "setkeyyy.tab.c"
break;
case 71:
#line 730 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val.buf = strdup("0");
                        if (!yyval.val.buf) {
                                yyerror("insufficient memory");
                                return -1;
                        }
                        yyval.val.len = strlen(yyval.val.buf);
                }
#line 2002 "setkeyyy.tab.c"
break;
case 72:
#line 739 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val.buf = strdup("0");
                        if (!yyval.val.buf) {
                                yyerror("insufficient memory");
                                return -1;
                        }
                        yyval.val.len = strlen(yyval.val.buf);
                }
#line 2014 "setkeyyy.tab.c"
break;
case 73:
#line 748 "../../freebsd/sbin/setkey/parse.y"
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
#line 2028 "setkeyyy.tab.c"
break;
case 74:
#line 759 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val = yystack.l_mark[-1].val;
                }
#line 2035 "setkeyyy.tab.c"
break;
case 75:
#line 765 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = yystack.l_mark[0].ulnum; }
#line 2040 "setkeyyy.tab.c"
break;
case 76:
#line 766 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = IPSEC_ULPROTO_ANY; }
#line 2045 "setkeyyy.tab.c"
break;
case 77:
#line 767 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = IPPROTO_TCP; }
#line 2050 "setkeyyy.tab.c"
break;
case 78:
#line 768 "../../freebsd/sbin/setkey/parse.y"
        { yyval.num = IPPROTO_ESP; }
#line 2055 "setkeyyy.tab.c"
break;
case 79:
#line 770 "../../freebsd/sbin/setkey/parse.y"
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
#line 2077 "setkeyyy.tab.c"
break;
case 80:
#line 792 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val.buf = NULL;
                        yyval.val.len = 0;
                }
#line 2085 "setkeyyy.tab.c"
break;
case 81:
#line 797 "../../freebsd/sbin/setkey/parse.y"
        {
                        yyval.val.buf = strdup(yystack.l_mark[0].val.buf);
                        if (!yyval.val.buf) {
                                yyerror("insufficient memory");
                                return -1;
                        }
                        yyval.val.len = strlen(yyval.val.buf);
                }
#line 2097 "setkeyyy.tab.c"
break;
case 82:
#line 809 "../../freebsd/sbin/setkey/parse.y"
        {
                        char *policy;

                        policy = ipsec_set_policy(yystack.l_mark[0].val.buf, yystack.l_mark[0].val.len);
                        if (policy == NULL) {
                                yyerror(ipsec_strerror());
                                return -1;
                        }

                        yyval.val.buf = policy;
                        yyval.val.len = ipsec_get_policylen(policy);
                }
#line 2113 "setkeyyy.tab.c"
break;
case 83:
#line 824 "../../freebsd/sbin/setkey/parse.y"
        { yyval.val = yystack.l_mark[0].val; }
#line 2118 "setkeyyy.tab.c"
break;
#line 2120 "setkeyyy.tab.c"
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