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
#define yyparse    pfctlyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      pfctlylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    pfctlyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     pfctlychar
#endif /* yychar */

#ifndef yyval
#define yyval      pfctlyval
#endif /* yyval */

#ifndef yylval
#define yylval     pfctlylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    pfctlydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    pfctlynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  pfctlyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      pfctlylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      pfctlylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   pfctlydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    pfctlydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   pfctlysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   pfctlyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   pfctlygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    pfctlytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    pfctlycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     pfctlyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     pfctlyrule
#endif /* yyrule */
#define YYPREFIX "pfctly"

#define YYPURE 0

#line 32 "parse.y"
#ifdef __rtems__
#include <machine/rtems-bsd-user-space.h>
#undef INET6
#endif /* __rtems__ */

#ifdef __rtems__
#include "rtems-bsd-pfctl-namespace.h"

/* Provided by kernel-space modules */
#define	pf_find_or_create_ruleset _bsd_pf_find_or_create_ruleset
#define	pf_anchor_setup _bsd_pf_anchor_setup
#define	pf_remove_if_empty_ruleset _bsd_pf_remove_if_empty_ruleset

#include <machine/rtems-bsd-program.h>
#endif /* __rtems__ */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#define PFIOC_USE_LATEST

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <net/pfvar.h>
#include <arpa/inet.h>
#include <net/altq/altq.h>
#include <net/altq/altq_cbq.h>
#include <net/altq/altq_codel.h>
#include <net/altq/altq_priq.h>
#include <net/altq/altq_hfsc.h>
#include <net/altq/altq_fairq.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <err.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <md5.h>

#include "pfctl_parser.h"
#include "pfctl.h"
#ifdef __rtems__
#include "rtems-bsd-pfctl-parse-data.h"
#endif /* __rtems__ */

static struct pfctl	*pf = NULL;
static int		 debug = 0;
static int		 rulestate = 0;
static u_int16_t	 returnicmpdefault =
			    (ICMP_UNREACH << 8) | ICMP_UNREACH_PORT;
static u_int16_t	 returnicmp6default =
			    (ICMP6_DST_UNREACH << 8) | ICMP6_DST_UNREACH_NOPORT;
static int		 blockpolicy = PFRULE_DROP;
static int		 failpolicy = PFRULE_DROP;
static int		 require_order = 1;
static int		 default_statelock;

static TAILQ_HEAD(files, file)	 files = TAILQ_HEAD_INITIALIZER(files);
static struct file {
	TAILQ_ENTRY(file)	 entry;
	FILE			*stream;
	char			*name;
	int			 lineno;
	int			 errors;
} *file;
struct file	*pushfile(const char *, int);
int		 popfile(void);
int		 check_file_secrecy(int, const char *);
int		 yyparse(void);
int		 yylex(void);
int		 yyerror(const char *, ...);
int		 kw_cmp(const void *, const void *);
int		 lookup(char *);
int		 lgetc(int);
int		 lungetc(int);
int		 findeol(void);

static TAILQ_HEAD(symhead, sym)	 symhead = TAILQ_HEAD_INITIALIZER(symhead);
struct sym {
	TAILQ_ENTRY(sym)	 entry;
	int			 used;
	int			 persist;
	char			*nam;
	char			*val;
};
int		 symset(const char *, const char *, int);
char		*symget(const char *);

int		 atoul(char *, u_long *);

enum {
	PFCTL_STATE_NONE,
	PFCTL_STATE_OPTION,
	PFCTL_STATE_SCRUB,
	PFCTL_STATE_QUEUE,
	PFCTL_STATE_NAT,
	PFCTL_STATE_FILTER
};

struct node_proto {
	u_int8_t		 proto;
	struct node_proto	*next;
	struct node_proto	*tail;
};

struct node_port {
	u_int16_t		 port[2];
	u_int8_t		 op;
	struct node_port	*next;
	struct node_port	*tail;
};

struct node_uid {
	uid_t			 uid[2];
	u_int8_t		 op;
	struct node_uid		*next;
	struct node_uid		*tail;
};

struct node_gid {
	gid_t			 gid[2];
	u_int8_t		 op;
	struct node_gid		*next;
	struct node_gid		*tail;
};

struct node_icmp {
	u_int8_t		 code;
	u_int8_t		 type;
	u_int8_t		 proto;
	struct node_icmp	*next;
	struct node_icmp	*tail;
};

enum	{ PF_STATE_OPT_MAX, PF_STATE_OPT_NOSYNC, PF_STATE_OPT_SRCTRACK,
	    PF_STATE_OPT_MAX_SRC_STATES, PF_STATE_OPT_MAX_SRC_CONN,
	    PF_STATE_OPT_MAX_SRC_CONN_RATE, PF_STATE_OPT_MAX_SRC_NODES,
	    PF_STATE_OPT_OVERLOAD, PF_STATE_OPT_STATELOCK,
	    PF_STATE_OPT_TIMEOUT, PF_STATE_OPT_SLOPPY, };

enum	{ PF_SRCTRACK_NONE, PF_SRCTRACK, PF_SRCTRACK_GLOBAL, PF_SRCTRACK_RULE };

struct node_state_opt {
	int			 type;
	union {
		u_int32_t	 max_states;
		u_int32_t	 max_src_states;
		u_int32_t	 max_src_conn;
		struct {
			u_int32_t	limit;
			u_int32_t	seconds;
		}		 max_src_conn_rate;
		struct {
			u_int8_t	flush;
			char		tblname[PF_TABLE_NAME_SIZE];
		}		 overload;
		u_int32_t	 max_src_nodes;
		u_int8_t	 src_track;
		u_int32_t	 statelock;
		struct {
			int		number;
			u_int32_t	seconds;
		}		 timeout;
	}			 data;
	struct node_state_opt	*next;
	struct node_state_opt	*tail;
};

struct peer {
	struct node_host	*host;
	struct node_port	*port;
};

static struct node_queue {
	char			 queue[PF_QNAME_SIZE];
	char			 parent[PF_QNAME_SIZE];
	char			 ifname[IFNAMSIZ];
	int			 scheduler;
	struct node_queue	*next;
	struct node_queue	*tail;
}	*queues = NULL;

struct node_qassign {
	char		*qname;
	char		*pqname;
};

static struct filter_opts {
	int			 marker;
#define FOM_FLAGS	0x01
#define FOM_ICMP	0x02
#define FOM_TOS		0x04
#define FOM_KEEP	0x08
#define FOM_SRCTRACK	0x10
#define FOM_SETPRIO	0x0400
#define FOM_PRIO	0x2000
	struct node_uid		*uid;
	struct node_gid		*gid;
	struct {
		u_int8_t	 b1;
		u_int8_t	 b2;
		u_int16_t	 w;
		u_int16_t	 w2;
	} flags;
	struct node_icmp	*icmpspec;
	u_int32_t		 tos;
	u_int32_t		 prob;
	struct {
		int			 action;
		struct node_state_opt	*options;
	} keep;
	int			 fragment;
	int			 allowopts;
	char			*label;
	struct node_qassign	 queues;
	char			*tag;
	char			*match_tag;
	u_int8_t		 match_tag_not;
	u_int			 rtableid;
	u_int8_t		 prio;
	u_int8_t		 set_prio[2];
	struct {
		struct node_host	*addr;
		u_int16_t		port;
	}			 divert;
} filter_opts;

static struct antispoof_opts {
	char			*label;
	u_int			 rtableid;
} antispoof_opts;

static struct scrub_opts {
	int			 marker;
#define SOM_MINTTL	0x01
#define SOM_MAXMSS	0x02
#define SOM_FRAGCACHE	0x04
#define SOM_SETTOS	0x08
	int			 nodf;
	int			 minttl;
	int			 maxmss;
	int			 settos;
	int			 fragcache;
	int			 randomid;
	int			 reassemble_tcp;
	char			*match_tag;
	u_int8_t		 match_tag_not;
	u_int			 rtableid;
} scrub_opts;

static struct queue_opts {
	int			marker;
#define QOM_BWSPEC	0x01
#define QOM_SCHEDULER	0x02
#define QOM_PRIORITY	0x04
#define QOM_TBRSIZE	0x08
#define QOM_QLIMIT	0x10
	struct node_queue_bw	queue_bwspec;
	struct node_queue_opt	scheduler;
	int			priority;
	unsigned int		tbrsize;
	int			qlimit;
} queue_opts;

static struct table_opts {
	int			flags;
	int			init_addr;
	struct node_tinithead	init_nodes;
} table_opts;

static struct pool_opts {
	int			 marker;
#define POM_TYPE		0x01
#define POM_STICKYADDRESS	0x02
	u_int8_t		 opts;
	int			 type;
	int			 staticport;
	struct pf_poolhashkey	*key;

} pool_opts;

static struct codel_opts	 codel_opts;
static struct node_hfsc_opts	 hfsc_opts;
static struct node_fairq_opts	 fairq_opts;
static struct node_state_opt	*keep_state_defaults = NULL;

int		 disallow_table(struct node_host *, const char *);
int		 disallow_urpf_failed(struct node_host *, const char *);
int		 disallow_alias(struct node_host *, const char *);
int		 rule_consistent(struct pf_rule *, int);
int		 filter_consistent(struct pf_rule *, int);
int		 nat_consistent(struct pf_rule *);
int		 rdr_consistent(struct pf_rule *);
int		 process_tabledef(char *, struct table_opts *);
void		 expand_label_str(char *, size_t, const char *, const char *);
void		 expand_label_if(const char *, char *, size_t, const char *);
void		 expand_label_addr(const char *, char *, size_t, u_int8_t,
		    struct node_host *);
void		 expand_label_port(const char *, char *, size_t,
		    struct node_port *);
void		 expand_label_proto(const char *, char *, size_t, u_int8_t);
void		 expand_label_nr(const char *, char *, size_t);
void		 expand_label(char *, size_t, const char *, u_int8_t,
		    struct node_host *, struct node_port *, struct node_host *,
		    struct node_port *, u_int8_t);
void		 expand_rule(struct pf_rule *, struct node_if *,
		    struct node_host *, struct node_proto *, struct node_os *,
		    struct node_host *, struct node_port *, struct node_host *,
		    struct node_port *, struct node_uid *, struct node_gid *,
		    struct node_icmp *, const char *);
int		 expand_altq(struct pf_altq *, struct node_if *,
		    struct node_queue *, struct node_queue_bw bwspec,
		    struct node_queue_opt *);
int		 expand_queue(struct pf_altq *, struct node_if *,
		    struct node_queue *, struct node_queue_bw,
		    struct node_queue_opt *);
int		 expand_skip_interface(struct node_if *);

int	 check_rulestate(int);
int	 getservice(char *);
int	 rule_label(struct pf_rule *, char *);
int	 rt_tableid_max(void);

void	 mv_rules(struct pf_ruleset *, struct pf_ruleset *);
void	 decide_address_family(struct node_host *, sa_family_t *);
void	 remove_invalid_hosts(struct node_host **, sa_family_t *);
int	 invalid_redirect(struct node_host *, sa_family_t);
u_int16_t parseicmpspec(char *, sa_family_t);
int	 kw_casecmp(const void *, const void *);
int	 map_tos(char *string, int *);

static TAILQ_HEAD(loadanchorshead, loadanchors)
    loadanchorshead = TAILQ_HEAD_INITIALIZER(loadanchorshead);

struct loadanchors {
	TAILQ_ENTRY(loadanchors)	 entries;
	char				*anchorname;
	char				*filename;
};

typedef struct {
	union {
		int64_t			 number;
		double			 probability;
		int			 i;
		char			*string;
		u_int			 rtableid;
		struct {
			u_int8_t	 b1;
			u_int8_t	 b2;
			u_int16_t	 w;
			u_int16_t	 w2;
		}			 b;
		struct range {
			int		 a;
			int		 b;
			int		 t;
		}			 range;
		struct node_if		*interface;
		struct node_proto	*proto;
		struct node_icmp	*icmp;
		struct node_host	*host;
		struct node_os		*os;
		struct node_port	*port;
		struct node_uid		*uid;
		struct node_gid		*gid;
		struct node_state_opt	*state_opt;
		struct peer		 peer;
		struct {
			struct peer	 src, dst;
			struct node_os	*src_os;
		}			 fromto;
		struct {
			struct node_host	*host;
			u_int8_t		 rt;
			u_int8_t		 pool_opts;
			sa_family_t		 af;
			struct pf_poolhashkey	*key;
		}			 route;
		struct redirection {
			struct node_host	*host;
			struct range		 rport;
		}			*redirection;
		struct {
			int			 action;
			struct node_state_opt	*options;
		}			 keep_state;
		struct {
			u_int8_t	 log;
			u_int8_t	 logif;
			u_int8_t	 quick;
		}			 logquick;
		struct {
			int		 neg;
			char		*name;
		}			 tagged;
		struct pf_poolhashkey	*hashkey;
		struct node_queue	*queue;
		struct node_queue_opt	 queue_options;
		struct node_queue_bw	 queue_bwspec;
		struct node_qassign	 qassign;
		struct filter_opts	 filter_opts;
		struct antispoof_opts	 antispoof_opts;
		struct queue_opts	 queue_opts;
		struct scrub_opts	 scrub_opts;
		struct table_opts	 table_opts;
		struct pool_opts	 pool_opts;
		struct node_hfsc_opts	 hfsc_opts;
		struct node_fairq_opts	 fairq_opts;
		struct codel_opts	 codel_opts;
	} v;
	int lineno;
} YYSTYPE;

#define PPORT_RANGE	1
#define PPORT_STAR	2
int	parseport(char *, struct range *r, int);

#define DYNIF_MULTIADDR(addr) ((addr).type == PF_ADDR_DYNIFTL && \
	(!((addr).iflags & PFI_AFLAG_NOALIAS) ||		 \
	!isdigit((addr).v.ifname[strlen((addr).v.ifname)-1])))

#line 541 "parse.c"

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

#define PASS 257
#define BLOCK 258
#define SCRUB 259
#define RETURN 260
#define IN 261
#define OS 262
#define OUT 263
#define LOG 264
#define QUICK 265
#define ON 266
#define FROM 267
#define TO 268
#define FLAGS 269
#define RETURNRST 270
#define RETURNICMP 271
#define RETURNICMP6 272
#define PROTO 273
#define INET 274
#define INET6 275
#define ALL 276
#define ANY 277
#define ICMPTYPE 278
#define ICMP6TYPE 279
#define CODE 280
#define KEEP 281
#define MODULATE 282
#define STATE 283
#define PORT 284
#define RDR 285
#define NAT 286
#define BINAT 287
#define ARROW 288
#define NODF 289
#define MINTTL 290
#define ERROR 291
#define ALLOWOPTS 292
#define FASTROUTE 293
#define FILENAME 294
#define ROUTETO 295
#define DUPTO 296
#define REPLYTO 297
#define NO 298
#define LABEL 299
#define NOROUTE 300
#define URPFFAILED 301
#define FRAGMENT 302
#define USER 303
#define GROUP 304
#define MAXMSS 305
#define MAXIMUM 306
#define TTL 307
#define TOS 308
#define DROP 309
#define TABLE 310
#define REASSEMBLE 311
#define FRAGDROP 312
#define FRAGCROP 313
#define ANCHOR 314
#define NATANCHOR 315
#define RDRANCHOR 316
#define BINATANCHOR 317
#define SET 318
#define OPTIMIZATION 319
#define TIMEOUT 320
#define LIMIT 321
#define LOGINTERFACE 322
#define BLOCKPOLICY 323
#define FAILPOLICY 324
#define RANDOMID 325
#define REQUIREORDER 326
#define SYNPROXY 327
#define FINGERPRINTS 328
#define NOSYNC 329
#define DEBUG 330
#define SKIP 331
#define HOSTID 332
#define ANTISPOOF 333
#define FOR 334
#define INCLUDE 335
#define BITMASK 336
#define RANDOM 337
#define SOURCEHASH 338
#define ROUNDROBIN 339
#define STATICPORT 340
#define PROBABILITY 341
#define ALTQ 342
#define CBQ 343
#define CODEL 344
#define PRIQ 345
#define HFSC 346
#define FAIRQ 347
#define BANDWIDTH 348
#define TBRSIZE 349
#define LINKSHARE 350
#define REALTIME 351
#define UPPERLIMIT 352
#define QUEUE 353
#define PRIORITY 354
#define QLIMIT 355
#define HOGS 356
#define BUCKETS 357
#define RTABLE 358
#define TARGET 359
#define INTERVAL 360
#define LOAD 361
#define RULESET_OPTIMIZATION 362
#define PRIO 363
#define STICKYADDRESS 364
#define MAXSRCSTATES 365
#define MAXSRCNODES 366
#define SOURCETRACK 367
#define GLOBAL 368
#define RULE 369
#define MAXSRCCONN 370
#define MAXSRCCONNRATE 371
#define OVERLOAD 372
#define FLUSH 373
#define SLOPPY 374
#define TAGGED 375
#define TAG 376
#define IFBOUND 377
#define FLOATING 378
#define STATEPOLICY 379
#define STATEDEFAULTS 380
#define ROUTE 381
#define SETTOS 382
#define DIVERTTO 383
#define DIVERTREPLY 384
#define STRING 385
#define NUMBER 386
#define PORTBINARY 387
#define YYERRCODE 256
typedef int YYINT;
static const YYINT pfctlylhs[] = {                       -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  137,  150,  150,
  150,  150,  150,  150,   18,  138,  138,  138,  138,  138,
  138,  138,  138,  138,  138,  138,  138,  138,  138,  138,
  138,  138,  138,   77,   77,   80,   80,   81,   81,   82,
   82,  147,   79,   79,  156,  156,  156,  156,  158,  157,
  157,  143,  143,  143,  143,  144,   26,  139,  159,  126,
  126,  128,  128,  127,  127,  127,  127,  127,  127,  127,
  127,  127,   17,   17,   17,  148,   92,   92,   93,   93,
   94,   94,  161,  120,  120,  122,  122,  121,  121,   11,
   11,  149,  162,  129,  129,  131,  131,  130,  130,  130,
  130,  145,  146,  163,  123,  123,  125,  125,  124,  124,
  124,  124,  124,  113,  113,   99,   99,   99,   99,   99,
   99,   99,   99,   99,   99,  100,  100,  101,  102,  102,
  103,  164,  106,  104,  104,  105,  105,  105,  105,  105,
  105,  105,  165,  109,  107,  107,  108,  108,  108,  108,
  108,  166,  112,  110,  110,  111,  111,  111,   96,   96,
   96,   97,   97,   98,  142,  167,  114,  114,  116,  116,
  115,  115,  115,  115,  115,  115,  115,  115,  115,  115,
  115,  115,  115,  115,  115,  115,  115,  115,  115,  117,
  117,  119,  119,  118,   30,   30,   13,   13,   23,   23,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   29,
   44,   44,   45,   45,   15,   15,   15,   88,   88,   87,
   87,   87,   87,   87,   89,   89,   90,   90,   91,   91,
   91,   91,    1,    1,    1,    2,    2,    3,    4,   16,
   16,   16,   35,   35,   35,   36,   36,   37,   38,   38,
   46,   46,   61,   61,   61,   62,   63,   63,   48,   48,
   49,   49,   47,   47,   47,  152,  152,   50,   50,   50,
   51,   51,   55,   55,   52,   52,   52,   53,   53,   53,
   53,   53,   53,   53,    5,    5,   54,   64,   64,   65,
   65,   66,   66,   66,   31,   33,   67,   67,   68,   68,
   69,   69,   69,    8,    8,   70,   70,   71,   71,   72,
   72,   72,    9,    9,   28,   27,   27,   27,   39,   39,
   39,   39,   40,   40,   42,   42,   41,   41,   41,   43,
   43,   43,    6,    6,    7,    7,   10,   10,   19,   19,
   19,   22,   22,   83,   83,   83,   83,   20,   20,   20,
   84,   84,   85,   85,   86,   86,   86,   86,   86,   86,
   86,   86,   86,   86,   86,   76,   95,   95,   95,   14,
   14,   32,   57,   57,   56,   56,   75,   75,   75,   34,
   34,  168,  132,  132,  134,  134,  133,  133,  133,  133,
  133,  133,   74,   74,   74,   25,   25,   25,   25,   24,
   24,  140,  141,   78,   78,  135,  135,  136,  136,   58,
   58,   59,   59,   60,   60,   73,   73,   73,   73,   73,
  151,  151,  153,  153,  154,  155,  155,  160,  160,   12,
   12,   21,   21,   21,   21,   21,   21,
};
static const YYINT pfctlylen[] = {                        2,
    0,    3,    2,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    4,    3,    2,    2,    3,
    3,    3,    3,    3,    1,    3,    3,    3,    6,    3,
    6,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    1,    1,    2,    1,    2,    1,    1,
    1,    3,    1,    0,    0,    2,    3,    3,    0,    5,
    0,   10,    7,    7,    7,    5,    2,    8,    0,    2,
    0,    2,    1,    1,    2,    2,    2,    1,    2,    1,
    2,    3,    2,    2,    2,    5,    2,    5,    2,    4,
    1,    3,    0,    2,    0,    2,    1,    1,    2,    1,
    0,    5,    0,    2,    0,    2,    1,    1,    3,    4,
    2,    5,    5,    0,    2,    0,    2,    1,    2,    2,
    2,    1,    2,    1,    1,    1,    4,    1,    4,    1,
    4,    1,    4,    1,    4,    1,    3,    1,    1,    3,
    1,    0,    2,    1,    3,    2,    8,    2,    8,    2,
    8,    1,    0,    2,    1,    3,    2,    6,    2,    2,
    1,    0,    2,    1,    3,    2,    2,    1,    0,    1,
    4,    2,    4,    1,    9,    0,    2,    0,    2,    1,
    2,    2,    1,    1,    2,    2,    1,    1,    1,    1,
    1,    2,    3,    2,    2,    2,    4,    1,    1,    4,
    2,    3,    1,    1,    2,    6,    1,    1,    1,    2,
    0,    1,    1,    5,    1,    1,    4,    4,    6,    1,
    1,    1,    1,    1,    0,    1,    1,    0,    1,    0,
    1,    1,    2,    2,    1,    4,    1,    3,    1,    1,
    1,    2,    0,    2,    5,    2,    4,    2,    1,    0,
    1,    1,    0,    2,    5,    2,    4,    1,    1,    1,
    1,    3,    0,    2,    5,    1,    2,    4,    0,    2,
    0,    2,    1,    3,    2,    2,    0,    1,    1,    4,
    2,    0,    2,    4,    2,    2,    2,    1,    3,    3,
    3,    1,    3,    3,    1,    1,    3,    1,    4,    2,
    4,    1,    2,    3,    1,    1,    1,    4,    2,    4,
    1,    2,    3,    1,    1,    1,    4,    2,    4,    1,
    2,    3,    1,    1,    1,    4,    3,    2,    2,    5,
    2,    5,    2,    4,    2,    4,    1,    3,    3,    1,
    3,    3,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    1,    1,    2,    3,    3,    3,    0,    1,    2,
    3,    0,    1,    3,    2,    1,    2,    2,    4,    5,
    2,    1,    1,    1,    2,    2,    2,    4,    6,    0,
    1,    1,    1,    4,    2,    4,    0,    2,    4,    0,
    1,    0,    2,    0,    2,    1,    1,    1,    2,    1,
    1,    1,    0,    2,    4,    0,    1,    2,    1,    3,
    3,   10,   13,    0,    2,    0,    3,    0,    2,    1,
    4,    2,    4,    1,    4,    0,    1,    3,    3,    3,
    2,    2,    4,    2,    2,    4,    2,    1,    0,    1,
    1,    1,    2,    2,    1,    2,    1,
};
static const YYINT pfctlydefred[] = {                     0,
    0,    0,    0,    0,  209,    0,  381,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   19,    0,
    0,    0,    0,    0,    0,   17,  220,    0,    0,    0,
  212,  210,    0,   53,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   18,    0,    0,
    0,    0,    0,   67,    0,    0,    0,  226,  227,    0,
    0,    0,    2,    4,    5,    6,    7,    8,    9,   10,
   11,   12,   13,   14,   15,   24,   16,   22,   21,   23,
   20,    0,    0,    0,    0,    0,   46,    0,    0,    0,
   26,    0,    0,    0,   28,    0,    0,   30,   45,   44,
   32,   35,   34,   37,   36,  440,  441,   38,   39,   41,
   42,  296,  295,   33,   27,   25,  352,  353,   40,    0,
  366,    0,    0,    0,    0,    0,    0,  374,    0,  372,
  373,    0,  363,    0,  234,    0,    0,  233,    0,  100,
  244,    0,    0,    0,    0,    0,   51,   50,   52,    0,
    0,  411,  409,  410,    0,    0,  251,  252,    0,    0,
    0,  221,  222,    0,  223,  224,    0,    0,  229,    0,
    0,    0,    0,  432,  431,    0,    0,  435,    0,  365,
  367,  371,  350,  351,  368,    0,    0,  375,  438,    0,
    0,  239,  240,  241,    0,  237,  249,    0,    0,   91,
   87,    0,    0,  248,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  122,  118,    0,    0,    0,   48,
  408,    0,    0,    0,    0,    0,    0,  217,    0,  218,
  102,    0,    0,    0,    0,    0,  276,    0,    0,    0,
    0,    0,    0,  364,  242,  236,    0,    0,    0,   86,
    0,    0,    0,  174,    0,  112,  170,    0,  162,    0,
  142,  153,  124,  125,  119,  123,  120,  121,  117,  113,
   66,    0,  427,    0,    0,    0,    0,  259,  260,    0,
  254,  258,    0,  261,    0,    0,    0,  214,    0,    0,
  108,    0,  107,    0,    0,    0,    0,    0,  434,   29,
    0,  437,   31,    0,  369,    0,  238,    0,    0,   92,
    0,    0,   98,   97,    0,  245,    0,  246,    0,  138,
    0,  136,    0,    0,  141,    0,  139,    0,    0,    0,
    0,    0,  420,    0,    0,  424,    0,    0,    0,    0,
    0,  278,    0,    0,    0,  270,    0,  279,    0,    0,
    0,    0,    0,  219,  111,    0,  106,    0,    0,   63,
   64,   65,    0,    0,    0,  370,   88,    0,   89,  376,
   99,   96,    0,    0,    0,  127,    0,  135,    0,    0,
  168,    0,  164,  129,    0,  131,    0,    0,    0,  152,
    0,  144,  133,    0,    0,    0,  161,    0,  155,    0,
    0,    0,  428,    0,  430,  429,    0,    0,    0,    0,
  442,    0,    0,    0,    0,    0,  275,  298,  306,    0,
  286,  287,    0,    0,    0,    0,  285,    0,    0,  415,
    0,    0,  266,    0,  264,    0,  262,    0,  109,    0,
    0,    0,  419,  433,  436,  360,    0,  247,  171,    0,
  172,  137,  167,  166,    0,  140,    0,  146,    0,  148,
    0,  150,    0,    0,  157,  159,  160,    0,    0,    0,
    0,    0,  397,  398,    0,  400,  401,  402,  396,    0,
    0,  255,    0,  256,    0,  443,  444,  446,  303,    0,
    0,    0,    0,    0,    0,    0,    0,  274,    0,    0,
    0,  272,   68,    0,  283,  110,    0,    0,    0,   90,
    0,  165,    0,    0,    0,  145,    0,  156,    0,    0,
  422,  425,    0,  421,  399,  391,  395,  175,    0,    0,
    0,  304,  280,  289,  290,  291,  297,  294,  293,  417,
    0,    0,    0,    0,   74,    0,    0,    0,    0,   80,
    0,    0,    0,   78,   73,    0,    0,   59,   62,    0,
    0,    0,    0,    0,  189,    0,  188,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  198,    0,
  183,  184,  190,  187,  191,  180,    0,  199,  173,    0,
    0,    0,    0,  281,    0,    0,  257,  299,    0,  300,
    0,  383,    0,  412,  267,  265,    0,   75,   83,   85,
   84,   76,   79,   81,  347,  348,   77,    0,   72,  284,
    0,  328,  325,    0,    0,  343,  344,    0,    0,  329,
  345,  346,    0,    0,  331,    0,    0,  354,  314,  315,
    0,    0,    0,  181,  307,  323,  324,    0,    0,    0,
  182,  316,  186,    0,    0,  204,  201,    0,  207,  208,
  194,  377,    0,  195,  185,  192,    0,  196,  305,    0,
  179,    0,    0,    0,    0,    0,  423,    0,    0,    0,
    0,   82,   55,  327,    0,    0,    0,    0,    0,    0,
  355,  356,    0,    0,  312,    0,    0,  321,  205,    0,
  203,    0,  357,    0,    0,  193,    0,    0,    0,  158,
    0,  301,    0,    0,  389,  382,  268,    0,  326,    0,
    0,  338,  339,    0,    0,  341,  342,    0,    0,    0,
  313,    0,    0,  322,    0,  200,    0,  378,    0,  197,
    0,    0,    0,    0,  413,  385,  384,    0,   56,   60,
    0,    0,  330,    0,  333,  332,    0,  335,  361,  308,
    0,  309,  317,    0,  318,    0,  202,    0,  147,  149,
  151,    0,    0,   57,   58,    0,    0,    0,    0,    0,
  379,    0,  386,  334,  336,  310,  319,  206,  405,
};
static const YYINT pfctlydgoto[] = {                      2,
   80,  282,  171,  230,  144,  659,  664,  672,  679,  647,
  375,  138,  691,   21,   90,  189,  584,  145,  160,  396,
  445,  161,   22,   23,  182,   24,  611,  655,   52,  686,
  698,  745,  446,  555,  255,  438,  311,  312,  612,  750,
  660,  754,  665,  194,  197,  315,  376,  316,  467,  377,
  550,  378,  457,  458,  471,  744,  633,  366,  501,  367,
  382,  465,  574,  447,  560,  448,  674,  759,  675,  681,
  762,  682,  307,  775,  572,  343,  131,  380,   55,   57,
  179,  449,  614,  721,  162,  163,   76,  200,   77,  225,
  226,  167,  338,  231,  615,  286,  404,  287,  245,  351,
  352,  356,  357,  421,  422,  358,  428,  429,  360,  412,
  413,  353,  295,  538,  616,  617,  618,  687,  732,  280,
  344,  345,  173,  246,  247,  533,  585,  586,  261,  323,
  324,  433,  509,  510,  462,  390,   25,   26,   27,   28,
   29,   30,   31,   32,   33,   34,   35,   36,   37,    3,
  125,  207,  269,  128,  271,  748,  589,  651,  534,  220,
  281,  262,  174,  359,  361,  354,  539,  434,
};
static const YYINT pfctlysindex[] = {                   -37,
    0,  118, 1390,   90,    0,  551,    0,   60, -179, -170,
 -170, -170, 2646,  237, -158,  -44, -139,    1,  205,    0,
  504,  372,  -44,  372,  315,  321,  331,  338,  405,  465,
  480,  501,  578,  618,  637,  645,  660,  664,    0,  681,
  577,  687,  700,  719,  731,    0,    0,  571,  626,  736,
    0,    0,  158,    0,  372, -170,  -44,  -44,  -44,  164,
  -48,  -79, -184, -196,  -61, -269,  403,  411,  -44,  317,
 -170,  371, 1550,  765,  553,  486,  565,    0,   52,    0,
  -44, -170,  380,    0, -154, -154, -154,    0,    0,  237,
  485,  237,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  534,  387,  423,  804,  611,    0,  485,  485,  485,
    0,  484,  492,  888,    0,  516,  888,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  518,
    0,  541,  561,  467,  563,  574,  840,    0,  584,    0,
    0,  893,    0,  374,    0,  -21,  485,    0,  888,    0,
    0,  558,  663,  987,    0,  684,    0,    0,    0,  380,
  553,    0,    0,    0,  -44,  -44,    0,    0,  740,  -44,
  636,    0,    0,   64,    0,    0, 1008,    0,    0,  -44,
  740,  740,  740,    0,    0,  888,  123,    0,  665,    0,
    0,    0,    0,    0,    0, 1009,  679,    0,    0, 1550,
 -170,    0,    0,    0,  188,    0,    0,  888,  558,    0,
    0,    0, 1022,    0,  -68, 1031, 1037, 1038, 1039, 1040,
  488,  695,  696,  698,    0,    0,  987,  -68, -170,    0,
    0,  485,  670,  -96,   42,  485, 1029,    0,  423,    0,
    0,  -86,  485,   42,   42,   42,    0,  888,   57,  888,
   92,  699, 1025,    0,    0,    0,  374,   54, 1052,    0,
 -232,   96,  888,    0,  888,    0,    0,  709,    0,  713,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  740,    0,    6,    6,    6,  485,    0,    0,  888,
    0,    0,  229,    0,  727,  833,  740,    0, 1066,  729,
    0,  888,    0,  -86,  740,  759,  759,  759,    0,    0,
  123,    0,    0,  665,    0,  751,    0,  106,  888,    0,
  733,  742,    0,    0, -232,    0, 1022,    0,  744,    0,
  298,    0, 1089,  533,    0,  602,    0, 1091,  362, 1092,
  412,  871,    0,  888,  761,    0,    0,    0,    0,  740,
  495,    0,  110,  888,  165,    0,  868,    0,  771, 1022,
  -52,  889,   42,    0,    0,  211,    0,   42,  772,    0,
    0,    0,  888,  888,  792,    0,    0,   54,    0,    0,
    0,    0,  888,  191,  888,    0,  709,    0,  317,  317,
    0,  893,    0,    0,  713,    0,   33,   39,   44,    0,
  893,    0,    0,  113,  488,  317,    0,  893,    0,   10,
   56,  137,    0,  883,    0,    0,   42,  213,  888,  888,
    0, 1104, 1106, 1107,  380,  786,    0,    0,    0,   10,
    0,    0,  592, 1128,  793,  797,    0, 1130,  110,    0,
  808,  759,    0,  888,    0,  229,    0,    0,    0,  888,
  215,    0,    0,    0,    0,    0,  888,    0,    0,  744,
    0,    0,    0,    0,  533,    0,  488,    0,  488,    0,
  488,    0,  362,  488,    0,    0,    0,  412,  919,  888,
  219, 1148,    0,    0, -170,    0,    0,    0,    0,  883,
    0,    0,  495,    0,   28,    0,    0,    0,    0,  380,
  221,  806,  811,  815, 1153, 1140,  819,    0, -170,  922,
  826,    0,    0, 1406,    0,    0,   10, 1093, 4438,    0,
  888,    0,  893,  893,  893,    0,  317,    0,   10,  727,
    0,    0,   56,    0,    0,    0,    0,    0,  888,  471,
  888,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   77,    0,  888,  494,    0,  842,  598,  843,  827,    0,
  845,  510,  851,    0,    0, 1406,  888,    0,    0,  241,
  -91,  -82,  951,  952,    0,  953,    0,  119,  124,  510,
  -16,  955,  538,   58,  857,  860, -170,  545,    0,  879,
    0,    0,    0,    0,    0,    0, 4438,    0,    0,  870,
  873,  874,  488,    0, 1022,  888,    0,    0,   28,    0,
  888,    0,  973,    0,    0,    0,  826,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -170,    0,    0,
 1253,    0,    0,  890, 1221,    0,    0,  888,  984,    0,
    0,    0,  888,  994,    0, 1237, 1237,    0,    0,    0,
  888,  891,  554,    0,    0,    0,    0,  888,  896,  589,
    0,    0,    0,  -17,  929,    0,    0, 1237,    0,    0,
    0,    0,  900,    0,    0,    0, 1007,    0,    0, -170,
    0,  893,  893,  893, 1252,  759,    0,  888,  137,  380,
  888,    0,    0,    0,  890,  601,  605,  614,  624, 1550,
    0,    0,  134,  554,    0,  150,  589,    0,    0,  910,
    0,  623,    0,  667,  380,    0,  488,  488,  488,    0,
 1010,    0,  888,  513,    0,    0,    0,  794,    0,  523,
  888,    0,    0,  527,  888,    0,    0,  686,  568,  888,
    0,  579,  888,    0,  893,    0,  929,    0,  915,    0,
 1256, 1260, 1261,  137,    0,    0,    0,  137,    0,    0,
 1297, 1299,    0,  601,    0,    0,  614,    0,    0,    0,
  134,    0,    0,  150,    0,  925,    0, 1271,    0,    0,
    0, 1033,  888,    0,    0,  888,  888,  888,  888, 1272,
    0,  380,    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT pfctlyrindex[] = {                    70,
    0,  539,  717,    0,    0, 1829,    0,    0, 2866,    0,
    0,    0,    0,  980,    0, 1609,    0,    0,    0,    0,
    0, 2469, 4357, 2079,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1898, 2022, 2148,
    0,    0,    0,    0, 3145, 1454,    5,    5,    5,    0,
    0,    0,    0,    0,    0,    0,    0,    0, 1305,    0,
    0,    0,    0, 1318, 1585,    0, 1704,    0,  933, 1728,
 1204,    0,    0,    0, 4303, 4303,  710,    0,    0, 2913,
  912, 4230,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, 3192,    0,   46,   46,   46,
    0,    0,    0,  210,    0,    0,  934,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  115,    0,    0,    0,    0,    0,    0,
    0,  777,    0,    0,    0,    0,   50,    0,  -26,    0,
    0,    0,    0,    0, 1217,    0,    0,    0,    0, 1310,
 4347,    0,    0,    0,  745, 3025,    0,    0, 2231, 4278,
    0,    0,    0,  647,    0,    0,    0,    8,    0, 3315,
  301,  301,  301,    0,    0, 1385,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  503,    0,    0,   74,    0,    0,
    0,  112,  933,    0, 1312,  260,  714, 1030, 1135, 1183,
    0,    0,    0,    0,    0,    0,    7, 1312,    0,    0,
    0,  -43, 3362,    0,   62,  639,    0,    0,    0,    0,
    0,    0, 3526,   -4,   -4,   -4,    0,  509,  240,   91,
  938,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -12,  -13,    0,  934,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, 1058,    0,    0,    0,    0, 3570,    0,    0,  651,
    0,    0,  245,    0,  109, 2346,  928,    0,    0,    0,
    0, 1239,    0,  645, 3690, 1319, 1319, 1319,    0,    0,
    0,    0,    0,    0,    0,  208,    0,   87,   67,    0,
    0,    0,    0,    0,  321,    0,  933,    0,    0,    0,
  938,    0,    0,    0,    0,  938,    0,    0,    0,    0,
    0,    0,    0,   74,    0,    0, 2622, 2622, 2622, 3737,
    0,    0,    0,  620,    0,    0, 2247,    0,    0,  507,
    0, 2510, 1941,    0,    0,  245,    0, 3837,    0,    0,
    0,    0,  509,   91,  291,    0,    0,    0,    0,    0,
    0,    0,  -13,  938,   91,    0,    0,    0,    0,    0,
    0,  557,    0,    0,    0,    0,    0,    0,    0,    0,
  266,    0,    0,    0,    0,    0,    0,  376,    0,  245,
    0,    0,    0,    0,    0,    0, 3881,  647,  143,  176,
    0,    0,  653,  656,    0,  690,    0,    0,    0,  245,
    0,    0,  417,    0,    0,    0,    0,  550,    0,    0,
    0,  617,    0,  934,    0,  245,    0, 1458,    0,  734,
  839, 3968,    0,    0,    0,    0,   67,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   -5,   67,
   87,    0,    0,    0, 2742,    0,    0,    0,    0, 3482,
 4132,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  839,    0,    0,    0,    0,    0,    0,    0,    0,  194,
    0,    0,    0,  962,    0,    0,  245, 1334,  962,    0,
   91,    0,  959,  959,  959,    0,    0,    0,  245,    3,
    0,    0,    0,    0,    0,    0,    0,    0,  143,  236,
   21,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  400,   91,  938,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,  734,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   15,    0,    0,    0,
    0,    0,    0,    0,    4,   67,    0,    0,    0,    0,
  179,    0,  446,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  651, 1090,    0,
    0,    0,  651, 1159,    0, 4000, 4000,    0,    0,    0,
  176,  823,    0,    0,    0,    0,    0,  176,  963,    0,
    0,    0,    0,    0,    0,    0,    0, 4000,    0,    0,
    0,    0,    0,    0,    0,    0, 4100,    0,    0,    0,
    0,  647,  647,  647,    0,   12,    0,   21,    0,    0,
   91,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  989,    0,  938,    0,    0,    0,    0,    0,    0,
 1340,    0,    9,  216,    0,    0,    0,    0,    0,  647,
  143,    0,    0,  647,  143,    0,    0, 1669,  236,   21,
    0,  236,   21,    0,  959,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, 1343,    9,    0,    0,  143,  143,   21,   21,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT pfctlygindex[] = {                     0,
 1611,    0, -199,  319, -358,    0,    0, -471, -186,  754,
  -70,    0,    0, 1352,  199, 1017,    0,    0,    0,    0,
  514, 1284,    0,    0,  958,    0,    0, -464,    0,    0,
  622,  546, -322,    0, -163,    0, -345,    0,    0,    0,
 -610,    0, -640,    0, 1101,  906,  895,    0,    0, -309,
    0,    0, -300,    0,  913,    0,    0, -365,    0,  741,
    0, -472,    0,  907,    0, -453,    0,    0, -647,    0,
    0, -512,    0,    0,    0, -424,    0,  817,    0,   -8,
 1185,  -81,    0, -570,  650, 1160,  724,    0,  596,    0,
 1109,    0,    0, -243,    0, 1133,    0, -319,    0,    0,
  983,    0,  972,    0,  898,    0,    0,  897,    0,    0,
  911,    0,  672,  886,  782,    0,    0, -643,    0,    0,
 1056,    0, 1227, 1156,    0,    0,  818,    0,    0, 1081,
    0, -311,  902,    0,  781, -317,    0,    0,    0, 1404,
 1410,   -3,   -2,    0,    0,    0,    0,    0,    0,    0,
 -162, -119,    0, -176,    0,    0,    0,    0,    0, -178,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 4822
static const YYINT pfctlytable[] = {                     44,
   45,  180,   58,   59,  282,  269,  277,  209,  172,  391,
  392,   70,  414,  416,  243,  259,  115,  105,  229,  277,
  439,  418,  730,  685,  177,  439,  310,  282,  136,  405,
  277,  658,  270,  283,  339,  414,  322,  264,  265,  266,
  663,  731,  170,  127,  268,  365,  277,  117,  277,  233,
  483,  484,  277,  277,  285,  250,  435,  436,  573,  250,
  442,  561,  146,  132,  277,  500,  341,  497,  277,    1,
  464,  269,  487,  176,  124,  760,  470,  755,  489,    1,
  277,  277,  277,  491,  170,    1,  267,  443,  441,  444,
  331,  129,  334,  229,  269,  365,  722,  693,  180,   46,
  219,  228,  181,  347,  258,  751,  277,  219,  278,   74,
  277,  277,  133,  277,  613,  137,  455,  733,  414,   53,
  499,   95,  519,  797,  349,  342,  439,   20,  364,  115,
  103,  502,  374,  277,  277,  219,  456,  177,  362,  219,
  470,  414,  442,  808,  530,  277,  807,  403,  329,  219,
  332,  442,  494,  383,  477,  349,  442,  394,  349,  398,
  541,  388,  172,  348,  711,  349,  442,  559,  393,  443,
  441,  444,  407,  806,  169,  708,  455,  415,  443,  441,
  444,  330,  442,  443,  441,  444,  277,  626,  623,  714,
  371,  277,  613,  443,  441,  444,  456,  562,  134,  631,
  130,  725,  386,  387,  455,   54,  437,  320,  277,  443,
  441,  444,  275,  763,   56,  277,  333,  358,  277,  399,
  346,   79,   92,  250,  456,  480,   78,  587,  276,  250,
  397,  219,  440,  485,  219,  277,  277,  277,  277,  624,
  301,  671,  493,  170,  431,   81,  678,  135,  358,  498,
  749,  358,  761,  116,  450,  439,  219,  269,  219,  513,
  634,  170,  219,  269,  219,   83,  243,  277,  439,  126,
  632,  243,  243,  474,  475,  439,  172,  243,  243,  243,
  243,  809,  282,  478,  101,  481,  372,  654,  308,  309,
  414,  416,  537,  656,  657,  439,  439,  439,  321,  418,
  359,  103,  661,  662,  101,  126,  143,  250,  313,  461,
  253,  122,  250,  250,   82,  479,  284,  314,  250,  514,
  515,  250,  553,  269,   93,    1,    1,    1,    1,  269,
   94,  359,  463,  374,  359,  469,  123,  512,  406,  536,
   95,  219,  537,  552,  531,  563,  684,   96,  250,  269,
  535,  374,  282,  269,    1,    1,    1,  540,  277,  115,
  414,  416,  243,  227,  620,  621,  622,    1,  729,  282,
  282,  277,  439,    4,    5,    6,  101,  414,  101,    1,
  551,  629,  126,    1,    1,    1,    1,    1,  741,  101,
  363,  115,  103,  277,  277,  637,  414,  269,  269,  269,
  269,  269,    1,  250,    1,  277,  277,  250,  743,  394,
   93,    1,  177,  178,   97,    7,  154,  293,  294,  269,
  349,  619,    1,  293,  294,  269,  288,    8,  293,  294,
    1,    9,   10,   11,   12,   13,  269,  269,  227,  627,
  363,  630,  692,  349,  414,  414,  414,  414,  414,  288,
   14,  277,   15,  635,    1,  388,  288,  288,  277,   16,
  288,  453,  454,  583,  451,  452,  414,  650,  610,   93,
   17,  439,  414,  802,   98,  277,  288,  803,   18,  349,
  349,  349,  122,  414,  349,  349,  349,  372,  349,   99,
  234,  349,  349,  728,  177,  178,  556,  293,  294,  349,
   74,   75,   19,  669,  670,  372,  707,  123,  676,  677,
  100,  709,  373,  358,  219,  583,  416,  652,  669,  670,
  570,  453,  454,  737,  738,  739,  699,  277,  277,  387,
  387,  387,  387,  387,  676,  677,  358,  219,  716,  288,
  764,  288,  115,  718,  101,  101,  610,  279,  121,  453,
  454,  723,  277,  767,  461,  769,  219,  387,  726,  292,
  277,  277,  253,  277,  277,  778,  219,  253,  253,  277,
  219,  784,  358,  358,  358,  787,  253,  358,  358,  358,
  791,  358,  292,  794,  358,  358,  796,  101,  742,  292,
  292,  747,  358,  292,  277,  628,  359,  163,  696,  439,
  439,  439,  126,  126,  126,  126,  126,  126,  126,  292,
  112,  219,  126,  126,  126,  439,  439,  439,  636,  359,
  439,  439,  219,  776,  439,  653,  418,  102,  746,  101,
  101,  785,   88,  277,   89,  788,  522,  777,  523,  712,
  792,  221,  414,  795,  126,  219,  103,  783,  250,  222,
  439,  786,  277,  699,  104,  359,  359,  359,  253,  277,
  359,  359,  359,  766,  359,  113,  219,  359,  359,  105,
  165,  250,  292,  106,  292,  359,  223,  224,  288,  277,
  183,  183,  183,  813,  288,  288,  814,  815,  816,  817,
  107,  736,  790,  288,  288,  288,  108,  288,  288,  302,
  288,  142,  143,  793,  288,  288,  288,  768,  288,  109,
  219,  417,  418,  419,  288,  288,  288,  288,  288,  288,
  288,  288,  302,  134,  288,  439,  789,  288,  110,  219,
  746,  439,  439,  302,  288,  392,  392,  392,  392,  392,
  111,  288,  277,  288,  781,  782,  420,  147,  148,  302,
  302,  302,  288,  288,  288,  288,  288,  288,  187,  188,
  439,  424,   84,  392,  177,  178,  277,  425,  426,  288,
  439,  192,  193,  277,  288,  114,  251,  277,  439,  288,
  288,  388,  388,  388,  388,  388,   43,  139,   85,   86,
   87,  288,  288,  277,  416,  140,  427,  380,  288,  288,
  288,  288,  288,  779,  164,  439,  439,  195,  196,  388,
   47,  292,  302,  186,  302,  190,   74,  292,  292,  166,
   48,   49,   50,  380,  380,  380,  292,  292,  292,  168,
  292,  292,  311,  292,  213,  214,  134,  292,  292,  292,
  191,  292,  416,  416,  416,  416,  416,  292,  292,  292,
  292,  292,  292,  292,  292,  311,  277,  292,  277,   51,
  292,   85,   86,   87,  416,  198,  311,  292,  277,  204,
  416,  439,  293,  294,  292,  199,  292,  205,  439,  308,
  309,  101,  311,  311,  311,  292,  292,  292,  292,  292,
  292,  409,  410,  277,  645,  646,  277,  206,  439,  217,
  250,  208,  292,  210,  418,  250,  250,  292,  639,  640,
  641,  250,  292,  292,  250,  439,  439,  411,  780,  277,
  277,  250,  689,  690,  292,  292,  211,  250,  250,  697,
  178,  292,  292,  292,  292,  292,  219,  253,  669,  670,
  250,  439,  227,  250,  250,  311,  212,  311,  215,  250,
  249,  302,  418,  418,  418,  418,  418,  302,  302,  216,
  253,  439,  303,  250,  304,  305,  306,  302,  302,  218,
  302,  302,  320,  676,  677,  406,  406,  302,  302,  302,
  418,  302,  406,  406,  406,  656,  657,  302,  302,  752,
  753,  302,  302,  302,  302,  320,  250,  302,  661,  662,
  302,  380,  380,  380,  277,  277,  320,  302,  756,  757,
  277,  243,  254,  250,  302,  235,  302,  243,  243,  243,
  250,  257,  320,  320,  320,  302,  302,  302,  302,  302,
  302,  439,  439,  277,  277,  277,  277,  445,  445,  128,
  447,  447,  302,  184,  185,  368,  369,  302,  260,  126,
    5,    6,  302,  302,  170,  272,  134,  134,  134,  134,
  134,  134,  134,  273,  302,  302,  134,  134,  134,  318,
  288,  302,  302,  302,  302,  302,  289,  290,  291,  292,
  296,  297,  439,  298,  335,  320,  336,  320,  488,  490,
  492,  311,  340,  350,  381,  495,  496,  355,  134,  337,
  311,  311,  379,  311,  311,  439,  384,    9,   10,   11,
   12,  673,  680,  385,  311,  439,  389,  400,  277,  277,
  311,  311,  337,  395,  311,  311,  311,  401,  284,  408,
  311,  416,  423,  337,  201,  202,  203,  430,  439,  439,
  311,  439,  439,  439,  130,  432,  439,  439,  439,  311,
  439,  459,  128,  439,  439,  460,  466,  473,  543,  476,
  544,  439,  545,  311,  516,  547,  517,  518,  340,  326,
  327,  328,  520,  250,  524,  311,  527,  525,  250,  250,
  311,  526,  529,  232,  250,  311,  549,  250,  554,  253,
  564,  340,  132,  567,  253,  253,  565,  311,  311,  250,
  566,  568,  340,  253,  569,  311,  311,  311,  311,  571,
  463,  643,  337,  243,  337,  588,  253,  253,  503,  504,
  505,  506,  507,  439,  439,  648,  116,  638,  642,  253,
  644,  320,  253,  666,  667,  668,  673,  688,  253,  680,
  320,  320,  694,  320,  320,  695,  508,  250,  250,  250,
  250,  250,  253,  700,  320,  702,  710,  130,  703,  704,
  320,  320,  713,  717,  320,  320,  320,  715,  302,  250,
  320,  277,  317,  719,  653,  250,  720,  724,  277,  325,
  320,  340,  727,  340,  734,  253,  250,  250,  468,  320,
  735,  684,  740,  472,  705,  765,  799,  774,  277,  798,
  800,  801,  253,  320,  673,  132,  804,  680,  805,  253,
  810,  811,  818,  230,  243,  320,  812,  101,  277,   49,
  320,  169,  439,  370,  253,  320,  243,  235,  418,  236,
  237,  238,  239,  240,  241,  242,  101,  320,  320,  116,
  243,  244,  511,   61,  439,  320,  320,  320,  320,  403,
  235,  439,  404,  683,   41,  149,  770,  819,  337,  319,
  532,  277,  521,  277,  250,  528,  625,  337,  337,  758,
  337,  337,  128,  128,  128,  128,  128,  128,  128,  274,
  300,  337,  128,  128,  128,  337,  486,  337,  337,  482,
  546,  337,  337,  337,  548,  542,  558,  337,  701,   39,
  402,  248,  299,  649,  387,  706,   42,  337,  771,  772,
  773,  557,   43,    0,  128,    0,  337,  277,    0,    0,
    0,    0,    0,    0,  277,    0,    0,  340,  277,    0,
  337,    0,    0,    0,    0,    0,  340,  340,  170,  340,
  340,    0,  337,    0,  277,  277,  277,  337,    0,    0,
  340,    0,  337,    0,    0,    0,  340,  340,    0,    0,
  340,  340,  340,   47,  337,  337,  340,   71,    0,    0,
    0,    0,  337,  337,  337,  337,  340,  130,  130,  130,
  130,  130,  130,  130,    0,  340,   47,  130,  130,  130,
   69,    0,    0,    0,   47,    0,    0,   47,    0,  340,
    0,    0,    0,    0,    0,    0,    0,  277,    0,  277,
    0,  340,    0,    0,   40,  277,  340,    0,    0,  130,
    0,  340,    0,    0,    0,  132,  132,  132,  132,  132,
  132,  132,    0,  340,  340,  132,  132,  132,  277,  277,
    0,  340,  340,  340,  340,    0,  243,  243,  243,  243,
  243,  243,  243,    0,    0,    0,    0,  243,  243,  114,
  114,  114,  114,  114,  114,  114,    0,  132,    0,    0,
  114,  114,    0,    0,    0,    0,   47,    0,    0,  235,
    0,    0,  235,  235,  235,  235,  235,    0,  243,    0,
  235,  235,  235,  235,  232,  235,  235,    0,  235,  235,
    0,  116,    0,    0,    0,  235,  235,  235,    0,  235,
  235,    0,  235,  235,  235,  235,  235,  232,    0,  235,
  235,  235,  235,  277,  277,  235,    0,    0,  235,    0,
    0,    0,    0,   91,    0,  235,    0,    0,    0,    0,
    0,    0,  235,    0,  235,   38,    5,    6,    0,    0,
    0,  235,    0,  235,  235,  235,  235,  235,  235,    0,
    0,  277,    0,    0,    0,    0,    0,  118,  119,  120,
  235,    0,    0,    0,    0,  235,    0,    0,    0,  141,
  235,  235,    0,    0,  277,  277,    0,    7,    0,    0,
    0,  175,  235,  235,  575,  576,    0,    0,    0,  235,
  235,  235,    0,    9,   10,   11,   12,  577,    0,    0,
  578,    0,    0,  231,    0,   47,  579,    0,    0,   47,
   47,   47,   47,    0,    0,    0,   47,   47,   47,   47,
  580,   47,   47,    0,   47,   47,  231,    0,    0,    0,
    0,   47,   47,   47,  277,   47,   69,   69,    0,    0,
    0,   47,   47,    0,    0,   47,   47,   47,   47,   69,
    0,   47,   69,  581,   47,    0,    0,    0,   69,  277,
  277,   47,    0,    0,    0,    0,    0,    0,   47,    0,
   47,    0,   69,    0,    0,    0,    0,  582,    0,   47,
   47,   47,   47,   47,   47,  252,  253,    0,    0,    0,
  256,    0,    0,    0,    0,    0,   47,    0,    0,    0,
  263,   47,    0,    0,    0,   69,   47,   47,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   47,   47,
    0,    0,   69,    0,    0,   47,   47,   47,  211,   69,
    0,    0,    0,    0,    0,    0,  232,    0,    0,    0,
  232,  232,  232,  232,    0,  150,    0,  232,  232,  232,
  232,  211,  232,  232,    0,  232,  232,    0,    0,    0,
    0,    0,    0,  232,  232,    0,  232,  232,  151,  232,
  232,  232,  232,  232,    0,    0,  232,  232,  232,  232,
    0,    0,  232,    0,    0,  232,    0,    0,    0,    0,
    0,    0,  232,    0,    0,    0,    0,  213,    0,  232,
    0,  232,    0,    0,  152,  153,  154,    0,  232,  155,
  156,  157,    0,  158,    0,  232,  147,  148,    0,    0,
  213,    0,    0,    0,  159,    0,    0,  232,    0,    0,
    0,    0,  232,    0,    0,    0,    0,  232,    0,    0,
  269,  243,  243,  243,  243,  243,  243,  243,    0,  232,
  232,  243,  243,  243,    0,  231,  232,  232,  232,  231,
  231,  231,  231,  269,  439,    0,  231,  231,  231,  231,
    0,  231,  231,    0,  231,  231,    0,    0,    0,    0,
    0,    0,  231,  231,    0,  231,  231,  439,  231,  231,
  231,  231,  231,    0,    0,  231,  231,  231,  231,    0,
    0,  231,    0,    0,  231,    0,    0,    0,    0,    0,
    0,  231,    0,    0,    0,    0,    0,    0,  231,    0,
  231,  215,    0,  439,  439,  439,    0,  231,  439,  439,
  439,    0,  439,    0,  231,  439,  439,    0,    0,    0,
    0,    0,    0,  439,  215,    0,  231,    0,    0,    0,
    0,  231,    0,    0,    0,    0,  231,    0,    0,    0,
  114,  114,  114,  114,  114,  114,  114,    0,  231,  231,
  116,  114,  114,    0,    0,  231,  231,  231,  225,  211,
  211,  211,  211,  211,  211,  211,  211,  211,    0,    0,
    0,  211,  211,  211,  211,    0,  211,  211,    0,  211,
  211,  225,    0,    0,    0,    0,    0,    0,    0,    0,
  211,  211,    0,  211,  211,  211,  211,  211,    0,    0,
  211,  211,  211,    0,    0,    0,  211,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  211,    0,    0,    0,
    0,    0,    0,    0,    0,  211,    0,  216,  213,  213,
  213,  213,  213,  213,  213,  213,  213,    0,    0,  211,
  213,  213,  213,  213,    0,  213,  213,    0,  213,  213,
  216,  211,    0,    0,    0,    0,  211,    0,    0,  213,
  213,  211,  213,  213,  213,  213,  213,    0,    0,  213,
  213,  213,  269,  211,  211,  213,    0,    0,  269,    0,
    0,  211,  211,    0,    0,  213,    0,    0,    0,    0,
    0,    0,    0,    0,  213,    0,    0,    0,    0,  269,
  269,    0,    0,    0,    0,    0,    0,    0,  213,    0,
  253,    0,  269,    0,    0,  269,    0,    0,    0,    0,
  213,  269,    0,    0,    0,  213,  273,    0,    0,    0,
  213,    0,    0,  253,    0,  269,    0,    0,    0,    0,
    0,    0,  213,  213,    0,    0,    0,    0,    0,  273,
  213,  213,  215,  215,  215,  215,  215,  215,  215,  215,
  215,    0,    0,    0,  215,  215,  215,  215,  269,  215,
  215,    0,  215,  215,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  215,  215,  269,  215,  215,  215,  215,
  215,    0,  269,  215,  215,  215,    0,    0,    0,  215,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  215,
  225,    0,  225,  225,  225,  225,  225,    0,  215,    0,
    0,  225,  225,  225,  225,  263,    0,    0,    0,    0,
    0,    0,  215,    0,    0,    0,    0,  225,  225,  273,
    0,    0,    0,    0,  215,    0,    0,    0,  263,  215,
  225,    0,    0,  225,  215,    0,    0,    0,    0,  225,
    0,    0,    0,    0,    0,    0,  215,  215,    0,    0,
    0,    0,    0,  225,  215,  215,    0,    0,  216,  216,
  216,  216,  216,  216,  216,  216,  216,    0,    0,    0,
  216,  216,  216,  216,    0,  216,  216,    0,  216,  216,
    0,    0,    0,    0,    0,    0,  225,    0,    0,  216,
  216,    0,  216,  216,  216,  216,  216,    0,    0,  216,
  216,  216,    0,  225,    0,  216,    0,    0,    0,    0,
  225,    0,    0,    0,    0,  216,    0,    0,  263,    0,
    0,    0,    0,    0,  216,    0,    0,    0,  225,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  216,    0,
    0,    0,  253,    0,    0,    0,    0,  253,  253,    0,
  216,  225,    0,    0,    0,  216,  253,    0,  273,    0,
  216,    0,    0,    0,  273,  273,    0,    0,  253,  271,
    0,    0,  216,  216,  273,  273,    0,  273,  273,    0,
  216,  216,    0,    0,  273,  273,  273,    0,  273,    0,
    0,    0,  271,    0,  273,  273,    0,    0,  273,  273,
  273,  273,    0,    0,  273,    0,    0,  273,    0,    0,
    0,    0,    0,    0,  273,    0,  253,  253,  253,  253,
  253,  273,    0,  273,    0,    0,    0,    0,    0,    0,
    0,    0,  273,  273,  273,  273,  273,  273,  253,    0,
    0,    0,    0,    0,  253,    0,    0,    0,    0,  273,
    0,    0,    0,    0,  273,  253,  253,    0,    0,  273,
  273,    0,    0,  263,  263,    0,    0,    0,    0,    0,
    0,  273,  273,  263,  263,    0,  263,  263,  273,  273,
  273,  394,  271,  263,  263,  263,    0,  263,    0,    0,
    0,    0,    0,  263,  263,    0,    0,  263,  263,  263,
  263,    0,    0,  263,  394,    0,  263,    0,    0,    0,
    0,    0,    0,  263,    0,    0,    0,    0,    0,    0,
  263,    0,  263,    0,    0,    0,    0,    0,    0,    0,
    0,  263,  263,  263,  263,  263,  263,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  263,    0,
    0,    0,    0,  263,    0,    0,    0,    0,  263,  263,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  263,  263,    0,    0,    0,    0,    0,  263,  263,  263,
  225,    0,  225,  225,  225,  225,  225,  225,    0,    0,
    0,  225,  225,  225,  225,    0,  225,  225,    0,  225,
  225,  390,    0,    0,    0,    0,    0,    0,    0,    0,
  225,  225,    0,  225,  225,  225,  225,  225,    0,    0,
  225,  225,  225,    0,  390,    0,  225,    0,  271,    0,
    0,    0,    0,    0,    0,    0,  225,  271,  271,    0,
  271,  271,    0,    0,    0,  225,    0,  271,  271,  271,
    0,  271,    0,    0,    0,    0,    0,  271,  271,  225,
    0,  271,  271,  271,  271,    0,    0,  271,    0,    0,
  271,  225,    0,    0,    0,    0,  225,  271,    0,    0,
    0,  225,    0,    0,  271,    0,  271,    0,    0,    0,
    0,    0,    0,  225,  225,  271,  271,  271,  271,  271,
  271,  225,  225,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  271,    0,    0,    0,    0,  271,    0,    0,
    0,    0,  271,  271,    0,   54,    0,    0,    0,    0,
    0,    0,    0,  394,  271,  271,    0,    0,  394,  394,
  394,  271,  271,  271,  394,  394,  394,  394,   54,  394,
  394,    0,  394,  394,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  394,    0,    0,    0,    0,    0,  394,
  394,    0,  230,  394,  394,  394,    0,    0,    0,  394,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  394,
    0,    0,    0,    0,    0,  230,    0,    0,  394,    0,
    0,    0,    0,    0,    0,    0,    0,  392,  392,  392,
  392,  392,  394,    0,   60,   61,   62,   63,   64,   65,
    0,   66,    0,   67,  394,   68,   69,   70,    0,  394,
    0,    0,    0,    0,  394,  392,    0,    0,   54,    0,
    0,    0,    0,    0,    0,    0,  394,  394,    0,    0,
    0,    0,    0,  390,  394,  394,    0,   71,  390,  390,
  390,    0,    0,    0,  390,  390,  390,  390,    0,  390,
  390,    0,  390,  390,   72,   73,    0,    0,    0,    0,
    0,    0,    0,  390,  243,    0,    0,    0,    0,  390,
  390,    0,    0,  390,  390,  390,    0,    0,    0,  390,
    0,    0,    0,    0,    0,    0,    0,  243,    0,  390,
    0,    0,    0,    0,    0,    0,    0,    0,  390,    0,
    0,    0,    0,    0,    0,    0,    0,  390,  390,  390,
  390,  390,  390,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  390,    0,    0,    0,    0,  390,
    0,    0,    0,    0,  390,  390,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  390,  390,    0,    0,
    0,    0,    0,    0,  390,  390,   54,   54,   54,    0,
   54,   54,   54,   54,   54,    0,    0,    0,   54,   54,
   54,   54,    0,   54,   54,    0,   54,   54,    0,    0,
    0,    0,    0,    0,  225,    0,    0,   54,    0,    0,
    0,    0,    0,   54,   54,    0,    0,   54,   54,   54,
    0,    0,    0,   54,  230,    0,    0,  225,  230,  230,
  230,  230,    0,   54,    0,  230,  230,  230,  230,    0,
  230,  230,   54,  230,  230,    0,    0,    0,    0,    0,
    0,  228,    0,    0,  230,  230,   54,  230,  230,  230,
  230,  230,    0,    0,  230,  230,  230,    0,   54,    0,
  230,    0,    0,   54,  228,    0,    0,    0,   54,    0,
  230,    0,    0,    0,    0,    0,    0,    0,    0,  230,
   54,   54,    0,    0,    0,    0,    0,    0,   54,   54,
    0,    0,    0,  230,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  230,    0,  225,    0,    0,
  230,    0,    0,    0,    0,  230,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  243,  230,  230,    0,
    0,  243,  243,  243,    0,  230,  230,  243,  243,  243,
  243,    0,  243,  243,    0,  243,  243,    0,    0,    0,
    0,    0,    0,    0,  228,    0,  243,  243,    0,  243,
  243,  243,  243,  243,  243,    0,  243,  243,  243,    0,
    0,    0,  243,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  243,    0,    0,    0,    0,  243,    0,    0,
    0,  243,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  243,    0,    0,    0,    0,
    0,  426,    0,    0,    0,    0,    0,  243,    0,    0,
    0,    0,  243,    0,    0,    0,    0,  243,    0,    0,
    0,    0,    0,    0,  426,    0,    0,    0,    0,  243,
  243,    0,    0,    0,    0,    0,  225,  243,  243,  225,
  225,  225,  225,  225,    0,    0,    0,  225,  225,  225,
  225,    0,  225,  225,    0,  225,  225,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  225,  243,    0,    0,
    0,    0,  225,  225,    0,    0,  225,  225,  225,    0,
    0,    0,  225,  228,    0,    0,    0,  228,  228,  228,
  228,    0,  225,    0,  228,  228,  228,  228,    0,  228,
  228,  225,  228,  228,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  228,    0,  225,    0,    0,    0,  228,
  228,  393,    0,  228,  228,  228,    0,  225,    0,  228,
    0,    0,  225,    0,    0,    0,    0,  225,    0,  228,
    0,    0,    0,    0,  393,    0,    0,    0,  228,  225,
  225,    0,    0,    0,    0,    0,    0,  225,  225,    0,
    0,    0,  228,    0,    0,  250,    0,    0,    0,    0,
    0,    0,    0,    0,  228,    0,    0,    0,    0,  228,
    0,    0,    0,    0,  228,    0,    0,    0,  250,    0,
    0,    0,    0,    0,    0,    0,  228,  228,    0,    0,
    0,    0,    0,    0,  228,  228,  243,    0,    0,  250,
    0,  243,  243,  243,    0,    0,    0,  243,  243,  243,
  243,    0,  243,  243,    0,  243,  243,    0,    0,    0,
    0,    0,  250,    0,    0,    0,  243,    0,    0,    0,
    0,    0,  243,  243,    0,    0,  243,  243,  243,    0,
    0,    0,  243,  426,    0,    0,    0,    0,  426,  426,
  426,    0,  243,    0,  426,  426,  426,  426,    0,  426,
  426,  243,  426,  426,    0,    0,    0,    0,  250,    0,
    0,    0,    0,  426,    0,  243,    0,    0,    0,  426,
  426,    0,    0,  426,  426,  426,    0,  243,    0,  426,
    0,    0,  243,    0,    0,    0,    0,  243,    0,  426,
    0,    0,    0,    0,    0,    0,    0,    0,  426,  243,
  243,    0,    0,    0,    0,    0,    0,  243,  243,  253,
    0,    0,  426,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  426,    0,    0,    0,    0,  426,
    0,    0,  253,    0,  426,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  426,  426,    0,    0,
    0,    0,    0,  393,  426,  426,  253,    0,  393,  393,
  393,    0,    0,    0,  393,  393,  393,  393,    0,  393,
  393,    0,  393,  393,    0,    0,    0,    0,    0,  253,
    0,    0,    0,  393,    0,    0,    0,    0,    0,  393,
  393,    0,    0,  393,  393,  393,    0,  250,    0,  393,
    0,    0,  250,  250,  250,    0,    0,    0,  250,  393,
    0,  250,    0,  250,  250,    0,  250,  250,  393,    0,
    0,    0,  253,    0,    0,    0,    0,  250,    0,    0,
    0,    0,  393,  250,  250,    0,    0,  250,  250,  250,
    0,  250,    0,  250,  393,    0,  250,  250,  250,  393,
    0,    0,  250,  250,  393,  250,  269,  250,  250,    0,
  250,  250,  250,    0,    0,    0,  393,  393,    0,    0,
    0,  250,    0,    0,  393,  393,  250,  250,  250,  269,
    0,  250,  250,  250,    0,    0,    0,  250,  250,    0,
    0,    0,    0,  250,    0,    0,    0,  250,  250,    0,
  269,    0,    0,    0,    0,    0,  250,    0,    0,    0,
  250,  250,    0,    0,    0,    0,    0,    0,  250,  250,
  250,    0,    0,  269,    0,    0,    0,    0,    0,    0,
    0,    0,  250,    0,    0,    0,    0,  250,    0,    0,
    0,    0,  250,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  250,  250,    0,    0,    0,    0,
    0,  253,  250,  250,    0,    0,  253,  253,  253,  269,
    0,    0,    0,    0,    0,  253,    0,  253,  253,    0,
  253,  253,    0,    0,    0,    0,    0,  178,    0,    0,
    0,  253,    0,    0,    0,    0,    0,  253,  253,    0,
    0,  253,  253,  253,    0,    0,    0,  253,  253,    0,
  176,    0,    0,  253,  253,  253,    0,  253,    0,  362,
    0,    0,  253,    0,  253,  253,  253,  253,  253,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  253,    0,
  253,    0,  362,    0,  253,  253,    0,    0,  253,  253,
  253,    0,  253,    0,  253,    0,    0,  253,    0,    0,
    0,    0,  253,    0,  253,    0,    0,    0,    0,    0,
    0,    0,    0,  253,  253,  253,    0,    0,    0,    0,
    0,    0,  253,  253,    0,    0,    0,  253,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  253,
  178,    0,    0,    0,  253,    0,    0,    0,  269,  253,
    0,    0,    0,    0,  269,  269,    0,    0,    0,   51,
    0,  253,  253,    0,  269,  269,    0,  269,  269,  253,
  253,    0,  362,    0,    0,    0,    0,    0,  269,    0,
    0,    0,   51,    0,  269,  269,    0,    0,  269,  269,
  269,  178,  269,    0,  269,    0,    0,    0,  269,  269,
    0,    0,    0,    0,  269,    0,    0,    0,  269,  269,
    0,  269,  269,  269,  176,    0,    0,    0,    0,    0,
    0,    0,  269,    0,    0,    0,    0,  269,  269,  269,
    0,    0,  269,  269,  269,    0,    0,    0,  269,  269,
    0,    0,    0,    0,  269,    0,    0,    0,  269,  269,
    0,    0,    0,    0,    0,    0,    0,  269,    0,    0,
    0,  269,  269,    0,    0,    0,    0,    0,    0,  269,
  269,  269,   51,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  269,    0,    0,  176,    0,  269,  230,
    0,    0,    0,  269,    0,  176,  176,    0,  176,  176,
    0,    0,    0,    0,    0,  269,  269,    0,    0,  176,
    0,    0,  230,  269,  269,  176,  176,    0,  362,  176,
  176,  176,    0,    0,    0,  176,    0,  362,  362,    0,
  362,  362,    0,    0,    0,  176,    0,  243,    0,    0,
    0,  362,    0,    0,  176,    0,    0,  362,  362,    0,
    0,  362,  362,  362,    0,    0,    0,  362,  176,    0,
  243,    0,  406,    0,    0,    0,    0,  362,    0,    0,
  176,    0,    0,    0,    0,  176,  362,    0,    0,    0,
  176,    0,    0,    0,    0,  406,    0,    0,    0,    0,
  362,    0,  176,  176,    0,    0,    0,    0,    0,    0,
  176,  176,  362,    0,    0,    0,  407,  362,    0,    0,
    0,    0,  362,    0,    0,    0,  243,    0,   51,    0,
    0,    0,    0,    0,  362,  362,    0,   51,   51,  407,
   51,   51,  362,  362,    0,    0,    0,    0,    0,  243,
    0,   51,    0,    0,    0,    0,    0,   51,   51,    0,
  176,   51,   51,   51,    0,    0,    0,   51,    0,  176,
  176,    0,  176,  176,    0,    0,    0,   51,    0,    0,
    0,    0,    0,  176,    0,    0,   51,    0,    0,  176,
  176,    0,    0,  176,  176,  176,    0,    0,    0,  176,
   51,    0,    0,    0,    0,    0,    0,    0,    0,  176,
    0,    0,   51,    0,    0,    0,    0,   51,  176,    0,
    0,    0,   51,    0,    0,    0,    0,    0,    0,    0,
  170,    0,  176,    0,   51,   51,    0,    0,    0,    0,
    0,    0,   51,   51,  176,    0,    0,    0,    0,  176,
    0,  230,    0,    0,  176,  230,  230,  230,    0,    0,
    0,    0,  230,  230,  230,  230,  176,  176,    0,    0,
    0,    0,    0,    0,  176,  176,    0,    0,  230,  230,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  230,    0,    0,  230,    0,    0,    0,    0,  243,
  230,    0,    0,    0,  243,  243,    0,    0,    0,    0,
  243,  243,  243,  243,  230,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  406,    0,  243,  243,  406,  406,
  406,    0,    0,    0,    0,  406,  406,  406,  406,  243,
    0,    0,  243,    0,    0,    0,    0,  230,  243,    0,
  406,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  243,    0,  230,    0,    0,    0,  407,    0,
    0,  230,  407,  407,  407,    0,    0,    0,  243,  407,
  407,  407,  407,  243,  243,    0,    0,    0,    0,  243,
  243,  243,  243,    0,  407,  243,    0,    0,  406,  406,
  406,  406,  406,    0,  243,    0,    0,    0,    0,    0,
    0,    0,  243,    0,    0,    0,    0,    0,    0,  243,
  406,    0,    0,    0,    0,    0,  406,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  406,  406,    0,
    0,    0,  407,  407,  407,  407,  407,    0,    0,    0,
    0,    0,  243,  243,  243,  243,  243,    0,    0,    0,
    0,    0,    0,    0,  407,    0,  590,    0,    0,    0,
  407,    0,    0,    0,  243,  591,  592,    0,  593,  594,
  243,  407,  407,    0,    0,    0,    0,    0,    0,  595,
    0,  243,  243,    0,    0,  596,  341,    0,    0,  597,
  598,  599,    0,    0,    0,  600,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  601,    0,    0,    0,    0,
    0,    0,    0,    0,  602,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  603,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  604,    0,    0,    0,    0,  605,    0,    0,    0,    0,
  606,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  607,    0,    0,    0,    0,    0,    0,
  608,  609,
};
static const YYINT pfctlycheck[] = {                      3,
    3,   83,   11,   12,   10,   10,   33,  127,   79,  327,
  328,   10,   10,   10,   10,  194,   10,   10,   40,   33,
   33,   10,   40,   40,   10,  371,  123,   33,  298,  349,
   44,  123,  209,  233,  278,   33,  123,  201,  202,  203,
  123,  685,   33,  123,  207,   40,  225,   56,   40,  169,
  409,  410,   44,   33,  123,   10,  368,  369,  531,   10,
   33,  515,   71,  260,   44,  431,  299,  426,   60,    0,
  123,   10,   40,   82,  123,  723,  386,  718,   40,   10,
   60,   61,   62,   40,   33,  123,  206,   60,   61,   62,
  269,  276,  271,   40,   33,   40,  667,   40,  180,   10,
   44,  123,  257,  282,   41,  716,   40,   44,  228,  264,
   44,  125,  309,   40,  539,  385,   40,  688,   10,   60,
  430,   10,  445,  767,   10,  358,   40,   10,  123,  123,
  123,  432,  123,  125,   44,   44,   60,  123,  302,   44,
  450,   33,   33,  791,  462,  125,  787,  347,  268,   44,
  270,   33,   40,  317,  398,   41,   33,  334,   44,  338,
  480,  325,  233,  283,  637,  285,   33,  513,  331,   60,
   61,   62,  351,  784,  123,  629,   40,  356,   60,   61,
   62,  125,   33,   60,   61,   62,   44,  553,  547,  654,
  310,  125,  617,   60,   61,   62,   60,  520,  260,  123,
  385,  673,  322,   10,   40,  385,  370,  294,   33,   60,
   61,   62,  221,  726,  385,  125,  125,   10,   40,  339,
  125,  266,   24,  267,   60,  404,  385,  537,   41,  273,
  125,   44,  123,  412,   44,   60,   61,   62,   60,  549,
  249,  123,  421,   33,  364,  385,  123,  309,   41,  428,
  715,   44,  724,   55,  374,   40,   44,  262,   44,  438,
  572,   33,   44,  268,   44,   61,  262,  125,   33,   10,
  571,  267,  268,  393,  394,   60,  347,  273,  274,  275,
  276,  794,  288,  403,   40,  405,  277,   47,  385,  386,
  288,  288,  471,  385,  386,   60,   61,   62,  385,  288,
   10,  294,  385,  386,   60,  385,   41,  262,  267,  380,
   10,  360,  267,  268,  314,  125,  385,  276,  273,  439,
  440,  276,  501,  262,   10,  256,  257,  258,  259,  268,
   10,   41,  385,  123,   44,  125,  385,  125,   41,  125,
   10,   44,  521,  125,  464,  125,  363,   10,  299,  288,
  470,  123,  358,  358,  285,  286,  287,  477,  385,  353,
  358,  358,  358,  385,  543,  544,  545,  298,  386,  375,
  376,  385,  385,  256,  257,  258,  375,  375,  375,  310,
  500,  560,  123,  314,  315,  316,  317,  318,  706,  375,
  385,  385,  385,  385,  386,  574,  288,  336,  337,  338,
  339,  340,  333,  358,  335,  385,  386,  358,  709,   10,
  299,  342,  385,  386,   10,  298,   41,  385,  386,  358,
  306,  541,  353,  385,  386,  364,   10,  310,  385,  386,
  361,  314,  315,  316,  317,  318,  375,  376,  385,  559,
  385,  561,  385,  329,  336,  337,  338,  339,  340,   33,
  333,  385,  335,  573,  385,   10,   40,   41,  385,  342,
   44,  385,  386,  534,  300,  301,  358,  587,  539,  358,
  353,  385,  364,  774,   10,  385,   60,  778,  361,  365,
  366,  367,  360,  375,  370,  371,  372,  277,  374,   10,
  172,  377,  378,  680,  385,  386,  505,  385,  386,  385,
  264,  265,  385,  385,  386,  277,  626,  385,  385,  386,
   10,  631,  284,  306,   44,  586,   10,  277,  385,  386,
  529,  385,  386,  702,  703,  704,  608,  385,  386,  336,
  337,  338,  339,  340,  385,  386,  329,   44,  658,  123,
  727,  125,  385,  663,  300,  301,  617,  229,  385,  385,
  386,  671,   44,  732,  625,  734,   44,  364,  678,   10,
  385,  386,  262,  385,  386,  744,   44,  267,  268,  360,
   44,  750,  365,  366,  367,  754,  276,  370,  371,  372,
  759,  374,   33,  762,  377,  378,  765,   10,  708,   40,
   41,  711,  385,   44,  385,  125,  306,   41,  607,  360,
  385,  386,  343,  344,  345,  346,  347,  348,  349,   60,
   40,   44,  353,  354,  355,  350,  351,  352,  125,  329,
  385,  386,   44,  743,  385,  385,   10,   10,  710,  385,
  386,  751,  261,  125,  263,  755,   45,  125,   47,  648,
  760,  268,   41,  763,  385,   44,   10,  125,   10,  276,
  385,  125,   33,  735,   10,  365,  366,  367,  358,   40,
  370,  371,  372,   41,  374,   40,   44,  377,  378,   10,
   75,   33,  123,   10,  125,  385,  303,  304,  262,   60,
   85,   86,   87,  803,  268,  269,  806,  807,  808,  809,
   10,  700,  125,  277,  278,  279,   10,  281,  282,   10,
  284,  385,  386,  125,  288,  289,  290,   41,  292,   10,
   44,  350,  351,  352,  298,  299,  300,  301,  302,  303,
  304,  305,   33,   10,  308,  350,   41,  311,   10,   44,
  812,  356,  357,   44,  318,  336,  337,  338,  339,  340,
   10,  325,  123,  327,  748,  748,  385,  377,  378,   60,
   61,   62,  336,  337,  338,  339,  340,  341,  274,  275,
  385,  350,  259,  364,  385,  386,   33,  356,  357,  353,
  268,  385,  386,   40,  358,   40,  181,   44,  276,  363,
  364,  336,  337,  338,  339,  340,   10,  385,  285,  286,
  287,  375,  376,   60,  288,  385,  385,  259,  382,  383,
  384,  385,  386,   10,   40,  303,  304,  385,  386,  364,
  260,  262,  123,   90,  125,   92,  264,  268,  269,  334,
  270,  271,  272,  285,  286,  287,  277,  278,  279,  265,
  281,  282,   10,  284,  368,  369,  123,  288,  289,  290,
  307,  292,  336,  337,  338,  339,  340,  298,  299,  300,
  301,  302,  303,  304,  305,   33,  123,  308,  125,  309,
  311,  285,  286,  287,  358,   62,   44,  318,  360,  386,
  364,   33,  385,  386,  325,  265,  327,  386,   40,  385,
  386,  375,   60,   61,   62,  336,  337,  338,  339,  340,
  341,  359,  360,  385,  385,  386,  277,   10,   60,   60,
  262,  386,  353,  386,  288,  267,  268,  358,  311,  312,
  313,  273,  363,  364,  276,  359,  360,  385,  125,  300,
  301,   10,  385,  386,  375,  376,  386,  289,  290,  385,
  386,  382,  383,  384,  385,  386,   44,   10,  385,  386,
  302,  385,  385,  305,   33,  123,  386,  125,  386,  311,
  267,  262,  336,  337,  338,  339,  340,  268,  269,  386,
   33,  123,  293,  325,  295,  296,  297,  278,  279,  386,
  281,  282,   10,  385,  386,  266,  267,  288,  289,  290,
  364,  292,  273,  274,  275,  385,  386,  298,  299,  385,
  386,  302,  303,  304,  305,   33,  358,  308,  385,  386,
  311,  285,  286,  287,  385,  386,   44,  318,  385,  386,
  277,  267,  273,  375,  325,  353,  327,  273,  274,  275,
  382,  386,   60,   61,   62,  336,  337,  338,  339,  340,
  341,  385,  386,  300,  301,  385,  386,  385,  386,   10,
  385,  386,  353,   86,   87,  305,  306,  358,   41,  385,
  257,  258,  363,  364,   33,   47,  343,  344,  345,  346,
  347,  348,  349,  385,  375,  376,  353,  354,  355,   41,
   40,  382,  383,  384,  385,  386,   40,   40,   40,   40,
  386,  386,  306,  386,  386,  123,   62,  125,  417,  418,
  419,  269,   41,  385,  262,  424,  425,  385,  385,   10,
  278,  279,  376,  281,  282,  329,   41,  314,  315,  316,
  317,  598,  599,  385,  292,  277,  358,  385,  385,  386,
  298,  299,   33,  373,  302,  303,  304,  386,  385,   41,
  308,   41,   41,   44,  118,  119,  120,  267,  300,  301,
  318,  365,  366,  367,   10,  385,  370,  371,  372,  327,
  374,  284,  123,  377,  378,  385,  268,  386,  487,  368,
  489,  385,  491,  341,   61,  494,   61,   61,   10,  264,
  265,  266,  387,  262,   47,  353,   47,  385,  267,  268,
  358,  385,  375,  167,  273,  363,  268,  276,   41,  262,
  385,   33,   10,   41,  267,  268,  386,  375,  376,  288,
  386,   62,   44,  276,  386,  383,  384,  385,  386,  288,
  385,  385,  123,   10,  125,  123,  289,  290,  336,  337,
  338,  339,  340,  385,  386,  375,   10,  386,  386,  302,
  386,  269,  305,  283,  283,  283,  723,  283,  311,  726,
  278,  279,  386,  281,  282,  386,  364,  336,  337,  338,
  339,  340,  325,  375,  292,  386,  284,  123,  386,  386,
  298,  299,   10,  280,  302,  303,  304,   47,  252,  358,
  308,   33,  256,  280,  385,  364,   40,  387,   40,  263,
  318,  123,  387,  125,  385,  358,  375,  376,  383,  327,
  284,  363,   41,  388,  623,  386,   41,  288,   60,  385,
   41,   41,  375,  341,  791,  123,   10,  794,   10,  382,
  386,   41,   41,  334,   10,  353,  284,  385,  385,   10,
  358,   10,  385,  307,  267,  363,  123,   10,   10,  343,
  344,  345,  346,  347,  348,  349,  375,  375,  376,  123,
  354,  355,  437,   10,  386,  383,  384,  385,  386,   10,
   33,  363,   10,  600,    3,   72,  735,  812,  269,  259,
  466,  123,  450,  125,  180,  459,  550,  278,  279,  720,
  281,  282,  343,  344,  345,  346,  347,  348,  349,  220,
  248,  292,  353,  354,  355,  277,  415,  298,  299,  407,
  493,  302,  303,  304,  498,  485,  511,  308,  617,   10,
  345,  175,  247,  586,  324,  625,    3,  318,  737,  738,
  739,  510,    3,   -1,  385,   -1,  327,   33,   -1,   -1,
   -1,   -1,   -1,   -1,   40,   -1,   -1,  269,   44,   -1,
  341,   -1,   -1,   -1,   -1,   -1,  278,  279,   33,  281,
  282,   -1,  353,   -1,   60,   61,   62,  358,   -1,   -1,
  292,   -1,  363,   -1,   -1,   -1,  298,  299,   -1,   -1,
  302,  303,  304,   10,  375,  376,  308,   10,   -1,   -1,
   -1,   -1,  383,  384,  385,  386,  318,  343,  344,  345,
  346,  347,  348,  349,   -1,  327,   33,  353,  354,  355,
   33,   -1,   -1,   -1,   41,   -1,   -1,   44,   -1,  341,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,  125,
   -1,  353,   -1,   -1,  125,  277,  358,   -1,   -1,  385,
   -1,  363,   -1,   -1,   -1,  343,  344,  345,  346,  347,
  348,  349,   -1,  375,  376,  353,  354,  355,  300,  301,
   -1,  383,  384,  385,  386,   -1,  343,  344,  345,  346,
  347,  348,  349,   -1,   -1,   -1,   -1,  354,  355,  343,
  344,  345,  346,  347,  348,  349,   -1,  385,   -1,   -1,
  354,  355,   -1,   -1,   -1,   -1,  123,   -1,   -1,  262,
   -1,   -1,  265,  266,  267,  268,  269,   -1,  385,   -1,
  273,  274,  275,  276,   10,  278,  279,   -1,  281,  282,
   -1,  385,   -1,   -1,   -1,  288,  289,  290,   -1,  292,
  293,   -1,  295,  296,  297,  298,  299,   33,   -1,  302,
  303,  304,  305,  385,  386,  308,   -1,   -1,  311,   -1,
   -1,   -1,   -1,   23,   -1,  318,   -1,   -1,   -1,   -1,
   -1,   -1,  325,   -1,  327,  256,  257,  258,   -1,   -1,
   -1,  334,   -1,  336,  337,  338,  339,  340,  341,   -1,
   -1,  277,   -1,   -1,   -1,   -1,   -1,   57,   58,   59,
  353,   -1,   -1,   -1,   -1,  358,   -1,   -1,   -1,   69,
  363,  364,   -1,   -1,  300,  301,   -1,  298,   -1,   -1,
   -1,   81,  375,  376,  289,  290,   -1,   -1,   -1,  382,
  383,  384,   -1,  314,  315,  316,  317,  302,   -1,   -1,
  305,   -1,   -1,   10,   -1,  262,  311,   -1,   -1,  266,
  267,  268,  269,   -1,   -1,   -1,  273,  274,  275,  276,
  325,  278,  279,   -1,  281,  282,   33,   -1,   -1,   -1,
   -1,  288,  289,  290,  360,  292,  289,  290,   -1,   -1,
   -1,  298,  299,   -1,   -1,  302,  303,  304,  305,  302,
   -1,  308,  305,  358,  311,   -1,   -1,   -1,  311,  385,
  386,  318,   -1,   -1,   -1,   -1,   -1,   -1,  325,   -1,
  327,   -1,  325,   -1,   -1,   -1,   -1,  382,   -1,  336,
  337,  338,  339,  340,  341,  185,  186,   -1,   -1,   -1,
  190,   -1,   -1,   -1,   -1,   -1,  353,   -1,   -1,   -1,
  200,  358,   -1,   -1,   -1,  358,  363,  364,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,
   -1,   -1,  375,   -1,   -1,  382,  383,  384,   10,  382,
   -1,   -1,   -1,   -1,   -1,   -1,  262,   -1,   -1,   -1,
  266,  267,  268,  269,   -1,  306,   -1,  273,  274,  275,
  276,   33,  278,  279,   -1,  281,  282,   -1,   -1,   -1,
   -1,   -1,   -1,  289,  290,   -1,  292,  293,  329,  295,
  296,  297,  298,  299,   -1,   -1,  302,  303,  304,  305,
   -1,   -1,  308,   -1,   -1,  311,   -1,   -1,   -1,   -1,
   -1,   -1,  318,   -1,   -1,   -1,   -1,   10,   -1,  325,
   -1,  327,   -1,   -1,  365,  366,  367,   -1,  334,  370,
  371,  372,   -1,  374,   -1,  341,  377,  378,   -1,   -1,
   33,   -1,   -1,   -1,  385,   -1,   -1,  353,   -1,   -1,
   -1,   -1,  358,   -1,   -1,   -1,   -1,  363,   -1,   -1,
   10,  343,  344,  345,  346,  347,  348,  349,   -1,  375,
  376,  353,  354,  355,   -1,  262,  382,  383,  384,  266,
  267,  268,  269,   33,  306,   -1,  273,  274,  275,  276,
   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,   -1,
   -1,   -1,  289,  290,   -1,  292,  293,  329,  295,  296,
  297,  298,  299,   -1,   -1,  302,  303,  304,  305,   -1,
   -1,  308,   -1,   -1,  311,   -1,   -1,   -1,   -1,   -1,
   -1,  318,   -1,   -1,   -1,   -1,   -1,   -1,  325,   -1,
  327,   10,   -1,  365,  366,  367,   -1,  334,  370,  371,
  372,   -1,  374,   -1,  341,  377,  378,   -1,   -1,   -1,
   -1,   -1,   -1,  385,   33,   -1,  353,   -1,   -1,   -1,
   -1,  358,   -1,   -1,   -1,   -1,  363,   -1,   -1,   -1,
  343,  344,  345,  346,  347,  348,  349,   -1,  375,  376,
  353,  354,  355,   -1,   -1,  382,  383,  384,   10,  261,
  262,  263,  264,  265,  266,  267,  268,  269,   -1,   -1,
   -1,  273,  274,  275,  276,   -1,  278,  279,   -1,  281,
  282,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  292,  293,   -1,  295,  296,  297,  298,  299,   -1,   -1,
  302,  303,  304,   -1,   -1,   -1,  308,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  318,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  327,   -1,   10,  261,  262,
  263,  264,  265,  266,  267,  268,  269,   -1,   -1,  341,
  273,  274,  275,  276,   -1,  278,  279,   -1,  281,  282,
   33,  353,   -1,   -1,   -1,   -1,  358,   -1,   -1,  292,
  293,  363,  295,  296,  297,  298,  299,   -1,   -1,  302,
  303,  304,  262,  375,  376,  308,   -1,   -1,  268,   -1,
   -1,  383,  384,   -1,   -1,  318,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  327,   -1,   -1,   -1,   -1,  289,
  290,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  341,   -1,
   10,   -1,  302,   -1,   -1,  305,   -1,   -1,   -1,   -1,
  353,  311,   -1,   -1,   -1,  358,   10,   -1,   -1,   -1,
  363,   -1,   -1,   33,   -1,  325,   -1,   -1,   -1,   -1,
   -1,   -1,  375,  376,   -1,   -1,   -1,   -1,   -1,   33,
  383,  384,  261,  262,  263,  264,  265,  266,  267,  268,
  269,   -1,   -1,   -1,  273,  274,  275,  276,  358,  278,
  279,   -1,  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,  293,  375,  295,  296,  297,  298,
  299,   -1,  382,  302,  303,  304,   -1,   -1,   -1,  308,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  318,
  262,   -1,  264,  265,  266,  267,  268,   -1,  327,   -1,
   -1,  273,  274,  275,  276,   10,   -1,   -1,   -1,   -1,
   -1,   -1,  341,   -1,   -1,   -1,   -1,  289,  290,  123,
   -1,   -1,   -1,   -1,  353,   -1,   -1,   -1,   33,  358,
  302,   -1,   -1,  305,  363,   -1,   -1,   -1,   -1,  311,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,   -1,   -1,  325,  383,  384,   -1,   -1,  261,  262,
  263,  264,  265,  266,  267,  268,  269,   -1,   -1,   -1,
  273,  274,  275,  276,   -1,  278,  279,   -1,  281,  282,
   -1,   -1,   -1,   -1,   -1,   -1,  358,   -1,   -1,  292,
  293,   -1,  295,  296,  297,  298,  299,   -1,   -1,  302,
  303,  304,   -1,  375,   -1,  308,   -1,   -1,   -1,   -1,
  382,   -1,   -1,   -1,   -1,  318,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,  327,   -1,   -1,   -1,   10,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  341,   -1,
   -1,   -1,  262,   -1,   -1,   -1,   -1,  267,  268,   -1,
  353,   33,   -1,   -1,   -1,  358,  276,   -1,  262,   -1,
  363,   -1,   -1,   -1,  268,  269,   -1,   -1,  288,   10,
   -1,   -1,  375,  376,  278,  279,   -1,  281,  282,   -1,
  383,  384,   -1,   -1,  288,  289,  290,   -1,  292,   -1,
   -1,   -1,   33,   -1,  298,  299,   -1,   -1,  302,  303,
  304,  305,   -1,   -1,  308,   -1,   -1,  311,   -1,   -1,
   -1,   -1,   -1,   -1,  318,   -1,  336,  337,  338,  339,
  340,  325,   -1,  327,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  336,  337,  338,  339,  340,  341,  358,   -1,
   -1,   -1,   -1,   -1,  364,   -1,   -1,   -1,   -1,  353,
   -1,   -1,   -1,   -1,  358,  375,  376,   -1,   -1,  363,
  364,   -1,   -1,  268,  269,   -1,   -1,   -1,   -1,   -1,
   -1,  375,  376,  278,  279,   -1,  281,  282,  382,  383,
  384,   10,  123,  288,  289,  290,   -1,  292,   -1,   -1,
   -1,   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,
  305,   -1,   -1,  308,   33,   -1,  311,   -1,   -1,   -1,
   -1,   -1,   -1,  318,   -1,   -1,   -1,   -1,   -1,   -1,
  325,   -1,  327,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  336,  337,  338,  339,  340,  341,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  353,   -1,
   -1,   -1,   -1,  358,   -1,   -1,   -1,   -1,  363,  364,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  375,  376,   -1,   -1,   -1,   -1,   -1,  382,  383,  384,
  262,   -1,  264,  265,  266,  267,  268,  269,   -1,   -1,
   -1,  273,  274,  275,  276,   -1,  278,  279,   -1,  281,
  282,   10,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  292,  293,   -1,  295,  296,  297,  298,  299,   -1,   -1,
  302,  303,  304,   -1,   33,   -1,  308,   -1,  269,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  318,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,  327,   -1,  288,  289,  290,
   -1,  292,   -1,   -1,   -1,   -1,   -1,  298,  299,  341,
   -1,  302,  303,  304,  305,   -1,   -1,  308,   -1,   -1,
  311,  353,   -1,   -1,   -1,   -1,  358,  318,   -1,   -1,
   -1,  363,   -1,   -1,  325,   -1,  327,   -1,   -1,   -1,
   -1,   -1,   -1,  375,  376,  336,  337,  338,  339,  340,
  341,  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,   -1,   -1,
   -1,   -1,  363,  364,   -1,   10,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  262,  375,  376,   -1,   -1,  267,  268,
  269,  382,  383,  384,  273,  274,  275,  276,   33,  278,
  279,   -1,  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,   -1,  298,
  299,   -1,   10,  302,  303,  304,   -1,   -1,   -1,  308,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  318,
   -1,   -1,   -1,   -1,   -1,   33,   -1,   -1,  327,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  336,  337,  338,
  339,  340,  341,   -1,  319,  320,  321,  322,  323,  324,
   -1,  326,   -1,  328,  353,  330,  331,  332,   -1,  358,
   -1,   -1,   -1,   -1,  363,  364,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,   -1,   -1,  262,  383,  384,   -1,  362,  267,  268,
  269,   -1,   -1,   -1,  273,  274,  275,  276,   -1,  278,
  279,   -1,  281,  282,  379,  380,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   10,   -1,   -1,   -1,   -1,  298,
  299,   -1,   -1,  302,  303,  304,   -1,   -1,   -1,  308,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   33,   -1,  318,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  327,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  336,  337,  338,
  339,  340,  341,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,
   -1,   -1,   -1,   -1,  363,  364,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,   -1,   -1,   -1,  383,  384,  261,  262,  263,   -1,
  265,  266,  267,  268,  269,   -1,   -1,   -1,  273,  274,
  275,  276,   -1,  278,  279,   -1,  281,  282,   -1,   -1,
   -1,   -1,   -1,   -1,   10,   -1,   -1,  292,   -1,   -1,
   -1,   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,
   -1,   -1,   -1,  308,  262,   -1,   -1,   33,  266,  267,
  268,  269,   -1,  318,   -1,  273,  274,  275,  276,   -1,
  278,  279,  327,  281,  282,   -1,   -1,   -1,   -1,   -1,
   -1,   10,   -1,   -1,  292,  293,  341,  295,  296,  297,
  298,  299,   -1,   -1,  302,  303,  304,   -1,  353,   -1,
  308,   -1,   -1,  358,   33,   -1,   -1,   -1,  363,   -1,
  318,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  327,
  375,  376,   -1,   -1,   -1,   -1,   -1,   -1,  383,  384,
   -1,   -1,   -1,  341,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  353,   -1,  123,   -1,   -1,
  358,   -1,   -1,   -1,   -1,  363,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  262,  375,  376,   -1,
   -1,  267,  268,  269,   -1,  383,  384,  273,  274,  275,
  276,   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  123,   -1,  292,  293,   -1,  295,
  296,  297,  298,  299,   10,   -1,  302,  303,  304,   -1,
   -1,   -1,  308,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  318,   -1,   -1,   -1,   -1,   33,   -1,   -1,
   -1,  327,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  341,   -1,   -1,   -1,   -1,
   -1,   10,   -1,   -1,   -1,   -1,   -1,  353,   -1,   -1,
   -1,   -1,  358,   -1,   -1,   -1,   -1,  363,   -1,   -1,
   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,   -1,  375,
  376,   -1,   -1,   -1,   -1,   -1,  262,  383,  384,  265,
  266,  267,  268,  269,   -1,   -1,   -1,  273,  274,  275,
  276,   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  292,  123,   -1,   -1,
   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,   -1,
   -1,   -1,  308,  262,   -1,   -1,   -1,  266,  267,  268,
  269,   -1,  318,   -1,  273,  274,  275,  276,   -1,  278,
  279,  327,  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,   -1,  341,   -1,   -1,   -1,  298,
  299,   10,   -1,  302,  303,  304,   -1,  353,   -1,  308,
   -1,   -1,  358,   -1,   -1,   -1,   -1,  363,   -1,  318,
   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,  327,  375,
  376,   -1,   -1,   -1,   -1,   -1,   -1,  383,  384,   -1,
   -1,   -1,  341,   -1,   -1,   10,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,
   -1,   -1,   -1,   -1,  363,   -1,   -1,   -1,   33,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,   -1,   -1,   -1,  383,  384,  262,   -1,   -1,   10,
   -1,  267,  268,  269,   -1,   -1,   -1,  273,  274,  275,
  276,   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,
   -1,   -1,   33,   -1,   -1,   -1,  292,   -1,   -1,   -1,
   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,   -1,
   -1,   -1,  308,  262,   -1,   -1,   -1,   -1,  267,  268,
  269,   -1,  318,   -1,  273,  274,  275,  276,   -1,  278,
  279,  327,  281,  282,   -1,   -1,   -1,   -1,  123,   -1,
   -1,   -1,   -1,  292,   -1,  341,   -1,   -1,   -1,  298,
  299,   -1,   -1,  302,  303,  304,   -1,  353,   -1,  308,
   -1,   -1,  358,   -1,   -1,   -1,   -1,  363,   -1,  318,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  327,  375,
  376,   -1,   -1,   -1,   -1,   -1,   -1,  383,  384,   10,
   -1,   -1,  341,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,
   -1,   -1,   33,   -1,  363,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,   -1,   -1,  262,  383,  384,   10,   -1,  267,  268,
  269,   -1,   -1,   -1,  273,  274,  275,  276,   -1,  278,
  279,   -1,  281,  282,   -1,   -1,   -1,   -1,   -1,   33,
   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,   -1,  298,
  299,   -1,   -1,  302,  303,  304,   -1,  262,   -1,  308,
   -1,   -1,  267,  268,  269,   -1,   -1,   -1,  273,  318,
   -1,  276,   -1,  278,  279,   -1,  281,  282,  327,   -1,
   -1,   -1,  123,   -1,   -1,   -1,   -1,  292,   -1,   -1,
   -1,   -1,  341,  298,  299,   -1,   -1,  302,  303,  304,
   -1,  262,   -1,  308,  353,   -1,  267,  268,  269,  358,
   -1,   -1,  273,  318,  363,  276,   10,  278,  279,   -1,
  281,  282,  327,   -1,   -1,   -1,  375,  376,   -1,   -1,
   -1,  292,   -1,   -1,  383,  384,  341,  298,  299,   33,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,  353,   -1,
   -1,   -1,   -1,  358,   -1,   -1,   -1,  318,  363,   -1,
   10,   -1,   -1,   -1,   -1,   -1,  327,   -1,   -1,   -1,
  375,  376,   -1,   -1,   -1,   -1,   -1,   -1,  383,  384,
  341,   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,   -1,   -1,
   -1,   -1,  363,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,   -1,   -1,
   -1,  262,  383,  384,   -1,   -1,  267,  268,  269,  123,
   -1,   -1,   -1,   -1,   -1,  276,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,   -1,   -1,   10,   -1,   -1,
   -1,  292,   -1,   -1,   -1,   -1,   -1,  298,  299,   -1,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,  262,   -1,
   33,   -1,   -1,  267,  268,  269,   -1,  318,   -1,   10,
   -1,   -1,  276,   -1,  278,  279,  327,  281,  282,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  292,   -1,
  341,   -1,   33,   -1,  298,  299,   -1,   -1,  302,  303,
  304,   -1,  353,   -1,  308,   -1,   -1,  358,   -1,   -1,
   -1,   -1,  363,   -1,  318,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  327,  375,  376,   -1,   -1,   -1,   -1,
   -1,   -1,  383,  384,   -1,   -1,   -1,  341,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  353,
  123,   -1,   -1,   -1,  358,   -1,   -1,   -1,  262,  363,
   -1,   -1,   -1,   -1,  268,  269,   -1,   -1,   -1,   10,
   -1,  375,  376,   -1,  278,  279,   -1,  281,  282,  383,
  384,   -1,  123,   -1,   -1,   -1,   -1,   -1,  292,   -1,
   -1,   -1,   33,   -1,  298,  299,   -1,   -1,  302,  303,
  304,   10,  262,   -1,  308,   -1,   -1,   -1,  268,  269,
   -1,   -1,   -1,   -1,  318,   -1,   -1,   -1,  278,  279,
   -1,  281,  282,  327,   33,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  292,   -1,   -1,   -1,   -1,  341,  298,  299,
   -1,   -1,  302,  303,  304,   -1,   -1,   -1,  308,  353,
   -1,   -1,   -1,   -1,  358,   -1,   -1,   -1,  318,  363,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  327,   -1,   -1,
   -1,  375,  376,   -1,   -1,   -1,   -1,   -1,   -1,  383,
  384,  341,  123,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  353,   -1,   -1,  269,   -1,  358,   10,
   -1,   -1,   -1,  363,   -1,  278,  279,   -1,  281,  282,
   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,  292,
   -1,   -1,   33,  383,  384,  298,  299,   -1,  269,  302,
  303,  304,   -1,   -1,   -1,  308,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,  318,   -1,   10,   -1,   -1,
   -1,  292,   -1,   -1,  327,   -1,   -1,  298,  299,   -1,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,  341,   -1,
   33,   -1,   10,   -1,   -1,   -1,   -1,  318,   -1,   -1,
  353,   -1,   -1,   -1,   -1,  358,  327,   -1,   -1,   -1,
  363,   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,   -1,
  341,   -1,  375,  376,   -1,   -1,   -1,   -1,   -1,   -1,
  383,  384,  353,   -1,   -1,   -1,   10,  358,   -1,   -1,
   -1,   -1,  363,   -1,   -1,   -1,   10,   -1,  269,   -1,
   -1,   -1,   -1,   -1,  375,  376,   -1,  278,  279,   33,
  281,  282,  383,  384,   -1,   -1,   -1,   -1,   -1,   33,
   -1,  292,   -1,   -1,   -1,   -1,   -1,  298,  299,   -1,
  269,  302,  303,  304,   -1,   -1,   -1,  308,   -1,  278,
  279,   -1,  281,  282,   -1,   -1,   -1,  318,   -1,   -1,
   -1,   -1,   -1,  292,   -1,   -1,  327,   -1,   -1,  298,
  299,   -1,   -1,  302,  303,  304,   -1,   -1,   -1,  308,
  341,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  318,
   -1,   -1,  353,   -1,   -1,   -1,   -1,  358,  327,   -1,
   -1,   -1,  363,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   33,   -1,  341,   -1,  375,  376,   -1,   -1,   -1,   -1,
   -1,   -1,  383,  384,  353,   -1,   -1,   -1,   -1,  358,
   -1,  262,   -1,   -1,  363,  266,  267,  268,   -1,   -1,
   -1,   -1,  273,  274,  275,  276,  375,  376,   -1,   -1,
   -1,   -1,   -1,   -1,  383,  384,   -1,   -1,  289,  290,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  302,   -1,   -1,  305,   -1,   -1,   -1,   -1,  262,
  311,   -1,   -1,   -1,  267,  268,   -1,   -1,   -1,   -1,
  273,  274,  275,  276,  325,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  262,   -1,  289,  290,  266,  267,
  268,   -1,   -1,   -1,   -1,  273,  274,  275,  276,  302,
   -1,   -1,  305,   -1,   -1,   -1,   -1,  358,  311,   -1,
  288,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  325,   -1,  375,   -1,   -1,   -1,  262,   -1,
   -1,  382,  266,  267,  268,   -1,   -1,   -1,  262,  273,
  274,  275,  276,  267,  268,   -1,   -1,   -1,   -1,  273,
  274,  275,  276,   -1,  288,  358,   -1,   -1,  336,  337,
  338,  339,  340,   -1,  288,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  375,   -1,   -1,   -1,   -1,   -1,   -1,  382,
  358,   -1,   -1,   -1,   -1,   -1,  364,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,
   -1,   -1,  336,  337,  338,  339,  340,   -1,   -1,   -1,
   -1,   -1,  336,  337,  338,  339,  340,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  358,   -1,  269,   -1,   -1,   -1,
  364,   -1,   -1,   -1,  358,  278,  279,   -1,  281,  282,
  364,  375,  376,   -1,   -1,   -1,   -1,   -1,   -1,  292,
   -1,  375,  376,   -1,   -1,  298,  299,   -1,   -1,  302,
  303,  304,   -1,   -1,   -1,  308,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  318,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  327,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  341,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  353,   -1,   -1,   -1,   -1,  358,   -1,   -1,   -1,   -1,
  363,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  376,   -1,   -1,   -1,   -1,   -1,   -1,
  383,  384,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 387
#define YYUNDFTOKEN 558
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const pfctlyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'!'",0,0,0,0,0,0,"'('","')'",0,0,"','","'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,
0,"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"PASS","BLOCK","SCRUB","RETURN","IN","OS","OUT","LOG","QUICK",
"ON","FROM","TO","FLAGS","RETURNRST","RETURNICMP","RETURNICMP6","PROTO","INET",
"INET6","ALL","ANY","ICMPTYPE","ICMP6TYPE","CODE","KEEP","MODULATE","STATE",
"PORT","RDR","NAT","BINAT","ARROW","NODF","MINTTL","ERROR","ALLOWOPTS",
"FASTROUTE","FILENAME","ROUTETO","DUPTO","REPLYTO","NO","LABEL","NOROUTE",
"URPFFAILED","FRAGMENT","USER","GROUP","MAXMSS","MAXIMUM","TTL","TOS","DROP",
"TABLE","REASSEMBLE","FRAGDROP","FRAGCROP","ANCHOR","NATANCHOR","RDRANCHOR",
"BINATANCHOR","SET","OPTIMIZATION","TIMEOUT","LIMIT","LOGINTERFACE",
"BLOCKPOLICY","FAILPOLICY","RANDOMID","REQUIREORDER","SYNPROXY","FINGERPRINTS",
"NOSYNC","DEBUG","SKIP","HOSTID","ANTISPOOF","FOR","INCLUDE","BITMASK","RANDOM",
"SOURCEHASH","ROUNDROBIN","STATICPORT","PROBABILITY","ALTQ","CBQ","CODEL",
"PRIQ","HFSC","FAIRQ","BANDWIDTH","TBRSIZE","LINKSHARE","REALTIME","UPPERLIMIT",
"QUEUE","PRIORITY","QLIMIT","HOGS","BUCKETS","RTABLE","TARGET","INTERVAL",
"LOAD","RULESET_OPTIMIZATION","PRIO","STICKYADDRESS","MAXSRCSTATES",
"MAXSRCNODES","SOURCETRACK","GLOBAL","RULE","MAXSRCCONN","MAXSRCCONNRATE",
"OVERLOAD","FLUSH","SLOPPY","TAGGED","TAG","IFBOUND","FLOATING","STATEPOLICY",
"STATEDEFAULTS","ROUTE","SETTOS","DIVERTTO","DIVERTREPLY","STRING","NUMBER",
"PORTBINARY",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const pfctlyrule[] = {
"$accept : ruleset",
"ruleset :",
"ruleset : ruleset include '\\n'",
"ruleset : ruleset '\\n'",
"ruleset : ruleset option '\\n'",
"ruleset : ruleset scrubrule '\\n'",
"ruleset : ruleset natrule '\\n'",
"ruleset : ruleset binatrule '\\n'",
"ruleset : ruleset pfrule '\\n'",
"ruleset : ruleset anchorrule '\\n'",
"ruleset : ruleset loadrule '\\n'",
"ruleset : ruleset altqif '\\n'",
"ruleset : ruleset queuespec '\\n'",
"ruleset : ruleset varset '\\n'",
"ruleset : ruleset antispoof '\\n'",
"ruleset : ruleset tabledef '\\n'",
"ruleset : '{' fakeanchor '}' '\\n'",
"ruleset : ruleset error '\\n'",
"include : INCLUDE STRING",
"fakeanchor : fakeanchor '\\n'",
"fakeanchor : fakeanchor anchorrule '\\n'",
"fakeanchor : fakeanchor binatrule '\\n'",
"fakeanchor : fakeanchor natrule '\\n'",
"fakeanchor : fakeanchor pfrule '\\n'",
"fakeanchor : fakeanchor error '\\n'",
"optimizer : string",
"option : SET OPTIMIZATION STRING",
"option : SET RULESET_OPTIMIZATION optimizer",
"option : SET TIMEOUT timeout_spec",
"option : SET TIMEOUT '{' optnl timeout_list '}'",
"option : SET LIMIT limit_spec",
"option : SET LIMIT '{' optnl limit_list '}'",
"option : SET LOGINTERFACE stringall",
"option : SET HOSTID number",
"option : SET BLOCKPOLICY DROP",
"option : SET BLOCKPOLICY RETURN",
"option : SET FAILPOLICY DROP",
"option : SET FAILPOLICY RETURN",
"option : SET REQUIREORDER yesno",
"option : SET FINGERPRINTS STRING",
"option : SET STATEPOLICY statelock",
"option : SET DEBUG STRING",
"option : SET SKIP interface",
"option : SET STATEDEFAULTS state_opt_list",
"stringall : STRING",
"stringall : ALL",
"string : STRING string",
"string : STRING",
"varstring : numberstring varstring",
"varstring : numberstring",
"numberstring : NUMBER",
"numberstring : STRING",
"varset : STRING '=' varstring",
"anchorname : STRING",
"anchorname :",
"pfa_anchorlist :",
"pfa_anchorlist : pfa_anchorlist '\\n'",
"pfa_anchorlist : pfa_anchorlist pfrule '\\n'",
"pfa_anchorlist : pfa_anchorlist anchorrule '\\n'",
"$$1 :",
"pfa_anchor : '{' $$1 '\\n' pfa_anchorlist '}'",
"pfa_anchor :",
"anchorrule : ANCHOR anchorname dir quick interface af proto fromto filter_opts pfa_anchor",
"anchorrule : NATANCHOR string interface af proto fromto rtable",
"anchorrule : RDRANCHOR string interface af proto fromto rtable",
"anchorrule : BINATANCHOR string interface af proto fromto rtable",
"loadrule : LOAD ANCHOR string FROM string",
"scrubaction : no SCRUB",
"scrubrule : scrubaction dir logquick interface af proto fromto scrub_opts",
"$$2 :",
"scrub_opts : $$2 scrub_opts_l",
"scrub_opts :",
"scrub_opts_l : scrub_opts_l scrub_opt",
"scrub_opts_l : scrub_opt",
"scrub_opt : NODF",
"scrub_opt : MINTTL NUMBER",
"scrub_opt : MAXMSS NUMBER",
"scrub_opt : SETTOS tos",
"scrub_opt : fragcache",
"scrub_opt : REASSEMBLE STRING",
"scrub_opt : RANDOMID",
"scrub_opt : RTABLE NUMBER",
"scrub_opt : not TAGGED string",
"fragcache : FRAGMENT REASSEMBLE",
"fragcache : FRAGMENT FRAGCROP",
"fragcache : FRAGMENT FRAGDROP",
"antispoof : ANTISPOOF logquick antispoof_ifspc af antispoof_opts",
"antispoof_ifspc : FOR antispoof_if",
"antispoof_ifspc : FOR '{' optnl antispoof_iflst '}'",
"antispoof_iflst : antispoof_if optnl",
"antispoof_iflst : antispoof_iflst comma antispoof_if optnl",
"antispoof_if : if_item",
"antispoof_if : '(' if_item ')'",
"$$3 :",
"antispoof_opts : $$3 antispoof_opts_l",
"antispoof_opts :",
"antispoof_opts_l : antispoof_opts_l antispoof_opt",
"antispoof_opts_l : antispoof_opt",
"antispoof_opt : label",
"antispoof_opt : RTABLE NUMBER",
"not : '!'",
"not :",
"tabledef : TABLE '<' STRING '>' table_opts",
"$$4 :",
"table_opts : $$4 table_opts_l",
"table_opts :",
"table_opts_l : table_opts_l table_opt",
"table_opts_l : table_opt",
"table_opt : STRING",
"table_opt : '{' optnl '}'",
"table_opt : '{' optnl host_list '}'",
"table_opt : FILENAME STRING",
"altqif : ALTQ interface queue_opts QUEUE qassign",
"queuespec : QUEUE STRING interface queue_opts qassign",
"$$5 :",
"queue_opts : $$5 queue_opts_l",
"queue_opts :",
"queue_opts_l : queue_opts_l queue_opt",
"queue_opts_l : queue_opt",
"queue_opt : BANDWIDTH bandwidth",
"queue_opt : PRIORITY NUMBER",
"queue_opt : QLIMIT NUMBER",
"queue_opt : scheduler",
"queue_opt : TBRSIZE NUMBER",
"bandwidth : STRING",
"bandwidth : NUMBER",
"scheduler : CBQ",
"scheduler : CBQ '(' cbqflags_list ')'",
"scheduler : PRIQ",
"scheduler : PRIQ '(' priqflags_list ')'",
"scheduler : HFSC",
"scheduler : HFSC '(' hfsc_opts ')'",
"scheduler : FAIRQ",
"scheduler : FAIRQ '(' fairq_opts ')'",
"scheduler : CODEL",
"scheduler : CODEL '(' codel_opts ')'",
"cbqflags_list : cbqflags_item",
"cbqflags_list : cbqflags_list comma cbqflags_item",
"cbqflags_item : STRING",
"priqflags_list : priqflags_item",
"priqflags_list : priqflags_list comma priqflags_item",
"priqflags_item : STRING",
"$$6 :",
"hfsc_opts : $$6 hfscopts_list",
"hfscopts_list : hfscopts_item",
"hfscopts_list : hfscopts_list comma hfscopts_item",
"hfscopts_item : LINKSHARE bandwidth",
"hfscopts_item : LINKSHARE '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : REALTIME bandwidth",
"hfscopts_item : REALTIME '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : UPPERLIMIT bandwidth",
"hfscopts_item : UPPERLIMIT '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : STRING",
"$$7 :",
"fairq_opts : $$7 fairqopts_list",
"fairqopts_list : fairqopts_item",
"fairqopts_list : fairqopts_list comma fairqopts_item",
"fairqopts_item : LINKSHARE bandwidth",
"fairqopts_item : LINKSHARE '(' bandwidth number bandwidth ')'",
"fairqopts_item : HOGS bandwidth",
"fairqopts_item : BUCKETS number",
"fairqopts_item : STRING",
"$$8 :",
"codel_opts : $$8 codelopts_list",
"codelopts_list : codelopts_item",
"codelopts_list : codelopts_list comma codelopts_item",
"codelopts_item : INTERVAL number",
"codelopts_item : TARGET number",
"codelopts_item : STRING",
"qassign :",
"qassign : qassign_item",
"qassign : '{' optnl qassign_list '}'",
"qassign_list : qassign_item optnl",
"qassign_list : qassign_list comma qassign_item optnl",
"qassign_item : STRING",
"pfrule : action dir logquick interface route af proto fromto filter_opts",
"$$9 :",
"filter_opts : $$9 filter_opts_l",
"filter_opts :",
"filter_opts_l : filter_opts_l filter_opt",
"filter_opts_l : filter_opt",
"filter_opt : USER uids",
"filter_opt : GROUP gids",
"filter_opt : flags",
"filter_opt : icmpspec",
"filter_opt : PRIO NUMBER",
"filter_opt : TOS tos",
"filter_opt : keep",
"filter_opt : FRAGMENT",
"filter_opt : ALLOWOPTS",
"filter_opt : label",
"filter_opt : qname",
"filter_opt : TAG string",
"filter_opt : not TAGGED string",
"filter_opt : PROBABILITY probability",
"filter_opt : RTABLE NUMBER",
"filter_opt : DIVERTTO portplain",
"filter_opt : DIVERTTO STRING PORT portplain",
"filter_opt : DIVERTREPLY",
"filter_opt : filter_sets",
"filter_sets : SET '(' filter_sets_l ')'",
"filter_sets : SET filter_set",
"filter_sets_l : filter_sets_l comma filter_set",
"filter_sets_l : filter_set",
"filter_set : prio",
"prio : PRIO NUMBER",
"prio : PRIO '(' NUMBER comma NUMBER ')'",
"probability : STRING",
"probability : NUMBER",
"action : PASS",
"action : BLOCK blockspec",
"blockspec :",
"blockspec : DROP",
"blockspec : RETURNRST",
"blockspec : RETURNRST '(' TTL NUMBER ')'",
"blockspec : RETURNICMP",
"blockspec : RETURNICMP6",
"blockspec : RETURNICMP '(' reticmpspec ')'",
"blockspec : RETURNICMP6 '(' reticmp6spec ')'",
"blockspec : RETURNICMP '(' reticmpspec comma reticmp6spec ')'",
"blockspec : RETURN",
"reticmpspec : STRING",
"reticmpspec : NUMBER",
"reticmp6spec : STRING",
"reticmp6spec : NUMBER",
"dir :",
"dir : IN",
"dir : OUT",
"quick :",
"quick : QUICK",
"logquick :",
"logquick : log",
"logquick : QUICK",
"logquick : log QUICK",
"logquick : QUICK log",
"log : LOG",
"log : LOG '(' logopts ')'",
"logopts : logopt",
"logopts : logopts comma logopt",
"logopt : ALL",
"logopt : USER",
"logopt : GROUP",
"logopt : TO string",
"interface :",
"interface : ON if_item_not",
"interface : ON '{' optnl if_list '}'",
"if_list : if_item_not optnl",
"if_list : if_list comma if_item_not optnl",
"if_item_not : not if_item",
"if_item : STRING",
"af :",
"af : INET",
"af : INET6",
"proto :",
"proto : PROTO proto_item",
"proto : PROTO '{' optnl proto_list '}'",
"proto_list : proto_item optnl",
"proto_list : proto_list comma proto_item optnl",
"proto_item : protoval",
"protoval : STRING",
"protoval : NUMBER",
"fromto : ALL",
"fromto : from os to",
"os :",
"os : OS xos",
"os : OS '{' optnl os_list '}'",
"xos : STRING",
"os_list : xos optnl",
"os_list : os_list comma xos optnl",
"from :",
"from : FROM ipportspec",
"to :",
"to : TO ipportspec",
"ipportspec : ipspec",
"ipportspec : ipspec PORT portspec",
"ipportspec : PORT portspec",
"optnl : '\\n' optnl",
"optnl :",
"ipspec : ANY",
"ipspec : xhost",
"ipspec : '{' optnl host_list '}'",
"toipspec : TO ipspec",
"toipspec :",
"host_list : ipspec optnl",
"host_list : host_list comma ipspec optnl",
"xhost : not host",
"xhost : not NOROUTE",
"xhost : not URPFFAILED",
"host : STRING",
"host : STRING '-' STRING",
"host : STRING '/' NUMBER",
"host : NUMBER '/' NUMBER",
"host : dynaddr",
"host : dynaddr '/' NUMBER",
"host : '<' STRING '>'",
"number : NUMBER",
"number : STRING",
"dynaddr : '(' STRING ')'",
"portspec : port_item",
"portspec : '{' optnl port_list '}'",
"port_list : port_item optnl",
"port_list : port_list comma port_item optnl",
"port_item : portrange",
"port_item : unaryop portrange",
"port_item : portrange PORTBINARY portrange",
"portplain : numberstring",
"portrange : numberstring",
"uids : uid_item",
"uids : '{' optnl uid_list '}'",
"uid_list : uid_item optnl",
"uid_list : uid_list comma uid_item optnl",
"uid_item : uid",
"uid_item : unaryop uid",
"uid_item : uid PORTBINARY uid",
"uid : STRING",
"uid : NUMBER",
"gids : gid_item",
"gids : '{' optnl gid_list '}'",
"gid_list : gid_item optnl",
"gid_list : gid_list comma gid_item optnl",
"gid_item : gid",
"gid_item : unaryop gid",
"gid_item : gid PORTBINARY gid",
"gid : STRING",
"gid : NUMBER",
"flag : STRING",
"flags : FLAGS flag '/' flag",
"flags : FLAGS '/' flag",
"flags : FLAGS ANY",
"icmpspec : ICMPTYPE icmp_item",
"icmpspec : ICMPTYPE '{' optnl icmp_list '}'",
"icmpspec : ICMP6TYPE icmp6_item",
"icmpspec : ICMP6TYPE '{' optnl icmp6_list '}'",
"icmp_list : icmp_item optnl",
"icmp_list : icmp_list comma icmp_item optnl",
"icmp6_list : icmp6_item optnl",
"icmp6_list : icmp6_list comma icmp6_item optnl",
"icmp_item : icmptype",
"icmp_item : icmptype CODE STRING",
"icmp_item : icmptype CODE NUMBER",
"icmp6_item : icmp6type",
"icmp6_item : icmp6type CODE STRING",
"icmp6_item : icmp6type CODE NUMBER",
"icmptype : STRING",
"icmptype : NUMBER",
"icmp6type : STRING",
"icmp6type : NUMBER",
"tos : STRING",
"tos : NUMBER",
"sourcetrack : SOURCETRACK",
"sourcetrack : SOURCETRACK GLOBAL",
"sourcetrack : SOURCETRACK RULE",
"statelock : IFBOUND",
"statelock : FLOATING",
"keep : NO STATE",
"keep : KEEP STATE state_opt_spec",
"keep : MODULATE STATE state_opt_spec",
"keep : SYNPROXY STATE state_opt_spec",
"flush :",
"flush : FLUSH",
"flush : FLUSH GLOBAL",
"state_opt_spec : '(' state_opt_list ')'",
"state_opt_spec :",
"state_opt_list : state_opt_item",
"state_opt_list : state_opt_list comma state_opt_item",
"state_opt_item : MAXIMUM NUMBER",
"state_opt_item : NOSYNC",
"state_opt_item : MAXSRCSTATES NUMBER",
"state_opt_item : MAXSRCCONN NUMBER",
"state_opt_item : MAXSRCCONNRATE NUMBER '/' NUMBER",
"state_opt_item : OVERLOAD '<' STRING '>' flush",
"state_opt_item : MAXSRCNODES NUMBER",
"state_opt_item : sourcetrack",
"state_opt_item : statelock",
"state_opt_item : SLOPPY",
"state_opt_item : STRING NUMBER",
"label : LABEL STRING",
"qname : QUEUE STRING",
"qname : QUEUE '(' STRING ')'",
"qname : QUEUE '(' STRING comma STRING ')'",
"no :",
"no : NO",
"portstar : numberstring",
"redirspec : host",
"redirspec : '{' optnl redir_host_list '}'",
"redir_host_list : host optnl",
"redir_host_list : redir_host_list comma host optnl",
"redirpool :",
"redirpool : ARROW redirspec",
"redirpool : ARROW redirspec PORT portstar",
"hashkey :",
"hashkey : string",
"$$10 :",
"pool_opts : $$10 pool_opts_l",
"pool_opts :",
"pool_opts_l : pool_opts_l pool_opt",
"pool_opts_l : pool_opt",
"pool_opt : BITMASK",
"pool_opt : RANDOM",
"pool_opt : SOURCEHASH hashkey",
"pool_opt : ROUNDROBIN",
"pool_opt : STATICPORT",
"pool_opt : STICKYADDRESS",
"redirection :",
"redirection : ARROW host",
"redirection : ARROW host PORT portstar",
"natpasslog :",
"natpasslog : PASS",
"natpasslog : PASS log",
"natpasslog : log",
"nataction : no NAT natpasslog",
"nataction : no RDR natpasslog",
"natrule : nataction interface af proto fromto tag tagged rtable redirpool pool_opts",
"binatrule : no BINAT natpasslog interface af proto FROM ipspec toipspec tag tagged rtable redirection",
"tag :",
"tag : TAG STRING",
"tagged :",
"tagged : not TAGGED string",
"rtable :",
"rtable : RTABLE NUMBER",
"route_host : STRING",
"route_host : '(' STRING host ')'",
"route_host_list : route_host optnl",
"route_host_list : route_host_list comma route_host optnl",
"routespec : route_host",
"routespec : '{' optnl route_host_list '}'",
"route :",
"route : FASTROUTE",
"route : ROUTETO routespec pool_opts",
"route : REPLYTO routespec pool_opts",
"route : DUPTO routespec pool_opts",
"timeout_spec : STRING NUMBER",
"timeout_spec : INTERVAL NUMBER",
"timeout_list : timeout_list comma timeout_spec optnl",
"timeout_list : timeout_spec optnl",
"limit_spec : STRING NUMBER",
"limit_list : limit_list comma limit_spec optnl",
"limit_list : limit_spec optnl",
"comma : ','",
"comma :",
"yesno : NO",
"yesno : STRING",
"unaryop : '='",
"unaryop : '!' '='",
"unaryop : '<' '='",
"unaryop : '<'",
"unaryop : '>' '='",
"unaryop : '>'",

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
#line 4579 "parse.y"
#ifdef __rtems__
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static YYSTACKDATA yystack);
#endif /* __rtems__ */

int
yyerror(const char *fmt, ...)
{
	va_list		 ap;

	file->errors++;
	va_start(ap, fmt);
	fprintf(stderr, "%s:%d: ", file->name, yylval.lineno);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	return (0);
}

int
disallow_table(struct node_host *h, const char *fmt)
{
	for (; h != NULL; h = h->next)
		if (h->addr.type == PF_ADDR_TABLE) {
			yyerror(fmt, h->addr.v.tblname);
			return (1);
		}
	return (0);
}

int
disallow_urpf_failed(struct node_host *h, const char *fmt)
{
	for (; h != NULL; h = h->next)
		if (h->addr.type == PF_ADDR_URPFFAILED) {
			yyerror(fmt);
			return (1);
		}
	return (0);
}

int
disallow_alias(struct node_host *h, const char *fmt)
{
	for (; h != NULL; h = h->next)
		if (DYNIF_MULTIADDR(h->addr)) {
			yyerror(fmt, h->addr.v.tblname);
			return (1);
		}
	return (0);
}

int
rule_consistent(struct pf_rule *r, int anchor_call)
{
	int	problems = 0;

	switch (r->action) {
	case PF_PASS:
	case PF_DROP:
	case PF_SCRUB:
	case PF_NOSCRUB:
		problems = filter_consistent(r, anchor_call);
		break;
	case PF_NAT:
	case PF_NONAT:
		problems = nat_consistent(r);
		break;
	case PF_RDR:
	case PF_NORDR:
		problems = rdr_consistent(r);
		break;
	case PF_BINAT:
	case PF_NOBINAT:
	default:
		break;
	}
	return (problems);
}

int
filter_consistent(struct pf_rule *r, int anchor_call)
{
	int	problems = 0;

	if (r->proto != IPPROTO_TCP && r->proto != IPPROTO_UDP &&
	    (r->src.port_op || r->dst.port_op)) {
		yyerror("port only applies to tcp/udp");
		problems++;
	}
	if (r->proto != IPPROTO_ICMP && r->proto != IPPROTO_ICMPV6 &&
	    (r->type || r->code)) {
		yyerror("icmp-type/code only applies to icmp");
		problems++;
	}
	if (!r->af && (r->type || r->code)) {
		yyerror("must indicate address family with icmp-type/code");
		problems++;
	}
	if (r->overload_tblname[0] &&
	    r->max_src_conn == 0 && r->max_src_conn_rate.seconds == 0) {
		yyerror("'overload' requires 'max-src-conn' "
		    "or 'max-src-conn-rate'");
		problems++;
	}
	if ((r->proto == IPPROTO_ICMP && r->af == AF_INET6) ||
	    (r->proto == IPPROTO_ICMPV6 && r->af == AF_INET)) {
		yyerror("proto %s doesn't match address family %s",
		    r->proto == IPPROTO_ICMP ? "icmp" : "icmp6",
		    r->af == AF_INET ? "inet" : "inet6");
		problems++;
	}
	if (r->allow_opts && r->action != PF_PASS) {
		yyerror("allow-opts can only be specified for pass rules");
		problems++;
	}
	if (r->rule_flag & PFRULE_FRAGMENT && (r->src.port_op ||
	    r->dst.port_op || r->flagset || r->type || r->code)) {
		yyerror("fragments can be filtered only on IP header fields");
		problems++;
	}
	if (r->rule_flag & PFRULE_RETURNRST && r->proto != IPPROTO_TCP) {
		yyerror("return-rst can only be applied to TCP rules");
		problems++;
	}
	if (r->max_src_nodes && !(r->rule_flag & PFRULE_RULESRCTRACK)) {
		yyerror("max-src-nodes requires 'source-track rule'");
		problems++;
	}
	if (r->action == PF_DROP && r->keep_state) {
		yyerror("keep state on block rules doesn't make sense");
		problems++;
	}
	if (r->rule_flag & PFRULE_STATESLOPPY &&
	    (r->keep_state == PF_STATE_MODULATE ||
	    r->keep_state == PF_STATE_SYNPROXY)) {
		yyerror("sloppy state matching cannot be used with "
		    "synproxy state or modulate state");
		problems++;
	}
	return (-problems);
}

int
nat_consistent(struct pf_rule *r)
{
	return (0);	/* yeah! */
}

int
rdr_consistent(struct pf_rule *r)
{
	int			 problems = 0;

	if (r->proto != IPPROTO_TCP && r->proto != IPPROTO_UDP) {
		if (r->src.port_op) {
			yyerror("src port only applies to tcp/udp");
			problems++;
		}
		if (r->dst.port_op) {
			yyerror("dst port only applies to tcp/udp");
			problems++;
		}
		if (r->rpool.proxy_port[0]) {
			yyerror("rpool port only applies to tcp/udp");
			problems++;
		}
	}
	if (r->dst.port_op &&
	    r->dst.port_op != PF_OP_EQ && r->dst.port_op != PF_OP_RRG) {
		yyerror("invalid port operator for rdr destination port");
		problems++;
	}
	return (-problems);
}

int
process_tabledef(char *name, struct table_opts *opts)
{
	struct pfr_buffer	 ab;
	struct node_tinit	*ti;

	bzero(&ab, sizeof(ab));
	ab.pfrb_type = PFRB_ADDRS;
	SIMPLEQ_FOREACH(ti, &opts->init_nodes, entries) {
		if (ti->file)
			if (pfr_buf_load(&ab, ti->file, 0, append_addr)) {
				if (errno)
					yyerror("cannot load \"%s\": %s",
					    ti->file, strerror(errno));
				else
					yyerror("file \"%s\" contains bad data",
					    ti->file);
				goto _error;
			}
		if (ti->host)
			if (append_addr_host(&ab, ti->host, 0, 0)) {
				yyerror("cannot create address buffer: %s",
				    strerror(errno));
				goto _error;
			}
	}
	if (pf->opts & PF_OPT_VERBOSE)
		print_tabledef(name, opts->flags, opts->init_addr,
		    &opts->init_nodes);
	if (!(pf->opts & PF_OPT_NOACTION) &&
	    pfctl_define_table(name, opts->flags, opts->init_addr,
	    pf->anchor->name, &ab, pf->anchor->ruleset.tticket)) {
		yyerror("cannot define table %s: %s", name,
		    pfr_strerror(errno));
		goto _error;
	}
	pf->tdirty = 1;
	pfr_buf_clear(&ab);
	return (0);
_error:
	pfr_buf_clear(&ab);
	return (-1);
}

struct keywords {
	const char	*k_name;
	int		 k_val;
};

/* macro gore, but you should've seen the prior indentation nightmare... */

#define FREE_LIST(T,r) \
	do { \
		T *p, *node = r; \
		while (node != NULL) { \
			p = node; \
			node = node->next; \
			free(p); \
		} \
	} while (0)

#define LOOP_THROUGH(T,n,r,C) \
	do { \
		T *n; \
		if (r == NULL) { \
			r = calloc(1, sizeof(T)); \
			if (r == NULL) \
				err(1, "LOOP: calloc"); \
			r->next = NULL; \
		} \
		n = r; \
		while (n != NULL) { \
			do { \
				C; \
			} while (0); \
			n = n->next; \
		} \
	} while (0)

void
expand_label_str(char *label, size_t len, const char *srch, const char *repl)
{
	char *tmp;
	char *p, *q;

	if ((tmp = calloc(1, len)) == NULL)
		err(1, "expand_label_str: calloc");
	p = q = label;
	while ((q = strstr(p, srch)) != NULL) {
		*q = '\0';
		if ((strlcat(tmp, p, len) >= len) ||
		    (strlcat(tmp, repl, len) >= len))
			errx(1, "expand_label: label too long");
		q += strlen(srch);
		p = q;
	}
	if (strlcat(tmp, p, len) >= len)
		errx(1, "expand_label: label too long");
	strlcpy(label, tmp, len);	/* always fits */
	free(tmp);
}

void
expand_label_if(const char *name, char *label, size_t len, const char *ifname)
{
	if (strstr(label, name) != NULL) {
		if (!*ifname)
			expand_label_str(label, len, name, "any");
		else
			expand_label_str(label, len, name, ifname);
	}
}

void
expand_label_addr(const char *name, char *label, size_t len, sa_family_t af,
    struct node_host *h)
{
	char tmp[64], tmp_not[66];

	if (strstr(label, name) != NULL) {
		switch (h->addr.type) {
		case PF_ADDR_DYNIFTL:
			snprintf(tmp, sizeof(tmp), "(%s)", h->addr.v.ifname);
			break;
		case PF_ADDR_TABLE:
			snprintf(tmp, sizeof(tmp), "<%s>", h->addr.v.tblname);
			break;
		case PF_ADDR_NOROUTE:
			snprintf(tmp, sizeof(tmp), "no-route");
			break;
		case PF_ADDR_URPFFAILED:
			snprintf(tmp, sizeof(tmp), "urpf-failed");
			break;
		case PF_ADDR_ADDRMASK:
			if (!af || (PF_AZERO(&h->addr.v.a.addr, af) &&
			    PF_AZERO(&h->addr.v.a.mask, af)))
				snprintf(tmp, sizeof(tmp), "any");
			else {
				char	a[48];
				int	bits;

				if (inet_ntop(af, &h->addr.v.a.addr, a,
				    sizeof(a)) == NULL)
					snprintf(tmp, sizeof(tmp), "?");
				else {
					bits = unmask(&h->addr.v.a.mask, af);
					if ((af == AF_INET && bits < 32) ||
					    (af == AF_INET6 && bits < 128))
						snprintf(tmp, sizeof(tmp),
						    "%s/%d", a, bits);
					else
						snprintf(tmp, sizeof(tmp),
						    "%s", a);
				}
			}
			break;
		default:
			snprintf(tmp, sizeof(tmp), "?");
			break;
		}

		if (h->not) {
			snprintf(tmp_not, sizeof(tmp_not), "! %s", tmp);
			expand_label_str(label, len, name, tmp_not);
		} else
			expand_label_str(label, len, name, tmp);
	}
}

void
expand_label_port(const char *name, char *label, size_t len,
    struct node_port *port)
{
	char	 a1[6], a2[6], op[13] = "";

	if (strstr(label, name) != NULL) {
		snprintf(a1, sizeof(a1), "%u", ntohs(port->port[0]));
		snprintf(a2, sizeof(a2), "%u", ntohs(port->port[1]));
		if (!port->op)
			;
		else if (port->op == PF_OP_IRG)
			snprintf(op, sizeof(op), "%s><%s", a1, a2);
		else if (port->op == PF_OP_XRG)
			snprintf(op, sizeof(op), "%s<>%s", a1, a2);
		else if (port->op == PF_OP_EQ)
			snprintf(op, sizeof(op), "%s", a1);
		else if (port->op == PF_OP_NE)
			snprintf(op, sizeof(op), "!=%s", a1);
		else if (port->op == PF_OP_LT)
			snprintf(op, sizeof(op), "<%s", a1);
		else if (port->op == PF_OP_LE)
			snprintf(op, sizeof(op), "<=%s", a1);
		else if (port->op == PF_OP_GT)
			snprintf(op, sizeof(op), ">%s", a1);
		else if (port->op == PF_OP_GE)
			snprintf(op, sizeof(op), ">=%s", a1);
		expand_label_str(label, len, name, op);
	}
}

void
expand_label_proto(const char *name, char *label, size_t len, u_int8_t proto)
{
	struct protoent *pe;
	char n[4];

	if (strstr(label, name) != NULL) {
		pe = getprotobynumber(proto);
		if (pe != NULL)
			expand_label_str(label, len, name, pe->p_name);
		else {
			snprintf(n, sizeof(n), "%u", proto);
			expand_label_str(label, len, name, n);
		}
	}
}

void
expand_label_nr(const char *name, char *label, size_t len)
{
	char n[11];

	if (strstr(label, name) != NULL) {
		snprintf(n, sizeof(n), "%u", pf->anchor->match);
		expand_label_str(label, len, name, n);
	}
}

void
expand_label(char *label, size_t len, const char *ifname, sa_family_t af,
    struct node_host *src_host, struct node_port *src_port,
    struct node_host *dst_host, struct node_port *dst_port,
    u_int8_t proto)
{
	expand_label_if("$if", label, len, ifname);
	expand_label_addr("$srcaddr", label, len, af, src_host);
	expand_label_addr("$dstaddr", label, len, af, dst_host);
	expand_label_port("$srcport", label, len, src_port);
	expand_label_port("$dstport", label, len, dst_port);
	expand_label_proto("$proto", label, len, proto);
	expand_label_nr("$nr", label, len);
}

int
expand_altq(struct pf_altq *a, struct node_if *interfaces,
    struct node_queue *nqueues, struct node_queue_bw bwspec,
    struct node_queue_opt *opts)
{
	struct pf_altq		 pa, pb;
	char			 qname[PF_QNAME_SIZE];
	struct node_queue	*n;
	struct node_queue_bw	 bw;
	int			 errs = 0;

	if ((pf->loadopt & PFCTL_FLAG_ALTQ) == 0) {
		FREE_LIST(struct node_if, interfaces);
		if (nqueues)
			FREE_LIST(struct node_queue, nqueues);
		return (0);
	}

	LOOP_THROUGH(struct node_if, interface, interfaces,
		memcpy(&pa, a, sizeof(struct pf_altq));
		if (strlcpy(pa.ifname, interface->ifname,
		    sizeof(pa.ifname)) >= sizeof(pa.ifname))
			errx(1, "expand_altq: strlcpy");

		if (interface->not) {
			yyerror("altq on ! <interface> is not supported");
			errs++;
		} else {
			if (eval_pfaltq(pf, &pa, &bwspec, opts))
				errs++;
			else
				if (pfctl_add_altq(pf, &pa))
					errs++;

			if (pf->opts & PF_OPT_VERBOSE) {
				print_altq(&pf->paltq->altq, 0,
				    &bwspec, opts);
				if (nqueues && nqueues->tail) {
					printf("queue { ");
					LOOP_THROUGH(struct node_queue, queue,
					    nqueues,
						printf("%s ",
						    queue->queue);
					);
					printf("}");
				}
				printf("\n");
			}

			if (pa.scheduler == ALTQT_CBQ ||
			    pa.scheduler == ALTQT_HFSC) {
				/* now create a root queue */
				memset(&pb, 0, sizeof(struct pf_altq));
				if (strlcpy(qname, "root_", sizeof(qname)) >=
				    sizeof(qname))
					errx(1, "expand_altq: strlcpy");
				if (strlcat(qname, interface->ifname,
				    sizeof(qname)) >= sizeof(qname))
					errx(1, "expand_altq: strlcat");
				if (strlcpy(pb.qname, qname,
				    sizeof(pb.qname)) >= sizeof(pb.qname))
					errx(1, "expand_altq: strlcpy");
				if (strlcpy(pb.ifname, interface->ifname,
				    sizeof(pb.ifname)) >= sizeof(pb.ifname))
					errx(1, "expand_altq: strlcpy");
				pb.qlimit = pa.qlimit;
				pb.scheduler = pa.scheduler;
				bw.bw_absolute = pa.ifbandwidth;
				bw.bw_percent = 0;
				if (eval_pfqueue(pf, &pb, &bw, opts))
					errs++;
				else
					if (pfctl_add_altq(pf, &pb))
						errs++;
			}

			LOOP_THROUGH(struct node_queue, queue, nqueues,
				n = calloc(1, sizeof(struct node_queue));
				if (n == NULL)
					err(1, "expand_altq: calloc");
				if (pa.scheduler == ALTQT_CBQ ||
				    pa.scheduler == ALTQT_HFSC)
					if (strlcpy(n->parent, qname,
					    sizeof(n->parent)) >=
					    sizeof(n->parent))
						errx(1, "expand_altq: strlcpy");
				if (strlcpy(n->queue, queue->queue,
				    sizeof(n->queue)) >= sizeof(n->queue))
					errx(1, "expand_altq: strlcpy");
				if (strlcpy(n->ifname, interface->ifname,
				    sizeof(n->ifname)) >= sizeof(n->ifname))
					errx(1, "expand_altq: strlcpy");
				n->scheduler = pa.scheduler;
				n->next = NULL;
				n->tail = n;
				if (queues == NULL)
					queues = n;
				else {
					queues->tail->next = n;
					queues->tail = n;
				}
			);
		}
	);
	FREE_LIST(struct node_if, interfaces);
	if (nqueues)
		FREE_LIST(struct node_queue, nqueues);

	return (errs);
}

int
expand_queue(struct pf_altq *a, struct node_if *interfaces,
    struct node_queue *nqueues, struct node_queue_bw bwspec,
    struct node_queue_opt *opts)
{
	struct node_queue	*n, *nq;
	struct pf_altq		 pa;
	u_int8_t		 found = 0;
	u_int8_t		 errs = 0;

	if ((pf->loadopt & PFCTL_FLAG_ALTQ) == 0) {
		FREE_LIST(struct node_queue, nqueues);
		return (0);
	}

	if (queues == NULL) {
		yyerror("queue %s has no parent", a->qname);
		FREE_LIST(struct node_queue, nqueues);
		return (1);
	}

	LOOP_THROUGH(struct node_if, interface, interfaces,
		LOOP_THROUGH(struct node_queue, tqueue, queues,
			if (!strncmp(a->qname, tqueue->queue, PF_QNAME_SIZE) &&
			    (interface->ifname[0] == 0 ||
			    (!interface->not && !strncmp(interface->ifname,
			    tqueue->ifname, IFNAMSIZ)) ||
			    (interface->not && strncmp(interface->ifname,
			    tqueue->ifname, IFNAMSIZ)))) {
				/* found ourself in queues */
				found++;

				memcpy(&pa, a, sizeof(struct pf_altq));

				if (pa.scheduler != ALTQT_NONE &&
				    pa.scheduler != tqueue->scheduler) {
					yyerror("exactly one scheduler type "
					    "per interface allowed");
					return (1);
				}
				pa.scheduler = tqueue->scheduler;

				/* scheduler dependent error checking */
				switch (pa.scheduler) {
				case ALTQT_PRIQ:
					if (nqueues != NULL) {
						yyerror("priq queues cannot "
						    "have child queues");
						return (1);
					}
					if (bwspec.bw_absolute > 0 ||
					    bwspec.bw_percent < 100) {
						yyerror("priq doesn't take "
						    "bandwidth");
						return (1);
					}
					break;
				default:
					break;
				}

				if (strlcpy(pa.ifname, tqueue->ifname,
				    sizeof(pa.ifname)) >= sizeof(pa.ifname))
					errx(1, "expand_queue: strlcpy");
				if (strlcpy(pa.parent, tqueue->parent,
				    sizeof(pa.parent)) >= sizeof(pa.parent))
					errx(1, "expand_queue: strlcpy");

				if (eval_pfqueue(pf, &pa, &bwspec, opts))
					errs++;
				else
					if (pfctl_add_altq(pf, &pa))
						errs++;

				for (nq = nqueues; nq != NULL; nq = nq->next) {
					if (!strcmp(a->qname, nq->queue)) {
						yyerror("queue cannot have "
						    "itself as child");
						errs++;
						continue;
					}
					n = calloc(1,
					    sizeof(struct node_queue));
					if (n == NULL)
						err(1, "expand_queue: calloc");
					if (strlcpy(n->parent, a->qname,
					    sizeof(n->parent)) >=
					    sizeof(n->parent))
						errx(1, "expand_queue strlcpy");
					if (strlcpy(n->queue, nq->queue,
					    sizeof(n->queue)) >=
					    sizeof(n->queue))
						errx(1, "expand_queue strlcpy");
					if (strlcpy(n->ifname, tqueue->ifname,
					    sizeof(n->ifname)) >=
					    sizeof(n->ifname))
						errx(1, "expand_queue strlcpy");
					n->scheduler = tqueue->scheduler;
					n->next = NULL;
					n->tail = n;
					if (queues == NULL)
						queues = n;
					else {
						queues->tail->next = n;
						queues->tail = n;
					}
				}
				if ((pf->opts & PF_OPT_VERBOSE) && (
				    (found == 1 && interface->ifname[0] == 0) ||
				    (found > 0 && interface->ifname[0] != 0))) {
					print_queue(&pf->paltq->altq, 0,
					    &bwspec, interface->ifname[0] != 0,
					    opts);
					if (nqueues && nqueues->tail) {
						printf("{ ");
						LOOP_THROUGH(struct node_queue,
						    queue, nqueues,
							printf("%s ",
							    queue->queue);
						);
						printf("}");
					}
					printf("\n");
				}
			}
		);
	);

	FREE_LIST(struct node_queue, nqueues);
	FREE_LIST(struct node_if, interfaces);

	if (!found) {
		yyerror("queue %s has no parent", a->qname);
		errs++;
	}

	if (errs)
		return (1);
	else
		return (0);
}

void
expand_rule(struct pf_rule *r,
    struct node_if *interfaces, struct node_host *rpool_hosts,
    struct node_proto *protos, struct node_os *src_oses,
    struct node_host *src_hosts, struct node_port *src_ports,
    struct node_host *dst_hosts, struct node_port *dst_ports,
    struct node_uid *uids, struct node_gid *gids, struct node_icmp *icmp_types,
    const char *anchor_call)
{
	sa_family_t		 af = r->af;
	int			 added = 0, error = 0;
	char			 ifname[IF_NAMESIZE];
	char			 label[PF_RULE_LABEL_SIZE];
	char			 tagname[PF_TAG_NAME_SIZE];
	char			 match_tagname[PF_TAG_NAME_SIZE];
	struct pf_pooladdr	*pa;
	struct node_host	*h;
	u_int8_t		 flags, flagset, keep_state;

	if (strlcpy(label, r->label, sizeof(label)) >= sizeof(label))
		errx(1, "expand_rule: strlcpy");
	if (strlcpy(tagname, r->tagname, sizeof(tagname)) >= sizeof(tagname))
		errx(1, "expand_rule: strlcpy");
	if (strlcpy(match_tagname, r->match_tagname, sizeof(match_tagname)) >=
	    sizeof(match_tagname))
		errx(1, "expand_rule: strlcpy");
	flags = r->flags;
	flagset = r->flagset;
	keep_state = r->keep_state;

	LOOP_THROUGH(struct node_if, interface, interfaces,
	LOOP_THROUGH(struct node_proto, proto, protos,
	LOOP_THROUGH(struct node_icmp, icmp_type, icmp_types,
	LOOP_THROUGH(struct node_host, src_host, src_hosts,
	LOOP_THROUGH(struct node_port, src_port, src_ports,
	LOOP_THROUGH(struct node_os, src_os, src_oses,
	LOOP_THROUGH(struct node_host, dst_host, dst_hosts,
	LOOP_THROUGH(struct node_port, dst_port, dst_ports,
	LOOP_THROUGH(struct node_uid, uid, uids,
	LOOP_THROUGH(struct node_gid, gid, gids,

		r->af = af;
		/* for link-local IPv6 address, interface must match up */
		if ((r->af && src_host->af && r->af != src_host->af) ||
		    (r->af && dst_host->af && r->af != dst_host->af) ||
		    (src_host->af && dst_host->af &&
		    src_host->af != dst_host->af) ||
		    (src_host->ifindex && dst_host->ifindex &&
		    src_host->ifindex != dst_host->ifindex) ||
		    (src_host->ifindex && *interface->ifname &&
		    src_host->ifindex != if_nametoindex(interface->ifname)) ||
		    (dst_host->ifindex && *interface->ifname &&
		    dst_host->ifindex != if_nametoindex(interface->ifname)))
			continue;
		if (!r->af && src_host->af)
			r->af = src_host->af;
		else if (!r->af && dst_host->af)
			r->af = dst_host->af;

		if (*interface->ifname)
			strlcpy(r->ifname, interface->ifname,
			    sizeof(r->ifname));
		else if (if_indextoname(src_host->ifindex, ifname))
			strlcpy(r->ifname, ifname, sizeof(r->ifname));
		else if (if_indextoname(dst_host->ifindex, ifname))
			strlcpy(r->ifname, ifname, sizeof(r->ifname));
		else
			memset(r->ifname, '\0', sizeof(r->ifname));

		if (strlcpy(r->label, label, sizeof(r->label)) >=
		    sizeof(r->label))
			errx(1, "expand_rule: strlcpy");
		if (strlcpy(r->tagname, tagname, sizeof(r->tagname)) >=
		    sizeof(r->tagname))
			errx(1, "expand_rule: strlcpy");
		if (strlcpy(r->match_tagname, match_tagname,
		    sizeof(r->match_tagname)) >= sizeof(r->match_tagname))
			errx(1, "expand_rule: strlcpy");
		expand_label(r->label, PF_RULE_LABEL_SIZE, r->ifname, r->af,
		    src_host, src_port, dst_host, dst_port, proto->proto);
		expand_label(r->tagname, PF_TAG_NAME_SIZE, r->ifname, r->af,
		    src_host, src_port, dst_host, dst_port, proto->proto);
		expand_label(r->match_tagname, PF_TAG_NAME_SIZE, r->ifname,
		    r->af, src_host, src_port, dst_host, dst_port,
		    proto->proto);

		error += check_netmask(src_host, r->af);
		error += check_netmask(dst_host, r->af);

		r->ifnot = interface->not;
		r->proto = proto->proto;
		r->src.addr = src_host->addr;
		r->src.neg = src_host->not;
		r->src.port[0] = src_port->port[0];
		r->src.port[1] = src_port->port[1];
		r->src.port_op = src_port->op;
		r->dst.addr = dst_host->addr;
		r->dst.neg = dst_host->not;
		r->dst.port[0] = dst_port->port[0];
		r->dst.port[1] = dst_port->port[1];
		r->dst.port_op = dst_port->op;
		r->uid.op = uid->op;
		r->uid.uid[0] = uid->uid[0];
		r->uid.uid[1] = uid->uid[1];
		r->gid.op = gid->op;
		r->gid.gid[0] = gid->gid[0];
		r->gid.gid[1] = gid->gid[1];
		r->type = icmp_type->type;
		r->code = icmp_type->code;

		if ((keep_state == PF_STATE_MODULATE ||
		    keep_state == PF_STATE_SYNPROXY) &&
		    r->proto && r->proto != IPPROTO_TCP)
			r->keep_state = PF_STATE_NORMAL;
		else
			r->keep_state = keep_state;

		if (r->proto && r->proto != IPPROTO_TCP) {
			r->flags = 0;
			r->flagset = 0;
		} else {
			r->flags = flags;
			r->flagset = flagset;
		}
		if (icmp_type->proto && r->proto != icmp_type->proto) {
			yyerror("icmp-type mismatch");
			error++;
		}

		if (src_os && src_os->os) {
			r->os_fingerprint = pfctl_get_fingerprint(src_os->os);
			if ((pf->opts & PF_OPT_VERBOSE2) &&
			    r->os_fingerprint == PF_OSFP_NOMATCH)
				fprintf(stderr,
				    "warning: unknown '%s' OS fingerprint\n",
				    src_os->os);
		} else {
			r->os_fingerprint = PF_OSFP_ANY;
		}

		TAILQ_INIT(&r->rpool.list);
		for (h = rpool_hosts; h != NULL; h = h->next) {
			pa = calloc(1, sizeof(struct pf_pooladdr));
			if (pa == NULL)
				err(1, "expand_rule: calloc");
			pa->addr = h->addr;
			if (h->ifname != NULL) {
				if (strlcpy(pa->ifname, h->ifname,
				    sizeof(pa->ifname)) >=
				    sizeof(pa->ifname))
					errx(1, "expand_rule: strlcpy");
			} else
				pa->ifname[0] = 0;
			TAILQ_INSERT_TAIL(&r->rpool.list, pa, entries);
		}

		if (rule_consistent(r, anchor_call[0]) < 0 || error)
			yyerror("skipping rule due to errors");
		else {
			r->nr = pf->astack[pf->asd]->match++;
			pfctl_add_rule(pf, r, anchor_call);
			added++;
		}

	))))))))));

	FREE_LIST(struct node_if, interfaces);
	FREE_LIST(struct node_proto, protos);
	FREE_LIST(struct node_host, src_hosts);
	FREE_LIST(struct node_port, src_ports);
	FREE_LIST(struct node_os, src_oses);
	FREE_LIST(struct node_host, dst_hosts);
	FREE_LIST(struct node_port, dst_ports);
	FREE_LIST(struct node_uid, uids);
	FREE_LIST(struct node_gid, gids);
	FREE_LIST(struct node_icmp, icmp_types);
	FREE_LIST(struct node_host, rpool_hosts);

	if (!added)
		yyerror("rule expands to no valid combination");
}

int
expand_skip_interface(struct node_if *interfaces)
{
	int	errs = 0;

	if (!interfaces || (!interfaces->next && !interfaces->not &&
	    !strcmp(interfaces->ifname, "none"))) {
		if (pf->opts & PF_OPT_VERBOSE)
			printf("set skip on none\n");
		errs = pfctl_set_interface_flags(pf, "", PFI_IFLAG_SKIP, 0);
		return (errs);
	}

	if (pf->opts & PF_OPT_VERBOSE)
		printf("set skip on {");
	LOOP_THROUGH(struct node_if, interface, interfaces,
		if (pf->opts & PF_OPT_VERBOSE)
			printf(" %s", interface->ifname);
		if (interface->not) {
			yyerror("skip on ! <interface> is not supported");
			errs++;
		} else
			errs += pfctl_set_interface_flags(pf,
			    interface->ifname, PFI_IFLAG_SKIP, 1);
	);
	if (pf->opts & PF_OPT_VERBOSE)
		printf(" }\n");

	FREE_LIST(struct node_if, interfaces);

	if (errs)
		return (1);
	else
		return (0);
}

#undef FREE_LIST
#undef LOOP_THROUGH

int
check_rulestate(int desired_state)
{
	if (require_order && (rulestate > desired_state)) {
		yyerror("Rules must be in order: options, normalization, "
		    "queueing, translation, filtering");
		return (1);
	}
	rulestate = desired_state;
	return (0);
}

int
kw_cmp(const void *k, const void *e)
{
	return (strcmp(k, ((const struct keywords *)e)->k_name));
}

int
lookup(char *s)
{
	/* this has to be sorted always */
	static const struct keywords keywords[] = {
		{ "all",		ALL},
		{ "allow-opts",		ALLOWOPTS},
		{ "altq",		ALTQ},
		{ "anchor",		ANCHOR},
		{ "antispoof",		ANTISPOOF},
		{ "any",		ANY},
		{ "bandwidth",		BANDWIDTH},
		{ "binat",		BINAT},
		{ "binat-anchor",	BINATANCHOR},
		{ "bitmask",		BITMASK},
		{ "block",		BLOCK},
		{ "block-policy",	BLOCKPOLICY},
		{ "buckets",		BUCKETS},
		{ "cbq",		CBQ},
		{ "code",		CODE},
		{ "codelq",		CODEL},
		{ "crop",		FRAGCROP},
		{ "debug",		DEBUG},
		{ "divert-reply",	DIVERTREPLY},
		{ "divert-to",		DIVERTTO},
		{ "drop",		DROP},
		{ "drop-ovl",		FRAGDROP},
		{ "dup-to",		DUPTO},
		{ "fail-policy",	FAILPOLICY},
		{ "fairq",		FAIRQ},
		{ "fastroute",		FASTROUTE},
		{ "file",		FILENAME},
		{ "fingerprints",	FINGERPRINTS},
		{ "flags",		FLAGS},
		{ "floating",		FLOATING},
		{ "flush",		FLUSH},
		{ "for",		FOR},
		{ "fragment",		FRAGMENT},
		{ "from",		FROM},
		{ "global",		GLOBAL},
		{ "group",		GROUP},
		{ "hfsc",		HFSC},
		{ "hogs",		HOGS},
		{ "hostid",		HOSTID},
		{ "icmp-type",		ICMPTYPE},
		{ "icmp6-type",		ICMP6TYPE},
		{ "if-bound",		IFBOUND},
		{ "in",			IN},
		{ "include",		INCLUDE},
		{ "inet",		INET},
		{ "inet6",		INET6},
		{ "interval",		INTERVAL},
		{ "keep",		KEEP},
		{ "label",		LABEL},
		{ "limit",		LIMIT},
		{ "linkshare",		LINKSHARE},
		{ "load",		LOAD},
		{ "log",		LOG},
		{ "loginterface",	LOGINTERFACE},
		{ "max",		MAXIMUM},
		{ "max-mss",		MAXMSS},
		{ "max-src-conn",	MAXSRCCONN},
		{ "max-src-conn-rate",	MAXSRCCONNRATE},
		{ "max-src-nodes",	MAXSRCNODES},
		{ "max-src-states",	MAXSRCSTATES},
		{ "min-ttl",		MINTTL},
		{ "modulate",		MODULATE},
		{ "nat",		NAT},
		{ "nat-anchor",		NATANCHOR},
		{ "no",			NO},
		{ "no-df",		NODF},
		{ "no-route",		NOROUTE},
		{ "no-sync",		NOSYNC},
		{ "on",			ON},
		{ "optimization",	OPTIMIZATION},
		{ "os",			OS},
		{ "out",		OUT},
		{ "overload",		OVERLOAD},
		{ "pass",		PASS},
		{ "port",		PORT},
		{ "prio",		PRIO},
		{ "priority",		PRIORITY},
		{ "priq",		PRIQ},
		{ "probability",	PROBABILITY},
		{ "proto",		PROTO},
		{ "qlimit",		QLIMIT},
		{ "queue",		QUEUE},
		{ "quick",		QUICK},
		{ "random",		RANDOM},
		{ "random-id",		RANDOMID},
		{ "rdr",		RDR},
		{ "rdr-anchor",		RDRANCHOR},
		{ "realtime",		REALTIME},
		{ "reassemble",		REASSEMBLE},
		{ "reply-to",		REPLYTO},
		{ "require-order",	REQUIREORDER},
		{ "return",		RETURN},
		{ "return-icmp",	RETURNICMP},
		{ "return-icmp6",	RETURNICMP6},
		{ "return-rst",		RETURNRST},
		{ "round-robin",	ROUNDROBIN},
		{ "route",		ROUTE},
		{ "route-to",		ROUTETO},
		{ "rtable",		RTABLE},
		{ "rule",		RULE},
		{ "ruleset-optimization",	RULESET_OPTIMIZATION},
		{ "scrub",		SCRUB},
		{ "set",		SET},
		{ "set-tos",		SETTOS},
		{ "skip",		SKIP},
		{ "sloppy",		SLOPPY},
		{ "source-hash",	SOURCEHASH},
		{ "source-track",	SOURCETRACK},
		{ "state",		STATE},
		{ "state-defaults",	STATEDEFAULTS},
		{ "state-policy",	STATEPOLICY},
		{ "static-port",	STATICPORT},
		{ "sticky-address",	STICKYADDRESS},
		{ "synproxy",		SYNPROXY},
		{ "table",		TABLE},
		{ "tag",		TAG},
		{ "tagged",		TAGGED},
		{ "target",		TARGET},
		{ "tbrsize",		TBRSIZE},
		{ "timeout",		TIMEOUT},
		{ "to",			TO},
		{ "tos",		TOS},
		{ "ttl",		TTL},
		{ "upperlimit",		UPPERLIMIT},
		{ "urpf-failed",	URPFFAILED},
		{ "user",		USER},
	};
	const struct keywords	*p;

	p = bsearch(s, keywords, sizeof(keywords)/sizeof(keywords[0]),
	    sizeof(keywords[0]), kw_cmp);

	if (p) {
		if (debug > 1)
			fprintf(stderr, "%s: %d\n", s, p->k_val);
		return (p->k_val);
	} else {
		if (debug > 1)
			fprintf(stderr, "string: %s\n", s);
		return (STRING);
	}
}

#define MAXPUSHBACK	128

static char	*parsebuf;
static int	 parseindex;
static char	 pushback_buffer[MAXPUSHBACK];
static int	 pushback_index = 0;

int
lgetc(int quotec)
{
	int		c, next;

	if (parsebuf) {
		/* Read character from the parsebuffer instead of input. */
		if (parseindex >= 0) {
			c = parsebuf[parseindex++];
			if (c != '\0')
				return (c);
			parsebuf = NULL;
		} else
			parseindex++;
	}

	if (pushback_index)
		return (pushback_buffer[--pushback_index]);

	if (quotec) {
		if ((c = getc(file->stream)) == EOF) {
			yyerror("reached end of file while parsing quoted string");
			if (popfile() == EOF)
				return (EOF);
			return (quotec);
		}
		return (c);
	}

	while ((c = getc(file->stream)) == '\\') {
		next = getc(file->stream);
		if (next != '\n') {
			c = next;
			break;
		}
		yylval.lineno = file->lineno;
		file->lineno++;
	}

	while (c == EOF) {
		if (popfile() == EOF)
			return (EOF);
		c = getc(file->stream);
	}
	return (c);
}

int
lungetc(int c)
{
	if (c == EOF)
		return (EOF);
	if (parsebuf) {
		parseindex--;
		if (parseindex >= 0)
			return (c);
	}
	if (pushback_index < MAXPUSHBACK-1)
		return (pushback_buffer[pushback_index++] = c);
	else
		return (EOF);
}

int
findeol(void)
{
	int	c;

	parsebuf = NULL;

	/* skip to either EOF or the first real EOL */
	while (1) {
		if (pushback_index)
			c = pushback_buffer[--pushback_index];
		else
			c = lgetc(0);
		if (c == '\n') {
			file->lineno++;
			break;
		}
		if (c == EOF)
			break;
	}
	return (ERROR);
}

int
yylex(void)
{
	char	 buf[8096];
	char	*p, *val;
	int	 quotec, next, c;
	int	 token;

top:
	p = buf;
	while ((c = lgetc(0)) == ' ' || c == '\t')
		; /* nothing */

	yylval.lineno = file->lineno;
	if (c == '#')
		while ((c = lgetc(0)) != '\n' && c != EOF)
			; /* nothing */
	if (c == '$' && parsebuf == NULL) {
		while (1) {
			if ((c = lgetc(0)) == EOF)
				return (0);

			if (p + 1 >= buf + sizeof(buf) - 1) {
				yyerror("string too long");
				return (findeol());
			}
			if (isalnum(c) || c == '_') {
				*p++ = (char)c;
				continue;
			}
			*p = '\0';
			lungetc(c);
			break;
		}
		val = symget(buf);
		if (val == NULL) {
			yyerror("macro '%s' not defined", buf);
			return (findeol());
		}
		parsebuf = val;
		parseindex = 0;
		goto top;
	}

	switch (c) {
	case '\'':
	case '"':
		quotec = c;
		while (1) {
			if ((c = lgetc(quotec)) == EOF)
				return (0);
			if (c == '\n') {
				file->lineno++;
				continue;
			} else if (c == '\\') {
				if ((next = lgetc(quotec)) == EOF)
					return (0);
				if (next == quotec || c == ' ' || c == '\t')
					c = next;
				else if (next == '\n')
					continue;
				else
					lungetc(next);
			} else if (c == quotec) {
				*p = '\0';
				break;
			}
			if (p + 1 >= buf + sizeof(buf) - 1) {
				yyerror("string too long");
				return (findeol());
			}
			*p++ = (char)c;
		}
		yylval.v.string = strdup(buf);
		if (yylval.v.string == NULL)
			err(1, "yylex: strdup");
		return (STRING);
	case '<':
		next = lgetc(0);
		if (next == '>') {
			yylval.v.i = PF_OP_XRG;
			return (PORTBINARY);
		}
		lungetc(next);
		break;
	case '>':
		next = lgetc(0);
		if (next == '<') {
			yylval.v.i = PF_OP_IRG;
			return (PORTBINARY);
		}
		lungetc(next);
		break;
	case '-':
		next = lgetc(0);
		if (next == '>')
			return (ARROW);
		lungetc(next);
		break;
	}

#define allowed_to_end_number(x) \
	(isspace(x) || x == ')' || x ==',' || x == '/' || x == '}' || x == '=')

	if (c == '-' || isdigit(c)) {
		do {
			*p++ = c;
			if ((unsigned)(p-buf) >= sizeof(buf)) {
				yyerror("string too long");
				return (findeol());
			}
		} while ((c = lgetc(0)) != EOF && isdigit(c));
		lungetc(c);
		if (p == buf + 1 && buf[0] == '-')
			goto nodigits;
		if (c == EOF || allowed_to_end_number(c)) {
			const char *errstr = NULL;

			*p = '\0';
			yylval.v.number = strtonum(buf, LLONG_MIN,
			    LLONG_MAX, &errstr);
			if (errstr) {
				yyerror("\"%s\" invalid number: %s",
				    buf, errstr);
				return (findeol());
			}
			return (NUMBER);
		} else {
nodigits:
			while (p > buf + 1)
				lungetc(*--p);
			c = *--p;
			if (c == '-')
				return (c);
		}
	}

#define allowed_in_string(x) \
	(isalnum(x) || (ispunct(x) && x != '(' && x != ')' && \
	x != '{' && x != '}' && x != '<' && x != '>' && \
	x != '!' && x != '=' && x != '/' && x != '#' && \
	x != ','))

	if (isalnum(c) || c == ':' || c == '_') {
		do {
			*p++ = c;
			if ((unsigned)(p-buf) >= sizeof(buf)) {
				yyerror("string too long");
				return (findeol());
			}
		} while ((c = lgetc(0)) != EOF && (allowed_in_string(c)));
		lungetc(c);
		*p = '\0';
		if ((token = lookup(buf)) == STRING)
			if ((yylval.v.string = strdup(buf)) == NULL)
				err(1, "yylex: strdup");
		return (token);
	}
	if (c == '\n') {
		yylval.lineno = file->lineno;
		file->lineno++;
	}
	if (c == EOF)
		return (0);
	return (c);
}

int
check_file_secrecy(int fd, const char *fname)
{
	struct stat	st;

	if (fstat(fd, &st)) {
		warn("cannot stat %s", fname);
		return (-1);
	}
	if (st.st_uid != 0 && st.st_uid != getuid()) {
		warnx("%s: owner not root or current user", fname);
		return (-1);
	}
	if (st.st_mode & (S_IRWXG | S_IRWXO)) {
		warnx("%s: group/world readable/writeable", fname);
		return (-1);
	}
	return (0);
}

struct file *
pushfile(const char *name, int secret)
{
	struct file	*nfile;

	if ((nfile = calloc(1, sizeof(struct file))) == NULL ||
	    (nfile->name = strdup(name)) == NULL) {
		warn("malloc");
		return (NULL);
	}
	if (TAILQ_FIRST(&files) == NULL && strcmp(nfile->name, "-") == 0) {
		nfile->stream = stdin;
		free(nfile->name);
		if ((nfile->name = strdup("stdin")) == NULL) {
			warn("strdup");
			free(nfile);
			return (NULL);
		}
	} else if ((nfile->stream = fopen(nfile->name, "r")) == NULL) {
		warn("%s", nfile->name);
		free(nfile->name);
		free(nfile);
		return (NULL);
	} else if (secret &&
	    check_file_secrecy(fileno(nfile->stream), nfile->name)) {
		fclose(nfile->stream);
		free(nfile->name);
		free(nfile);
		return (NULL);
	}
	nfile->lineno = 1;
	TAILQ_INSERT_TAIL(&files, nfile, entry);
	return (nfile);
}

int
popfile(void)
{
	struct file	*prev;

	if ((prev = TAILQ_PREV(file, files, entry)) != NULL) {
		prev->errors += file->errors;
		TAILQ_REMOVE(&files, file, entry);
		fclose(file->stream);
		free(file->name);
		free(file);
		file = prev;
		return (0);
	}
	return (EOF);
}

int
parse_config(char *filename, struct pfctl *xpf)
{
	int		 errors = 0;
	struct sym	*sym;

	pf = xpf;
	errors = 0;
	rulestate = PFCTL_STATE_NONE;
	returnicmpdefault = (ICMP_UNREACH << 8) | ICMP_UNREACH_PORT;
	returnicmp6default =
	    (ICMP6_DST_UNREACH << 8) | ICMP6_DST_UNREACH_NOPORT;
	blockpolicy = PFRULE_DROP;
	failpolicy = PFRULE_DROP;
	require_order = 1;

	if ((file = pushfile(filename, 0)) == NULL) {
		warn("cannot open the main config file!");
		return (-1);
	}

	yyparse();
	errors = file->errors;
	popfile();

	/* Free macros and check which have not been used. */
	while ((sym = TAILQ_FIRST(&symhead))) {
		if ((pf->opts & PF_OPT_VERBOSE2) && !sym->used)
			fprintf(stderr, "warning: macro '%s' not "
			    "used\n", sym->nam);
		free(sym->nam);
		free(sym->val);
		TAILQ_REMOVE(&symhead, sym, entry);
		free(sym);
	}

	return (errors ? -1 : 0);
}

int
symset(const char *nam, const char *val, int persist)
{
	struct sym	*sym;

	for (sym = TAILQ_FIRST(&symhead); sym && strcmp(nam, sym->nam);
	    sym = TAILQ_NEXT(sym, entry))
		;	/* nothing */

	if (sym != NULL) {
		if (sym->persist == 1)
			return (0);
		else {
			free(sym->nam);
			free(sym->val);
			TAILQ_REMOVE(&symhead, sym, entry);
			free(sym);
		}
	}
	if ((sym = calloc(1, sizeof(*sym))) == NULL)
		return (-1);

	sym->nam = strdup(nam);
	if (sym->nam == NULL) {
		free(sym);
		return (-1);
	}
	sym->val = strdup(val);
	if (sym->val == NULL) {
		free(sym->nam);
		free(sym);
		return (-1);
	}
	sym->used = 0;
	sym->persist = persist;
	TAILQ_INSERT_TAIL(&symhead, sym, entry);
	return (0);
}

int
pfctl_cmdline_symset(char *s)
{
	char	*sym, *val;
	int	 ret;

	if ((val = strrchr(s, '=')) == NULL)
		return (-1);

	if ((sym = malloc(strlen(s) - strlen(val) + 1)) == NULL)
		err(1, "pfctl_cmdline_symset: malloc");

	strlcpy(sym, s, strlen(s) - strlen(val) + 1);

	ret = symset(sym, val + 1, 1);
	free(sym);

	return (ret);
}

char *
symget(const char *nam)
{
	struct sym	*sym;

	TAILQ_FOREACH(sym, &symhead, entry)
		if (strcmp(nam, sym->nam) == 0) {
			sym->used = 1;
			return (sym->val);
		}
	return (NULL);
}

void
mv_rules(struct pf_ruleset *src, struct pf_ruleset *dst)
{
	int i;
	struct pf_rule *r;

	for (i = 0; i < PF_RULESET_MAX; ++i) {
		while ((r = TAILQ_FIRST(src->rules[i].active.ptr))
		    != NULL) {
			TAILQ_REMOVE(src->rules[i].active.ptr, r, entries);
			TAILQ_INSERT_TAIL(dst->rules[i].active.ptr, r, entries);
			dst->anchor->match++;
		}
		src->anchor->match = 0;
		while ((r = TAILQ_FIRST(src->rules[i].inactive.ptr))
		    != NULL) {
			TAILQ_REMOVE(src->rules[i].inactive.ptr, r, entries);
			TAILQ_INSERT_TAIL(dst->rules[i].inactive.ptr,
				r, entries);
		}
	}
}

void
decide_address_family(struct node_host *n, sa_family_t *af)
{
	if (*af != 0 || n == NULL)
		return;
	*af = n->af;
	while ((n = n->next) != NULL) {
		if (n->af != *af) {
			*af = 0;
			return;
		}
	}
}

void
remove_invalid_hosts(struct node_host **nh, sa_family_t *af)
{
	struct node_host	*n = *nh, *prev = NULL;

	while (n != NULL) {
		if (*af && n->af && n->af != *af) {
			/* unlink and free n */
			struct node_host *next = n->next;

			/* adjust tail pointer */
			if (n == (*nh)->tail)
				(*nh)->tail = prev;
			/* adjust previous node's next pointer */
			if (prev == NULL)
				*nh = next;
			else
				prev->next = next;
			/* free node */
			if (n->ifname != NULL)
				free(n->ifname);
			free(n);
			n = next;
		} else {
			if (n->af && !*af)
				*af = n->af;
			prev = n;
			n = n->next;
		}
	}
}

int
invalid_redirect(struct node_host *nh, sa_family_t af)
{
	if (!af) {
		struct node_host *n;

		/* tables and dyniftl are ok without an address family */
		for (n = nh; n != NULL; n = n->next) {
			if (n->addr.type != PF_ADDR_TABLE &&
			    n->addr.type != PF_ADDR_DYNIFTL) {
				yyerror("address family not given and "
				    "translation address expands to multiple "
				    "address families");
				return (1);
			}
		}
	}
	if (nh == NULL) {
		yyerror("no translation address with matching address family "
		    "found.");
		return (1);
	}
	return (0);
}

int
atoul(char *s, u_long *ulvalp)
{
	u_long	 ulval;
	char	*ep;

	errno = 0;
	ulval = strtoul(s, &ep, 0);
	if (s[0] == '\0' || *ep != '\0')
		return (-1);
	if (errno == ERANGE && ulval == ULONG_MAX)
		return (-1);
	*ulvalp = ulval;
	return (0);
}

int
getservice(char *n)
{
	struct servent	*s;
	u_long		 ulval;

	if (atoul(n, &ulval) == 0) {
		if (ulval > 65535) {
			yyerror("illegal port value %lu", ulval);
			return (-1);
		}
		return (htons(ulval));
	} else {
		s = getservbyname(n, "tcp");
		if (s == NULL)
			s = getservbyname(n, "udp");
		if (s == NULL) {
			yyerror("unknown port %s", n);
			return (-1);
		}
		return (s->s_port);
	}
}

int
rule_label(struct pf_rule *r, char *s)
{
	if (s) {
		if (strlcpy(r->label, s, sizeof(r->label)) >=
		    sizeof(r->label)) {
			yyerror("rule label too long (max %d chars)",
			    sizeof(r->label)-1);
			return (-1);
		}
	}
	return (0);
}

u_int16_t
parseicmpspec(char *w, sa_family_t af)
{
	const struct icmpcodeent	*p;
	u_long				 ulval;
	u_int8_t			 icmptype;

	if (af == AF_INET)
		icmptype = returnicmpdefault >> 8;
	else
		icmptype = returnicmp6default >> 8;

	if (atoul(w, &ulval) == -1) {
		if ((p = geticmpcodebyname(icmptype, w, af)) == NULL) {
			yyerror("unknown icmp code %s", w);
			return (0);
		}
		ulval = p->code;
	}
	if (ulval > 255) {
		yyerror("invalid icmp code %lu", ulval);
		return (0);
	}
	return (icmptype << 8 | ulval);
}

int
parseport(char *port, struct range *r, int extensions)
{
	char	*p = strchr(port, ':');

	if (p == NULL) {
		if ((r->a = getservice(port)) == -1)
			return (-1);
		r->b = 0;
		r->t = PF_OP_NONE;
		return (0);
	}
	if ((extensions & PPORT_STAR) && !strcmp(p+1, "*")) {
		*p = 0;
		if ((r->a = getservice(port)) == -1)
			return (-1);
		r->b = 0;
		r->t = PF_OP_IRG;
		return (0);
	}
	if ((extensions & PPORT_RANGE)) {
		*p++ = 0;
		if ((r->a = getservice(port)) == -1 ||
		    (r->b = getservice(p)) == -1)
			return (-1);
		if (r->a == r->b) {
			r->b = 0;
			r->t = PF_OP_NONE;
		} else
			r->t = PF_OP_RRG;
		return (0);
	}
	return (-1);
}

int
pfctl_load_anchors(int dev, struct pfctl *pf, struct pfr_buffer *trans)
{
	struct loadanchors	*la;

	TAILQ_FOREACH(la, &loadanchorshead, entries) {
		if (pf->opts & PF_OPT_VERBOSE)
			fprintf(stderr, "\nLoading anchor %s from %s\n",
			    la->anchorname, la->filename);
		if (pfctl_rules(dev, la->filename, pf->opts, pf->optimize,
		    la->anchorname, trans) == -1)
			return (-1);
	}

	return (0);
}

int
kw_casecmp(const void *k, const void *e)
{
	return (strcasecmp(k, ((const struct keywords *)e)->k_name));
}

int
map_tos(char *s, int *val)
{
	/* DiffServ Codepoints and other TOS mappings */
	const struct keywords	 toswords[] = {
		{ "af11",		IPTOS_DSCP_AF11 },
		{ "af12",		IPTOS_DSCP_AF12 },
		{ "af13",		IPTOS_DSCP_AF13 },
		{ "af21",		IPTOS_DSCP_AF21 },
		{ "af22",		IPTOS_DSCP_AF22 },
		{ "af23",		IPTOS_DSCP_AF23 },
		{ "af31",		IPTOS_DSCP_AF31 },
		{ "af32",		IPTOS_DSCP_AF32 },
		{ "af33",		IPTOS_DSCP_AF33 },
		{ "af41",		IPTOS_DSCP_AF41 },
		{ "af42",		IPTOS_DSCP_AF42 },
		{ "af43",		IPTOS_DSCP_AF43 },
		{ "critical",		IPTOS_PREC_CRITIC_ECP },
		{ "cs0",		IPTOS_DSCP_CS0 },
		{ "cs1",		IPTOS_DSCP_CS1 },
		{ "cs2",		IPTOS_DSCP_CS2 },
		{ "cs3",		IPTOS_DSCP_CS3 },
		{ "cs4",		IPTOS_DSCP_CS4 },
		{ "cs5",		IPTOS_DSCP_CS5 },
		{ "cs6",		IPTOS_DSCP_CS6 },
		{ "cs7",		IPTOS_DSCP_CS7 },
		{ "ef",			IPTOS_DSCP_EF },
		{ "inetcontrol",	IPTOS_PREC_INTERNETCONTROL },
		{ "lowdelay",		IPTOS_LOWDELAY },
		{ "netcontrol",		IPTOS_PREC_NETCONTROL },
		{ "reliability",	IPTOS_RELIABILITY },
		{ "throughput",		IPTOS_THROUGHPUT }
	};
	const struct keywords	*p;

	p = bsearch(s, toswords, sizeof(toswords)/sizeof(toswords[0]),
	    sizeof(toswords[0]), kw_casecmp);

	if (p) {
		*val = p->k_val;
		return (1);
	}
	return (0);
}

int
rt_tableid_max(void)
{
#ifdef __FreeBSD__
	int fibs;
	size_t l = sizeof(fibs);

        if (sysctlbyname("net.fibs", &fibs, &l, NULL, 0) == -1)
		fibs = 16;	/* XXX RT_MAXFIBS, at least limit it some. */
	/*
	 * As the OpenBSD code only compares > and not >= we need to adjust
	 * here given we only accept values of 0..n and want to avoid #ifdefs
	 * in the grammar.
	 */
	return (fibs - 1);
#else
	return (RT_TABLEID_MAX);
#endif
}
#line 4388 "parse.c"

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
case 17:
#line 562 "parse.y"
	{ file->errors++; }
break;
case 18:
#line 565 "parse.y"
	{
			struct file	*nfile;

			if ((nfile = pushfile(yystack.l_mark[0].v.string, 0)) == NULL) {
				yyerror("failed to include file %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);

			file = nfile;
			lungetc('\n');
		}
break;
case 25:
#line 592 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "none"))
				yyval.v.i = 0;
			else if (!strcmp(yystack.l_mark[0].v.string, "basic"))
				yyval.v.i = PF_OPTIMIZE_BASIC;
			else if (!strcmp(yystack.l_mark[0].v.string, "profile"))
				yyval.v.i = PF_OPTIMIZE_BASIC | PF_OPTIMIZE_PROFILE;
			else {
				yyerror("unknown ruleset-optimization %s", yystack.l_mark[0].v.string);
				YYERROR;
			}
		}
break;
case 26:
#line 606 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			if (pfctl_set_optimization(pf, yystack.l_mark[0].v.string) != 0) {
				yyerror("unknown optimization %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 27:
#line 618 "parse.y"
	{
			if (!(pf->opts & PF_OPT_OPTIMIZE)) {
				pf->opts |= PF_OPT_OPTIMIZE;
				pf->optimize = yystack.l_mark[0].v.i;
			}
		}
break;
case 32:
#line 628 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			if (pfctl_set_logif(pf, yystack.l_mark[0].v.string) != 0) {
				yyerror("error setting loginterface %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 33:
#line 640 "parse.y"
	{
			if (yystack.l_mark[0].v.number == 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("hostid must be non-zero");
				YYERROR;
			}
			if (pfctl_set_hostid(pf, yystack.l_mark[0].v.number) != 0) {
				yyerror("error setting hostid %08x", yystack.l_mark[0].v.number);
				YYERROR;
			}
		}
break;
case 34:
#line 650 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_DROP;
		}
break;
case 35:
#line 657 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_RETURN;
		}
break;
case 36:
#line 664 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_DROP;
		}
break;
case 37:
#line 671 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_RETURN;
		}
break;
case 38:
#line 678 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set require-order %s\n",
				    yystack.l_mark[0].v.number == 1 ? "yes" : "no");
			require_order = yystack.l_mark[0].v.number;
		}
break;
case 39:
#line 684 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fingerprints \"%s\"\n", yystack.l_mark[0].v.string);
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			if (!pf->anchor->name[0]) {
				if (pfctl_file_fingerprints(pf->dev,
				    pf->opts, yystack.l_mark[0].v.string)) {
					yyerror("error loading "
					    "fingerprints %s", yystack.l_mark[0].v.string);
					free(yystack.l_mark[0].v.string);
					YYERROR;
				}
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 40:
#line 702 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				switch (yystack.l_mark[0].v.i) {
				case 0:
					printf("set state-policy floating\n");
					break;
				case PFRULE_IFBOUND:
					printf("set state-policy if-bound\n");
					break;
				}
			default_statelock = yystack.l_mark[0].v.i;
		}
break;
case 41:
#line 714 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			if (pfctl_set_debug(pf, yystack.l_mark[0].v.string) != 0) {
				yyerror("error setting debuglevel %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 42:
#line 726 "parse.y"
	{
			if (expand_skip_interface(yystack.l_mark[0].v.interface) != 0) {
				yyerror("error setting skip interface(s)");
				YYERROR;
			}
		}
break;
case 43:
#line 732 "parse.y"
	{
			if (keep_state_defaults != NULL) {
				yyerror("cannot redefine state-defaults");
				YYERROR;
			}
			keep_state_defaults = yystack.l_mark[0].v.state_opt;
		}
break;
case 44:
#line 741 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 45:
#line 742 "parse.y"
	{
			if ((yyval.v.string = strdup("all")) == NULL) {
				err(1, "stringall: strdup");
			}
		}
break;
case 46:
#line 749 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 48:
#line 758 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 50:
#line 767 "parse.y"
	{
			char	*s;
			if (asprintf(&s, "%lld", (long long)yystack.l_mark[0].v.number) == -1) {
				yyerror("string: asprintf");
				YYERROR;
			}
			yyval.v.string = s;
		}
break;
case 52:
#line 778 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("%s = \"%s\"\n", yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string);
			if (symset(yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string, 0) == -1)
				err(1, "cannot store variable %s", yystack.l_mark[-2].v.string);
			free(yystack.l_mark[-2].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 53:
#line 788 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 54:
#line 789 "parse.y"
	{ yyval.v.string = NULL; }
break;
case 59:
#line 799 "parse.y"
	{
			char ta[PF_ANCHOR_NAME_SIZE];
			struct pf_ruleset *rs;

			/* steping into a brace anchor */
			pf->asd++;
			pf->bn++;
			pf->brace = 1;

			/* create a holding ruleset in the root */
			snprintf(ta, PF_ANCHOR_NAME_SIZE, "_%d", pf->bn);
			rs = pf_find_or_create_ruleset(ta);
			if (rs == NULL)
				err(1, "pfa_anchor: pf_find_or_create_ruleset");
			pf->astack[pf->asd] = rs->anchor;
			pf->anchor = rs->anchor;
		}
break;
case 60:
#line 816 "parse.y"
	{
			pf->alast = pf->anchor;
			pf->asd--;
			pf->anchor = pf->astack[pf->asd];
		}
break;
case 62:
#line 826 "parse.y"
	{
			struct pf_rule	r;
			struct node_proto	*proto;

			if (check_rulestate(PFCTL_STATE_FILTER)) {
				if (yystack.l_mark[-8].v.string)
					free(yystack.l_mark[-8].v.string);
				YYERROR;
			}

			if (yystack.l_mark[-8].v.string && (yystack.l_mark[-8].v.string[0] == '_' || strstr(yystack.l_mark[-8].v.string, "/_") != NULL)) {
				free(yystack.l_mark[-8].v.string);
				yyerror("anchor names beginning with '_' "
				    "are reserved for internal use");
				YYERROR;
			}

			memset(&r, 0, sizeof(r));
			if (pf->astack[pf->asd + 1]) {
				/* move inline rules into relative location */
				pf_anchor_setup(&r,
				    &pf->astack[pf->asd]->ruleset,
				    yystack.l_mark[-8].v.string ? yystack.l_mark[-8].v.string : pf->alast->name);
		
				if (r.anchor == NULL)
					err(1, "anchorrule: unable to "
					    "create ruleset");

				if (pf->alast != r.anchor) {
					if (r.anchor->match) {
						yyerror("inline anchor '%s' "
						    "already exists",
						    r.anchor->name);
						YYERROR;
					}
					mv_rules(&pf->alast->ruleset,
					    &r.anchor->ruleset);
				}
				pf_remove_if_empty_ruleset(&pf->alast->ruleset);
				pf->alast = r.anchor;
			} else {
				if (!yystack.l_mark[-8].v.string) {
					yyerror("anchors without explicit "
					    "rules must specify a name");
					YYERROR;
				}
			}
			r.direction = yystack.l_mark[-7].v.i;
			r.quick = yystack.l_mark[-6].v.logquick.quick;
			r.af = yystack.l_mark[-4].v.i;
			r.prob = yystack.l_mark[-1].v.filter_opts.prob;
			r.rtableid = yystack.l_mark[-1].v.filter_opts.rtableid;

			if (yystack.l_mark[-1].v.filter_opts.tag)
				if (strlcpy(r.tagname, yystack.l_mark[-1].v.filter_opts.tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			if (yystack.l_mark[-1].v.filter_opts.match_tag)
				if (strlcpy(r.match_tagname, yystack.l_mark[-1].v.filter_opts.match_tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[-1].v.filter_opts.match_tag_not;
			if (rule_label(&r, yystack.l_mark[-1].v.filter_opts.label))
				YYERROR;
			free(yystack.l_mark[-1].v.filter_opts.label);
			r.flags = yystack.l_mark[-1].v.filter_opts.flags.b1;
			r.flagset = yystack.l_mark[-1].v.filter_opts.flags.b2;
			if ((yystack.l_mark[-1].v.filter_opts.flags.b1 & yystack.l_mark[-1].v.filter_opts.flags.b2) != yystack.l_mark[-1].v.filter_opts.flags.b1) {
				yyerror("flags always false");
				YYERROR;
			}
			if (yystack.l_mark[-1].v.filter_opts.flags.b1 || yystack.l_mark[-1].v.filter_opts.flags.b2 || yystack.l_mark[-2].v.fromto.src_os) {
				for (proto = yystack.l_mark[-3].v.proto; proto != NULL &&
				    proto->proto != IPPROTO_TCP;
				    proto = proto->next)
					;	/* nothing */
				if (proto == NULL && yystack.l_mark[-3].v.proto != NULL) {
					if (yystack.l_mark[-1].v.filter_opts.flags.b1 || yystack.l_mark[-1].v.filter_opts.flags.b2)
						yyerror(
						    "flags only apply to tcp");
					if (yystack.l_mark[-2].v.fromto.src_os)
						yyerror(
						    "OS fingerprinting only "
						    "applies to tcp");
					YYERROR;
				}
			}

			r.tos = yystack.l_mark[-1].v.filter_opts.tos;

			if (yystack.l_mark[-1].v.filter_opts.keep.action) {
				yyerror("cannot specify state handling "
				    "on anchors");
				YYERROR;
			}

			if (yystack.l_mark[-1].v.filter_opts.match_tag)
				if (strlcpy(r.match_tagname, yystack.l_mark[-1].v.filter_opts.match_tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[-1].v.filter_opts.match_tag_not;
			if (yystack.l_mark[-1].v.filter_opts.marker & FOM_PRIO) {
				if (yystack.l_mark[-1].v.filter_opts.prio == 0)
					r.prio = PF_PRIO_ZERO;
				else
					r.prio = yystack.l_mark[-1].v.filter_opts.prio;
			}
			if (yystack.l_mark[-1].v.filter_opts.marker & FOM_SETPRIO) {
				r.set_prio[0] = yystack.l_mark[-1].v.filter_opts.set_prio[0];
				r.set_prio[1] = yystack.l_mark[-1].v.filter_opts.set_prio[1];
				r.scrub_flags |= PFSTATE_SETPRIO;
			}

			decide_address_family(yystack.l_mark[-2].v.fromto.src.host, &r.af);
			decide_address_family(yystack.l_mark[-2].v.fromto.dst.host, &r.af);

			expand_rule(&r, yystack.l_mark[-5].v.interface, NULL, yystack.l_mark[-3].v.proto, yystack.l_mark[-2].v.fromto.src_os,
			    yystack.l_mark[-2].v.fromto.src.host, yystack.l_mark[-2].v.fromto.src.port, yystack.l_mark[-2].v.fromto.dst.host, yystack.l_mark[-2].v.fromto.dst.port,
			    yystack.l_mark[-1].v.filter_opts.uid, yystack.l_mark[-1].v.filter_opts.gid, yystack.l_mark[-1].v.filter_opts.icmpspec,
			    pf->astack[pf->asd + 1] ? pf->alast->name : yystack.l_mark[-8].v.string);
			free(yystack.l_mark[-8].v.string);
			pf->astack[pf->asd + 1] = NULL;
		}
break;
case 63:
#line 958 "parse.y"
	{
			struct pf_rule	r;

			if (check_rulestate(PFCTL_STATE_NAT)) {
				free(yystack.l_mark[-5].v.string);
				YYERROR;
			}

			memset(&r, 0, sizeof(r));
			r.action = PF_NAT;
			r.af = yystack.l_mark[-3].v.i;
			r.rtableid = yystack.l_mark[0].v.rtableid;

			decide_address_family(yystack.l_mark[-1].v.fromto.src.host, &r.af);
			decide_address_family(yystack.l_mark[-1].v.fromto.dst.host, &r.af);

			expand_rule(&r, yystack.l_mark[-4].v.interface, NULL, yystack.l_mark[-2].v.proto, yystack.l_mark[-1].v.fromto.src_os,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.src.port, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-1].v.fromto.dst.port,
			    0, 0, 0, yystack.l_mark[-5].v.string);
			free(yystack.l_mark[-5].v.string);
		}
break;
case 64:
#line 979 "parse.y"
	{
			struct pf_rule	r;

			if (check_rulestate(PFCTL_STATE_NAT)) {
				free(yystack.l_mark[-5].v.string);
				YYERROR;
			}

			memset(&r, 0, sizeof(r));
			r.action = PF_RDR;
			r.af = yystack.l_mark[-3].v.i;
			r.rtableid = yystack.l_mark[0].v.rtableid;

			decide_address_family(yystack.l_mark[-1].v.fromto.src.host, &r.af);
			decide_address_family(yystack.l_mark[-1].v.fromto.dst.host, &r.af);

			if (yystack.l_mark[-1].v.fromto.src.port != NULL) {
				yyerror("source port parameter not supported"
				    " in rdr-anchor");
				YYERROR;
			}
			if (yystack.l_mark[-1].v.fromto.dst.port != NULL) {
				if (yystack.l_mark[-1].v.fromto.dst.port->next != NULL) {
					yyerror("destination port list "
					    "expansion not supported in "
					    "rdr-anchor");
					YYERROR;
				} else if (yystack.l_mark[-1].v.fromto.dst.port->op != PF_OP_EQ) {
					yyerror("destination port operators"
					    " not supported in rdr-anchor");
					YYERROR;
				}
				r.dst.port[0] = yystack.l_mark[-1].v.fromto.dst.port->port[0];
				r.dst.port[1] = yystack.l_mark[-1].v.fromto.dst.port->port[1];
				r.dst.port_op = yystack.l_mark[-1].v.fromto.dst.port->op;
			}

			expand_rule(&r, yystack.l_mark[-4].v.interface, NULL, yystack.l_mark[-2].v.proto, yystack.l_mark[-1].v.fromto.src_os,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.src.port, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-1].v.fromto.dst.port,
			    0, 0, 0, yystack.l_mark[-5].v.string);
			free(yystack.l_mark[-5].v.string);
		}
break;
case 65:
#line 1021 "parse.y"
	{
			struct pf_rule	r;

			if (check_rulestate(PFCTL_STATE_NAT)) {
				free(yystack.l_mark[-5].v.string);
				YYERROR;
			}

			memset(&r, 0, sizeof(r));
			r.action = PF_BINAT;
			r.af = yystack.l_mark[-3].v.i;
			r.rtableid = yystack.l_mark[0].v.rtableid;
			if (yystack.l_mark[-2].v.proto != NULL) {
				if (yystack.l_mark[-2].v.proto->next != NULL) {
					yyerror("proto list expansion"
					    " not supported in binat-anchor");
					YYERROR;
				}
				r.proto = yystack.l_mark[-2].v.proto->proto;
				free(yystack.l_mark[-2].v.proto);
			}

			if (yystack.l_mark[-1].v.fromto.src.host != NULL || yystack.l_mark[-1].v.fromto.src.port != NULL ||
			    yystack.l_mark[-1].v.fromto.dst.host != NULL || yystack.l_mark[-1].v.fromto.dst.port != NULL) {
				yyerror("fromto parameter not supported"
				    " in binat-anchor");
				YYERROR;
			}

			decide_address_family(yystack.l_mark[-1].v.fromto.src.host, &r.af);
			decide_address_family(yystack.l_mark[-1].v.fromto.dst.host, &r.af);

			pfctl_add_rule(pf, &r, yystack.l_mark[-5].v.string);
			free(yystack.l_mark[-5].v.string);
		}
break;
case 66:
#line 1058 "parse.y"
	{
			struct loadanchors	*loadanchor;

			if (strlen(pf->anchor->name) + 1 +
			    strlen(yystack.l_mark[-2].v.string) >= MAXPATHLEN) {
				yyerror("anchorname %s too long, max %u\n",
				    yystack.l_mark[-2].v.string, MAXPATHLEN - 1);
				free(yystack.l_mark[-2].v.string);
				YYERROR;
			}
			loadanchor = calloc(1, sizeof(struct loadanchors));
			if (loadanchor == NULL)
				err(1, "loadrule: calloc");
			if ((loadanchor->anchorname = malloc(MAXPATHLEN)) ==
			    NULL)
				err(1, "loadrule: malloc");
			if (pf->anchor->name[0])
				snprintf(loadanchor->anchorname, MAXPATHLEN,
				    "%s/%s", pf->anchor->name, yystack.l_mark[-2].v.string);
			else
				strlcpy(loadanchor->anchorname, yystack.l_mark[-2].v.string, MAXPATHLEN);
			if ((loadanchor->filename = strdup(yystack.l_mark[0].v.string)) == NULL)
				err(1, "loadrule: strdup");

			TAILQ_INSERT_TAIL(&loadanchorshead, loadanchor,
			    entries);

			free(yystack.l_mark[-2].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 67:
#line 1089 "parse.y"
	{
			yyval.v.b.b2 = yyval.v.b.w = 0;
			if (yystack.l_mark[-1].v.i)
				yyval.v.b.b1 = PF_NOSCRUB;
			else
				yyval.v.b.b1 = PF_SCRUB;
		}
break;
case 68:
#line 1099 "parse.y"
	{
			struct pf_rule	r;

			if (check_rulestate(PFCTL_STATE_SCRUB))
				YYERROR;

			memset(&r, 0, sizeof(r));

			r.action = yystack.l_mark[-7].v.b.b1;
			r.direction = yystack.l_mark[-6].v.i;

			r.log = yystack.l_mark[-5].v.logquick.log;
			r.logif = yystack.l_mark[-5].v.logquick.logif;
			if (yystack.l_mark[-5].v.logquick.quick) {
				yyerror("scrub rules do not support 'quick'");
				YYERROR;
			}

			r.af = yystack.l_mark[-3].v.i;
			if (yystack.l_mark[0].v.scrub_opts.nodf)
				r.rule_flag |= PFRULE_NODF;
			if (yystack.l_mark[0].v.scrub_opts.randomid)
				r.rule_flag |= PFRULE_RANDOMID;
			if (yystack.l_mark[0].v.scrub_opts.reassemble_tcp) {
				if (r.direction != PF_INOUT) {
					yyerror("reassemble tcp rules can not "
					    "specify direction");
					YYERROR;
				}
				r.rule_flag |= PFRULE_REASSEMBLE_TCP;
			}
			if (yystack.l_mark[0].v.scrub_opts.minttl)
				r.min_ttl = yystack.l_mark[0].v.scrub_opts.minttl;
			if (yystack.l_mark[0].v.scrub_opts.maxmss)
				r.max_mss = yystack.l_mark[0].v.scrub_opts.maxmss;
			if (yystack.l_mark[0].v.scrub_opts.marker & SOM_SETTOS) {
				r.rule_flag |= PFRULE_SET_TOS;
				r.set_tos = yystack.l_mark[0].v.scrub_opts.settos;
			}
			if (yystack.l_mark[0].v.scrub_opts.fragcache)
				r.rule_flag |= yystack.l_mark[0].v.scrub_opts.fragcache;
			if (yystack.l_mark[0].v.scrub_opts.match_tag)
				if (strlcpy(r.match_tagname, yystack.l_mark[0].v.scrub_opts.match_tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[0].v.scrub_opts.match_tag_not;
			r.rtableid = yystack.l_mark[0].v.scrub_opts.rtableid;

			expand_rule(&r, yystack.l_mark[-4].v.interface, NULL, yystack.l_mark[-2].v.proto, yystack.l_mark[-1].v.fromto.src_os,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.src.port, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-1].v.fromto.dst.port,
			    NULL, NULL, NULL, "");
		}
break;
case 69:
#line 1156 "parse.y"
	{
				bzero(&scrub_opts, sizeof scrub_opts);
				scrub_opts.rtableid = -1;
			}
break;
case 70:
#line 1161 "parse.y"
	{ yyval.v.scrub_opts = scrub_opts; }
break;
case 71:
#line 1162 "parse.y"
	{
			bzero(&scrub_opts, sizeof scrub_opts);
			scrub_opts.rtableid = -1;
			yyval.v.scrub_opts = scrub_opts;
		}
break;
case 74:
#line 1173 "parse.y"
	{
			if (scrub_opts.nodf) {
				yyerror("no-df cannot be respecified");
				YYERROR;
			}
			scrub_opts.nodf = 1;
		}
break;
case 75:
#line 1180 "parse.y"
	{
			if (scrub_opts.marker & SOM_MINTTL) {
				yyerror("min-ttl cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal min-ttl value %d", yystack.l_mark[0].v.number);
				YYERROR;
			}
			scrub_opts.marker |= SOM_MINTTL;
			scrub_opts.minttl = yystack.l_mark[0].v.number;
		}
break;
case 76:
#line 1192 "parse.y"
	{
			if (scrub_opts.marker & SOM_MAXMSS) {
				yyerror("max-mss cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65535) {
				yyerror("illegal max-mss value %d", yystack.l_mark[0].v.number);
				YYERROR;
			}
			scrub_opts.marker |= SOM_MAXMSS;
			scrub_opts.maxmss = yystack.l_mark[0].v.number;
		}
break;
case 77:
#line 1204 "parse.y"
	{
			if (scrub_opts.marker & SOM_SETTOS) {
				yyerror("set-tos cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= SOM_SETTOS;
			scrub_opts.settos = yystack.l_mark[0].v.number;
		}
break;
case 78:
#line 1212 "parse.y"
	{
			if (scrub_opts.marker & SOM_FRAGCACHE) {
				yyerror("fragcache cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= SOM_FRAGCACHE;
			scrub_opts.fragcache = yystack.l_mark[0].v.i;
		}
break;
case 79:
#line 1220 "parse.y"
	{
			if (strcasecmp(yystack.l_mark[0].v.string, "tcp") != 0) {
				yyerror("scrub reassemble supports only tcp, "
				    "not '%s'", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
			if (scrub_opts.reassemble_tcp) {
				yyerror("reassemble tcp cannot be respecified");
				YYERROR;
			}
			scrub_opts.reassemble_tcp = 1;
		}
break;
case 80:
#line 1234 "parse.y"
	{
			if (scrub_opts.randomid) {
				yyerror("random-id cannot be respecified");
				YYERROR;
			}
			scrub_opts.randomid = 1;
		}
break;
case 81:
#line 1241 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			scrub_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 82:
#line 1248 "parse.y"
	{
			scrub_opts.match_tag = yystack.l_mark[0].v.string;
			scrub_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
break;
case 83:
#line 1254 "parse.y"
	{ yyval.v.i = 0; /* default */ }
break;
case 84:
#line 1255 "parse.y"
	{ yyval.v.i = 0; }
break;
case 85:
#line 1256 "parse.y"
	{ yyval.v.i = 0; }
break;
case 86:
#line 1259 "parse.y"
	{
			struct pf_rule		 r;
			struct node_host	*h = NULL, *hh;
			struct node_if		*i, *j;

			if (check_rulestate(PFCTL_STATE_FILTER))
				YYERROR;

			for (i = yystack.l_mark[-2].v.interface; i; i = i->next) {
				bzero(&r, sizeof(r));

				r.action = PF_DROP;
				r.direction = PF_IN;
				r.log = yystack.l_mark[-3].v.logquick.log;
				r.logif = yystack.l_mark[-3].v.logquick.logif;
				r.quick = yystack.l_mark[-3].v.logquick.quick;
				r.af = yystack.l_mark[-1].v.i;
				if (rule_label(&r, yystack.l_mark[0].v.antispoof_opts.label))
					YYERROR;
				r.rtableid = yystack.l_mark[0].v.antispoof_opts.rtableid;
				j = calloc(1, sizeof(struct node_if));
				if (j == NULL)
					err(1, "antispoof: calloc");
				if (strlcpy(j->ifname, i->ifname,
				    sizeof(j->ifname)) >= sizeof(j->ifname)) {
					free(j);
					yyerror("interface name too long");
					YYERROR;
				}
				j->not = 1;
				if (i->dynamic) {
					h = calloc(1, sizeof(*h));
					if (h == NULL)
						err(1, "address: calloc");
					h->addr.type = PF_ADDR_DYNIFTL;
					set_ipmask(h, 128);
					if (strlcpy(h->addr.v.ifname, i->ifname,
					    sizeof(h->addr.v.ifname)) >=
					    sizeof(h->addr.v.ifname)) {
						free(h);
						yyerror(
						    "interface name too long");
						YYERROR;
					}
					hh = malloc(sizeof(*hh));
					if (hh == NULL)
						 err(1, "address: malloc");
					bcopy(h, hh, sizeof(*hh));
					h->addr.iflags = PFI_AFLAG_NETWORK;
				} else {
					h = ifa_lookup(j->ifname,
					    PFI_AFLAG_NETWORK);
					hh = NULL;
				}

				if (h != NULL)
					expand_rule(&r, j, NULL, NULL, NULL, h,
					    NULL, NULL, NULL, NULL, NULL,
					    NULL, "");

				if ((i->ifa_flags & IFF_LOOPBACK) == 0) {
					bzero(&r, sizeof(r));

					r.action = PF_DROP;
					r.direction = PF_IN;
					r.log = yystack.l_mark[-3].v.logquick.log;
					r.logif = yystack.l_mark[-3].v.logquick.logif;
					r.quick = yystack.l_mark[-3].v.logquick.quick;
					r.af = yystack.l_mark[-1].v.i;
					if (rule_label(&r, yystack.l_mark[0].v.antispoof_opts.label))
						YYERROR;
					r.rtableid = yystack.l_mark[0].v.antispoof_opts.rtableid;
					if (hh != NULL)
						h = hh;
					else
						h = ifa_lookup(i->ifname, 0);
					if (h != NULL)
						expand_rule(&r, NULL, NULL,
						    NULL, NULL, h, NULL, NULL,
						    NULL, NULL, NULL, NULL, "");
				} else
					free(hh);
			}
			free(yystack.l_mark[0].v.antispoof_opts.label);
		}
break;
case 87:
#line 1346 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 88:
#line 1347 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 89:
#line 1350 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 90:
#line 1351 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
break;
case 91:
#line 1358 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 92:
#line 1359 "parse.y"
	{
			yystack.l_mark[-1].v.interface->dynamic = 1;
			yyval.v.interface = yystack.l_mark[-1].v.interface;
		}
break;
case 93:
#line 1365 "parse.y"
	{
				bzero(&antispoof_opts, sizeof antispoof_opts);
				antispoof_opts.rtableid = -1;
			}
break;
case 94:
#line 1370 "parse.y"
	{ yyval.v.antispoof_opts = antispoof_opts; }
break;
case 95:
#line 1371 "parse.y"
	{
			bzero(&antispoof_opts, sizeof antispoof_opts);
			antispoof_opts.rtableid = -1;
			yyval.v.antispoof_opts = antispoof_opts;
		}
break;
case 98:
#line 1382 "parse.y"
	{
			if (antispoof_opts.label) {
				yyerror("label cannot be redefined");
				YYERROR;
			}
			antispoof_opts.label = yystack.l_mark[0].v.string;
		}
break;
case 99:
#line 1389 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			antispoof_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 100:
#line 1398 "parse.y"
	{ yyval.v.number = 1; }
break;
case 101:
#line 1399 "parse.y"
	{ yyval.v.number = 0; }
break;
case 102:
#line 1402 "parse.y"
	{
			struct node_host	 *h, *nh;
			struct node_tinit	 *ti, *nti;

			if (strlen(yystack.l_mark[-2].v.string) >= PF_TABLE_NAME_SIZE) {
				yyerror("table name too long, max %d chars",
				    PF_TABLE_NAME_SIZE - 1);
				free(yystack.l_mark[-2].v.string);
				YYERROR;
			}
			if (pf->loadopt & PFCTL_FLAG_TABLE)
				if (process_tabledef(yystack.l_mark[-2].v.string, &yystack.l_mark[0].v.table_opts)) {
					free(yystack.l_mark[-2].v.string);
					YYERROR;
				}
			free(yystack.l_mark[-2].v.string);
			for (ti = SIMPLEQ_FIRST(&yystack.l_mark[0].v.table_opts.init_nodes);
			    ti != SIMPLEQ_END(&yystack.l_mark[0].v.table_opts.init_nodes); ti = nti) {
				if (ti->file)
					free(ti->file);
				for (h = ti->host; h != NULL; h = nh) {
					nh = h->next;
					free(h);
				}
				nti = SIMPLEQ_NEXT(ti, entries);
				free(ti);
			}
		}
break;
case 103:
#line 1432 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
		}
break;
case 104:
#line 1437 "parse.y"
	{ yyval.v.table_opts = table_opts; }
break;
case 105:
#line 1439 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
			yyval.v.table_opts = table_opts;
		}
break;
case 108:
#line 1450 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "const"))
				table_opts.flags |= PFR_TFLAG_CONST;
			else if (!strcmp(yystack.l_mark[0].v.string, "persist"))
				table_opts.flags |= PFR_TFLAG_PERSIST;
			else if (!strcmp(yystack.l_mark[0].v.string, "counters"))
				table_opts.flags |= PFR_TFLAG_COUNTERS;
			else {
				yyerror("invalid table option '%s'", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 109:
#line 1464 "parse.y"
	{ table_opts.init_addr = 1; }
break;
case 110:
#line 1465 "parse.y"
	{
			struct node_host	*n;
			struct node_tinit	*ti;

			for (n = yystack.l_mark[-1].v.host; n != NULL; n = n->next) {
				switch (n->addr.type) {
				case PF_ADDR_ADDRMASK:
					continue; /* ok */
				case PF_ADDR_RANGE:
					yyerror("address ranges are not "
					    "permitted inside tables");
					break;
				case PF_ADDR_DYNIFTL:
					yyerror("dynamic addresses are not "
					    "permitted inside tables");
					break;
				case PF_ADDR_TABLE:
					yyerror("tables cannot contain tables");
					break;
				case PF_ADDR_NOROUTE:
					yyerror("\"no-route\" is not permitted "
					    "inside tables");
					break;
				case PF_ADDR_URPFFAILED:
					yyerror("\"urpf-failed\" is not "
					    "permitted inside tables");
					break;
				default:
					yyerror("unknown address type %d",
					    n->addr.type);
				}
				YYERROR;
			}
			if (!(ti = calloc(1, sizeof(*ti))))
				err(1, "table_opt: calloc");
			ti->host = yystack.l_mark[-1].v.host;
			SIMPLEQ_INSERT_TAIL(&table_opts.init_nodes, ti,
			    entries);
			table_opts.init_addr = 1;
		}
break;
case 111:
#line 1505 "parse.y"
	{
			struct node_tinit	*ti;

			if (!(ti = calloc(1, sizeof(*ti))))
				err(1, "table_opt: calloc");
			ti->file = yystack.l_mark[0].v.string;
			SIMPLEQ_INSERT_TAIL(&table_opts.init_nodes, ti,
			    entries);
			table_opts.init_addr = 1;
		}
break;
case 112:
#line 1517 "parse.y"
	{
			struct pf_altq	a;

			if (check_rulestate(PFCTL_STATE_QUEUE))
				YYERROR;

			memset(&a, 0, sizeof(a));
			if (yystack.l_mark[-2].v.queue_opts.scheduler.qtype == ALTQT_NONE) {
				yyerror("no scheduler specified!");
				YYERROR;
			}
			a.scheduler = yystack.l_mark[-2].v.queue_opts.scheduler.qtype;
			a.qlimit = yystack.l_mark[-2].v.queue_opts.qlimit;
			a.tbrsize = yystack.l_mark[-2].v.queue_opts.tbrsize;
			if (yystack.l_mark[0].v.queue == NULL && yystack.l_mark[-2].v.queue_opts.scheduler.qtype != ALTQT_CODEL) {
				yyerror("no child queues specified");
				YYERROR;
			}
			if (expand_altq(&a, yystack.l_mark[-3].v.interface, yystack.l_mark[0].v.queue, yystack.l_mark[-2].v.queue_opts.queue_bwspec,
			    &yystack.l_mark[-2].v.queue_opts.scheduler))
				YYERROR;
		}
break;
case 113:
#line 1541 "parse.y"
	{
			struct pf_altq	a;

			if (check_rulestate(PFCTL_STATE_QUEUE)) {
				free(yystack.l_mark[-3].v.string);
				YYERROR;
			}

			memset(&a, 0, sizeof(a));

			if (strlcpy(a.qname, yystack.l_mark[-3].v.string, sizeof(a.qname)) >=
			    sizeof(a.qname)) {
				yyerror("queue name too long (max "
				    "%d chars)", PF_QNAME_SIZE-1);
				free(yystack.l_mark[-3].v.string);
				YYERROR;
			}
			free(yystack.l_mark[-3].v.string);
			if (yystack.l_mark[-1].v.queue_opts.tbrsize) {
				yyerror("cannot specify tbrsize for queue");
				YYERROR;
			}
			if (yystack.l_mark[-1].v.queue_opts.priority > 255) {
				yyerror("priority out of range: max 255");
				YYERROR;
			}
			a.priority = yystack.l_mark[-1].v.queue_opts.priority;
			a.qlimit = yystack.l_mark[-1].v.queue_opts.qlimit;
			a.scheduler = yystack.l_mark[-1].v.queue_opts.scheduler.qtype;
			if (expand_queue(&a, yystack.l_mark[-2].v.interface, yystack.l_mark[0].v.queue, yystack.l_mark[-1].v.queue_opts.queue_bwspec,
			    &yystack.l_mark[-1].v.queue_opts.scheduler)) {
				yyerror("errors in queue definition");
				YYERROR;
			}
		}
break;
case 114:
#line 1578 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
		}
break;
case 115:
#line 1586 "parse.y"
	{ yyval.v.queue_opts = queue_opts; }
break;
case 116:
#line 1587 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
			yyval.v.queue_opts = queue_opts;
		}
break;
case 119:
#line 1601 "parse.y"
	{
			if (queue_opts.marker & QOM_BWSPEC) {
				yyerror("bandwidth cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_BWSPEC;
			queue_opts.queue_bwspec = yystack.l_mark[0].v.queue_bwspec;
		}
break;
case 120:
#line 1609 "parse.y"
	{
			if (queue_opts.marker & QOM_PRIORITY) {
				yyerror("priority cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("priority out of range: max 255");
				YYERROR;
			}
			queue_opts.marker |= QOM_PRIORITY;
			queue_opts.priority = yystack.l_mark[0].v.number;
		}
break;
case 121:
#line 1621 "parse.y"
	{
			if (queue_opts.marker & QOM_QLIMIT) {
				yyerror("qlimit cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65535) {
				yyerror("qlimit out of range: max 65535");
				YYERROR;
			}
			queue_opts.marker |= QOM_QLIMIT;
			queue_opts.qlimit = yystack.l_mark[0].v.number;
		}
break;
case 122:
#line 1633 "parse.y"
	{
			if (queue_opts.marker & QOM_SCHEDULER) {
				yyerror("scheduler cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_SCHEDULER;
			queue_opts.scheduler = yystack.l_mark[0].v.queue_options;
		}
break;
case 123:
#line 1641 "parse.y"
	{
			if (queue_opts.marker & QOM_TBRSIZE) {
				yyerror("tbrsize cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("tbrsize too big: max %u", UINT_MAX);
				YYERROR;
			}
			queue_opts.marker |= QOM_TBRSIZE;
			queue_opts.tbrsize = yystack.l_mark[0].v.number;
		}
break;
case 124:
#line 1655 "parse.y"
	{
			double	 bps;
			char	*cp;

			yyval.v.queue_bwspec.bw_percent = 0;

			bps = strtod(yystack.l_mark[0].v.string, &cp);
			if (cp != NULL) {
				if (strlen(cp) > 1) {
					char *cu = cp + 1;
					if (!strcmp(cu, "Bit") ||
					    !strcmp(cu, "B") ||
					    !strcmp(cu, "bit") ||
					    !strcmp(cu, "b")) {
						*cu = 0;
					}
				}
				if (!strcmp(cp, "b"))
					; /* nothing */
				else if (!strcmp(cp, "K"))
					bps *= 1000;
				else if (!strcmp(cp, "M"))
					bps *= 1000 * 1000;
				else if (!strcmp(cp, "G"))
					bps *= 1000 * 1000 * 1000;
				else if (!strcmp(cp, "%")) {
					if (bps < 0 || bps > 100) {
						yyerror("bandwidth spec "
						    "out of range");
						free(yystack.l_mark[0].v.string);
						YYERROR;
					}
					yyval.v.queue_bwspec.bw_percent = bps;
					bps = 0;
				} else {
					yyerror("unknown unit %s", cp);
					free(yystack.l_mark[0].v.string);
					YYERROR;
				}
			}
			free(yystack.l_mark[0].v.string);
			yyval.v.queue_bwspec.bw_absolute = (u_int64_t)bps;
		}
break;
case 125:
#line 1698 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= LLONG_MAX) {
				yyerror("bandwidth number too big");
				YYERROR;
			}
			yyval.v.queue_bwspec.bw_percent = 0;
			yyval.v.queue_bwspec.bw_absolute = yystack.l_mark[0].v.number;
		}
break;
case 126:
#line 1708 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = 0;
		}
break;
case 127:
#line 1712 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = yystack.l_mark[-1].v.number;
		}
break;
case 128:
#line 1716 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = 0;
		}
break;
case 129:
#line 1720 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = yystack.l_mark[-1].v.number;
		}
break;
case 130:
#line 1724 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			bzero(&yyval.v.queue_options.data.hfsc_opts,
			    sizeof(struct node_hfsc_opts));
		}
break;
case 131:
#line 1729 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			yyval.v.queue_options.data.hfsc_opts = yystack.l_mark[-1].v.hfsc_opts;
		}
break;
case 132:
#line 1733 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			bzero(&yyval.v.queue_options.data.fairq_opts,
				sizeof(struct node_fairq_opts));
		}
break;
case 133:
#line 1738 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			yyval.v.queue_options.data.fairq_opts = yystack.l_mark[-1].v.fairq_opts;
		}
break;
case 134:
#line 1742 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			bzero(&yyval.v.queue_options.data.codel_opts,
				sizeof(struct codel_opts));
		}
break;
case 135:
#line 1747 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			yyval.v.queue_options.data.codel_opts = yystack.l_mark[-1].v.codel_opts;
		}
break;
case 136:
#line 1753 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 137:
#line 1754 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 138:
#line 1757 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				yyval.v.number = CBQCLF_DEFCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "borrow"))
				yyval.v.number = CBQCLF_BORROW;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				yyval.v.number = CBQCLF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				yyval.v.number = CBQCLF_RED|CBQCLF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				yyval.v.number = CBQCLF_RIO;
			else if (!strcmp(yystack.l_mark[0].v.string, "codel"))
				yyval.v.number = CBQCLF_CODEL;
			else {
				yyerror("unknown cbq flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 139:
#line 1779 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 140:
#line 1780 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 141:
#line 1783 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				yyval.v.number = PRCF_DEFAULTCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				yyval.v.number = PRCF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				yyval.v.number = PRCF_RED|PRCF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				yyval.v.number = PRCF_RIO;
			else if (!strcmp(yystack.l_mark[0].v.string, "codel"))
				yyval.v.number = PRCF_CODEL;
			else {
				yyerror("unknown priq flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 142:
#line 1803 "parse.y"
	{
				bzero(&hfsc_opts,
				    sizeof(struct node_hfsc_opts));
			}
break;
case 143:
#line 1807 "parse.y"
	{
			yyval.v.hfsc_opts = hfsc_opts;
		}
break;
case 146:
#line 1816 "parse.y"
	{
			if (hfsc_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			hfsc_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.linkshare.used = 1;
		}
break;
case 147:
#line 1825 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > INT_MAX) {
				yyerror("timing in curve out of range");
				YYERROR;
			}
			if (hfsc_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			hfsc_opts.linkshare.m1 = yystack.l_mark[-5].v.queue_bwspec;
			hfsc_opts.linkshare.d = yystack.l_mark[-3].v.number;
			hfsc_opts.linkshare.m2 = yystack.l_mark[-1].v.queue_bwspec;
			hfsc_opts.linkshare.used = 1;
		}
break;
case 148:
#line 1839 "parse.y"
	{
			if (hfsc_opts.realtime.used) {
				yyerror("realtime already specified");
				YYERROR;
			}
			hfsc_opts.realtime.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.realtime.used = 1;
		}
break;
case 149:
#line 1848 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > INT_MAX) {
				yyerror("timing in curve out of range");
				YYERROR;
			}
			if (hfsc_opts.realtime.used) {
				yyerror("realtime already specified");
				YYERROR;
			}
			hfsc_opts.realtime.m1 = yystack.l_mark[-5].v.queue_bwspec;
			hfsc_opts.realtime.d = yystack.l_mark[-3].v.number;
			hfsc_opts.realtime.m2 = yystack.l_mark[-1].v.queue_bwspec;
			hfsc_opts.realtime.used = 1;
		}
break;
case 150:
#line 1862 "parse.y"
	{
			if (hfsc_opts.upperlimit.used) {
				yyerror("upperlimit already specified");
				YYERROR;
			}
			hfsc_opts.upperlimit.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.upperlimit.used = 1;
		}
break;
case 151:
#line 1871 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > INT_MAX) {
				yyerror("timing in curve out of range");
				YYERROR;
			}
			if (hfsc_opts.upperlimit.used) {
				yyerror("upperlimit already specified");
				YYERROR;
			}
			hfsc_opts.upperlimit.m1 = yystack.l_mark[-5].v.queue_bwspec;
			hfsc_opts.upperlimit.d = yystack.l_mark[-3].v.number;
			hfsc_opts.upperlimit.m2 = yystack.l_mark[-1].v.queue_bwspec;
			hfsc_opts.upperlimit.used = 1;
		}
break;
case 152:
#line 1885 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				hfsc_opts.flags |= HFCF_DEFAULTCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				hfsc_opts.flags |= HFCF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				hfsc_opts.flags |= HFCF_RED|HFCF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				hfsc_opts.flags |= HFCF_RIO;
			else if (!strcmp(yystack.l_mark[0].v.string, "codel"))
				hfsc_opts.flags |= HFCF_CODEL;
			else {
				yyerror("unknown hfsc flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 153:
#line 1905 "parse.y"
	{
				bzero(&fairq_opts,
				    sizeof(struct node_fairq_opts));
			}
break;
case 154:
#line 1909 "parse.y"
	{
			yyval.v.fairq_opts = fairq_opts;
		}
break;
case 157:
#line 1918 "parse.y"
	{
			if (fairq_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			fairq_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			fairq_opts.linkshare.used = 1;
		}
break;
case 158:
#line 1926 "parse.y"
	{
			if (fairq_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			fairq_opts.linkshare.m1 = yystack.l_mark[-3].v.queue_bwspec;
			fairq_opts.linkshare.d = yystack.l_mark[-2].v.number;
			fairq_opts.linkshare.m2 = yystack.l_mark[-1].v.queue_bwspec;
			fairq_opts.linkshare.used = 1;
		}
break;
case 159:
#line 1936 "parse.y"
	{
			fairq_opts.hogs_bw = yystack.l_mark[0].v.queue_bwspec;
		}
break;
case 160:
#line 1939 "parse.y"
	{
			fairq_opts.nbuckets = yystack.l_mark[0].v.number;
		}
break;
case 161:
#line 1942 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				fairq_opts.flags |= FARF_DEFAULTCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				fairq_opts.flags |= FARF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				fairq_opts.flags |= FARF_RED|FARF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				fairq_opts.flags |= FARF_RIO;
			else if (!strcmp(yystack.l_mark[0].v.string, "codel"))
				fairq_opts.flags |= FARF_CODEL;
			else {
				yyerror("unknown fairq flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 162:
#line 1962 "parse.y"
	{
				bzero(&codel_opts,
				    sizeof(struct codel_opts));
			}
break;
case 163:
#line 1966 "parse.y"
	{
			yyval.v.codel_opts = codel_opts;
		}
break;
case 166:
#line 1975 "parse.y"
	{
			if (codel_opts.interval) {
				yyerror("interval already specified");
				YYERROR;
			}
			codel_opts.interval = yystack.l_mark[0].v.number;
		}
break;
case 167:
#line 1982 "parse.y"
	{
			if (codel_opts.target) {
				yyerror("target already specified");
				YYERROR;
			}
			codel_opts.target = yystack.l_mark[0].v.number;
		}
break;
case 168:
#line 1989 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				codel_opts.ecn = 1;
			else {
				yyerror("unknown codel option \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 169:
#line 2001 "parse.y"
	{ yyval.v.queue = NULL; }
break;
case 170:
#line 2002 "parse.y"
	{ yyval.v.queue = yystack.l_mark[0].v.queue; }
break;
case 171:
#line 2003 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
break;
case 172:
#line 2006 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
break;
case 173:
#line 2007 "parse.y"
	{
			yystack.l_mark[-3].v.queue->tail->next = yystack.l_mark[-1].v.queue;
			yystack.l_mark[-3].v.queue->tail = yystack.l_mark[-1].v.queue;
			yyval.v.queue = yystack.l_mark[-3].v.queue;
		}
break;
case 174:
#line 2014 "parse.y"
	{
			yyval.v.queue = calloc(1, sizeof(struct node_queue));
			if (yyval.v.queue == NULL)
				err(1, "qassign_item: calloc");
			if (strlcpy(yyval.v.queue->queue, yystack.l_mark[0].v.string, sizeof(yyval.v.queue->queue)) >=
			    sizeof(yyval.v.queue->queue)) {
				yyerror("queue name '%s' too long (max "
				    "%d chars)", yystack.l_mark[0].v.string, sizeof(yyval.v.queue->queue)-1);
				free(yystack.l_mark[0].v.string);
				free(yyval.v.queue);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
			yyval.v.queue->next = NULL;
			yyval.v.queue->tail = yyval.v.queue;
		}
break;
case 175:
#line 2034 "parse.y"
	{
			struct pf_rule		 r;
			struct node_state_opt	*o;
			struct node_proto	*proto;
			int			 srctrack = 0;
			int			 statelock = 0;
			int			 adaptive = 0;
			int			 defaults = 0;

			if (check_rulestate(PFCTL_STATE_FILTER))
				YYERROR;

			memset(&r, 0, sizeof(r));

			r.action = yystack.l_mark[-8].v.b.b1;
			switch (yystack.l_mark[-8].v.b.b2) {
			case PFRULE_RETURNRST:
				r.rule_flag |= PFRULE_RETURNRST;
				r.return_ttl = yystack.l_mark[-8].v.b.w;
				break;
			case PFRULE_RETURNICMP:
				r.rule_flag |= PFRULE_RETURNICMP;
				r.return_icmp = yystack.l_mark[-8].v.b.w;
				r.return_icmp6 = yystack.l_mark[-8].v.b.w2;
				break;
			case PFRULE_RETURN:
				r.rule_flag |= PFRULE_RETURN;
				r.return_icmp = yystack.l_mark[-8].v.b.w;
				r.return_icmp6 = yystack.l_mark[-8].v.b.w2;
				break;
			}
			r.direction = yystack.l_mark[-7].v.i;
			r.log = yystack.l_mark[-6].v.logquick.log;
			r.logif = yystack.l_mark[-6].v.logquick.logif;
			r.quick = yystack.l_mark[-6].v.logquick.quick;
			r.prob = yystack.l_mark[0].v.filter_opts.prob;
			r.rtableid = yystack.l_mark[0].v.filter_opts.rtableid;

			if (yystack.l_mark[0].v.filter_opts.marker & FOM_PRIO) {
				if (yystack.l_mark[0].v.filter_opts.prio == 0)
					r.prio = PF_PRIO_ZERO;
				else
					r.prio = yystack.l_mark[0].v.filter_opts.prio;
			}
			if (yystack.l_mark[0].v.filter_opts.marker & FOM_SETPRIO) {
				r.set_prio[0] = yystack.l_mark[0].v.filter_opts.set_prio[0];
				r.set_prio[1] = yystack.l_mark[0].v.filter_opts.set_prio[1];
				r.scrub_flags |= PFSTATE_SETPRIO;
			}

			r.af = yystack.l_mark[-3].v.i;
			if (yystack.l_mark[0].v.filter_opts.tag)
				if (strlcpy(r.tagname, yystack.l_mark[0].v.filter_opts.tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			if (yystack.l_mark[0].v.filter_opts.match_tag)
				if (strlcpy(r.match_tagname, yystack.l_mark[0].v.filter_opts.match_tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[0].v.filter_opts.match_tag_not;
			if (rule_label(&r, yystack.l_mark[0].v.filter_opts.label))
				YYERROR;
			free(yystack.l_mark[0].v.filter_opts.label);
			r.flags = yystack.l_mark[0].v.filter_opts.flags.b1;
			r.flagset = yystack.l_mark[0].v.filter_opts.flags.b2;
			if ((yystack.l_mark[0].v.filter_opts.flags.b1 & yystack.l_mark[0].v.filter_opts.flags.b2) != yystack.l_mark[0].v.filter_opts.flags.b1) {
				yyerror("flags always false");
				YYERROR;
			}
			if (yystack.l_mark[0].v.filter_opts.flags.b1 || yystack.l_mark[0].v.filter_opts.flags.b2 || yystack.l_mark[-1].v.fromto.src_os) {
				for (proto = yystack.l_mark[-2].v.proto; proto != NULL &&
				    proto->proto != IPPROTO_TCP;
				    proto = proto->next)
					;	/* nothing */
				if (proto == NULL && yystack.l_mark[-2].v.proto != NULL) {
					if (yystack.l_mark[0].v.filter_opts.flags.b1 || yystack.l_mark[0].v.filter_opts.flags.b2)
						yyerror(
						    "flags only apply to tcp");
					if (yystack.l_mark[-1].v.fromto.src_os)
						yyerror(
						    "OS fingerprinting only "
						    "apply to tcp");
					YYERROR;
				}
#if 0
				if ((yystack.l_mark[0].v.filter_opts.flags.b1 & parse_flags("S")) == 0 &&
				    yystack.l_mark[-1].v.fromto.src_os) {
					yyerror("OS fingerprinting requires "
					    "the SYN TCP flag (flags S/SA)");
					YYERROR;
				}
#endif
			}

			r.tos = yystack.l_mark[0].v.filter_opts.tos;
			r.keep_state = yystack.l_mark[0].v.filter_opts.keep.action;
			o = yystack.l_mark[0].v.filter_opts.keep.options;

			/* 'keep state' by default on pass rules. */
			if (!r.keep_state && !r.action &&
			    !(yystack.l_mark[0].v.filter_opts.marker & FOM_KEEP)) {
				r.keep_state = PF_STATE_NORMAL;
				o = keep_state_defaults;
				defaults = 1;
			}

			while (o) {
				struct node_state_opt	*p = o;

				switch (o->type) {
				case PF_STATE_OPT_MAX:
					if (r.max_states) {
						yyerror("state option 'max' "
						    "multiple definitions");
						YYERROR;
					}
					r.max_states = o->data.max_states;
					break;
				case PF_STATE_OPT_NOSYNC:
					if (r.rule_flag & PFRULE_NOSYNC) {
						yyerror("state option 'sync' "
						    "multiple definitions");
						YYERROR;
					}
					r.rule_flag |= PFRULE_NOSYNC;
					break;
				case PF_STATE_OPT_SRCTRACK:
					if (srctrack) {
						yyerror("state option "
						    "'source-track' "
						    "multiple definitions");
						YYERROR;
					}
					srctrack =  o->data.src_track;
					r.rule_flag |= PFRULE_SRCTRACK;
					break;
				case PF_STATE_OPT_MAX_SRC_STATES:
					if (r.max_src_states) {
						yyerror("state option "
						    "'max-src-states' "
						    "multiple definitions");
						YYERROR;
					}
					if (o->data.max_src_states == 0) {
						yyerror("'max-src-states' must "
						    "be > 0");
						YYERROR;
					}
					r.max_src_states =
					    o->data.max_src_states;
					r.rule_flag |= PFRULE_SRCTRACK;
					break;
				case PF_STATE_OPT_OVERLOAD:
					if (r.overload_tblname[0]) {
						yyerror("multiple 'overload' "
						    "table definitions");
						YYERROR;
					}
					if (strlcpy(r.overload_tblname,
					    o->data.overload.tblname,
					    PF_TABLE_NAME_SIZE) >=
					    PF_TABLE_NAME_SIZE) {
						yyerror("state option: "
						    "strlcpy");
						YYERROR;
					}
					r.flush = o->data.overload.flush;
					break;
				case PF_STATE_OPT_MAX_SRC_CONN:
					if (r.max_src_conn) {
						yyerror("state option "
						    "'max-src-conn' "
						    "multiple definitions");
						YYERROR;
					}
					if (o->data.max_src_conn == 0) {
						yyerror("'max-src-conn' "
						    "must be > 0");
						YYERROR;
					}
					r.max_src_conn =
					    o->data.max_src_conn;
					r.rule_flag |= PFRULE_SRCTRACK |
					    PFRULE_RULESRCTRACK;
					break;
				case PF_STATE_OPT_MAX_SRC_CONN_RATE:
					if (r.max_src_conn_rate.limit) {
						yyerror("state option "
						    "'max-src-conn-rate' "
						    "multiple definitions");
						YYERROR;
					}
					if (!o->data.max_src_conn_rate.limit ||
					    !o->data.max_src_conn_rate.seconds) {
						yyerror("'max-src-conn-rate' "
						    "values must be > 0");
						YYERROR;
					}
					if (o->data.max_src_conn_rate.limit >
					    PF_THRESHOLD_MAX) {
						yyerror("'max-src-conn-rate' "
						    "maximum rate must be < %u",
						    PF_THRESHOLD_MAX);
						YYERROR;
					}
					r.max_src_conn_rate.limit =
					    o->data.max_src_conn_rate.limit;
					r.max_src_conn_rate.seconds =
					    o->data.max_src_conn_rate.seconds;
					r.rule_flag |= PFRULE_SRCTRACK |
					    PFRULE_RULESRCTRACK;
					break;
				case PF_STATE_OPT_MAX_SRC_NODES:
					if (r.max_src_nodes) {
						yyerror("state option "
						    "'max-src-nodes' "
						    "multiple definitions");
						YYERROR;
					}
					if (o->data.max_src_nodes == 0) {
						yyerror("'max-src-nodes' must "
						    "be > 0");
						YYERROR;
					}
					r.max_src_nodes =
					    o->data.max_src_nodes;
					r.rule_flag |= PFRULE_SRCTRACK |
					    PFRULE_RULESRCTRACK;
					break;
				case PF_STATE_OPT_STATELOCK:
					if (statelock) {
						yyerror("state locking option: "
						    "multiple definitions");
						YYERROR;
					}
					statelock = 1;
					r.rule_flag |= o->data.statelock;
					break;
				case PF_STATE_OPT_SLOPPY:
					if (r.rule_flag & PFRULE_STATESLOPPY) {
						yyerror("state sloppy option: "
						    "multiple definitions");
						YYERROR;
					}
					r.rule_flag |= PFRULE_STATESLOPPY;
					break;
				case PF_STATE_OPT_TIMEOUT:
					if (o->data.timeout.number ==
					    PFTM_ADAPTIVE_START ||
					    o->data.timeout.number ==
					    PFTM_ADAPTIVE_END)
						adaptive = 1;
					if (r.timeout[o->data.timeout.number]) {
						yyerror("state timeout %s "
						    "multiple definitions",
						    pf_timeouts[o->data.
						    timeout.number].name);
						YYERROR;
					}
					r.timeout[o->data.timeout.number] =
					    o->data.timeout.seconds;
				}
				o = o->next;
				if (!defaults)
					free(p);
			}

			/* 'flags S/SA' by default on stateful rules */
			if (!r.action && !r.flags && !r.flagset &&
			    !yystack.l_mark[0].v.filter_opts.fragment && !(yystack.l_mark[0].v.filter_opts.marker & FOM_FLAGS) &&
			    r.keep_state) {
				r.flags = parse_flags("S");
				r.flagset =  parse_flags("SA");
			}
			if (!adaptive && r.max_states) {
				r.timeout[PFTM_ADAPTIVE_START] =
				    (r.max_states / 10) * 6;
				r.timeout[PFTM_ADAPTIVE_END] =
				    (r.max_states / 10) * 12;
			}
			if (r.rule_flag & PFRULE_SRCTRACK) {
				if (srctrack == PF_SRCTRACK_GLOBAL &&
				    r.max_src_nodes) {
					yyerror("'max-src-nodes' is "
					    "incompatible with "
					    "'source-track global'");
					YYERROR;
				}
				if (srctrack == PF_SRCTRACK_GLOBAL &&
				    r.max_src_conn) {
					yyerror("'max-src-conn' is "
					    "incompatible with "
					    "'source-track global'");
					YYERROR;
				}
				if (srctrack == PF_SRCTRACK_GLOBAL &&
				    r.max_src_conn_rate.seconds) {
					yyerror("'max-src-conn-rate' is "
					    "incompatible with "
					    "'source-track global'");
					YYERROR;
				}
				if (r.timeout[PFTM_SRC_NODE] <
				    r.max_src_conn_rate.seconds)
					r.timeout[PFTM_SRC_NODE] =
					    r.max_src_conn_rate.seconds;
				r.rule_flag |= PFRULE_SRCTRACK;
				if (srctrack == PF_SRCTRACK_RULE)
					r.rule_flag |= PFRULE_RULESRCTRACK;
			}
			if (r.keep_state && !statelock)
				r.rule_flag |= default_statelock;

			if (yystack.l_mark[0].v.filter_opts.fragment)
				r.rule_flag |= PFRULE_FRAGMENT;
			r.allow_opts = yystack.l_mark[0].v.filter_opts.allowopts;

			decide_address_family(yystack.l_mark[-1].v.fromto.src.host, &r.af);
			decide_address_family(yystack.l_mark[-1].v.fromto.dst.host, &r.af);

			if (yystack.l_mark[-4].v.route.rt) {
				if (!r.direction) {
					yyerror("direction must be explicit "
					    "with rules that specify routing");
					YYERROR;
				}
				r.rt = yystack.l_mark[-4].v.route.rt;
				r.rpool.opts = yystack.l_mark[-4].v.route.pool_opts;
				if (yystack.l_mark[-4].v.route.key != NULL)
					memcpy(&r.rpool.key, yystack.l_mark[-4].v.route.key,
					    sizeof(struct pf_poolhashkey));
			}
			if (r.rt) {
				decide_address_family(yystack.l_mark[-4].v.route.host, &r.af);
				remove_invalid_hosts(&yystack.l_mark[-4].v.route.host, &r.af);
				if (yystack.l_mark[-4].v.route.host == NULL) {
					yyerror("no routing address with "
					    "matching address family found.");
					YYERROR;
				}
				if ((r.rpool.opts & PF_POOL_TYPEMASK) ==
				    PF_POOL_NONE && (yystack.l_mark[-4].v.route.host->next != NULL ||
				    yystack.l_mark[-4].v.route.host->addr.type == PF_ADDR_TABLE ||
				    DYNIF_MULTIADDR(yystack.l_mark[-4].v.route.host->addr)))
					r.rpool.opts |= PF_POOL_ROUNDROBIN;
				if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
				    PF_POOL_ROUNDROBIN &&
				    disallow_table(yystack.l_mark[-4].v.route.host, "tables are only "
				    "supported in round-robin routing pools"))
					YYERROR;
				if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
				    PF_POOL_ROUNDROBIN &&
				    disallow_alias(yystack.l_mark[-4].v.route.host, "interface (%s) "
				    "is only supported in round-robin "
				    "routing pools"))
					YYERROR;
				if (yystack.l_mark[-4].v.route.host->next != NULL) {
					if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
					    PF_POOL_ROUNDROBIN) {
						yyerror("r.rpool.opts must "
						    "be PF_POOL_ROUNDROBIN");
						YYERROR;
					}
				}
			}
			if (yystack.l_mark[0].v.filter_opts.queues.qname != NULL) {
				if (strlcpy(r.qname, yystack.l_mark[0].v.filter_opts.queues.qname,
				    sizeof(r.qname)) >= sizeof(r.qname)) {
					yyerror("rule qname too long (max "
					    "%d chars)", sizeof(r.qname)-1);
					YYERROR;
				}
				free(yystack.l_mark[0].v.filter_opts.queues.qname);
			}
			if (yystack.l_mark[0].v.filter_opts.queues.pqname != NULL) {
				if (strlcpy(r.pqname, yystack.l_mark[0].v.filter_opts.queues.pqname,
				    sizeof(r.pqname)) >= sizeof(r.pqname)) {
					yyerror("rule pqname too long (max "
					    "%d chars)", sizeof(r.pqname)-1);
					YYERROR;
				}
				free(yystack.l_mark[0].v.filter_opts.queues.pqname);
			}
#ifdef __FreeBSD__
			r.divert.port = yystack.l_mark[0].v.filter_opts.divert.port;
#else
			if ((r.divert.port = yystack.l_mark[0].v.filter_opts.divert.port)) {
				if (r.direction == PF_OUT) {
					if (yystack.l_mark[0].v.filter_opts.divert.addr) {
						yyerror("address specified "
						    "for outgoing divert");
						YYERROR;
					}
					bzero(&r.divert.addr,
					    sizeof(r.divert.addr));
				} else {
					if (!yystack.l_mark[0].v.filter_opts.divert.addr) {
						yyerror("no address specified "
						    "for incoming divert");
						YYERROR;
					}
					if (yystack.l_mark[0].v.filter_opts.divert.addr->af != r.af) {
						yyerror("address family "
						    "mismatch for divert");
						YYERROR;
					}
					r.divert.addr =
					    yystack.l_mark[0].v.filter_opts.divert.addr->addr.v.a.addr;
				}
			}
#endif

			expand_rule(&r, yystack.l_mark[-5].v.interface, yystack.l_mark[-4].v.route.host, yystack.l_mark[-2].v.proto, yystack.l_mark[-1].v.fromto.src_os,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.src.port, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-1].v.fromto.dst.port,
			    yystack.l_mark[0].v.filter_opts.uid, yystack.l_mark[0].v.filter_opts.gid, yystack.l_mark[0].v.filter_opts.icmpspec, "");
		}
break;
case 176:
#line 2458 "parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
				filter_opts.rtableid = -1;
			}
break;
case 177:
#line 2463 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
break;
case 178:
#line 2464 "parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			filter_opts.rtableid = -1;
			yyval.v.filter_opts = filter_opts;
		}
break;
case 181:
#line 2475 "parse.y"
	{
			if (filter_opts.uid)
				yystack.l_mark[0].v.uid->tail->next = filter_opts.uid;
			filter_opts.uid = yystack.l_mark[0].v.uid;
		}
break;
case 182:
#line 2480 "parse.y"
	{
			if (filter_opts.gid)
				yystack.l_mark[0].v.gid->tail->next = filter_opts.gid;
			filter_opts.gid = yystack.l_mark[0].v.gid;
		}
break;
case 183:
#line 2485 "parse.y"
	{
			if (filter_opts.marker & FOM_FLAGS) {
				yyerror("flags cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_FLAGS;
			filter_opts.flags.b1 |= yystack.l_mark[0].v.b.b1;
			filter_opts.flags.b2 |= yystack.l_mark[0].v.b.b2;
			filter_opts.flags.w |= yystack.l_mark[0].v.b.w;
			filter_opts.flags.w2 |= yystack.l_mark[0].v.b.w2;
		}
break;
case 184:
#line 2496 "parse.y"
	{
			if (filter_opts.marker & FOM_ICMP) {
				yyerror("icmp-type cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_ICMP;
			filter_opts.icmpspec = yystack.l_mark[0].v.icmp;
		}
break;
case 185:
#line 2504 "parse.y"
	{
			if (filter_opts.marker & FOM_PRIO) {
				yyerror("prio cannot be redefined");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			filter_opts.marker |= FOM_PRIO;
			filter_opts.prio = yystack.l_mark[0].v.number;
		}
break;
case 186:
#line 2516 "parse.y"
	{
			if (filter_opts.marker & FOM_TOS) {
				yyerror("tos cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_TOS;
			filter_opts.tos = yystack.l_mark[0].v.number;
		}
break;
case 187:
#line 2524 "parse.y"
	{
			if (filter_opts.marker & FOM_KEEP) {
				yyerror("modulate or keep cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_KEEP;
			filter_opts.keep.action = yystack.l_mark[0].v.keep_state.action;
			filter_opts.keep.options = yystack.l_mark[0].v.keep_state.options;
		}
break;
case 188:
#line 2533 "parse.y"
	{
			filter_opts.fragment = 1;
		}
break;
case 189:
#line 2536 "parse.y"
	{
			filter_opts.allowopts = 1;
		}
break;
case 190:
#line 2539 "parse.y"
	{
			if (filter_opts.label) {
				yyerror("label cannot be redefined");
				YYERROR;
			}
			filter_opts.label = yystack.l_mark[0].v.string;
		}
break;
case 191:
#line 2546 "parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
break;
case 192:
#line 2553 "parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
break;
case 193:
#line 2556 "parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
break;
case 194:
#line 2560 "parse.y"
	{
			double	p;

			p = floor(yystack.l_mark[0].v.probability * UINT_MAX + 0.5);
			if (p < 0.0 || p > UINT_MAX) {
				yyerror("invalid probability: %lf", p);
				YYERROR;
			}
			filter_opts.prob = (u_int32_t)p;
			if (filter_opts.prob == 0)
				filter_opts.prob = 1;
		}
break;
case 195:
#line 2572 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			filter_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 196:
#line 2579 "parse.y"
	{
#ifdef __FreeBSD__
			filter_opts.divert.port = yystack.l_mark[0].v.range.a;
			if (!filter_opts.divert.port) {
				yyerror("invalid divert port: %u", ntohs(yystack.l_mark[0].v.range.a));
				YYERROR;
			}
#endif
		}
break;
case 197:
#line 2588 "parse.y"
	{
#ifndef __FreeBSD__
			if ((filter_opts.divert.addr = host(yystack.l_mark[-2].v.string)) == NULL) {
				yyerror("could not parse divert address: %s",
				    yystack.l_mark[-2].v.string);
				free(yystack.l_mark[-2].v.string);
				YYERROR;
			}
#else
			if (yystack.l_mark[-2].v.string)
#endif
			free(yystack.l_mark[-2].v.string);
			filter_opts.divert.port = yystack.l_mark[0].v.range.a;
			if (!filter_opts.divert.port) {
				yyerror("invalid divert port: %u", ntohs(yystack.l_mark[0].v.range.a));
				YYERROR;
			}
		}
break;
case 198:
#line 2606 "parse.y"
	{
#ifdef __FreeBSD__
			yyerror("divert-reply has no meaning in FreeBSD pf(4)");
			YYERROR;
#else
			filter_opts.divert.port = 1;	/* some random value */
#endif
		}
break;
case 200:
#line 2617 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
break;
case 201:
#line 2618 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
break;
case 204:
#line 2625 "parse.y"
	{
			if (filter_opts.marker & FOM_SETPRIO) {
				yyerror("prio cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_SETPRIO;
			filter_opts.set_prio[0] = yystack.l_mark[0].v.b.b1;
			filter_opts.set_prio[1] = yystack.l_mark[0].v.b.b2;
		}
break;
case 205:
#line 2634 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yyval.v.b.b2 = yystack.l_mark[0].v.number;
		}
break;
case 206:
#line 2641 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > PF_PRIO_MAX ||
			    yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yystack.l_mark[-3].v.number;
			yyval.v.b.b2 = yystack.l_mark[-1].v.number;
		}
break;
case 207:
#line 2652 "parse.y"
	{
			char	*e;
			double	 p = strtod(yystack.l_mark[0].v.string, &e);

			if (*e == '%') {
				p *= 0.01;
				e++;
			}
			if (*e) {
				yyerror("invalid probability: %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
			yyval.v.probability = p;
		}
break;
case 208:
#line 2668 "parse.y"
	{
			yyval.v.probability = (double)yystack.l_mark[0].v.number;
		}
break;
case 209:
#line 2674 "parse.y"
	{
			yyval.v.b.b1 = PF_PASS;
			yyval.v.b.b2 = failpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 210:
#line 2680 "parse.y"
	{ yyval.v.b = yystack.l_mark[0].v.b; yyval.v.b.b1 = PF_DROP; }
break;
case 211:
#line 2683 "parse.y"
	{
			yyval.v.b.b2 = blockpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 212:
#line 2688 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_DROP;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
break;
case 213:
#line 2693 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
break;
case 214:
#line 2698 "parse.y"
	{
			if (yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > 255) {
				yyerror("illegal ttl value %d", yystack.l_mark[-1].v.number);
				YYERROR;
			}
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = 0;
		}
break;
case 215:
#line 2707 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 216:
#line 2712 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 217:
#line 2717 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = returnicmpdefault;
		}
break;
case 218:
#line 2722 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
break;
case 219:
#line 2727 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-3].v.number;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
break;
case 220:
#line 2732 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURN;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 221:
#line 2739 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 222:
#line 2746 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmpdefault >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
break;
case 223:
#line 2758 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET6))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 224:
#line 2765 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmp6default >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
break;
case 225:
#line 2777 "parse.y"
	{ yyval.v.i = PF_INOUT; }
break;
case 226:
#line 2778 "parse.y"
	{ yyval.v.i = PF_IN; }
break;
case 227:
#line 2779 "parse.y"
	{ yyval.v.i = PF_OUT; }
break;
case 228:
#line 2782 "parse.y"
	{ yyval.v.logquick.quick = 0; }
break;
case 229:
#line 2783 "parse.y"
	{ yyval.v.logquick.quick = 1; }
break;
case 230:
#line 2786 "parse.y"
	{ yyval.v.logquick.log = 0; yyval.v.logquick.quick = 0; yyval.v.logquick.logif = 0; }
break;
case 231:
#line 2787 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 0; }
break;
case 232:
#line 2788 "parse.y"
	{ yyval.v.logquick.quick = 1; yyval.v.logquick.log = 0; yyval.v.logquick.logif = 0; }
break;
case 233:
#line 2789 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[-1].v.logquick; yyval.v.logquick.quick = 1; }
break;
case 234:
#line 2790 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 1; }
break;
case 235:
#line 2793 "parse.y"
	{ yyval.v.logquick.log = PF_LOG; yyval.v.logquick.logif = 0; }
break;
case 236:
#line 2794 "parse.y"
	{
			yyval.v.logquick.log = PF_LOG | yystack.l_mark[-1].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[-1].v.logquick.logif;
		}
break;
case 237:
#line 2800 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; }
break;
case 238:
#line 2801 "parse.y"
	{
			yyval.v.logquick.log = yystack.l_mark[-2].v.logquick.log | yystack.l_mark[0].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[0].v.logquick.logif;
			if (yyval.v.logquick.logif == 0)
				yyval.v.logquick.logif = yystack.l_mark[-2].v.logquick.logif;
		}
break;
case 239:
#line 2809 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_ALL; yyval.v.logquick.logif = 0; }
break;
case 240:
#line 2810 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
break;
case 241:
#line 2811 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
break;
case 242:
#line 2812 "parse.y"
	{
			const char	*errstr;
			u_int		 i;

			yyval.v.logquick.log = 0;
			if (strncmp(yystack.l_mark[0].v.string, "pflog", 5)) {
				yyerror("%s: should be a pflog interface", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			i = strtonum(yystack.l_mark[0].v.string + 5, 0, 255, &errstr);
			if (errstr) {
				yyerror("%s: %s", yystack.l_mark[0].v.string, errstr);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
			yyval.v.logquick.logif = i;
		}
break;
case 243:
#line 2833 "parse.y"
	{ yyval.v.interface = NULL; }
break;
case 244:
#line 2834 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 245:
#line 2835 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 246:
#line 2838 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 247:
#line 2839 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
break;
case 248:
#line 2846 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; yyval.v.interface->not = yystack.l_mark[-1].v.number; }
break;
case 249:
#line 2849 "parse.y"
	{
			struct node_host	*n;

			yyval.v.interface = calloc(1, sizeof(struct node_if));
			if (yyval.v.interface == NULL)
				err(1, "if_item: calloc");
			if (strlcpy(yyval.v.interface->ifname, yystack.l_mark[0].v.string, sizeof(yyval.v.interface->ifname)) >=
			    sizeof(yyval.v.interface->ifname)) {
				free(yystack.l_mark[0].v.string);
				free(yyval.v.interface);
				yyerror("interface name too long");
				YYERROR;
			}

			if ((n = ifa_exists(yystack.l_mark[0].v.string)) != NULL)
				yyval.v.interface->ifa_flags = n->ifa_flags;

			free(yystack.l_mark[0].v.string);
			yyval.v.interface->not = 0;
			yyval.v.interface->next = NULL;
			yyval.v.interface->tail = yyval.v.interface;
		}
break;
case 250:
#line 2873 "parse.y"
	{ yyval.v.i = 0; }
break;
case 251:
#line 2874 "parse.y"
	{ yyval.v.i = AF_INET; }
break;
case 252:
#line 2875 "parse.y"
	{ yyval.v.i = AF_INET6; }
break;
case 253:
#line 2878 "parse.y"
	{ yyval.v.proto = NULL; }
break;
case 254:
#line 2879 "parse.y"
	{ yyval.v.proto = yystack.l_mark[0].v.proto; }
break;
case 255:
#line 2880 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
break;
case 256:
#line 2883 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
break;
case 257:
#line 2884 "parse.y"
	{
			yystack.l_mark[-3].v.proto->tail->next = yystack.l_mark[-1].v.proto;
			yystack.l_mark[-3].v.proto->tail = yystack.l_mark[-1].v.proto;
			yyval.v.proto = yystack.l_mark[-3].v.proto;
		}
break;
case 258:
#line 2891 "parse.y"
	{
			u_int8_t	pr;

			pr = (u_int8_t)yystack.l_mark[0].v.number;
			if (pr == 0) {
				yyerror("proto 0 cannot be used");
				YYERROR;
			}
			yyval.v.proto = calloc(1, sizeof(struct node_proto));
			if (yyval.v.proto == NULL)
				err(1, "proto_item: calloc");
			yyval.v.proto->proto = pr;
			yyval.v.proto->next = NULL;
			yyval.v.proto->tail = yyval.v.proto;
		}
break;
case 259:
#line 2908 "parse.y"
	{
			struct protoent	*p;

			p = getprotobyname(yystack.l_mark[0].v.string);
			if (p == NULL) {
				yyerror("unknown protocol %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			yyval.v.number = p->p_proto;
			free(yystack.l_mark[0].v.string);
		}
break;
case 260:
#line 2920 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
break;
case 261:
#line 2928 "parse.y"
	{
			yyval.v.fromto.src.host = NULL;
			yyval.v.fromto.src.port = NULL;
			yyval.v.fromto.dst.host = NULL;
			yyval.v.fromto.dst.port = NULL;
			yyval.v.fromto.src_os = NULL;
		}
break;
case 262:
#line 2935 "parse.y"
	{
			yyval.v.fromto.src = yystack.l_mark[-2].v.peer;
			yyval.v.fromto.src_os = yystack.l_mark[-1].v.os;
			yyval.v.fromto.dst = yystack.l_mark[0].v.peer;
		}
break;
case 263:
#line 2942 "parse.y"
	{ yyval.v.os = NULL; }
break;
case 264:
#line 2943 "parse.y"
	{ yyval.v.os = yystack.l_mark[0].v.os; }
break;
case 265:
#line 2944 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
break;
case 266:
#line 2947 "parse.y"
	{
			yyval.v.os = calloc(1, sizeof(struct node_os));
			if (yyval.v.os == NULL)
				err(1, "os: calloc");
			yyval.v.os->os = yystack.l_mark[0].v.string;
			yyval.v.os->tail = yyval.v.os;
		}
break;
case 267:
#line 2956 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
break;
case 268:
#line 2957 "parse.y"
	{
			yystack.l_mark[-3].v.os->tail->next = yystack.l_mark[-1].v.os;
			yystack.l_mark[-3].v.os->tail = yystack.l_mark[-1].v.os;
			yyval.v.os = yystack.l_mark[-3].v.os;
		}
break;
case 269:
#line 2964 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
break;
case 270:
#line 2968 "parse.y"
	{
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
break;
case 271:
#line 2973 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
break;
case 272:
#line 2977 "parse.y"
	{
			if (disallow_urpf_failed(yystack.l_mark[0].v.peer.host, "\"urpf-failed\" is "
			    "not permitted in a destination address"))
				YYERROR;
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
break;
case 273:
#line 2985 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[0].v.host;
			yyval.v.peer.port = NULL;
		}
break;
case 274:
#line 2989 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[-2].v.host;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
break;
case 275:
#line 2993 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
break;
case 278:
#line 3003 "parse.y"
	{ yyval.v.host = NULL; }
break;
case 279:
#line 3004 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 280:
#line 3005 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 281:
#line 3008 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 282:
#line 3009 "parse.y"
	{ yyval.v.host = NULL; }
break;
case 283:
#line 3012 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 284:
#line 3013 "parse.y"
	{
			if (yystack.l_mark[-1].v.host == NULL)
				yyval.v.host = yystack.l_mark[-3].v.host;
			else if (yystack.l_mark[-3].v.host == NULL)
				yyval.v.host = yystack.l_mark[-1].v.host;
			else {
				yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
				yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
				yyval.v.host = yystack.l_mark[-3].v.host;
			}
		}
break;
case 285:
#line 3026 "parse.y"
	{
			struct node_host	*n;

			for (n = yystack.l_mark[0].v.host; n != NULL; n = n->next)
				n->not = yystack.l_mark[-1].v.number;
			yyval.v.host = yystack.l_mark[0].v.host;
		}
break;
case 286:
#line 3033 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_NOROUTE;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
break;
case 287:
#line 3042 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_URPFFAILED;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
break;
case 288:
#line 3053 "parse.y"
	{
			if ((yyval.v.host = host(yystack.l_mark[0].v.string)) == NULL)	{
				/* error. "any" is handled elsewhere */
				free(yystack.l_mark[0].v.string);
				yyerror("could not parse host specification");
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);

		}
break;
case 289:
#line 3063 "parse.y"
	{
			struct node_host *b, *e;

			if ((b = host(yystack.l_mark[-2].v.string)) == NULL || (e = host(yystack.l_mark[0].v.string)) == NULL) {
				free(yystack.l_mark[-2].v.string);
				free(yystack.l_mark[0].v.string);
				yyerror("could not parse host specification");
				YYERROR;
			}
			if (b->af != e->af ||
			    b->addr.type != PF_ADDR_ADDRMASK ||
			    e->addr.type != PF_ADDR_ADDRMASK ||
			    unmask(&b->addr.v.a.mask, b->af) !=
			    (b->af == AF_INET ? 32 : 128) ||
			    unmask(&e->addr.v.a.mask, e->af) !=
			    (e->af == AF_INET ? 32 : 128) ||
			    b->next != NULL || b->not ||
			    e->next != NULL || e->not) {
				free(b);
				free(e);
				free(yystack.l_mark[-2].v.string);
				free(yystack.l_mark[0].v.string);
				yyerror("invalid address range");
				YYERROR;
			}
			memcpy(&b->addr.v.a.mask, &e->addr.v.a.addr,
			    sizeof(b->addr.v.a.mask));
			b->addr.type = PF_ADDR_RANGE;
			yyval.v.host = b;
			free(e);
			free(yystack.l_mark[-2].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 290:
#line 3096 "parse.y"
	{
			char	*buf;

			if (asprintf(&buf, "%s/%lld", yystack.l_mark[-2].v.string, (long long)yystack.l_mark[0].v.number) == -1)
				err(1, "host: asprintf");
			free(yystack.l_mark[-2].v.string);
			if ((yyval.v.host = host(buf)) == NULL)	{
				/* error. "any" is handled elsewhere */
				free(buf);
				yyerror("could not parse host specification");
				YYERROR;
			}
			free(buf);
		}
break;
case 291:
#line 3110 "parse.y"
	{
			char	*buf;

			/* ie. for 10/8 parsing */
#ifdef __FreeBSD__
			if (asprintf(&buf, "%lld/%lld", (long long)yystack.l_mark[-2].v.number, (long long)yystack.l_mark[0].v.number) == -1)
#else
			if (asprintf(&buf, "%lld/%lld", yystack.l_mark[-2].v.number, yystack.l_mark[0].v.number) == -1)
#endif
				err(1, "host: asprintf");
			if ((yyval.v.host = host(buf)) == NULL)	{
				/* error. "any" is handled elsewhere */
				free(buf);
				yyerror("could not parse host specification");
				YYERROR;
			}
			free(buf);
		}
break;
case 293:
#line 3129 "parse.y"
	{
			struct node_host	*n;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 128) {
				yyerror("bit number too big");
				YYERROR;
			}
			yyval.v.host = yystack.l_mark[-2].v.host;
			for (n = yystack.l_mark[-2].v.host; n != NULL; n = n->next)
				set_ipmask(n, yystack.l_mark[0].v.number);
		}
break;
case 294:
#line 3140 "parse.y"
	{
			if (strlen(yystack.l_mark[-1].v.string) >= PF_TABLE_NAME_SIZE) {
				yyerror("table name '%s' too long", yystack.l_mark[-1].v.string);
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "host: calloc");
			yyval.v.host->addr.type = PF_ADDR_TABLE;
			if (strlcpy(yyval.v.host->addr.v.tblname, yystack.l_mark[-1].v.string,
			    sizeof(yyval.v.host->addr.v.tblname)) >=
			    sizeof(yyval.v.host->addr.v.tblname))
				errx(1, "host: strlcpy");
			free(yystack.l_mark[-1].v.string);
			yyval.v.host->next = NULL;
			yyval.v.host->tail = yyval.v.host;
		}
break;
case 296:
#line 3161 "parse.y"
	{
			u_long	ulval;

			if (atoul(yystack.l_mark[0].v.string, &ulval) == -1) {
				yyerror("%s is not a number", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			} else
				yyval.v.number = ulval;
			free(yystack.l_mark[0].v.string);
		}
break;
case 297:
#line 3174 "parse.y"
	{
			int	 flags = 0;
			char	*p, *op;

			op = yystack.l_mark[-1].v.string;
			if (!isalpha(op[0])) {
				yyerror("invalid interface name '%s'", op);
				free(op);
				YYERROR;
			}
			while ((p = strrchr(yystack.l_mark[-1].v.string, ':')) != NULL) {
				if (!strcmp(p+1, "network"))
					flags |= PFI_AFLAG_NETWORK;
				else if (!strcmp(p+1, "broadcast"))
					flags |= PFI_AFLAG_BROADCAST;
				else if (!strcmp(p+1, "peer"))
					flags |= PFI_AFLAG_PEER;
				else if (!strcmp(p+1, "0"))
					flags |= PFI_AFLAG_NOALIAS;
				else {
					yyerror("interface %s has bad modifier",
					    yystack.l_mark[-1].v.string);
					free(op);
					YYERROR;
				}
				*p = '\0';
			}
			if (flags & (flags - 1) & PFI_AFLAG_MODEMASK) {
				free(op);
				yyerror("illegal combination of "
				    "interface modifiers");
				YYERROR;
			}
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "address: calloc");
			yyval.v.host->af = 0;
			set_ipmask(yyval.v.host, 128);
			yyval.v.host->addr.type = PF_ADDR_DYNIFTL;
			yyval.v.host->addr.iflags = flags;
			if (strlcpy(yyval.v.host->addr.v.ifname, yystack.l_mark[-1].v.string,
			    sizeof(yyval.v.host->addr.v.ifname)) >=
			    sizeof(yyval.v.host->addr.v.ifname)) {
				free(op);
				free(yyval.v.host);
				yyerror("interface name too long");
				YYERROR;
			}
			free(op);
			yyval.v.host->next = NULL;
			yyval.v.host->tail = yyval.v.host;
		}
break;
case 298:
#line 3228 "parse.y"
	{ yyval.v.port = yystack.l_mark[0].v.port; }
break;
case 299:
#line 3229 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
break;
case 300:
#line 3232 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
break;
case 301:
#line 3233 "parse.y"
	{
			yystack.l_mark[-3].v.port->tail->next = yystack.l_mark[-1].v.port;
			yystack.l_mark[-3].v.port->tail = yystack.l_mark[-1].v.port;
			yyval.v.port = yystack.l_mark[-3].v.port;
		}
break;
case 302:
#line 3240 "parse.y"
	{
			yyval.v.port = calloc(1, sizeof(struct node_port));
			if (yyval.v.port == NULL)
				err(1, "port_item: calloc");
			yyval.v.port->port[0] = yystack.l_mark[0].v.range.a;
			yyval.v.port->port[1] = yystack.l_mark[0].v.range.b;
			if (yystack.l_mark[0].v.range.t)
				yyval.v.port->op = PF_OP_RRG;
			else
				yyval.v.port->op = PF_OP_EQ;
			yyval.v.port->next = NULL;
			yyval.v.port->tail = yyval.v.port;
		}
break;
case 303:
#line 3253 "parse.y"
	{
			if (yystack.l_mark[0].v.range.t) {
				yyerror("':' cannot be used with an other "
				    "port operator");
				YYERROR;
			}
			yyval.v.port = calloc(1, sizeof(struct node_port));
			if (yyval.v.port == NULL)
				err(1, "port_item: calloc");
			yyval.v.port->port[0] = yystack.l_mark[0].v.range.a;
			yyval.v.port->port[1] = yystack.l_mark[0].v.range.b;
			yyval.v.port->op = yystack.l_mark[-1].v.i;
			yyval.v.port->next = NULL;
			yyval.v.port->tail = yyval.v.port;
		}
break;
case 304:
#line 3268 "parse.y"
	{
			if (yystack.l_mark[-2].v.range.t || yystack.l_mark[0].v.range.t) {
				yyerror("':' cannot be used with an other "
				    "port operator");
				YYERROR;
			}
			yyval.v.port = calloc(1, sizeof(struct node_port));
			if (yyval.v.port == NULL)
				err(1, "port_item: calloc");
			yyval.v.port->port[0] = yystack.l_mark[-2].v.range.a;
			yyval.v.port->port[1] = yystack.l_mark[0].v.range.a;
			yyval.v.port->op = yystack.l_mark[-1].v.i;
			yyval.v.port->next = NULL;
			yyval.v.port->tail = yyval.v.port;
		}
break;
case 305:
#line 3285 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, 0) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 306:
#line 3294 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 307:
#line 3303 "parse.y"
	{ yyval.v.uid = yystack.l_mark[0].v.uid; }
break;
case 308:
#line 3304 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
break;
case 309:
#line 3307 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
break;
case 310:
#line 3308 "parse.y"
	{
			yystack.l_mark[-3].v.uid->tail->next = yystack.l_mark[-1].v.uid;
			yystack.l_mark[-3].v.uid->tail = yystack.l_mark[-1].v.uid;
			yyval.v.uid = yystack.l_mark[-3].v.uid;
		}
break;
case 311:
#line 3315 "parse.y"
	{
			yyval.v.uid = calloc(1, sizeof(struct node_uid));
			if (yyval.v.uid == NULL)
				err(1, "uid_item: calloc");
			yyval.v.uid->uid[0] = yystack.l_mark[0].v.number;
			yyval.v.uid->uid[1] = yystack.l_mark[0].v.number;
			yyval.v.uid->op = PF_OP_EQ;
			yyval.v.uid->next = NULL;
			yyval.v.uid->tail = yyval.v.uid;
		}
break;
case 312:
#line 3325 "parse.y"
	{
			if (yystack.l_mark[0].v.number == UID_MAX && yystack.l_mark[-1].v.i != PF_OP_EQ && yystack.l_mark[-1].v.i != PF_OP_NE) {
				yyerror("user unknown requires operator = or "
				    "!=");
				YYERROR;
			}
			yyval.v.uid = calloc(1, sizeof(struct node_uid));
			if (yyval.v.uid == NULL)
				err(1, "uid_item: calloc");
			yyval.v.uid->uid[0] = yystack.l_mark[0].v.number;
			yyval.v.uid->uid[1] = yystack.l_mark[0].v.number;
			yyval.v.uid->op = yystack.l_mark[-1].v.i;
			yyval.v.uid->next = NULL;
			yyval.v.uid->tail = yyval.v.uid;
		}
break;
case 313:
#line 3340 "parse.y"
	{
			if (yystack.l_mark[-2].v.number == UID_MAX || yystack.l_mark[0].v.number == UID_MAX) {
				yyerror("user unknown requires operator = or "
				    "!=");
				YYERROR;
			}
			yyval.v.uid = calloc(1, sizeof(struct node_uid));
			if (yyval.v.uid == NULL)
				err(1, "uid_item: calloc");
			yyval.v.uid->uid[0] = yystack.l_mark[-2].v.number;
			yyval.v.uid->uid[1] = yystack.l_mark[0].v.number;
			yyval.v.uid->op = yystack.l_mark[-1].v.i;
			yyval.v.uid->next = NULL;
			yyval.v.uid->tail = yyval.v.uid;
		}
break;
case 314:
#line 3357 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "unknown"))
				yyval.v.number = UID_MAX;
			else {
				struct passwd	*pw;

				if ((pw = getpwnam(yystack.l_mark[0].v.string)) == NULL) {
					yyerror("unknown user %s", yystack.l_mark[0].v.string);
					free(yystack.l_mark[0].v.string);
					YYERROR;
				}
				yyval.v.number = pw->pw_uid;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 315:
#line 3372 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= UID_MAX) {
				yyerror("illegal uid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
break;
case 316:
#line 3381 "parse.y"
	{ yyval.v.gid = yystack.l_mark[0].v.gid; }
break;
case 317:
#line 3382 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
break;
case 318:
#line 3385 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
break;
case 319:
#line 3386 "parse.y"
	{
			yystack.l_mark[-3].v.gid->tail->next = yystack.l_mark[-1].v.gid;
			yystack.l_mark[-3].v.gid->tail = yystack.l_mark[-1].v.gid;
			yyval.v.gid = yystack.l_mark[-3].v.gid;
		}
break;
case 320:
#line 3393 "parse.y"
	{
			yyval.v.gid = calloc(1, sizeof(struct node_gid));
			if (yyval.v.gid == NULL)
				err(1, "gid_item: calloc");
			yyval.v.gid->gid[0] = yystack.l_mark[0].v.number;
			yyval.v.gid->gid[1] = yystack.l_mark[0].v.number;
			yyval.v.gid->op = PF_OP_EQ;
			yyval.v.gid->next = NULL;
			yyval.v.gid->tail = yyval.v.gid;
		}
break;
case 321:
#line 3403 "parse.y"
	{
			if (yystack.l_mark[0].v.number == GID_MAX && yystack.l_mark[-1].v.i != PF_OP_EQ && yystack.l_mark[-1].v.i != PF_OP_NE) {
				yyerror("group unknown requires operator = or "
				    "!=");
				YYERROR;
			}
			yyval.v.gid = calloc(1, sizeof(struct node_gid));
			if (yyval.v.gid == NULL)
				err(1, "gid_item: calloc");
			yyval.v.gid->gid[0] = yystack.l_mark[0].v.number;
			yyval.v.gid->gid[1] = yystack.l_mark[0].v.number;
			yyval.v.gid->op = yystack.l_mark[-1].v.i;
			yyval.v.gid->next = NULL;
			yyval.v.gid->tail = yyval.v.gid;
		}
break;
case 322:
#line 3418 "parse.y"
	{
			if (yystack.l_mark[-2].v.number == GID_MAX || yystack.l_mark[0].v.number == GID_MAX) {
				yyerror("group unknown requires operator = or "
				    "!=");
				YYERROR;
			}
			yyval.v.gid = calloc(1, sizeof(struct node_gid));
			if (yyval.v.gid == NULL)
				err(1, "gid_item: calloc");
			yyval.v.gid->gid[0] = yystack.l_mark[-2].v.number;
			yyval.v.gid->gid[1] = yystack.l_mark[0].v.number;
			yyval.v.gid->op = yystack.l_mark[-1].v.i;
			yyval.v.gid->next = NULL;
			yyval.v.gid->tail = yyval.v.gid;
		}
break;
case 323:
#line 3435 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "unknown"))
				yyval.v.number = GID_MAX;
			else {
				struct group	*grp;

				if ((grp = getgrnam(yystack.l_mark[0].v.string)) == NULL) {
					yyerror("unknown group %s", yystack.l_mark[0].v.string);
					free(yystack.l_mark[0].v.string);
					YYERROR;
				}
				yyval.v.number = grp->gr_gid;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 324:
#line 3450 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= GID_MAX) {
				yyerror("illegal gid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
break;
case 325:
#line 3459 "parse.y"
	{
			int	f;

			if ((f = parse_flags(yystack.l_mark[0].v.string)) < 0) {
				yyerror("bad flags %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
			yyval.v.b.b1 = f;
		}
break;
case 326:
#line 3472 "parse.y"
	{ yyval.v.b.b1 = yystack.l_mark[-2].v.b.b1; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
break;
case 327:
#line 3473 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
break;
case 328:
#line 3474 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = 0; }
break;
case 329:
#line 3477 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
break;
case 330:
#line 3478 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 331:
#line 3479 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
break;
case 332:
#line 3480 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 333:
#line 3483 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 334:
#line 3484 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
break;
case 335:
#line 3491 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 336:
#line 3492 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
break;
case 337:
#line 3499 "parse.y"
	{
			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[0].v.number;
			yyval.v.icmp->code = 0;
			yyval.v.icmp->proto = IPPROTO_ICMP;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 338:
#line 3509 "parse.y"
	{
			const struct icmpcodeent	*p;

			if ((p = geticmpcodebyname(yystack.l_mark[-2].v.number-1, yystack.l_mark[0].v.string, AF_INET)) == NULL) {
				yyerror("unknown icmp-code %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}

			free(yystack.l_mark[0].v.string);
			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[-2].v.number;
			yyval.v.icmp->code = p->code + 1;
			yyval.v.icmp->proto = IPPROTO_ICMP;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 339:
#line 3528 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[-2].v.number;
			yyval.v.icmp->code = yystack.l_mark[0].v.number + 1;
			yyval.v.icmp->proto = IPPROTO_ICMP;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 340:
#line 3544 "parse.y"
	{
			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[0].v.number;
			yyval.v.icmp->code = 0;
			yyval.v.icmp->proto = IPPROTO_ICMPV6;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 341:
#line 3554 "parse.y"
	{
			const struct icmpcodeent	*p;

			if ((p = geticmpcodebyname(yystack.l_mark[-2].v.number-1, yystack.l_mark[0].v.string, AF_INET6)) == NULL) {
				yyerror("unknown icmp6-code %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);

			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[-2].v.number;
			yyval.v.icmp->code = p->code + 1;
			yyval.v.icmp->proto = IPPROTO_ICMPV6;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 342:
#line 3573 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.icmp = calloc(1, sizeof(struct node_icmp));
			if (yyval.v.icmp == NULL)
				err(1, "icmp_item: calloc");
			yyval.v.icmp->type = yystack.l_mark[-2].v.number;
			yyval.v.icmp->code = yystack.l_mark[0].v.number + 1;
			yyval.v.icmp->proto = IPPROTO_ICMPV6;
			yyval.v.icmp->next = NULL;
			yyval.v.icmp->tail = yyval.v.icmp;
		}
break;
case 343:
#line 3589 "parse.y"
	{
			const struct icmptypeent	*p;

			if ((p = geticmptypebyname(yystack.l_mark[0].v.string, AF_INET)) == NULL) {
				yyerror("unknown icmp-type %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			yyval.v.number = p->type + 1;
			free(yystack.l_mark[0].v.string);
		}
break;
case 344:
#line 3600 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
break;
case 345:
#line 3609 "parse.y"
	{
			const struct icmptypeent	*p;

			if ((p = geticmptypebyname(yystack.l_mark[0].v.string, AF_INET6)) ==
			    NULL) {
				yyerror("unknown icmp6-type %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			yyval.v.number = p->type + 1;
			free(yystack.l_mark[0].v.string);
		}
break;
case 346:
#line 3621 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp6-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
break;
case 347:
#line 3630 "parse.y"
	{
			int val;
			char *end;

			if (map_tos(yystack.l_mark[0].v.string, &val))
				yyval.v.number = val;
			else if (yystack.l_mark[0].v.string[0] == '0' && yystack.l_mark[0].v.string[1] == 'x') {
				errno = 0;
				yyval.v.number = strtoul(yystack.l_mark[0].v.string, &end, 16);
				if (errno || *end != '\0')
					yyval.v.number = 256;
			} else
				yyval.v.number = 256;		/* flag bad argument */
			if (yyval.v.number < 0 || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 348:
#line 3650 "parse.y"
	{
			yyval.v.number = yystack.l_mark[0].v.number;
			if (yyval.v.number < 0 || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.number);
				YYERROR;
			}
		}
break;
case 349:
#line 3659 "parse.y"
	{ yyval.v.i = PF_SRCTRACK; }
break;
case 350:
#line 3660 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_GLOBAL; }
break;
case 351:
#line 3661 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_RULE; }
break;
case 352:
#line 3664 "parse.y"
	{
			yyval.v.i = PFRULE_IFBOUND;
		}
break;
case 353:
#line 3667 "parse.y"
	{
			yyval.v.i = 0;
		}
break;
case 354:
#line 3672 "parse.y"
	{
			yyval.v.keep_state.action = 0;
			yyval.v.keep_state.options = NULL;
		}
break;
case 355:
#line 3676 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_NORMAL;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 356:
#line 3680 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_MODULATE;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 357:
#line 3684 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_SYNPROXY;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 358:
#line 3690 "parse.y"
	{ yyval.v.i = 0; }
break;
case 359:
#line 3691 "parse.y"
	{ yyval.v.i = PF_FLUSH; }
break;
case 360:
#line 3692 "parse.y"
	{
			yyval.v.i = PF_FLUSH | PF_FLUSH_GLOBAL;
		}
break;
case 361:
#line 3697 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[-1].v.state_opt; }
break;
case 362:
#line 3698 "parse.y"
	{ yyval.v.state_opt = NULL; }
break;
case 363:
#line 3701 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[0].v.state_opt; }
break;
case 364:
#line 3702 "parse.y"
	{
			yystack.l_mark[-2].v.state_opt->tail->next = yystack.l_mark[0].v.state_opt;
			yystack.l_mark[-2].v.state_opt->tail = yystack.l_mark[0].v.state_opt;
			yyval.v.state_opt = yystack.l_mark[-2].v.state_opt;
		}
break;
case 365:
#line 3709 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_MAX;
			yyval.v.state_opt->data.max_states = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 366:
#line 3722 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_NOSYNC;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 367:
#line 3730 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_MAX_SRC_STATES;
			yyval.v.state_opt->data.max_src_states = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 368:
#line 3743 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_MAX_SRC_CONN;
			yyval.v.state_opt->data.max_src_conn = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 369:
#line 3756 "parse.y"
	{
			if (yystack.l_mark[-2].v.number < 0 || yystack.l_mark[-2].v.number > UINT_MAX ||
			    yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_MAX_SRC_CONN_RATE;
			yyval.v.state_opt->data.max_src_conn_rate.limit = yystack.l_mark[-2].v.number;
			yyval.v.state_opt->data.max_src_conn_rate.seconds = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 370:
#line 3771 "parse.y"
	{
			if (strlen(yystack.l_mark[-2].v.string) >= PF_TABLE_NAME_SIZE) {
				yyerror("table name '%s' too long", yystack.l_mark[-2].v.string);
				free(yystack.l_mark[-2].v.string);
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			if (strlcpy(yyval.v.state_opt->data.overload.tblname, yystack.l_mark[-2].v.string,
			    PF_TABLE_NAME_SIZE) >= PF_TABLE_NAME_SIZE)
				errx(1, "state_opt_item: strlcpy");
			free(yystack.l_mark[-2].v.string);
			yyval.v.state_opt->type = PF_STATE_OPT_OVERLOAD;
			yyval.v.state_opt->data.overload.flush = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 371:
#line 3789 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_MAX_SRC_NODES;
			yyval.v.state_opt->data.max_src_nodes = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 372:
#line 3802 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SRCTRACK;
			yyval.v.state_opt->data.src_track = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 373:
#line 3811 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_STATELOCK;
			yyval.v.state_opt->data.statelock = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 374:
#line 3820 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SLOPPY;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 375:
#line 3828 "parse.y"
	{
			int	i;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			for (i = 0; pf_timeouts[i].name &&
			    strcmp(pf_timeouts[i].name, yystack.l_mark[-1].v.string); ++i)
				;	/* nothing */
			if (!pf_timeouts[i].name) {
				yyerror("illegal timeout name %s", yystack.l_mark[-1].v.string);
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			if (strchr(pf_timeouts[i].name, '.') == NULL) {
				yyerror("illegal state timeout %s", yystack.l_mark[-1].v.string);
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			free(yystack.l_mark[-1].v.string);
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_TIMEOUT;
			yyval.v.state_opt->data.timeout.number = pf_timeouts[i].timeout;
			yyval.v.state_opt->data.timeout.seconds = yystack.l_mark[0].v.number;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 376:
#line 3860 "parse.y"
	{
			yyval.v.string = yystack.l_mark[0].v.string;
		}
break;
case 377:
#line 3865 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
			yyval.v.qassign.pqname = NULL;
		}
break;
case 378:
#line 3869 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
			yyval.v.qassign.pqname = NULL;
		}
break;
case 379:
#line 3873 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-3].v.string;
			yyval.v.qassign.pqname = yystack.l_mark[-1].v.string;
		}
break;
case 380:
#line 3879 "parse.y"
	{ yyval.v.i = 0; }
break;
case 381:
#line 3880 "parse.y"
	{ yyval.v.i = 1; }
break;
case 382:
#line 3883 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE|PPORT_STAR) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 383:
#line 3892 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 384:
#line 3893 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 385:
#line 3896 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 386:
#line 3897 "parse.y"
	{
			yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
			yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
			yyval.v.host = yystack.l_mark[-3].v.host;
		}
break;
case 387:
#line 3904 "parse.y"
	{ yyval.v.redirection = NULL; }
break;
case 388:
#line 3905 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
break;
case 389:
#line 3912 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
break;
case 390:
#line 3922 "parse.y"
	{
			yyval.v.hashkey = calloc(1, sizeof(struct pf_poolhashkey));
			if (yyval.v.hashkey == NULL)
				err(1, "hashkey: calloc");
			yyval.v.hashkey->key32[0] = arc4random();
			yyval.v.hashkey->key32[1] = arc4random();
			yyval.v.hashkey->key32[2] = arc4random();
			yyval.v.hashkey->key32[3] = arc4random();
		}
break;
case 391:
#line 3932 "parse.y"
	{
			if (!strncmp(yystack.l_mark[0].v.string, "0x", 2)) {
				if (strlen(yystack.l_mark[0].v.string) != 34) {
					free(yystack.l_mark[0].v.string);
					yyerror("hex key must be 128 bits "
						"(32 hex digits) long");
					YYERROR;
				}
				yyval.v.hashkey = calloc(1, sizeof(struct pf_poolhashkey));
				if (yyval.v.hashkey == NULL)
					err(1, "hashkey: calloc");

				if (sscanf(yystack.l_mark[0].v.string, "0x%8x%8x%8x%8x",
				    &yyval.v.hashkey->key32[0], &yyval.v.hashkey->key32[1],
				    &yyval.v.hashkey->key32[2], &yyval.v.hashkey->key32[3]) != 4) {
					free(yyval.v.hashkey);
					free(yystack.l_mark[0].v.string);
					yyerror("invalid hex key");
					YYERROR;
				}
			} else {
				MD5_CTX	context;

				yyval.v.hashkey = calloc(1, sizeof(struct pf_poolhashkey));
				if (yyval.v.hashkey == NULL)
					err(1, "hashkey: calloc");
				MD5Init(&context);
				MD5Update(&context, (unsigned char *)yystack.l_mark[0].v.string,
				    strlen(yystack.l_mark[0].v.string));
				MD5Final((unsigned char *)yyval.v.hashkey, &context);
				HTONL(yyval.v.hashkey->key32[0]);
				HTONL(yyval.v.hashkey->key32[1]);
				HTONL(yyval.v.hashkey->key32[2]);
				HTONL(yyval.v.hashkey->key32[3]);
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 392:
#line 3971 "parse.y"
	{ bzero(&pool_opts, sizeof pool_opts); }
break;
case 393:
#line 3973 "parse.y"
	{ yyval.v.pool_opts = pool_opts; }
break;
case 394:
#line 3974 "parse.y"
	{
			bzero(&pool_opts, sizeof pool_opts);
			yyval.v.pool_opts = pool_opts;
		}
break;
case 397:
#line 3984 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type =  PF_POOL_BITMASK;
		}
break;
case 398:
#line 3991 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_RANDOM;
		}
break;
case 399:
#line 3998 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_SRCHASH;
			pool_opts.key = yystack.l_mark[0].v.hashkey;
		}
break;
case 400:
#line 4006 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_ROUNDROBIN;
		}
break;
case 401:
#line 4013 "parse.y"
	{
			if (pool_opts.staticport) {
				yyerror("static-port cannot be redefined");
				YYERROR;
			}
			pool_opts.staticport = 1;
		}
break;
case 402:
#line 4020 "parse.y"
	{
			if (filter_opts.marker & POM_STICKYADDRESS) {
				yyerror("sticky-address cannot be redefined");
				YYERROR;
			}
			pool_opts.marker |= POM_STICKYADDRESS;
			pool_opts.opts |= PF_POOL_STICKYADDR;
		}
break;
case 403:
#line 4030 "parse.y"
	{ yyval.v.redirection = NULL; }
break;
case 404:
#line 4031 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
break;
case 405:
#line 4038 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
break;
case 406:
#line 4047 "parse.y"
	{ yyval.v.b.b1 = yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
break;
case 407:
#line 4048 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
break;
case 408:
#line 4049 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
break;
case 409:
#line 4050 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
break;
case 410:
#line 4053 "parse.y"
	{
			if (yystack.l_mark[-2].v.i && yystack.l_mark[0].v.b.b1) {
				yyerror("\"pass\" not valid with \"no\"");
				YYERROR;
			}
			if (yystack.l_mark[-2].v.i)
				yyval.v.b.b1 = PF_NONAT;
			else
				yyval.v.b.b1 = PF_NAT;
			yyval.v.b.b2 = yystack.l_mark[0].v.b.b1;
			yyval.v.b.w = yystack.l_mark[0].v.b.b2;
			yyval.v.b.w2 = yystack.l_mark[0].v.b.w2;
		}
break;
case 411:
#line 4066 "parse.y"
	{
			if (yystack.l_mark[-2].v.i && yystack.l_mark[0].v.b.b1) {
				yyerror("\"pass\" not valid with \"no\"");
				YYERROR;
			}
			if (yystack.l_mark[-2].v.i)
				yyval.v.b.b1 = PF_NORDR;
			else
				yyval.v.b.b1 = PF_RDR;
			yyval.v.b.b2 = yystack.l_mark[0].v.b.b1;
			yyval.v.b.w = yystack.l_mark[0].v.b.b2;
			yyval.v.b.w2 = yystack.l_mark[0].v.b.w2;
		}
break;
case 412:
#line 4083 "parse.y"
	{
			struct pf_rule	r;

			if (check_rulestate(PFCTL_STATE_NAT))
				YYERROR;

			memset(&r, 0, sizeof(r));

			r.action = yystack.l_mark[-9].v.b.b1;
			r.natpass = yystack.l_mark[-9].v.b.b2;
			r.log = yystack.l_mark[-9].v.b.w;
			r.logif = yystack.l_mark[-9].v.b.w2;
			r.af = yystack.l_mark[-7].v.i;

			if (!r.af) {
				if (yystack.l_mark[-5].v.fromto.src.host && yystack.l_mark[-5].v.fromto.src.host->af &&
				    !yystack.l_mark[-5].v.fromto.src.host->ifindex)
					r.af = yystack.l_mark[-5].v.fromto.src.host->af;
				else if (yystack.l_mark[-5].v.fromto.dst.host && yystack.l_mark[-5].v.fromto.dst.host->af &&
				    !yystack.l_mark[-5].v.fromto.dst.host->ifindex)
					r.af = yystack.l_mark[-5].v.fromto.dst.host->af;
			}

			if (yystack.l_mark[-4].v.string != NULL)
				if (strlcpy(r.tagname, yystack.l_mark[-4].v.string, PF_TAG_NAME_SIZE) >=
				    PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}

			if (yystack.l_mark[-3].v.tagged.name)
				if (strlcpy(r.match_tagname, yystack.l_mark[-3].v.tagged.name,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[-3].v.tagged.neg;
			r.rtableid = yystack.l_mark[-2].v.rtableid;

			if (r.action == PF_NONAT || r.action == PF_NORDR) {
				if (yystack.l_mark[-1].v.redirection != NULL) {
					yyerror("translation rule with 'no' "
					    "does not need '->'");
					YYERROR;
				}
			} else {
				if (yystack.l_mark[-1].v.redirection == NULL || yystack.l_mark[-1].v.redirection->host == NULL) {
					yyerror("translation rule requires '-> "
					    "address'");
					YYERROR;
				}
				if (!r.af && ! yystack.l_mark[-1].v.redirection->host->ifindex)
					r.af = yystack.l_mark[-1].v.redirection->host->af;

				remove_invalid_hosts(&yystack.l_mark[-1].v.redirection->host, &r.af);
				if (invalid_redirect(yystack.l_mark[-1].v.redirection->host, r.af))
					YYERROR;
				if (check_netmask(yystack.l_mark[-1].v.redirection->host, r.af))
					YYERROR;

				r.rpool.proxy_port[0] = ntohs(yystack.l_mark[-1].v.redirection->rport.a);

				switch (r.action) {
				case PF_RDR:
					if (!yystack.l_mark[-1].v.redirection->rport.b && yystack.l_mark[-1].v.redirection->rport.t &&
					    yystack.l_mark[-5].v.fromto.dst.port != NULL) {
						r.rpool.proxy_port[1] =
						    ntohs(yystack.l_mark[-1].v.redirection->rport.a) +
						    (ntohs(
						    yystack.l_mark[-5].v.fromto.dst.port->port[1]) -
						    ntohs(
						    yystack.l_mark[-5].v.fromto.dst.port->port[0]));
					} else
						r.rpool.proxy_port[1] =
						    ntohs(yystack.l_mark[-1].v.redirection->rport.b);
					break;
				case PF_NAT:
					r.rpool.proxy_port[1] =
					    ntohs(yystack.l_mark[-1].v.redirection->rport.b);
					if (!r.rpool.proxy_port[0] &&
					    !r.rpool.proxy_port[1]) {
						r.rpool.proxy_port[0] =
						    PF_NAT_PROXY_PORT_LOW;
						r.rpool.proxy_port[1] =
						    PF_NAT_PROXY_PORT_HIGH;
					} else if (!r.rpool.proxy_port[1])
						r.rpool.proxy_port[1] =
						    r.rpool.proxy_port[0];
					break;
				default:
					break;
				}

				r.rpool.opts = yystack.l_mark[0].v.pool_opts.type;
				if ((r.rpool.opts & PF_POOL_TYPEMASK) ==
				    PF_POOL_NONE && (yystack.l_mark[-1].v.redirection->host->next != NULL ||
				    yystack.l_mark[-1].v.redirection->host->addr.type == PF_ADDR_TABLE ||
				    DYNIF_MULTIADDR(yystack.l_mark[-1].v.redirection->host->addr)))
					r.rpool.opts = PF_POOL_ROUNDROBIN;
				if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
				    PF_POOL_ROUNDROBIN &&
				    disallow_table(yystack.l_mark[-1].v.redirection->host, "tables are only "
				    "supported in round-robin redirection "
				    "pools"))
					YYERROR;
				if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
				    PF_POOL_ROUNDROBIN &&
				    disallow_alias(yystack.l_mark[-1].v.redirection->host, "interface (%s) "
				    "is only supported in round-robin "
				    "redirection pools"))
					YYERROR;
				if (yystack.l_mark[-1].v.redirection->host->next != NULL) {
					if ((r.rpool.opts & PF_POOL_TYPEMASK) !=
					    PF_POOL_ROUNDROBIN) {
						yyerror("only round-robin "
						    "valid for multiple "
						    "redirection addresses");
						YYERROR;
					}
				}
			}

			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				memcpy(&r.rpool.key, yystack.l_mark[0].v.pool_opts.key,
				    sizeof(struct pf_poolhashkey));

			 if (yystack.l_mark[0].v.pool_opts.opts)
				r.rpool.opts |= yystack.l_mark[0].v.pool_opts.opts;

			if (yystack.l_mark[0].v.pool_opts.staticport) {
				if (r.action != PF_NAT) {
					yyerror("the 'static-port' option is "
					    "only valid with nat rules");
					YYERROR;
				}
				if (r.rpool.proxy_port[0] !=
				    PF_NAT_PROXY_PORT_LOW &&
				    r.rpool.proxy_port[1] !=
				    PF_NAT_PROXY_PORT_HIGH) {
					yyerror("the 'static-port' option can't"
					    " be used when specifying a port"
					    " range");
					YYERROR;
				}
				r.rpool.proxy_port[0] = 0;
				r.rpool.proxy_port[1] = 0;
			}

			expand_rule(&r, yystack.l_mark[-8].v.interface, yystack.l_mark[-1].v.redirection == NULL ? NULL : yystack.l_mark[-1].v.redirection->host, yystack.l_mark[-6].v.proto,
			    yystack.l_mark[-5].v.fromto.src_os, yystack.l_mark[-5].v.fromto.src.host, yystack.l_mark[-5].v.fromto.src.port, yystack.l_mark[-5].v.fromto.dst.host,
			    yystack.l_mark[-5].v.fromto.dst.port, 0, 0, 0, "");
			free(yystack.l_mark[-1].v.redirection);
		}
break;
case 413:
#line 4242 "parse.y"
	{
			struct pf_rule		binat;
			struct pf_pooladdr	*pa;

			if (check_rulestate(PFCTL_STATE_NAT))
				YYERROR;
			if (disallow_urpf_failed(yystack.l_mark[-4].v.host, "\"urpf-failed\" is not "
			    "permitted as a binat destination"))
				YYERROR;

			memset(&binat, 0, sizeof(binat));

			if (yystack.l_mark[-12].v.i && yystack.l_mark[-10].v.b.b1) {
				yyerror("\"pass\" not valid with \"no\"");
				YYERROR;
			}
			if (yystack.l_mark[-12].v.i)
				binat.action = PF_NOBINAT;
			else
				binat.action = PF_BINAT;
			binat.natpass = yystack.l_mark[-10].v.b.b1;
			binat.log = yystack.l_mark[-10].v.b.b2;
			binat.logif = yystack.l_mark[-10].v.b.w2;
			binat.af = yystack.l_mark[-8].v.i;
			if (!binat.af && yystack.l_mark[-5].v.host != NULL && yystack.l_mark[-5].v.host->af)
				binat.af = yystack.l_mark[-5].v.host->af;
			if (!binat.af && yystack.l_mark[-4].v.host != NULL && yystack.l_mark[-4].v.host->af)
				binat.af = yystack.l_mark[-4].v.host->af;

			if (!binat.af && yystack.l_mark[0].v.redirection != NULL && yystack.l_mark[0].v.redirection->host)
				binat.af = yystack.l_mark[0].v.redirection->host->af;
			if (!binat.af) {
				yyerror("address family (inet/inet6) "
				    "undefined");
				YYERROR;
			}

			if (yystack.l_mark[-9].v.interface != NULL) {
				memcpy(binat.ifname, yystack.l_mark[-9].v.interface->ifname,
				    sizeof(binat.ifname));
				binat.ifnot = yystack.l_mark[-9].v.interface->not;
				free(yystack.l_mark[-9].v.interface);
			}

			if (yystack.l_mark[-3].v.string != NULL)
				if (strlcpy(binat.tagname, yystack.l_mark[-3].v.string,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			if (yystack.l_mark[-2].v.tagged.name)
				if (strlcpy(binat.match_tagname, yystack.l_mark[-2].v.tagged.name,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			binat.match_tag_not = yystack.l_mark[-2].v.tagged.neg;
			binat.rtableid = yystack.l_mark[-1].v.rtableid;

			if (yystack.l_mark[-7].v.proto != NULL) {
				binat.proto = yystack.l_mark[-7].v.proto->proto;
				free(yystack.l_mark[-7].v.proto);
			}

			if (yystack.l_mark[-5].v.host != NULL && disallow_table(yystack.l_mark[-5].v.host, "invalid use of "
			    "table <%s> as the source address of a binat rule"))
				YYERROR;
			if (yystack.l_mark[-5].v.host != NULL && disallow_alias(yystack.l_mark[-5].v.host, "invalid use of "
			    "interface (%s) as the source address of a binat "
			    "rule"))
				YYERROR;
			if (yystack.l_mark[0].v.redirection != NULL && yystack.l_mark[0].v.redirection->host != NULL && disallow_table(
			    yystack.l_mark[0].v.redirection->host, "invalid use of table <%s> as the "
			    "redirect address of a binat rule"))
				YYERROR;
			if (yystack.l_mark[0].v.redirection != NULL && yystack.l_mark[0].v.redirection->host != NULL && disallow_alias(
			    yystack.l_mark[0].v.redirection->host, "invalid use of interface (%s) as the "
			    "redirect address of a binat rule"))
				YYERROR;

			if (yystack.l_mark[-5].v.host != NULL) {
				if (yystack.l_mark[-5].v.host->next) {
					yyerror("multiple binat ip addresses");
					YYERROR;
				}
				if (yystack.l_mark[-5].v.host->addr.type == PF_ADDR_DYNIFTL)
					yystack.l_mark[-5].v.host->af = binat.af;
				if (yystack.l_mark[-5].v.host->af != binat.af) {
					yyerror("binat ip versions must match");
					YYERROR;
				}
				if (check_netmask(yystack.l_mark[-5].v.host, binat.af))
					YYERROR;
				memcpy(&binat.src.addr, &yystack.l_mark[-5].v.host->addr,
				    sizeof(binat.src.addr));
				free(yystack.l_mark[-5].v.host);
			}
			if (yystack.l_mark[-4].v.host != NULL) {
				if (yystack.l_mark[-4].v.host->next) {
					yyerror("multiple binat ip addresses");
					YYERROR;
				}
				if (yystack.l_mark[-4].v.host->af != binat.af && yystack.l_mark[-4].v.host->af) {
					yyerror("binat ip versions must match");
					YYERROR;
				}
				if (check_netmask(yystack.l_mark[-4].v.host, binat.af))
					YYERROR;
				memcpy(&binat.dst.addr, &yystack.l_mark[-4].v.host->addr,
				    sizeof(binat.dst.addr));
				binat.dst.neg = yystack.l_mark[-4].v.host->not;
				free(yystack.l_mark[-4].v.host);
			}

			if (binat.action == PF_NOBINAT) {
				if (yystack.l_mark[0].v.redirection != NULL) {
					yyerror("'no binat' rule does not need"
					    " '->'");
					YYERROR;
				}
			} else {
				if (yystack.l_mark[0].v.redirection == NULL || yystack.l_mark[0].v.redirection->host == NULL) {
					yyerror("'binat' rule requires"
					    " '-> address'");
					YYERROR;
				}

				remove_invalid_hosts(&yystack.l_mark[0].v.redirection->host, &binat.af);
				if (invalid_redirect(yystack.l_mark[0].v.redirection->host, binat.af))
					YYERROR;
				if (yystack.l_mark[0].v.redirection->host->next != NULL) {
					yyerror("binat rule must redirect to "
					    "a single address");
					YYERROR;
				}
				if (check_netmask(yystack.l_mark[0].v.redirection->host, binat.af))
					YYERROR;

				if (!PF_AZERO(&binat.src.addr.v.a.mask,
				    binat.af) &&
				    !PF_AEQ(&binat.src.addr.v.a.mask,
				    &yystack.l_mark[0].v.redirection->host->addr.v.a.mask, binat.af)) {
					yyerror("'binat' source mask and "
					    "redirect mask must be the same");
					YYERROR;
				}

				TAILQ_INIT(&binat.rpool.list);
				pa = calloc(1, sizeof(struct pf_pooladdr));
				if (pa == NULL)
					err(1, "binat: calloc");
				pa->addr = yystack.l_mark[0].v.redirection->host->addr;
				pa->ifname[0] = 0;
				TAILQ_INSERT_TAIL(&binat.rpool.list,
				    pa, entries);

				free(yystack.l_mark[0].v.redirection);
			}

			pfctl_add_rule(pf, &binat, "");
		}
break;
case 414:
#line 4407 "parse.y"
	{ yyval.v.string = NULL; }
break;
case 415:
#line 4408 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 416:
#line 4411 "parse.y"
	{ yyval.v.tagged.neg = 0; yyval.v.tagged.name = NULL; }
break;
case 417:
#line 4412 "parse.y"
	{ yyval.v.tagged.neg = yystack.l_mark[-2].v.number; yyval.v.tagged.name = yystack.l_mark[0].v.string; }
break;
case 418:
#line 4415 "parse.y"
	{ yyval.v.rtableid = -1; }
break;
case 419:
#line 4416 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			yyval.v.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 420:
#line 4425 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "route_host: calloc");
			yyval.v.host->ifname = yystack.l_mark[0].v.string;
			set_ipmask(yyval.v.host, 128);
			yyval.v.host->next = NULL;
			yyval.v.host->tail = yyval.v.host;
		}
break;
case 421:
#line 4434 "parse.y"
	{
			struct node_host *n;

			yyval.v.host = yystack.l_mark[-1].v.host;
			for (n = yystack.l_mark[-1].v.host; n != NULL; n = n->next)
				n->ifname = yystack.l_mark[-2].v.string;
		}
break;
case 422:
#line 4443 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 423:
#line 4444 "parse.y"
	{
			if (yystack.l_mark[-3].v.host->af == 0)
				yystack.l_mark[-3].v.host->af = yystack.l_mark[-1].v.host->af;
			if (yystack.l_mark[-3].v.host->af != yystack.l_mark[-1].v.host->af) {
				yyerror("all pool addresses must be in the "
				    "same address family");
				YYERROR;
			}
			yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
			yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
			yyval.v.host = yystack.l_mark[-3].v.host;
		}
break;
case 424:
#line 4458 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 425:
#line 4459 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 426:
#line 4462 "parse.y"
	{
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
break;
case 427:
#line 4467 "parse.y"
	{
			/* backwards-compat */
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
break;
case 428:
#line 4473 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_ROUTETO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 429:
#line 4480 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_REPLYTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 430:
#line 4487 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_DUPTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 431:
#line 4497 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			if (pfctl_set_timeout(pf, yystack.l_mark[-1].v.string, yystack.l_mark[0].v.number, 0) != 0) {
				yyerror("unknown timeout %s", yystack.l_mark[-1].v.string);
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			free(yystack.l_mark[-1].v.string);
		}
break;
case 432:
#line 4513 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			if (pfctl_set_timeout(pf, "interval", yystack.l_mark[0].v.number, 0) != 0)
				YYERROR;
		}
break;
case 435:
#line 4530 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION)) {
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("only positive values permitted");
				YYERROR;
			}
			if (pfctl_set_limit(pf, yystack.l_mark[-1].v.string, yystack.l_mark[0].v.number) != 0) {
				yyerror("unable to set limit %s %u", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.number);
				free(yystack.l_mark[-1].v.string);
				YYERROR;
			}
			free(yystack.l_mark[-1].v.string);
		}
break;
case 440:
#line 4556 "parse.y"
	{ yyval.v.number = 0; }
break;
case 441:
#line 4557 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "yes"))
				yyval.v.number = 1;
			else {
				yyerror("invalid value '%s', expected 'yes' "
				    "or 'no'", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 442:
#line 4570 "parse.y"
	{ yyval.v.i = PF_OP_EQ; }
break;
case 443:
#line 4571 "parse.y"
	{ yyval.v.i = PF_OP_NE; }
break;
case 444:
#line 4572 "parse.y"
	{ yyval.v.i = PF_OP_LE; }
break;
case 445:
#line 4573 "parse.y"
	{ yyval.v.i = PF_OP_LT; }
break;
case 446:
#line 4574 "parse.y"
	{ yyval.v.i = PF_OP_GE; }
break;
case 447:
#line 4575 "parse.y"
	{ yyval.v.i = PF_OP_GT; }
break;
#line 9356 "parse.c"
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
