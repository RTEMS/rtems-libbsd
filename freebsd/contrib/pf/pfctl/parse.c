/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20141006

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

#line 30 "../../freebsd/contrib/pf/pfctl/parse.y"
#ifdef __rtems__
#include <machine/rtems-bsd-user-space.h>
#include <machine/rtems-bsd-program.h>
#define	pf_find_or_create_ruleset _bsd_pf_find_or_create_ruleset
#define	pf_anchor_setup _bsd_pf_anchor_setup
#define	pf_remove_if_empty_ruleset _bsd_pf_remove_if_empty_ruleset
#endif /* __rtems__ */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

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
#include <altq/altq.h>
#include <altq/altq_cbq.h>
#include <altq/altq_priq.h>
#include <altq/altq_hfsc.h>

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

static struct pfctl	*pf = NULL;
static int		 debug = 0;
static int		 rulestate = 0;
static u_int16_t	 returnicmpdefault =
			    (ICMP_UNREACH << 8) | ICMP_UNREACH_PORT;
static u_int16_t	 returnicmp6default =
			    (ICMP6_DST_UNREACH << 8) | ICMP6_DST_UNREACH_NOPORT;
static int		 blockpolicy = PFRULE_DROP;
static int		 require_order = 1;
static int		 default_statelock;

#ifndef __rtems__
TAILQ_HEAD(files, file)		 files = TAILQ_HEAD_INITIALIZER(files);
#else /* __rtems__ */
static TAILQ_HEAD(files, file)	 files = TAILQ_HEAD_INITIALIZER(files);
#endif /* __rtems__ */
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

#ifndef __rtems__
TAILQ_HEAD(symhead, sym)	 symhead = TAILQ_HEAD_INITIALIZER(symhead);
#else /* __rtems__ */
static TAILQ_HEAD(symhead, sym)	 symhead = TAILQ_HEAD_INITIALIZER(symhead);
#endif /* __rtems__ */
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
	    PF_STATE_OPT_TIMEOUT, PF_STATE_OPT_SLOPPY, 
	    PF_STATE_OPT_PFLOW };

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

#ifndef __rtems__
struct node_queue {
#else /* __rtems__ */
static struct node_queue {
#endif /* __rtems__ */
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

#ifndef __rtems__
struct filter_opts {
#else /* __rtems__ */
static struct filter_opts {
#endif /* __rtems__ */
	int			 marker;
#define FOM_FLAGS	0x01
#define FOM_ICMP	0x02
#define FOM_TOS		0x04
#define FOM_KEEP	0x08
#define FOM_SRCTRACK	0x10
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
	struct {
		struct node_host	*addr;
		u_int16_t		port;
	}			 divert;
} filter_opts;

#ifndef __rtems__
struct antispoof_opts {
#else /* __rtems__ */
static struct antispoof_opts {
#endif /* __rtems__ */
	char			*label;
	u_int			 rtableid;
} antispoof_opts;

#ifndef __rtems__
struct scrub_opts {
#else /* __rtems__ */
static struct scrub_opts {
#endif /* __rtems__ */
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

#ifndef __rtems__
struct queue_opts {
#else /* __rtems__ */
static struct queue_opts {
#endif /* __rtems__ */
	int			marker;
#define QOM_BWSPEC	0x01
#define QOM_SCHEDULER	0x02
#define QOM_PRIORITY	0x04
#define QOM_TBRSIZE	0x08
#define QOM_QLIMIT	0x10
	struct node_queue_bw	queue_bwspec;
	struct node_queue_opt	scheduler;
	int			priority;
	int			tbrsize;
	int			qlimit;
} queue_opts;

#ifndef __rtems__
struct table_opts {
#else /* __rtems__ */
static struct table_opts {
#endif /* __rtems__ */
	int			flags;
	int			init_addr;
	struct node_tinithead	init_nodes;
} table_opts;

#ifndef __rtems__
struct pool_opts {
#else /* __rtems__ */
static struct pool_opts {
#endif /* __rtems__ */
	int			 marker;
#define POM_TYPE		0x01
#define POM_STICKYADDRESS	0x02
	u_int8_t		 opts;
	int			 type;
	int			 staticport;
	struct pf_poolhashkey	*key;

} pool_opts;


#ifndef __rtems__
struct node_hfsc_opts	 hfsc_opts;
struct node_state_opt	*keep_state_defaults = NULL;
#else /* __rtems__ */
static struct node_hfsc_opts	 hfsc_opts;
static struct node_state_opt	*keep_state_defaults = NULL;
#endif /* __rtems__ */

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

#ifndef __rtems__
TAILQ_HEAD(loadanchorshead, loadanchors)
#else /* __rtems__ */
static TAILQ_HEAD(loadanchorshead, loadanchors)
#endif /* __rtems__ */
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
	} v;
	int lineno;
} YYSTYPE;

#define PPORT_RANGE	1
#define PPORT_STAR	2
int	parseport(char *, struct range *r, int);

#define DYNIF_MULTIADDR(addr) ((addr).type == PF_ADDR_DYNIFTL && \
	(!((addr).iflags & PFI_AFLAG_NOALIAS) ||		 \
	!isdigit((addr).v.ifname[strlen((addr).v.ifname)-1])))

#line 562 "pfctly.tab.c"

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
#define RANDOMID 324
#define REQUIREORDER 325
#define SYNPROXY 326
#define FINGERPRINTS 327
#define NOSYNC 328
#define DEBUG 329
#define SKIP 330
#define HOSTID 331
#define ANTISPOOF 332
#define FOR 333
#define INCLUDE 334
#define BITMASK 335
#define RANDOM 336
#define SOURCEHASH 337
#define ROUNDROBIN 338
#define STATICPORT 339
#define PROBABILITY 340
#define ALTQ 341
#define CBQ 342
#define PRIQ 343
#define HFSC 344
#define BANDWIDTH 345
#define TBRSIZE 346
#define LINKSHARE 347
#define REALTIME 348
#define UPPERLIMIT 349
#define QUEUE 350
#define PRIORITY 351
#define QLIMIT 352
#define RTABLE 353
#define LOAD 354
#define RULESET_OPTIMIZATION 355
#define STICKYADDRESS 356
#define MAXSRCSTATES 357
#define MAXSRCNODES 358
#define SOURCETRACK 359
#define GLOBAL 360
#define RULE 361
#define MAXSRCCONN 362
#define MAXSRCCONNRATE 363
#define OVERLOAD 364
#define FLUSH 365
#define SLOPPY 366
#define PFLOW 367
#define TAGGED 368
#define TAG 369
#define IFBOUND 370
#define FLOATING 371
#define STATEPOLICY 372
#define STATEDEFAULTS 373
#define ROUTE 374
#define SETTOS 375
#define DIVERTTO 376
#define DIVERTREPLY 377
#define STRING 378
#define NUMBER 379
#define PORTBINARY 380
#define YYERRCODE 256
typedef int YYINT;
static const YYINT pfctlylhs[] = {                       -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  127,  140,  140,
  140,  140,  140,  140,   18,  128,  128,  128,  128,  128,
  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
  128,   76,   76,   79,   79,   80,   80,   81,   81,  137,
   78,   78,  146,  146,  146,  146,  148,  147,  147,  133,
  133,  133,  133,  134,   26,  129,  149,  116,  116,  118,
  118,  117,  117,  117,  117,  117,  117,  117,  117,  117,
   17,   17,   17,  138,   91,   91,   92,   92,   93,   93,
  151,  110,  110,  112,  112,  111,  111,   11,   11,  139,
  152,  119,  119,  121,  121,  120,  120,  120,  120,  135,
  136,  153,  113,  113,  115,  115,  114,  114,  114,  114,
  114,  106,  106,   98,   98,   98,   98,   98,   98,   99,
   99,  100,  101,  101,  102,  154,  105,  103,  103,  104,
  104,  104,  104,  104,  104,  104,   95,   95,   95,   96,
   96,   97,  132,  155,  107,  107,  109,  109,  108,  108,
  108,  108,  108,  108,  108,  108,  108,  108,  108,  108,
  108,  108,  108,  108,  108,   13,   13,   23,   23,   29,
   29,   29,   29,   29,   29,   29,   29,   29,   29,   43,
   43,   44,   44,   15,   15,   15,   87,   87,   86,   86,
   86,   86,   86,   88,   88,   89,   89,   90,   90,   90,
   90,    1,    1,    1,    2,    2,    3,    4,   16,   16,
   16,   34,   34,   34,   35,   35,   36,   37,   37,   45,
   45,   60,   60,   60,   61,   62,   62,   47,   47,   48,
   48,   46,   46,   46,  142,  142,   49,   49,   49,   50,
   50,   54,   54,   51,   51,   51,   52,   52,   52,   52,
   52,   52,   52,   52,    5,    5,   53,   63,   63,   64,
   64,   65,   65,   65,   30,   32,   66,   66,   67,   67,
   68,   68,   68,    8,    8,   69,   69,   70,   70,   71,
   71,   71,    9,    9,   28,   27,   27,   27,   38,   38,
   38,   38,   39,   39,   41,   41,   40,   40,   40,   42,
   42,   42,    6,    6,    7,    7,   10,   10,   19,   19,
   19,   22,   22,   82,   82,   82,   82,   20,   20,   20,
   83,   83,   84,   84,   85,   85,   85,   85,   85,   85,
   85,   85,   85,   85,   85,   85,   75,   94,   94,   94,
   14,   14,   31,   56,   56,   55,   55,   74,   74,   74,
   33,   33,  156,  122,  122,  124,  124,  123,  123,  123,
  123,  123,  123,   73,   73,   73,   25,   25,   25,   25,
   24,   24,  130,  131,   77,   77,  125,  125,  126,  126,
   57,   57,   58,   58,   59,   59,   72,   72,   72,   72,
   72,  141,  143,  143,  144,  145,  145,  150,  150,   12,
   12,   21,   21,   21,   21,   21,   21,
};
static const YYINT pfctlylen[] = {                        2,
    0,    3,    2,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    4,    3,    2,    2,    3,
    3,    3,    3,    3,    1,    3,    3,    3,    6,    3,
    6,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    1,    1,    2,    1,    2,    1,    1,    1,    3,
    1,    0,    0,    2,    3,    3,    0,    5,    0,   10,
    7,    7,    7,    5,    2,    8,    0,    2,    0,    2,
    1,    1,    2,    2,    2,    1,    2,    1,    2,    3,
    2,    2,    2,    5,    2,    5,    2,    4,    1,    3,
    0,    2,    0,    2,    1,    1,    2,    1,    0,    5,
    0,    2,    0,    2,    1,    1,    3,    4,    2,    5,
    5,    0,    2,    0,    2,    1,    2,    2,    2,    1,
    2,    1,    1,    1,    4,    1,    4,    1,    4,    1,
    3,    1,    1,    3,    1,    0,    2,    1,    3,    2,
    8,    2,    8,    2,    8,    1,    0,    1,    4,    2,
    4,    1,    9,    0,    2,    0,    2,    1,    2,    2,
    1,    1,    2,    1,    1,    1,    1,    1,    2,    3,
    2,    2,    2,    4,    1,    1,    1,    1,    2,    0,
    1,    1,    5,    1,    1,    4,    4,    6,    1,    1,
    1,    1,    1,    0,    1,    1,    0,    1,    0,    1,
    1,    2,    2,    1,    4,    1,    3,    1,    1,    1,
    2,    0,    2,    5,    2,    4,    2,    1,    0,    1,
    1,    0,    2,    5,    2,    4,    1,    1,    1,    1,
    3,    0,    2,    5,    1,    2,    4,    0,    2,    0,
    2,    1,    3,    2,    2,    0,    1,    1,    4,    2,
    0,    2,    4,    2,    2,    2,    1,    3,    3,    3,
    1,    3,    3,    2,    1,    1,    3,    1,    4,    2,
    4,    1,    2,    3,    1,    1,    1,    4,    2,    4,
    1,    2,    3,    1,    1,    1,    4,    2,    4,    1,
    2,    3,    1,    1,    1,    4,    3,    2,    2,    5,
    2,    5,    2,    4,    2,    4,    1,    3,    3,    1,
    3,    3,    1,    1,    1,    1,    1,    1,    1,    2,
    2,    1,    1,    2,    3,    3,    3,    0,    1,    2,
    3,    0,    1,    3,    2,    1,    2,    2,    4,    5,
    2,    1,    1,    1,    1,    2,    2,    2,    4,    6,
    0,    1,    1,    1,    4,    2,    4,    0,    2,    4,
    0,    1,    0,    2,    0,    2,    1,    1,    1,    2,
    1,    1,    1,    0,    2,    4,    0,    1,    2,    1,
    3,    3,   10,   13,    0,    2,    0,    3,    0,    2,
    1,    4,    2,    4,    1,    4,    0,    1,    3,    3,
    3,    2,    4,    2,    2,    4,    2,    1,    0,    1,
    1,    1,    2,    2,    1,    2,    1,
};
static const YYINT pfctlydefred[] = {                     0,
    0,    0,    0,    0,  178,    0,  352,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   19,    0,
    0,    0,    0,    0,    0,   17,  189,    0,    0,    0,
  181,  179,    0,   51,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   18,    0,    0,    0,
    0,    0,   65,    0,    0,    0,  195,  196,    0,    0,
    0,    2,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   24,   16,   22,   21,   23,   20,
    0,    0,    0,    0,    0,   44,    0,    0,    0,   26,
    0,    0,   28,    0,    0,   30,   43,   42,   32,   35,
   34,  410,  411,   36,   37,   39,   40,  266,  265,   33,
   27,   25,  322,  323,   38,    0,  336,    0,    0,    0,
    0,    0,    0,  344,  345,    0,  342,  343,    0,  333,
    0,  203,    0,    0,  202,    0,   98,  213,    0,    0,
    0,    0,    0,   49,   48,   50,    0,    0,  382,  380,
  381,    0,    0,  220,  221,    0,    0,    0,  190,  191,
    0,  192,  193,    0,    0,  198,    0,    0,    0,    0,
  402,    0,    0,  405,    0,  335,  337,  341,  320,  321,
  338,    0,    0,  346,  408,    0,    0,  208,  209,  210,
    0,  206,  218,    0,    0,   89,   85,    0,    0,  217,
    0,    0,    0,    0,    0,    0,    0,    0,  120,  116,
    0,    0,    0,   46,  379,    0,    0,    0,    0,    0,
    0,  186,    0,  187,  100,    0,    0,    0,    0,    0,
  245,    0,    0,    0,    0,    0,    0,  334,  211,  205,
    0,    0,    0,   84,    0,    0,    0,  152,    0,  110,
  148,    0,    0,  136,  122,  123,  117,  121,  118,  119,
  115,  111,   64,    0,  398,    0,    0,    0,    0,  228,
  229,    0,  223,  227,    0,  230,    0,    0,    0,  183,
    0,    0,  106,    0,  105,    0,    0,    0,    0,    0,
  404,   29,    0,  407,   31,    0,  339,    0,  207,    0,
    0,   90,    0,    0,   96,   95,    0,  214,    0,  215,
    0,  132,    0,  130,  135,    0,  133,    0,    0,    0,
  391,    0,    0,  395,    0,    0,    0,    0,    0,  247,
    0,    0,    0,  239,    0,  248,    0,    0,    0,    0,
    0,  188,  109,    0,  104,    0,    0,   61,   62,   63,
    0,    0,    0,  340,   86,    0,   87,  347,   97,   94,
    0,    0,    0,  125,    0,  127,    0,  129,    0,    0,
    0,  146,    0,  138,    0,    0,    0,  399,    0,  401,
  400,    0,    0,    0,    0,  412,    0,    0,    0,    0,
    0,  244,  268,  276,    0,  255,  256,    0,    0,    0,
    0,    0,  254,    0,    0,  386,    0,    0,  235,    0,
  233,    0,  231,    0,  107,    0,    0,    0,  390,  403,
  406,  330,    0,  216,  149,    0,  150,  131,  134,    0,
  140,    0,  142,    0,  144,    0,    0,    0,    0,    0,
  368,  369,    0,  371,  372,  373,  367,    0,    0,  224,
    0,  225,    0,  413,  414,  416,  273,    0,    0,  264,
    0,    0,    0,    0,    0,    0,  243,    0,    0,    0,
  241,   66,    0,  252,  108,    0,    0,    0,   88,    0,
    0,    0,    0,  139,    0,    0,  393,  396,    0,  392,
  370,  362,  366,  153,    0,    0,    0,  274,  249,  258,
  259,  260,  267,  263,  262,  388,    0,    0,    0,    0,
   72,    0,    0,    0,    0,   78,    0,    0,    0,   76,
   71,    0,    0,   57,   60,    0,    0,    0,    0,    0,
  166,    0,  165,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  175,    0,  161,  162,  167,  164,  168,  158,
    0,  151,    0,    0,    0,  250,    0,    0,  226,  269,
    0,  270,    0,  354,    0,  383,  236,  234,    0,   73,
   81,   83,   82,   74,   77,   79,  317,  318,   75,    0,
   70,  253,    0,  298,  295,    0,    0,  313,  314,    0,
    0,  299,  315,  316,    0,    0,  301,    0,    0,  324,
  284,  285,    0,    0,    0,  159,  277,  293,  294,    0,
    0,    0,  160,  286,  163,    0,  176,  177,  171,  348,
    0,  172,  169,    0,  173,  275,    0,  157,    0,    0,
    0,    0,  394,    0,    0,    0,    0,   80,   53,  297,
    0,    0,    0,    0,    0,    0,  325,  326,    0,    0,
  282,    0,    0,  291,  327,    0,    0,  170,    0,    0,
    0,    0,  271,    0,    0,  360,  353,  237,    0,  296,
    0,    0,  308,  309,    0,    0,  311,  312,    0,    0,
    0,  283,    0,    0,  292,  349,    0,  174,    0,    0,
    0,    0,  384,  356,  355,    0,   54,   58,    0,    0,
  300,    0,  303,  302,    0,  305,  331,  278,    0,  279,
  287,    0,  288,    0,  141,  143,  145,    0,    0,   55,
   56,    0,    0,    0,    0,  350,    0,  357,  304,  306,
  280,  289,  376,
};
static const YYINT pfctlydgoto[] = {                      2,
   79,  276,  168,  226,  140,  621,  626,  634,  641,  609,
  363,  134,  649,   21,   89,  186,  550,  141,  157,  384,
  420,  158,   22,   23,  179,   24,  575,  617,   52,  655,
  696,  421,  521,  249,  413,  303,  304,  576,  701,  622,
  705,  627,  191,  194,  307,  364,  308,  443,  365,  516,
  366,  433,  434,  447,  695,  595,  354,  469,  355,  370,
  441,  540,  422,  526,  423,  636,  710,  637,  643,  713,
  644,  299,  723,  538,  335,  129,  368,   55,   57,  176,
  424,  578,  677,  159,  160,   75,  197,   76,  221,  222,
  164,  330,  227,  579,  280,  392,  281,  239,  343,  344,
  346,  347,  403,  404,  348,  287,  507,  580,  581,  274,
  336,  337,  170,  240,  241,  502,  551,  552,  255,  315,
  316,  408,  477,  478,  438,  378,   25,   26,   27,   28,
   29,   30,   31,   32,   33,   34,   35,   36,   37,    3,
  123,  203,  263,  126,  265,  699,  555,  613,  503,  216,
  275,  256,  171,  349,  508,  409,
};
static const YYINT pfctlysindex[] = {                   -41,
    0,  240, 1290,   91,    0,  289,    0,   36, -160, -144,
 -144, -144, 1628,  235, -129,   26, -113,   29,  258,    0,
  423, -155,   26, -155,  321,  368,  372,  376,  388,  406,
  437,  460,  479,  516,  520,  526,  563,  568,    0,  579,
  -57,  605,  633,  636,  638,    0,    0,  529,  614,  617,
    0,    0,  231,    0, -155, -144,   26,   26,   26,  297,
  -89,  -84, -196,  -45, -189,  300,  302,   26, -124, -144,
  134, 1742,  648,  427,  385,  440,    0,    8,    0,   26,
 -144,  168,    0, -162, -162, -162,    0,    0,  235,  354,
  235,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  429,  280,  291,  688,  488,    0,  354,  354,  354,    0,
  379,  753,    0,  436,  753,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  438,    0,  471,  477,  312,
  486,  497,  710,    0,    0,  507,    0,    0,  764,    0,
  417,    0,   14,  354,    0,  753,    0,    0,  464,  548,
 1505,    0,  580,    0,    0,    0,  168,  427,    0,    0,
    0,   26,   26,    0,    0,  632,   26,  510,    0,    0,
  370,    0,    0,  866,    0,    0,   26,  632,  632,  632,
    0,  753,  544,    0,  550,    0,    0,    0,    0,    0,
    0,  883,  556,    0,    0, 1742, -144,    0,    0,    0,
  381,    0,    0,  753,  464,    0,    0,    0,  906,    0,
  -81,  903,  905,  908,  316,  570,  578,  594,    0,    0,
 1505,  -81, -144,    0,    0,  354,  671,  -64,  -59,  354,
  909,    0,  291,    0,    0,  -94,  354,  -59,  -59,  -59,
    0,  753,   49,  753,   63,  595,  898,    0,    0,    0,
  417,  -24,  938,    0, -211,   73,  753,    0,  753,    0,
    0,  604,  613,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  632,    0,   24,   24,   24,  354,    0,
    0,  753,    0,    0,   -9,    0,  623,  721,  632,    0,
  952,  618,    0,  753,    0,  -94,  632,  645,  645,  645,
    0,    0,  544,    0,    0,  550,    0,  639,    0,   74,
  753,    0,  629,  635,    0,    0, -211,    0,  906,    0,
  642,    0,  474,    0,    0,  523,    0,  975,  272,  750,
    0,  753,  644,    0,    0,    0,    0,  632,  333,    0,
   78,  753, 1053,    0,  741,    0,  649,  906,  -58,  760,
  -59,    0,    0,  -10,    0,  -59,  651,    0,    0,    0,
  753,  753,  678,    0,    0,  -24,    0,    0,    0,    0,
  753,   86,  753,    0,  604,    0,  613,    0,   64,   80,
   85,    0,  764,    0,  206,   -4,  206,    0, 1280,    0,
    0,  -59,  128,  753,  753,    0,  970,  978,  982,  168,
  664,    0,    0,    0,   -7,    0,    0,  667,  195,  999,
  682,  687,    0, 1021,   78,    0,  701,  645,    0,  753,
    0,   -9,    0,    0,    0,  753,  131,    0,    0,    0,
    0,    0,  753,    0,    0,  642,    0,    0,    0,  316,
    0,  316,    0,  316,    0,  272,  802,  753,  182, 1032,
    0,    0, -144,    0,    0,    0,    0, 1280,    0,    0,
  333,    0,  130,    0,    0,    0,    0,  168,  197,    0,
  698,  699,  700, 1039, 1020,  704,    0, -144,  798,  706,
    0,    0,  815,    0,    0,   -7,  964, 4116,    0,  753,
  764,  764,  764,    0,   -7,  623,    0,    0,   -4,    0,
    0,    0,    0,    0,  753,  213,  753,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  877,    0,  753,  217,
    0,  715,  311,  719,  711,    0,  720,  336,  735,    0,
    0,  815,  753,    0,    0,    5,  -53,   99,  823,  824,
    0,  835,    0,  116,  124,  336,  838,  346,   27,  743,
 -144,  377,    0,  748,    0,    0,    0,    0,    0,    0,
 4116,    0,  744,  745,  749,    0,  906,  753,    0,    0,
  130,    0,  753,    0,  843,    0,    0,    0,  706,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -144,
    0,    0, 1124,    0,    0,  762, 1091,    0,    0,  753,
  862,    0,    0,    0,  753,  864,    0, 1105, 1105,    0,
    0,    0,  753,  767,  390,    0,    0,    0,    0,  753,
  768,  411,    0,    0,    0, 1105,    0,    0,    0,    0,
  772,    0,    0,  867,    0,    0, -144,    0,  764,  764,
  764,  645,    0,  753,  206,  168,  753,    0,    0,    0,
  762,  414,  432,  446,  450, 1742,    0,    0,  163,  390,
    0,  198,  411,    0,    0,  559,  168,    0,  316,  316,
  316,  865,    0,  753,  325,    0,    0,    0, 1257,    0,
  463,  753,    0,    0,  468,  753,    0,    0,  573,  476,
  753,    0,  491,  753,    0,    0,  774,    0, 1113, 1115,
 1116,  206,    0,    0,    0,  206,    0,    0, 1148, 1150,
    0,  414,    0,    0,  446,    0,    0,    0,  163,    0,
    0,  198,    0, 1122,    0,    0,    0,  880,  753,    0,
    0,  753,  753,  753,  753,    0,  168,    0,    0,    0,
    0,    0,    0,
};
static const YYINT pfctlyrindex[] = {                    43,
    0,  545,  352,    0,    0, 1520,    0,    0, 2567,    0,
    0,    0,    0,  834,    0, 1412,    0,    0,    0,    0,
    0, 2174, 3980, 1569,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1637, 1757, 1874,
    0,    0,    0,    0, 2798, 1183,  337,  337,  337,    0,
    0,    0,    0,    0,    0,    0,    0, 1156,    0,    0,
    0,    0, 1067, 1288,    0, 1400,    0,  801, 1428,   84,
    0,    0,    0, 3867, 3867,  539,    0,    0, 2630, 4002,
 1705,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, 2854,    0,  481,  481,  481,    0,
    0,  803,    0,    0,  803,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   82,
    0,    0,    0,    0,    0,    0,    0,    0,  960,    0,
    0,    0,    0,   46,    0,  -16,    0,    0,    0,    0,
    0,  712,    0,    0,    0,    0, 1166, 3924,    0,    0,
    0,  571, 2742,    0,    0, 4037, 3769,    0,    0,    0,
  462,    0,    0,    0,    9,    0, 2975,   56,   56,   56,
    0, 1591,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  498,    0,    0,   34,    0,    0,    0,   47,  801,    0,
 1170,  558,  569,  581,    0,    0,    0,    0,    0,    0,
    1, 1170,    0,    0,    0,  -54, 3093,    0, 2622, 2979,
    0,    0,    0,    0,    0,    0, 3254,   23,   23,   23,
    0,  -34,  804,  -34,  804,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -12,  -13,    0,  803,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  917,    0,    0,    0,    0, 3317,    0,
    0,  485,    0,    0, 1089,    0,  391, 2058, 2062,    0,
    0,    0,    0,   39,    0,  520, 3370, 1175, 1175, 1175,
    0,    0,    0,    0,    0,    0,    0,  109,    0,   41,
  -22,    0,    0,    0,    0,    0,  321,    0,  801,    0,
    0,    0,  804,    0,    0,  804,    0,    0,    0,    0,
    0,   34,    0,    0, 2335, 2335, 2335, 3450,    0,    0,
    0,  154,    0,    0, 1966,    0,    0,  943,    0, 2230,
 2469,    0,    0, 1089,    0, 3530,    0,    0,    0,    0,
  -34,  -34,  521,    0,    0,    0,    0,    0,    0,    0,
  -13,  804,  -34,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  204,    0,    0,    0,    0,    0,    0,    0,
    0, 3593,  462,  161,  218,    0,    0,  489,  496,    0,
  673,    0,    0,    0, 1089,    0,    0,    0,  363,    0,
    0,    0,    0,  483,    0,    0,    0,  650,    0,  803,
    0, 1089,    0,  713,    0,   11, 1007, 3646,    0,    0,
    0,    0,  -22,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   -1,  -22,   41,    0,
    0,    0, 2447,    0,    0,    0,    0, 3149, 3869,    0,
    0,    0,    0,    0,    0,    0,    0,    0, 1007,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  441,    0,
    0,    0,  819,    0,    0, 1089, 1179,  819,    0,  -34,
  812,  812,  812,    0, 1089,   -5,    0,    0,    0,    0,
    0,    0,    0,    0,  161,  248,   16,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  534,  -34,  804,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    3,   11,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    4,    0,    0,    0,    0,    0,    2,  -22,    0,    0,
    0,    0,  421,    0,  697,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  485,
  896,    0,    0,    0,  485,  936,    0, 3726, 3726,    0,
    0,    0,  218,  793,    0,    0,    0,    0,    0,  218,
  833,    0,    0,    0,    0, 3726,    0,    0,    0,    0,
    0,    0,    0, 3806,    0,    0,    0,    0,  462,  462,
  462,   17,    0,   16,    0,    0,  -34,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  804,    0,    0,    0,    0,
    0, 1182,    0,  192,  616,    0,    0,    0,    0,    0,
  462,  161,    0,    0,  462,  161,    0,    0, 1859,  248,
   16,    0,  248,   16,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1186,  192,    0,
    0,  161,  161,   16,   16,    0,    0,    0,    0,    0,
    0,    0,    0,
};
static const YYINT pfctlygindex[] = {                     0,
 1552,    0, -166,   51,    0,    0,    0, -470, -439,  631,
  -70,    0,    0, 1200,   66,  991,    0,    0,    0,    0,
  737, 1135,    0,    0,  796,    0,    0, -423,    0,  532,
  451, -322,    0, 1328,    0, -352,    0,    0,    0, -622,
    0, -613,    0,  954, -174,  771,    0,    0,    7,    0,
    0,  279,    0,  795,    0,    0, -348,    0,  599,    0,
 -431,    0,  786,    0, -443,    0,    0, -611,    0,    0,
 -607,    0,    0,    0, -375,    0,  707,    0,   -8, 1048,
  -80,    0, -425,  553, 1010,  215,    0,   95,    0,  959,
    0,    0, -242,    0,  990,    0, -295,    0,    0,  847,
    0,  836,    0,  777,    0, 1038,  766,  666,    0,    0,
  913,    0, 1080, 1012,    0,    0,  702,    0,    0,  941,
    0, -205,  780,    0,  672, -282,    0,    0,    0, 1258,
 1260,   -3,   -2,    0,    0,    0,    0,    0,    0,    0,
 -168, -119,    0, -180,    0,    0,    0,    0,    0, -176,
    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 4493
static const YYINT pfctlytable[] = {                     44,
   45,  177,   58,   59,  385,  205,  414,  169,  251,  246,
  113,  387,   68,  155,  253,  225,  246,  246,  103,  246,
  409,  246,  167,  167,  264,  167,  389,  385,  314,  331,
  246,  251,  238,  122,  262,  353,  379,  380,  125,  527,
  167,  279,    1,  246,  271,  393,  229,  116,  246,  702,
  246,  616,    1,  225,  246,  219,   93,  468,  302,  246,
  706,  142,  277,  353,  440,  222,  651,  711,  539,  620,
  246,  246,  173,  246,  714,  246,  246,  246,  246,  127,
  409,    1,  261,  318,  319,  320,  323,  333,  326,   91,
  246,  319,  215,  212,  178,   53,  177,  487,  246,  339,
   46,   73,  246,  460,  272,   87,  215,   88,  132,  752,
  417,  246,  362,  362,  445,  362,  215,  215,  328,  462,
  115,  753,  319,  113,  464,  319,  155,  754,  525,  215,
  166,  101,  577,  246,  755,  246,  224,  418,  416,  419,
  246,  334,  321,  453,  324,  382,  352,  664,  417,  328,
  410,  411,  328,  386,  381,  499,  417,  340,  169,  341,
  510,  246,  417,  246,  681,  528,  395,  667,  162,  397,
  588,  215,  391,  322,  215,  418,  416,  419,  180,  180,
  180,  128,  359,  418,  416,  419,  246,  325,  133,  418,
  416,  419,  670,  246,  374,  417,  444,  338,  385,  312,
  415,  448,  684,  678,  246,  577,  212,  305,  269,  712,
  455,  387,  219,  246,  130,  456,  306,   54,  219,  230,
  685,  625,  418,  416,  419,  215,  466,   84,   85,   86,
  417,  246,  406,   56,  293,  246,  481,  479,  633,  491,
  215,  492,  425,  715,  137,  431,  640,  700,   77,   20,
  246,  246,  480,  138,  139,  505,  215,  418,  416,  419,
  215,  450,  451,  131,   80,  432,  360,  360,  169,  360,
  506,  454,  245,  457,  361,  273,  246,  246,  246,  246,
  409,  614,  385,  313,  238,  246,  251,  246,  121,  387,
  238,   78,  519,  124,  482,  483,  278,  437,    1,    1,
    1,    1,  101,  183,  389,  187,  518,  409,  409,  409,
  246,  246,  506,  300,  301,  246,  246,  222,   82,  439,
  500,  529,  222,  222,  618,  619,  504,    1,    1,    1,
   92,  222,  596,  509,  583,  584,  585,  590,  246,  246,
    1,  598,   81,  246,  219,   91,  212,  385,  517,  591,
  113,  251,    1,  223,  387,  246,    1,    1,    1,    1,
    1,  246,  385,  599,  246,  409,  251,  251,  215,   99,
   99,   99,  257,  351,    1,  238,    1,   93,  113,  692,
  446,   94,  615,    1,  246,   95,  101,  319,  246,  246,
  582,  223,    1,  246,  246,  257,    1,   96,  219,   91,
  385,  351,  257,  257,  650,  589,  257,  592,  222,  319,
  252,  246,  246,  215,  328,   97,  246,  246,  409,  597,
    1,  270,  257,  385,  215,  212,  212,  212,  212,  212,
  246,  446,  549,  612,  212,  212,  328,  574,  319,  319,
  319,  285,  286,  319,  319,  319,   98,  319,  319,  725,
  358,  319,  319,  246,  246,  174,  175,  285,  286,  319,
  246,  212,  285,  286,  522,  328,  328,  328,  663,   99,
  328,  328,  328,  665,  328,  328,  623,  624,  328,  328,
  246,  549,  689,  690,  691,  257,  328,  257,  100,  536,
  219,  656,  261,  631,  632,    4,    5,    6,   73,   74,
  672,  638,  639,  143,  144,  674,  215,  174,  175,  717,
  574,  215,  553,  679,  394,  261,  437,  215,  726,  215,
  682,  586,  261,  261,  732,  101,  261,  246,  735,  102,
  329,  246,  246,  739,  215,  103,  742,    7,  246,  246,
  631,  632,  261,  365,  693,  174,  175,  698,   47,    8,
  409,  409,  409,    9,   10,   11,   12,   13,   48,   49,
   50,  329,  653,  396,  329,  246,  215,  124,  111,  246,
  246,   14,  104,   15,  724,  638,  639,  105,  126,  428,
   16,  409,  733,  429,  430,  697,  736,  731,  106,   17,
  128,  740,  734,   18,  743,  246,  246,   51,  212,  716,
  738,  668,  215,  212,  212,  261,  656,  261,  114,  212,
  212,  212,  212,  737,  107,  741,  215,   19,  399,  400,
  401,  601,  602,  603,  257,  409,  409,  184,  185,  758,
  257,  257,  759,  760,  761,  762,  351,  351,  351,  257,
  257,  257,  108,  257,  257,  109,  257,  110,  688,  402,
  257,  257,  257,  112,  257,  409,  113,  189,  190,  389,
  257,  257,  257,  257,  257,  257,  257,  257,  192,  193,
  257,  209,  210,  257,  120,  409,  697,  135,  385,  136,
  124,   83,  272,  467,  217,  470,  257,  161,  257,  212,
   73,  126,  218,  285,  286,  729,  730,  257,  257,  257,
  257,  257,  257,  128,  165,  272,  359,   84,   85,   86,
  300,  301,  257,  607,  608,  257,  272,  163,  257,  219,
  220,  114,   69,  647,  648,  385,  385,  385,  385,  385,
  257,  257,  272,  272,  272,  188,  257,  257,  257,  257,
  257,  257,  219,  385,  261,   67,  385,  219,  219,  195,
  261,  261,  196,  219,  654,  175,  219,  201,  385,  261,
  261,  261,  202,  261,  261,  409,  261,  631,  632,  213,
  261,  261,  261,  409,  261,  358,  358,  358,  358,  358,
  261,  261,  261,  261,  261,  261,  261,  261,  638,  639,
  261,  618,  619,  261,  246,  272,  358,  272,  246,  246,
  409,  409,  281,  351,  377,  377,  261,  215,  261,  703,
  704,  377,  377,  377,  204,  594,  206,  261,  261,  261,
  261,  261,  261,  623,  624,  281,  329,  707,  708,  351,
  351,  351,  261,  219,  114,  261,  281,  212,  261,  409,
  409,  223,  290,  212,  212,  212,  243,  167,  329,  207,
  261,  261,  281,  281,  281,  208,  261,  261,  261,  261,
  261,  261,  246,  246,  211,  290,  415,  415,  363,  363,
  363,  363,  363,  417,  417,  212,  290,  329,  329,  329,
  181,  182,  329,  329,  329,  214,  329,  329,  251,  363,
  329,  329,  290,  290,  290,  356,  357,  231,  329,  124,
  124,  124,  124,  124,  248,  307,  254,  124,  124,  124,
  126,  126,  126,  126,  126,  281,  431,  281,  126,  126,
  126,  121,  128,  128,  128,  128,  128,  124,  307,  266,
  128,  128,  128,  267,  272,  124,  432,  389,  167,  307,
  272,  272,  282,  694,  283,  310,  126,  284,  288,  310,
  272,  272,  387,  272,  272,  290,  289,  290,  128,  328,
  272,  272,  272,  295,  272,  296,  297,  298,  310,   41,
  272,  272,  290,  327,  272,  272,  272,  272,  332,  310,
  272,  342,  369,  272,  389,  389,  389,  389,  389,  409,
  345,  367,  372,  409,  409,  373,  272,  377,  272,  593,
  748,   67,   67,  383,  749,  389,  388,  272,  272,  272,
  272,  272,  272,  389,   67,  398,  405,   67,  307,  278,
  307,  407,  272,   67,  435,  272,  436,  442,  272,  449,
  484,  359,  359,  359,  359,  359,   67,  452,  485,  409,
  272,  272,  486,  488,  490,  493,  409,  272,  272,  272,
  272,  272,  359,  112,  112,  112,  112,  112,  310,  494,
  310,  281,  112,  112,  495,   67,  409,  496,  498,  515,
  281,  281,  520,  281,  281,  530,  204,  531,  532,  533,
   67,  534,  535,  439,  281,  537,  554,   67,  605,  114,
  281,  281,  431,  600,  281,  281,  281,  604,  606,  204,
  281,  290,  610,  541,  542,  628,  629,  198,  199,  200,
  290,  290,  432,  290,  290,  657,  543,  630,  281,  544,
  646,  652,  659,  660,  290,  545,  666,  661,   99,  409,
  290,  290,  281,  669,  290,  290,  290,  671,  546,  615,
  290,  673,  281,  675,  676,  281,  680,  683,   99,  686,
  687,  744,  722,  745,  228,  746,  747,  750,  290,  751,
  281,  281,  756,  757,  307,  212,  199,  547,  281,  281,
  281,  281,  290,  307,  307,   47,  307,  307,   99,  147,
  246,  409,  290,  222,  389,  290,   99,  307,   59,  548,
  409,  374,   45,  307,  307,  375,  645,  307,  307,  307,
  290,  290,   41,  307,  310,  145,  311,  763,  290,  290,
  290,  290,  501,  310,  310,   45,  310,  310,  718,  489,
  497,  307,  587,   45,  244,  268,   45,  310,  709,  329,
  387,  292,  459,  310,  310,  307,  294,  310,  310,  310,
  309,  458,  514,  310,  524,  307,  658,  317,  307,  390,
  428,  242,  291,  611,  429,  430,  375,  523,  662,    0,
   42,  310,   43,  307,  307,  409,  727,    0,    0,    0,
    0,  307,  307,  307,  307,  310,    0,  387,  387,  387,
  387,  387,    0,  409,    0,  310,    0,  409,  310,  358,
    0,    0,    0,    0,    0,  387,    0,  201,  387,   39,
  635,  642,    0,  310,  310,   45,  409,  409,    0,    0,
   99,  310,  310,  310,  310,    0,  409,  409,  409,    0,
  201,  409,  409,  409,    0,  409,  409,    0,  204,  409,
  409,  204,  204,  204,  204,  204,    0,  409,    0,  204,
  204,  204,  204,    0,  204,  204,    0,  204,  204,    0,
    0,    0,  426,  427,  204,  204,  204,    0,  204,  204,
    0,  204,  204,  204,  204,  204,    0,    0,  204,  204,
  204,  204,    0,    0,  204,    0,    0,  204,    0,    0,
  409,  728,    0,    0,  409,  409,    0,    0,   99,   99,
  204,    0,  204,    0,    0,    0,    0,    0,    0,  204,
    0,  204,  204,  204,  204,  204,  204,    0,    0,  200,
    0,    0,    0,    0,   40,  635,  204,    0,  642,  204,
    0,    0,  204,    0,    0,    0,  428,    0,    0,    0,
  429,  430,  200,    0,  204,  204,  461,  463,  465,    0,
    0,  204,  204,  204,   45,    0,    0,    0,   45,   45,
   45,   45,    0,    0,    0,   45,   45,   45,   45,    0,
   45,   45,   99,   45,   45,    0,   99,   99,    0,    0,
   45,   45,   45,    0,   45,  635,    0,    0,  642,    0,
   45,   45,    0,    0,   45,   45,   45,   45,    0,    0,
   45,    0,    0,   45,    0,    0,    0,  511,    0,  512,
    0,  513,    0,    0,    0,    0,   45,    0,   45,    0,
    0,    0,    0,    5,    6,    0,    0,   45,   45,   45,
   45,   45,   45,    0,    0,  258,  259,  260,    0,  180,
    0,    0,   45,    0,    0,   45,    0,    0,   45,    0,
    0,    0,    0,    0,    0,   38,    5,    6,    0,  201,
   45,   45,  180,  201,  201,  201,  201,   45,   45,   45,
  201,  201,  201,  201,    0,  201,  201,    0,  201,  201,
    9,   10,   11,   12,   90,    0,  201,  201,  194,  201,
  201,    0,  201,  201,  201,  201,  201,    7,    0,  201,
  201,  201,  201,    0,    0,  201,    0,    0,  201,    0,
    0,  194,    0,    9,   10,   11,   12,    0,  117,  118,
  119,  201,    0,  201,  471,  472,  473,  474,  475,  137,
  201,  350,    0,  246,    0,    0,    0,  201,    0,    0,
  246,  172,    0,    0,  246,  476,  371,  201,    0,    0,
  201,    0,    0,    0,  376,    0,  182,    0,    0,    0,
  246,  246,  246,    0,    0,  201,  201,    0,    0,    0,
    0,  200,  201,  201,  201,  200,  200,  200,  200,  182,
    0,    0,  200,  200,  200,  200,    0,  200,  200,    0,
  200,  200,    0,    0,    0,  412,    0,    0,  200,  200,
    0,  200,  200,    0,  200,  200,  200,  200,  200,    0,
    0,  200,  200,  200,  200,    0,    0,  200,    0,    0,
  200,    0,    0,  246,  199,  246,    0,    0,    0,    0,
    0,    0,    0,  200,    0,  200,  719,  720,  721,    0,
    0,    0,  200,  246,  247,    0,    0,  199,  250,  200,
    0,    0,    0,    0,    0,    0,    0,    0,  257,  200,
    0,    0,  200,  212,  212,  212,  212,  212,    0,    0,
    0,  212,  212,  212,    0,    0,  184,  200,  200,  112,
  112,  112,  112,  112,  200,  200,  200,  114,  112,  112,
  180,  180,  180,  180,  180,  180,  180,  180,  180,  184,
    0,    0,  180,  180,  180,  180,    0,  180,  180,    0,
  180,  180,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  180,  180,    0,  180,  180,  180,  180,  180,    0,
    0,  180,  180,  180,    0,    0,    0,  180,    0,    0,
  194,    0,  194,  194,  194,  194,  194,    0,    0,    0,
    0,  194,  194,  194,  194,  180,  232,  233,  234,  235,
  236,    0,    0,    0,    0,  237,  238,  194,  194,  180,
    0,    0,    0,    0,    0,    0,    0,  246,    0,  180,
  194,    0,  180,  194,    0,    0,    0,    0,    0,  194,
    0,    0,    0,  185,    0,    0,    0,  180,  180,    0,
  246,  246,  194,    0,    0,  180,  180,  182,  182,  182,
  182,  182,  182,  182,  182,  182,  185,    0,    0,  182,
  182,  182,  182,    0,  182,  182,    0,  182,  182,    0,
    0,  194,    0,    0,    0,    0,    0,    0,  182,  182,
    0,  182,  182,  182,  182,  182,  194,    0,  182,  182,
  182,    0,    0,  194,  182,    0,   60,   61,   62,   63,
   64,    0,   65,    0,   66,    0,   67,   68,   69,    0,
    0,    0,  182,    0,  246,    0,  199,    0,  246,  246,
  199,  199,  199,    0,    0,  242,  182,  199,  199,  199,
  199,    0,   70,    0,    0,    0,  182,    0,    0,  182,
    0,    0,    0,  199,  199,    0,    0,    0,  242,   71,
   72,    0,    0,    0,  182,  182,  199,    0,    0,  199,
    0,    0,  182,  182,    0,  199,    0,  184,  184,  184,
  184,  184,  184,  184,  184,  184,    0,    0,  199,  184,
  184,  184,  184,    0,  184,  184,    0,  184,  184,    0,
    0,    0,    0,    0,    0,    0,    0,  146,  184,  184,
    0,  184,  184,  184,  184,  184,    0,  199,  184,  184,
  184,    0,    0,    0,  184,    0,    0,  232,    0,  147,
    0,  222,  199,    0,    0,    0,    0,    0,    0,  199,
    0,    0,  184,    0,    0,    0,    0,    0,  242,    0,
  232,    0,    0,    0,  222,    0,  184,    0,  148,  149,
  150,    0,    0,  151,  152,  153,  184,  154,  155,  184,
    0,  143,  144,    0,    0,    0,    0,    0,    0,  156,
    0,    0,    0,    0,  184,  184,    0,    0,    0,    0,
    0,    0,  184,  184,  185,  185,  185,  185,  185,  185,
  185,  185,  185,    0,    0,    0,  185,  185,  185,  185,
    0,  185,  185,    0,  185,  185,    0,    0,    0,    0,
    0,    0,    0,    0,  409,  185,  185,    0,  185,  185,
  185,  185,  185,    0,    0,  185,  185,  185,    0,    0,
  232,  185,    0,  194,    0,    0,  409,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  185,
    0,    0,    0,    0,    0,    0,  194,    0,    0,    0,
    0,    0,    0,  185,    0,  409,  409,  409,    0,    0,
  409,  409,  409,  185,  409,  409,  185,  242,  409,  409,
    0,    0,    0,  242,  242,    0,  409,    0,    0,  240,
    0,  185,  185,  242,  242,    0,  242,  242,    0,  185,
  185,    0,    0,  242,  242,  242,    0,  242,    0,    0,
    0,    0,  240,  242,  242,    0,    0,  242,  242,  242,
  242,    0,    0,  242,    0,    0,  242,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  242,
    0,  242,    0,    0,    0,    0,    0,    0,    0,    0,
  242,  242,  242,  242,  242,  242,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  242,    0,    0,  242,    0,
    0,  242,    0,  222,    0,  232,  232,    0,  222,  222,
    0,    0,    0,  242,  242,  232,  232,  222,  232,  232,
  242,  242,  242,    0,  365,  232,  232,  232,    0,  232,
  222,  222,  240,    0,    0,  232,  232,    0,    0,  232,
  232,  232,  232,  222,    0,  232,  222,  365,  232,    0,
    0,    0,  222,    0,    0,    0,    0,    0,    0,    0,
    0,  232,    0,  232,    0,  222,    0,    0,    0,    0,
    0,    0,  232,  232,  232,  232,  232,  232,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  232,    0,    0,
  232,    0,    0,  232,  222,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  232,  232,    0,    0,  222,
    0,    0,  232,  232,  232,  194,  222,  194,  194,  194,
  194,  194,  194,    0,    0,    0,  194,  194,  194,  194,
    0,  194,  194,    0,  194,  194,  361,    0,    0,    0,
    0,    0,    0,    0,    0,  194,  194,    0,  194,  194,
  194,  194,  194,    0,    0,  194,  194,  194,  238,  361,
    0,  194,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  240,  194,
    0,  238,    0,    0,    0,    0,    0,  240,  240,    0,
  240,  240,    0,  194,    0,    0,    0,  240,  240,  240,
    0,  240,    0,  194,    0,    0,  194,  240,  240,    0,
    0,  240,  240,  240,  240,    0,    0,  240,    0,    0,
  240,  194,  194,    0,    0,    0,    0,    0,    0,  194,
  194,    0,    0,  240,    0,  240,    0,    0,    0,    0,
    0,    0,    0,    0,  240,  240,  240,  240,  240,  240,
    0,    0,    0,    0,    0,    0,   52,    0,    0,  240,
    0,    0,  240,    0,    0,  240,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  365,  240,  240,   52,
    0,  365,  365,  365,  240,  240,  240,  365,  365,  365,
  365,    0,  365,  365,    0,  365,  365,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  365,    0,    0,    0,
    0,  238,  365,  365,    0,    0,  365,  365,  365,  199,
    0,    0,  365,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  238,    0,    0,    0,    0,    0,
  365,    0,  199,    0,    0,    0,    0,    0,    0,  363,
  363,  363,  363,  363,  365,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  365,    0,    0,  365,    0,   52,
  363,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  365,  365,    0,    0,    0,    0,  361,    0,
  365,  365,    0,  361,  361,  361,    0,    0,    0,  361,
  361,  361,  361,    0,  361,  361,    0,  361,  361,    0,
  238,    0,    0,    0,    0,    0,  238,    0,  361,    0,
    0,    0,    0,    0,  361,  361,    0,    0,  361,  361,
  361,  212,    0,    0,  361,    0,    0,  238,  238,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  238,    0,  361,  238,  212,    0,    0,    0,    0,  238,
    0,  361,  361,  361,  361,  361,  361,    0,    0,    0,
    0,    0,  238,    0,    0,    0,  361,    0,    0,  361,
    0,    0,  361,    0,    0,    0,    0,  194,    0,    0,
    0,    0,    0,    0,  361,  361,    0,    0,    0,    0,
    0,  238,  361,  361,    0,    0,    0,   52,   52,   52,
  194,   52,   52,   52,   52,   52,  238,    0,    0,   52,
   52,   52,   52,  238,   52,   52,    0,   52,   52,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   52,    0,
    0,    0,    0,  197,   52,   52,    0,    0,   52,   52,
   52,    0,    0,    0,   52,    0,    0,    0,    0,    0,
    0,    0,    0,  238,    0,    0,  197,    0,    0,  238,
    0,  199,   52,    0,    0,  199,  199,  199,  199,    0,
    0,    0,  199,  199,  199,  199,   52,  199,  199,  238,
  199,  199,    0,    0,    0,    0,   52,    0,    0,   52,
  194,  199,  199,    0,  199,  199,  199,  199,  199,    0,
    0,  199,  199,  199,   52,   52,    0,  199,    0,    0,
    0,    0,   52,   52,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  199,  238,  238,  238,  238,
  238,    0,    0,    0,    0,    0,    0,    0,    0,  199,
    0,    0,    0,    0,  238,    0,  197,  238,    0,  199,
    0,    0,  199,    0,  212,    0,    0,    0,  219,  238,
  238,    0,    0,    0,    0,    0,    0,  199,  199,    0,
    0,    0,    0,  212,    0,  199,  199,  212,  212,  212,
  212,  219,    0,    0,  212,  212,  212,  212,    0,  212,
  212,    0,  212,  212,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  212,  212,    0,  212,  212,  212,  212,
  212,    0,    0,  212,  212,  212,    0,    0,    0,  212,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  194,
    0,    0,  194,  194,  194,  194,  194,  212,    0,    0,
  194,  194,  194,  194,    0,  194,  194,    0,  194,  194,
    0,  212,    0,    0,    0,    0,    0,    0,    0,  194,
    0,  212,    0,    0,  212,  194,  194,  212,    0,  194,
  194,  194,  397,    0,    0,  194,    0,    0,    0,  212,
  212,    0,    0,    0,    0,  197,    0,  212,  212,  197,
  197,  197,  197,  194,    0,  397,  197,  197,  197,  197,
    0,  197,  197,    0,  197,  197,    0,  194,    0,    0,
    0,    0,    0,    0,    0,  197,    0,  194,    0,    0,
  194,  197,  197,    0,    0,  197,  197,  197,  364,    0,
    0,  197,    0,    0,    0,  194,  194,    0,    0,    0,
    0,    0,    0,  194,  194,    0,    0,    0,    0,  197,
    0,  364,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  197,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  197,    0,    0,  197,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  197,  197,    0,    0,    0,    0,    0,    0,  197,
  197,    0,    0,    0,    0,    0,  212,    0,    0,    0,
  219,  212,  212,  212,    0,  219,  219,  212,  212,  212,
  212,  219,  212,  212,  219,  212,  212,    0,    0,    0,
    0,    0,    0,  219,    0,    0,  212,  219,  219,    0,
    0,    0,  212,  212,    0,    0,  212,  212,  212,    0,
  219,    0,  212,  219,    0,    0,  219,    0,    0,  219,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  212,    0,  219,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  212,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  212,    0,  219,  212,    0,    0,
    0,  219,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  212,  212,    0,    0,  219,    0,    0,  219,
  212,  212,    0,  219,  397,    0,    0,    0,    0,  397,
  397,  397,    0,    0,    0,  397,  397,  397,  397,    0,
  397,  397,    0,  397,  397,    0,  219,    0,    0,  222,
    0,    0,    0,    0,  397,    0,    0,    0,    0,    0,
  397,  397,    0,    0,  397,  397,  397,    0,    0,    0,
  397,    0,  222,    0,    0,    0,    0,    0,    0,    0,
  364,    0,    0,    0,    0,  364,  364,  364,  397,    0,
    0,  364,  364,  364,  364,    0,  364,  364,    0,  364,
  364,    0,  397,    0,    0,    0,    0,    0,    0,    0,
  364,    0,  397,    0,    0,  397,  364,  364,    0,    0,
  364,  364,  364,    0,    0,    0,  364,    0,    0,  222,
  397,  397,    0,    0,    0,    0,    0,    0,  397,  397,
    0,    0,    0,    0,  364,    0,    0,    0,    0,    0,
    0,    0,  222,    0,    0,    0,    0,    0,  364,    0,
    0,    0,  222,    0,    0,    0,    0,    0,  364,    0,
    0,  364,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  219,  364,  364,    0,    0,
  219,  219,  219,    0,  364,  364,  219,    0,    0,  219,
    0,  219,  219,    0,  219,  219,    0,    0,    0,  238,
    0,    0,    0,    0,    0,  219,    0,    0,    0,    0,
    0,  219,  219,    0,    0,  219,  219,  219,    0,    0,
    0,  219,  238,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  219,  219,
    0,    0,    0,  219,  219,  219,    0,    0,    0,  219,
    0,    0,  219,  219,  219,  219,    0,  219,  219,    0,
    0,    0,  238,  219,    0,    0,  219,    0,  219,    0,
    0,    0,    0,    0,  219,  219,    0,    0,  219,  219,
  219,  219,  219,    0,  219,  238,    0,    0,    0,  219,
  219,  222,    0,    0,    0,    0,  222,  222,  222,    0,
    0,    0,  219,    0,    0,  222,    0,  222,  222,    0,
  222,  222,  238,    0,    0,  156,  219,    0,    0,    0,
    0,  222,    0,    0,    0,    0,  219,  222,  222,  219,
    0,  222,  222,  222,    0,    0,    0,  222,  154,    0,
    0,    0,    0,    0,  219,  219,    0,    0,    0,    0,
    0,    0,  219,  219,    0,  222,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  222,
    0,  222,    0,    0,    0,    0,  222,  222,  222,  222,
    0,    0,  222,    0,    0,  222,    0,  222,  222,    0,
  222,  222,    0,    0,    0,  332,    0,  222,  222,    0,
    0,  222,    0,    0,    0,  222,  222,  222,  222,    0,
    0,  222,  222,  222,    0,    0,    0,  222,  332,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  156,    0,
    0,    0,    0,    0,    0,  222,    0,    0,  212,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  222,
    0,  238,    0,    0,    0,    0,    0,  238,  238,  222,
    0,  212,  222,    0,    0,    0,    0,  238,  238,    0,
  238,  238,    0,    0,    0,   49,    0,  222,  222,    0,
    0,  238,    0,    0,    0,  222,  222,  238,  238,    0,
    0,  238,  238,  238,    0,    0,    0,  238,   49,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  332,    0,
    0,    0,    0,    0,  238,  238,    0,    0,    0,    0,
  238,  238,    0,    0,    0,    0,    0,    0,    0,  238,
  238,  238,    0,  238,  238,    0,  377,    0,  156,  238,
    0,    0,  238,    0,  238,    0,    0,    0,    0,    0,
  238,  238,    0,    0,  238,  238,  238,  238,  238,  377,
  238,  154,    0,    0,    0,  238,  238,    0,    0,    0,
    0,    0,    0,    0,  154,    0,    0,    0,  238,    0,
    0,    0,    0,  154,  154,    0,  154,  154,   49,    0,
    0,    0,  238,  378,    0,    0,    0,  154,    0,    0,
    0,    0,  238,  154,  154,  238,    0,  154,  154,  154,
    0,    0,    0,  154,    0,    0,  378,    0,    0,    0,
  238,  238,    0,    0,    0,    0,    0,    0,  238,  238,
    0,  154,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  154,    0,    0,    0,  212,
    0,    0,    0,    0,  332,  154,    0,    0,  154,    0,
    0,    0,    0,  332,  332,    0,  332,  332,    0,    0,
    0,  219,  212,  154,  154,    0,    0,  332,    0,    0,
    0,  154,  154,  332,  332,    0,    0,  332,  332,  332,
  212,    0,    0,  332,  219,  212,  212,    0,    0,    0,
    0,  212,  212,  212,  212,    0,  222,    0,    0,    0,
    0,  332,    0,    0,    0,    0,    0,  212,  212,    0,
    0,    0,    0,    0,    0,  332,    0,    0,    0,  222,
  212,    0,    0,  212,   49,  332,    0,    0,  332,  212,
    0,    0,    0,   49,   49,    0,   49,   49,    0,    0,
    0,    0,  212,  332,  332,    0,    0,   49,    0,    0,
    0,  332,  332,   49,   49,    0,    0,   49,   49,   49,
    0,    0,    0,   49,    0,    0,    0,    0,    0,    0,
    0,  212,    0,    0,    0,    0,    0,    0,  377,    0,
    0,   49,  377,  377,  377,    0,  212,  154,    0,  377,
  377,  377,  377,  212,    0,   49,  154,  154,  167,  154,
  154,    0,    0,    0,  377,   49,    0,    0,   49,    0,
  154,    0,    0,    0,    0,    0,  154,  154,    0,    0,
  154,  154,  154,   49,   49,    0,  154,    0,    0,    0,
    0,   49,   49,    0,    0,  378,    0,    0,    0,  378,
  378,  378,    0,    0,  154,    0,  378,  378,  378,  378,
    0,  377,  377,  377,  377,  377,    0,    0,  154,    0,
    0,  378,    0,    0,    0,    0,    0,    0,  154,  377,
    0,  154,  377,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  377,  377,  154,  154,    0,    0,
    0,  212,    0,    0,  154,  154,  212,  212,    0,    0,
    0,    0,  212,  212,  212,  212,    0,    0,  378,  378,
  378,  378,  378,  219,    0,    0,    0,  212,  219,  219,
    0,    0,    0,    0,  219,    0,  378,  219,    0,  378,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  219,
    0,  378,  378,    0,    0,    0,    0,    0,  222,    0,
    0,    0,    0,  222,  222,    0,    0,    0,    0,    0,
    0,    0,  222,    0,  212,  212,  212,  212,  212,    0,
    0,    0,    0,    0,  222,    0,    0,    0,    0,    0,
    0,    0,  212,    0,    0,  212,  219,  219,  219,  219,
  219,    0,    0,    0,    0,    0,    0,  212,  212,    0,
    0,    0,    0,    0,  219,    0,    0,  219,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  219,
  219,  222,  222,  222,  222,  222,    0,    0,    0,    0,
    0,    0,    0,    0,  556,    0,    0,    0,    0,  222,
    0,    0,  222,  557,  558,    0,  559,  560,    0,    0,
    0,    0,    0,    0,  222,  222,    0,  561,    0,    0,
    0,    0,    0,  562,  333,    0,    0,  563,  564,  565,
    0,    0,    0,  566,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  567,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  568,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  569,    0,    0,  570,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  571,    0,    0,    0,    0,    0,
    0,  572,  573,
};
static const YYINT pfctlycheck[] = {                      3,
    3,   82,   11,   12,   10,  125,  359,   78,   10,   44,
   10,   10,   10,   10,  191,   40,   33,   40,   10,   33,
   33,   44,   33,   33,  205,   33,   10,   33,  123,  272,
   44,   33,   10,  123,  203,   40,  319,  320,  123,  483,
   33,  123,    0,   33,  221,  341,  166,   56,   33,  672,
   40,   47,   10,   40,   44,   10,   10,  406,  123,   44,
  674,   70,  229,   40,  123,   10,   40,  679,  500,  123,
   60,   33,   81,   40,  682,   60,   61,   62,   40,  276,
   40,  123,  202,  258,  259,  260,  263,  299,  265,   24,
  125,   10,   44,   10,  257,   60,  177,  420,   60,  276,
   10,  264,  125,   40,  224,  261,   44,  263,  298,  732,
   33,  125,  123,  123,  125,  123,   44,   44,   10,   40,
   55,  735,   41,  123,   40,   44,  123,  739,  481,   44,
  123,  123,  508,  123,  742,  125,  123,   60,   61,   62,
  125,  353,  262,  386,  264,  326,  123,  591,   33,   41,
  356,  357,   44,  330,  323,  438,   33,  277,  229,  279,
  456,  123,   33,  125,  635,  488,  343,  599,   74,  346,
  519,   44,  339,  125,   44,   60,   61,   62,   84,   85,
   86,  378,  302,   60,   61,   62,   33,  125,  378,   60,
   61,   62,  616,   40,  314,   33,  371,  125,  125,  294,
  123,  376,  642,  629,   44,  581,  123,  267,  217,  680,
  125,  331,  267,   60,  260,  392,  276,  378,  273,  169,
  646,  123,   60,   61,   62,   44,  403,  285,  286,  287,
   33,   40,  352,  378,  243,   44,  413,  412,  123,   45,
   44,   47,  362,  683,   41,   40,  123,  671,  378,   10,
   33,   60,  125,  378,  379,  125,   44,   60,   61,   62,
   44,  381,  382,  309,  378,   60,  277,  277,  339,  277,
  447,  391,  178,  393,  284,  225,  123,   60,   61,   62,
   33,  277,  288,  378,  262,  125,  288,  277,  378,  288,
  268,  266,  469,  378,  414,  415,  378,  368,  256,  257,
  258,  259,  294,   89,  288,   91,  125,   60,   61,   62,
  300,  301,  489,  378,  379,  277,  125,  262,   61,  378,
  440,  125,  267,  268,  378,  379,  446,  285,  286,  287,
   10,  276,  538,  453,  511,  512,  513,  125,  300,  301,
  298,  125,  314,  378,  299,  299,   10,  353,  468,  526,
  350,  353,  310,  378,  353,  378,  314,  315,  316,  317,
  318,  378,  368,  540,  378,  378,  368,  369,   44,  368,
  368,  368,   10,  378,  332,  353,  334,   10,  378,  662,
  374,   10,  378,  341,  374,   10,  378,  306,  378,  379,
  510,  378,  350,  378,  379,   33,  354,   10,  353,  353,
   10,  378,   40,   41,  378,  525,   44,  527,  353,  328,
   41,  378,  374,   44,  306,   10,  378,  379,  378,  539,
  378,   41,   60,   33,   44,  342,  343,  344,  345,  346,
  277,  425,  503,  553,  351,  352,  328,  508,  357,  358,
  359,  378,  379,  362,  363,  364,   10,  366,  367,  125,
   10,  370,  371,  300,  301,  378,  379,  378,  379,  378,
   40,  378,  378,  379,  473,  357,  358,  359,  588,   10,
  362,  363,  364,  593,  366,  367,  378,  379,  370,  371,
   60,  552,  659,  660,  661,  123,  378,  125,   10,  498,
   10,  572,   10,  378,  379,  256,  257,  258,  264,  265,
  620,  378,  379,  370,  371,  625,   44,  378,  379,  686,
  581,   44,  506,  633,   41,   33,  587,   44,  695,   44,
  640,  515,   40,   41,  701,   10,   44,  374,  705,   10,
   10,  378,  379,  710,   44,   10,  713,  298,  378,  379,
  378,  379,   60,   10,  664,  378,  379,  667,  260,  310,
  347,  348,  349,  314,  315,  316,  317,  318,  270,  271,
  272,   41,  571,   41,   44,  374,   44,   10,   40,  378,
  379,  332,   10,  334,  694,  378,  379,   10,   10,  374,
  341,  378,  702,  378,  379,  666,  706,  125,   10,  350,
   10,  711,  125,  354,  714,  378,  379,  309,  262,   41,
  125,  610,   44,  267,  268,  123,  687,  125,  378,  273,
  274,  275,  276,   41,   10,  125,   44,  378,  347,  348,
  349,  311,  312,  313,  262,  378,  379,  274,  275,  749,
  268,  269,  752,  753,  754,  755,  285,  286,  287,  277,
  278,  279,   10,  281,  282,   10,  284,   10,  657,  378,
  288,  289,  290,   40,  292,   40,   40,  378,  379,   10,
  298,  299,  300,  301,  302,  303,  304,  305,  378,  379,
  308,  360,  361,  311,  378,   60,  757,  378,  288,  378,
  123,  259,   10,  405,  268,  407,  324,   40,  326,  353,
  264,  123,  276,  378,  379,  699,  699,  335,  336,  337,
  338,  339,  340,  123,  265,   33,   10,  285,  286,  287,
  378,  379,  350,  378,  379,  353,   44,  333,  356,  303,
  304,   10,   10,  378,  379,  335,  336,  337,  338,  339,
  368,  369,   60,   61,   62,  307,  374,  375,  376,  377,
  378,  379,  262,  353,  262,   33,  356,  267,  268,   62,
  268,  269,  265,  273,  378,  379,  276,  379,  368,  277,
  278,  279,   10,  281,  282,  268,  284,  378,  379,   60,
  288,  289,  290,  276,  292,  335,  336,  337,  338,  339,
  298,  299,  300,  301,  302,  303,  304,  305,  378,  379,
  308,  378,  379,  311,  374,  123,  356,  125,  378,  379,
  303,  304,   10,  259,  266,  267,  324,   44,  326,  378,
  379,  273,  274,  275,  379,  537,  379,  335,  336,  337,
  338,  339,  340,  378,  379,   33,  306,  378,  379,  285,
  286,  287,  350,  353,  123,  353,   44,  267,  356,  378,
  379,  378,   10,  273,  274,  275,  267,   33,  328,  379,
  368,  369,   60,   61,   62,  379,  374,  375,  376,  377,
  378,  379,  378,  379,  379,   33,  378,  379,  335,  336,
  337,  338,  339,  378,  379,  379,   44,  357,  358,  359,
   85,   86,  362,  363,  364,  379,  366,  367,  379,  356,
  370,  371,   60,   61,   62,  297,  298,  350,  378,  342,
  343,  344,  345,  346,  273,   10,   41,  350,  351,  352,
  342,  343,  344,  345,  346,  123,   40,  125,  350,  351,
  352,  378,  342,  343,  344,  345,  346,  378,   33,   47,
  350,  351,  352,  378,  262,  378,   60,  288,   33,   44,
  268,  269,   40,  665,   40,   10,  378,   40,  379,   41,
  278,  279,   10,  281,  282,  123,  379,  125,  378,   62,
  288,  289,  290,  293,  292,  295,  296,  297,   33,   10,
  298,  299,  379,  379,  302,  303,  304,  305,   41,   44,
  308,  378,  262,  311,  335,  336,  337,  338,  339,  374,
  378,  369,   41,  378,  379,  378,  324,  353,  326,  123,
  722,  289,  290,  365,  726,  356,  378,  335,  336,  337,
  338,  339,  340,  379,  302,   41,  267,  305,  123,  378,
  125,  378,  350,  311,  284,  353,  378,  268,  356,  379,
   61,  335,  336,  337,  338,  339,  324,  360,   61,   33,
  368,  369,   61,  380,  378,   47,   40,  375,  376,  377,
  378,  379,  356,  342,  343,  344,  345,  346,  123,  378,
  125,  269,  351,  352,  378,  353,   60,   47,  368,  268,
  278,  279,   41,  281,  282,  378,   10,  379,  379,   41,
  368,   62,  379,  378,  292,  288,  123,  375,  378,  378,
  298,  299,   40,  379,  302,  303,  304,  379,  379,   33,
  308,  269,  368,  289,  290,  283,  283,  117,  118,  119,
  278,  279,   60,  281,  282,  368,  302,  283,  326,  305,
  283,  379,  379,  379,  292,  311,  284,  379,   40,  123,
  298,  299,  340,   10,  302,  303,  304,   47,  324,  378,
  308,  280,  350,  280,   40,  353,  380,  380,   60,  378,
  284,  378,  288,   41,  164,   41,   41,   10,  326,   10,
  368,  369,   41,  284,  269,   10,  333,  353,  376,  377,
  378,  379,  340,  278,  279,   10,  281,  282,  378,   10,
  378,  378,  350,  267,   10,  353,  368,  292,   10,  375,
  379,   10,   10,  298,  299,   10,  566,  302,  303,  304,
  368,  369,    3,  308,  269,   71,  253,  757,  376,  377,
  378,  379,  442,  278,  279,   33,  281,  282,  687,  425,
  435,  326,  516,   41,  177,  216,   44,  292,  676,  271,
  288,  242,  397,  298,  299,  340,  246,  302,  303,  304,
  250,  395,  466,  308,  479,  350,  581,  257,  353,  337,
  374,  172,  241,  552,  378,  379,  316,  478,  587,   -1,
    3,  326,    3,  368,  369,  306,   10,   -1,   -1,   -1,
   -1,  376,  377,  378,  379,  340,   -1,  335,  336,  337,
  338,  339,   -1,  277,   -1,  350,   -1,  328,  353,  299,
   -1,   -1,   -1,   -1,   -1,  353,   -1,   10,  356,   10,
  564,  565,   -1,  368,  369,  123,  300,  301,   -1,   -1,
  368,  376,  377,  378,  379,   -1,  357,  358,  359,   -1,
   33,  362,  363,  364,   -1,  366,  367,   -1,  262,  370,
  371,  265,  266,  267,  268,  269,   -1,  378,   -1,  273,
  274,  275,  276,   -1,  278,  279,   -1,  281,  282,   -1,
   -1,   -1,  300,  301,  288,  289,  290,   -1,  292,  293,
   -1,  295,  296,  297,  298,  299,   -1,   -1,  302,  303,
  304,  305,   -1,   -1,  308,   -1,   -1,  311,   -1,   -1,
  374,  125,   -1,   -1,  378,  379,   -1,   -1,  300,  301,
  324,   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,  333,
   -1,  335,  336,  337,  338,  339,  340,   -1,   -1,   10,
   -1,   -1,   -1,   -1,  125,  679,  350,   -1,  682,  353,
   -1,   -1,  356,   -1,   -1,   -1,  374,   -1,   -1,   -1,
  378,  379,   33,   -1,  368,  369,  399,  400,  401,   -1,
   -1,  375,  376,  377,  262,   -1,   -1,   -1,  266,  267,
  268,  269,   -1,   -1,   -1,  273,  274,  275,  276,   -1,
  278,  279,  374,  281,  282,   -1,  378,  379,   -1,   -1,
  288,  289,  290,   -1,  292,  739,   -1,   -1,  742,   -1,
  298,  299,   -1,   -1,  302,  303,  304,  305,   -1,   -1,
  308,   -1,   -1,  311,   -1,   -1,   -1,  460,   -1,  462,
   -1,  464,   -1,   -1,   -1,   -1,  324,   -1,  326,   -1,
   -1,   -1,   -1,  257,  258,   -1,   -1,  335,  336,  337,
  338,  339,  340,   -1,   -1,  198,  199,  200,   -1,   10,
   -1,   -1,  350,   -1,   -1,  353,   -1,   -1,  356,   -1,
   -1,   -1,   -1,   -1,   -1,  256,  257,  258,   -1,  262,
  368,  369,   33,  266,  267,  268,  269,  375,  376,  377,
  273,  274,  275,  276,   -1,  278,  279,   -1,  281,  282,
  314,  315,  316,  317,   23,   -1,  289,  290,   10,  292,
  293,   -1,  295,  296,  297,  298,  299,  298,   -1,  302,
  303,  304,  305,   -1,   -1,  308,   -1,   -1,  311,   -1,
   -1,   33,   -1,  314,  315,  316,  317,   -1,   57,   58,
   59,  324,   -1,  326,  335,  336,  337,  338,  339,   68,
  333,  294,   -1,   33,   -1,   -1,   -1,  340,   -1,   -1,
   40,   80,   -1,   -1,   44,  356,  309,  350,   -1,   -1,
  353,   -1,   -1,   -1,  317,   -1,   10,   -1,   -1,   -1,
   60,   61,   62,   -1,   -1,  368,  369,   -1,   -1,   -1,
   -1,  262,  375,  376,  377,  266,  267,  268,  269,   33,
   -1,   -1,  273,  274,  275,  276,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,  358,   -1,   -1,  289,  290,
   -1,  292,  293,   -1,  295,  296,  297,  298,  299,   -1,
   -1,  302,  303,  304,  305,   -1,   -1,  308,   -1,   -1,
  311,   -1,   -1,  123,   10,  125,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  324,   -1,  326,  689,  690,  691,   -1,
   -1,   -1,  333,  182,  183,   -1,   -1,   33,  187,  340,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  197,  350,
   -1,   -1,  353,  342,  343,  344,  345,  346,   -1,   -1,
   -1,  350,  351,  352,   -1,   -1,   10,  368,  369,  342,
  343,  344,  345,  346,  375,  376,  377,  350,  351,  352,
  261,  262,  263,  264,  265,  266,  267,  268,  269,   33,
   -1,   -1,  273,  274,  275,  276,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  292,  293,   -1,  295,  296,  297,  298,  299,   -1,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,   -1,   -1,
  262,   -1,  264,  265,  266,  267,  268,   -1,   -1,   -1,
   -1,  273,  274,  275,  276,  326,  342,  343,  344,  345,
  346,   -1,   -1,   -1,   -1,  351,  352,  289,  290,  340,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  277,   -1,  350,
  302,   -1,  353,  305,   -1,   -1,   -1,   -1,   -1,  311,
   -1,   -1,   -1,   10,   -1,   -1,   -1,  368,  369,   -1,
  300,  301,  324,   -1,   -1,  376,  377,  261,  262,  263,
  264,  265,  266,  267,  268,  269,   33,   -1,   -1,  273,
  274,  275,  276,   -1,  278,  279,   -1,  281,  282,   -1,
   -1,  353,   -1,   -1,   -1,   -1,   -1,   -1,  292,  293,
   -1,  295,  296,  297,  298,  299,  368,   -1,  302,  303,
  304,   -1,   -1,  375,  308,   -1,  319,  320,  321,  322,
  323,   -1,  325,   -1,  327,   -1,  329,  330,  331,   -1,
   -1,   -1,  326,   -1,  374,   -1,  262,   -1,  378,  379,
  266,  267,  268,   -1,   -1,   10,  340,  273,  274,  275,
  276,   -1,  355,   -1,   -1,   -1,  350,   -1,   -1,  353,
   -1,   -1,   -1,  289,  290,   -1,   -1,   -1,   33,  372,
  373,   -1,   -1,   -1,  368,  369,  302,   -1,   -1,  305,
   -1,   -1,  376,  377,   -1,  311,   -1,  261,  262,  263,
  264,  265,  266,  267,  268,  269,   -1,   -1,  324,  273,
  274,  275,  276,   -1,  278,  279,   -1,  281,  282,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  306,  292,  293,
   -1,  295,  296,  297,  298,  299,   -1,  353,  302,  303,
  304,   -1,   -1,   -1,  308,   -1,   -1,   10,   -1,  328,
   -1,   10,  368,   -1,   -1,   -1,   -1,   -1,   -1,  375,
   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,  123,   -1,
   33,   -1,   -1,   -1,   33,   -1,  340,   -1,  357,  358,
  359,   -1,   -1,  362,  363,  364,  350,  366,  367,  353,
   -1,  370,  371,   -1,   -1,   -1,   -1,   -1,   -1,  378,
   -1,   -1,   -1,   -1,  368,  369,   -1,   -1,   -1,   -1,
   -1,   -1,  376,  377,  261,  262,  263,  264,  265,  266,
  267,  268,  269,   -1,   -1,   -1,  273,  274,  275,  276,
   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  306,  292,  293,   -1,  295,  296,
  297,  298,  299,   -1,   -1,  302,  303,  304,   -1,   -1,
  123,  308,   -1,   10,   -1,   -1,  328,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,
   -1,   -1,   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,
   -1,   -1,   -1,  340,   -1,  357,  358,  359,   -1,   -1,
  362,  363,  364,  350,  366,  367,  353,  262,  370,  371,
   -1,   -1,   -1,  268,  269,   -1,  378,   -1,   -1,   10,
   -1,  368,  369,  278,  279,   -1,  281,  282,   -1,  376,
  377,   -1,   -1,  288,  289,  290,   -1,  292,   -1,   -1,
   -1,   -1,   33,  298,  299,   -1,   -1,  302,  303,  304,
  305,   -1,   -1,  308,   -1,   -1,  311,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  324,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  335,  336,  337,  338,  339,  340,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  350,   -1,   -1,  353,   -1,
   -1,  356,   -1,  262,   -1,  268,  269,   -1,  267,  268,
   -1,   -1,   -1,  368,  369,  278,  279,  276,  281,  282,
  375,  376,  377,   -1,   10,  288,  289,  290,   -1,  292,
  289,  290,  123,   -1,   -1,  298,  299,   -1,   -1,  302,
  303,  304,  305,  302,   -1,  308,  305,   33,  311,   -1,
   -1,   -1,  311,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  324,   -1,  326,   -1,  324,   -1,   -1,   -1,   -1,
   -1,   -1,  335,  336,  337,  338,  339,  340,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  350,   -1,   -1,
  353,   -1,   -1,  356,  353,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  368,  369,   -1,   -1,  368,
   -1,   -1,  375,  376,  377,  262,  375,  264,  265,  266,
  267,  268,  269,   -1,   -1,   -1,  273,  274,  275,  276,
   -1,  278,  279,   -1,  281,  282,   10,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  292,  293,   -1,  295,  296,
  297,  298,  299,   -1,   -1,  302,  303,  304,   10,   33,
   -1,  308,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,  326,
   -1,   33,   -1,   -1,   -1,   -1,   -1,  278,  279,   -1,
  281,  282,   -1,  340,   -1,   -1,   -1,  288,  289,  290,
   -1,  292,   -1,  350,   -1,   -1,  353,  298,  299,   -1,
   -1,  302,  303,  304,  305,   -1,   -1,  308,   -1,   -1,
  311,  368,  369,   -1,   -1,   -1,   -1,   -1,   -1,  376,
  377,   -1,   -1,  324,   -1,  326,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  335,  336,  337,  338,  339,  340,
   -1,   -1,   -1,   -1,   -1,   -1,   10,   -1,   -1,  350,
   -1,   -1,  353,   -1,   -1,  356,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  262,  368,  369,   33,
   -1,  267,  268,  269,  375,  376,  377,  273,  274,  275,
  276,   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  292,   -1,   -1,   -1,
   -1,   10,  298,  299,   -1,   -1,  302,  303,  304,   10,
   -1,   -1,  308,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,
  326,   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,  335,
  336,  337,  338,  339,  340,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  350,   -1,   -1,  353,   -1,  123,
  356,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  368,  369,   -1,   -1,   -1,   -1,  262,   -1,
  376,  377,   -1,  267,  268,  269,   -1,   -1,   -1,  273,
  274,  275,  276,   -1,  278,  279,   -1,  281,  282,   -1,
  262,   -1,   -1,   -1,   -1,   -1,  268,   -1,  292,   -1,
   -1,   -1,   -1,   -1,  298,  299,   -1,   -1,  302,  303,
  304,   10,   -1,   -1,  308,   -1,   -1,  289,  290,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  302,   -1,  326,  305,   33,   -1,   -1,   -1,   -1,  311,
   -1,  335,  336,  337,  338,  339,  340,   -1,   -1,   -1,
   -1,   -1,  324,   -1,   -1,   -1,  350,   -1,   -1,  353,
   -1,   -1,  356,   -1,   -1,   -1,   -1,   10,   -1,   -1,
   -1,   -1,   -1,   -1,  368,  369,   -1,   -1,   -1,   -1,
   -1,  353,  376,  377,   -1,   -1,   -1,  261,  262,  263,
   33,  265,  266,  267,  268,  269,  368,   -1,   -1,  273,
  274,  275,  276,  375,  278,  279,   -1,  281,  282,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  292,   -1,
   -1,   -1,   -1,   10,  298,  299,   -1,   -1,  302,  303,
  304,   -1,   -1,   -1,  308,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  262,   -1,   -1,   33,   -1,   -1,  268,
   -1,  262,  326,   -1,   -1,  266,  267,  268,  269,   -1,
   -1,   -1,  273,  274,  275,  276,  340,  278,  279,  288,
  281,  282,   -1,   -1,   -1,   -1,  350,   -1,   -1,  353,
  123,  292,  293,   -1,  295,  296,  297,  298,  299,   -1,
   -1,  302,  303,  304,  368,  369,   -1,  308,   -1,   -1,
   -1,   -1,  376,  377,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  326,  335,  336,  337,  338,
  339,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  340,
   -1,   -1,   -1,   -1,  353,   -1,  123,  356,   -1,  350,
   -1,   -1,  353,   -1,   10,   -1,   -1,   -1,   10,  368,
  369,   -1,   -1,   -1,   -1,   -1,   -1,  368,  369,   -1,
   -1,   -1,   -1,  262,   -1,  376,  377,   33,  267,  268,
  269,   33,   -1,   -1,  273,  274,  275,  276,   -1,  278,
  279,   -1,  281,  282,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  292,  293,   -1,  295,  296,  297,  298,
  299,   -1,   -1,  302,  303,  304,   -1,   -1,   -1,  308,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  262,
   -1,   -1,  265,  266,  267,  268,  269,  326,   -1,   -1,
  273,  274,  275,  276,   -1,  278,  279,   -1,  281,  282,
   -1,  340,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  292,
   -1,  350,   -1,   -1,  353,  298,  299,  123,   -1,  302,
  303,  304,   10,   -1,   -1,  308,   -1,   -1,   -1,  368,
  369,   -1,   -1,   -1,   -1,  262,   -1,  376,  377,  266,
  267,  268,  269,  326,   -1,   33,  273,  274,  275,  276,
   -1,  278,  279,   -1,  281,  282,   -1,  340,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  292,   -1,  350,   -1,   -1,
  353,  298,  299,   -1,   -1,  302,  303,  304,   10,   -1,
   -1,  308,   -1,   -1,   -1,  368,  369,   -1,   -1,   -1,
   -1,   -1,   -1,  376,  377,   -1,   -1,   -1,   -1,  326,
   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  340,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  350,   -1,   -1,  353,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  368,  369,   -1,   -1,   -1,   -1,   -1,   -1,  376,
  377,   -1,   -1,   -1,   -1,   -1,  262,   -1,   -1,   -1,
  262,  267,  268,  269,   -1,  267,  268,  273,  274,  275,
  276,  273,  278,  279,  276,  281,  282,   -1,   -1,   -1,
   -1,   -1,   -1,   10,   -1,   -1,  292,  289,  290,   -1,
   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,   -1,
  302,   -1,  308,  305,   -1,   -1,   33,   -1,   -1,  311,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  326,   -1,  324,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  340,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  350,   -1,   10,  353,   -1,   -1,
   -1,  353,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  368,  369,   -1,   -1,  368,   -1,   -1,   33,
  376,  377,   -1,  375,  262,   -1,   -1,   -1,   -1,  267,
  268,  269,   -1,   -1,   -1,  273,  274,  275,  276,   -1,
  278,  279,   -1,  281,  282,   -1,  123,   -1,   -1,   10,
   -1,   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,   -1,
  298,  299,   -1,   -1,  302,  303,  304,   -1,   -1,   -1,
  308,   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  262,   -1,   -1,   -1,   -1,  267,  268,  269,  326,   -1,
   -1,  273,  274,  275,  276,   -1,  278,  279,   -1,  281,
  282,   -1,  340,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  292,   -1,  350,   -1,   -1,  353,  298,  299,   -1,   -1,
  302,  303,  304,   -1,   -1,   -1,  308,   -1,   -1,   10,
  368,  369,   -1,   -1,   -1,   -1,   -1,   -1,  376,  377,
   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,  340,   -1,
   -1,   -1,  123,   -1,   -1,   -1,   -1,   -1,  350,   -1,
   -1,  353,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  262,  368,  369,   -1,   -1,
  267,  268,  269,   -1,  376,  377,  273,   -1,   -1,  276,
   -1,  278,  279,   -1,  281,  282,   -1,   -1,   -1,   10,
   -1,   -1,   -1,   -1,   -1,  292,   -1,   -1,   -1,   -1,
   -1,  298,  299,   -1,   -1,  302,  303,  304,   -1,   -1,
   -1,  308,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  262,  326,
   -1,   -1,   -1,  267,  268,  269,   -1,   -1,   -1,  273,
   -1,   -1,  276,  340,  278,  279,   -1,  281,  282,   -1,
   -1,   -1,   10,  350,   -1,   -1,  353,   -1,  292,   -1,
   -1,   -1,   -1,   -1,  298,  299,   -1,   -1,  302,  303,
  304,  368,  369,   -1,  308,   33,   -1,   -1,   -1,  376,
  377,  262,   -1,   -1,   -1,   -1,  267,  268,  269,   -1,
   -1,   -1,  326,   -1,   -1,  276,   -1,  278,  279,   -1,
  281,  282,  123,   -1,   -1,   10,  340,   -1,   -1,   -1,
   -1,  292,   -1,   -1,   -1,   -1,  350,  298,  299,  353,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,   33,   -1,
   -1,   -1,   -1,   -1,  368,  369,   -1,   -1,   -1,   -1,
   -1,   -1,  376,  377,   -1,  326,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  340,
   -1,  262,   -1,   -1,   -1,   -1,  267,  268,  269,  350,
   -1,   -1,  353,   -1,   -1,  276,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,   10,   -1,  368,  369,   -1,
   -1,  292,   -1,   -1,   -1,  376,  377,  298,  299,   -1,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,   33,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,   -1,  326,   -1,   -1,   10,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  340,
   -1,  262,   -1,   -1,   -1,   -1,   -1,  268,  269,  350,
   -1,   33,  353,   -1,   -1,   -1,   -1,  278,  279,   -1,
  281,  282,   -1,   -1,   -1,   10,   -1,  368,  369,   -1,
   -1,  292,   -1,   -1,   -1,  376,  377,  298,  299,   -1,
   -1,  302,  303,  304,   -1,   -1,   -1,  308,   33,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,  262,  326,   -1,   -1,   -1,   -1,
  268,  269,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  340,
  278,  279,   -1,  281,  282,   -1,   10,   -1,   10,  350,
   -1,   -1,  353,   -1,  292,   -1,   -1,   -1,   -1,   -1,
  298,  299,   -1,   -1,  302,  303,  304,  368,  369,   33,
  308,   33,   -1,   -1,   -1,  376,  377,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,  326,   -1,
   -1,   -1,   -1,  278,  279,   -1,  281,  282,  123,   -1,
   -1,   -1,  340,   10,   -1,   -1,   -1,  292,   -1,   -1,
   -1,   -1,  350,  298,  299,  353,   -1,  302,  303,  304,
   -1,   -1,   -1,  308,   -1,   -1,   33,   -1,   -1,   -1,
  368,  369,   -1,   -1,   -1,   -1,   -1,   -1,  376,  377,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  340,   -1,   -1,   -1,   10,
   -1,   -1,   -1,   -1,  269,  350,   -1,   -1,  353,   -1,
   -1,   -1,   -1,  278,  279,   -1,  281,  282,   -1,   -1,
   -1,   10,   33,  368,  369,   -1,   -1,  292,   -1,   -1,
   -1,  376,  377,  298,  299,   -1,   -1,  302,  303,  304,
  262,   -1,   -1,  308,   33,  267,  268,   -1,   -1,   -1,
   -1,  273,  274,  275,  276,   -1,   10,   -1,   -1,   -1,
   -1,  326,   -1,   -1,   -1,   -1,   -1,  289,  290,   -1,
   -1,   -1,   -1,   -1,   -1,  340,   -1,   -1,   -1,   33,
  302,   -1,   -1,  305,  269,  350,   -1,   -1,  353,  311,
   -1,   -1,   -1,  278,  279,   -1,  281,  282,   -1,   -1,
   -1,   -1,  324,  368,  369,   -1,   -1,  292,   -1,   -1,
   -1,  376,  377,  298,  299,   -1,   -1,  302,  303,  304,
   -1,   -1,   -1,  308,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  353,   -1,   -1,   -1,   -1,   -1,   -1,  262,   -1,
   -1,  326,  266,  267,  268,   -1,  368,  269,   -1,  273,
  274,  275,  276,  375,   -1,  340,  278,  279,   33,  281,
  282,   -1,   -1,   -1,  288,  350,   -1,   -1,  353,   -1,
  292,   -1,   -1,   -1,   -1,   -1,  298,  299,   -1,   -1,
  302,  303,  304,  368,  369,   -1,  308,   -1,   -1,   -1,
   -1,  376,  377,   -1,   -1,  262,   -1,   -1,   -1,  266,
  267,  268,   -1,   -1,  326,   -1,  273,  274,  275,  276,
   -1,  335,  336,  337,  338,  339,   -1,   -1,  340,   -1,
   -1,  288,   -1,   -1,   -1,   -1,   -1,   -1,  350,  353,
   -1,  353,  356,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  368,  369,  368,  369,   -1,   -1,
   -1,  262,   -1,   -1,  376,  377,  267,  268,   -1,   -1,
   -1,   -1,  273,  274,  275,  276,   -1,   -1,  335,  336,
  337,  338,  339,  262,   -1,   -1,   -1,  288,  267,  268,
   -1,   -1,   -1,   -1,  273,   -1,  353,  276,   -1,  356,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  288,
   -1,  368,  369,   -1,   -1,   -1,   -1,   -1,  262,   -1,
   -1,   -1,   -1,  267,  268,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  276,   -1,  335,  336,  337,  338,  339,   -1,
   -1,   -1,   -1,   -1,  288,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  353,   -1,   -1,  356,  335,  336,  337,  338,
  339,   -1,   -1,   -1,   -1,   -1,   -1,  368,  369,   -1,
   -1,   -1,   -1,   -1,  353,   -1,   -1,  356,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  368,
  369,  335,  336,  337,  338,  339,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  269,   -1,   -1,   -1,   -1,  353,
   -1,   -1,  356,  278,  279,   -1,  281,  282,   -1,   -1,
   -1,   -1,   -1,   -1,  368,  369,   -1,  292,   -1,   -1,
   -1,   -1,   -1,  298,  299,   -1,   -1,  302,  303,  304,
   -1,   -1,   -1,  308,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  340,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  350,   -1,   -1,  353,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  369,   -1,   -1,   -1,   -1,   -1,
   -1,  376,  377,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 380
#define YYUNDFTOKEN 539
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
"BLOCKPOLICY","RANDOMID","REQUIREORDER","SYNPROXY","FINGERPRINTS","NOSYNC",
"DEBUG","SKIP","HOSTID","ANTISPOOF","FOR","INCLUDE","BITMASK","RANDOM",
"SOURCEHASH","ROUNDROBIN","STATICPORT","PROBABILITY","ALTQ","CBQ","PRIQ","HFSC",
"BANDWIDTH","TBRSIZE","LINKSHARE","REALTIME","UPPERLIMIT","QUEUE","PRIORITY",
"QLIMIT","RTABLE","LOAD","RULESET_OPTIMIZATION","STICKYADDRESS","MAXSRCSTATES",
"MAXSRCNODES","SOURCETRACK","GLOBAL","RULE","MAXSRCCONN","MAXSRCCONNRATE",
"OVERLOAD","FLUSH","SLOPPY","PFLOW","TAGGED","TAG","IFBOUND","FLOATING",
"STATEPOLICY","STATEDEFAULTS","ROUTE","SETTOS","DIVERTTO","DIVERTREPLY",
"STRING","NUMBER","PORTBINARY",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
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
"qassign :",
"qassign : qassign_item",
"qassign : '{' optnl qassign_list '}'",
"qassign_list : qassign_item optnl",
"qassign_list : qassign_list comma qassign_item optnl",
"qassign_item : STRING",
"pfrule : action dir logquick interface route af proto fromto filter_opts",
"$$7 :",
"filter_opts : $$7 filter_opts_l",
"filter_opts :",
"filter_opts_l : filter_opts_l filter_opt",
"filter_opts_l : filter_opt",
"filter_opt : USER uids",
"filter_opt : GROUP gids",
"filter_opt : flags",
"filter_opt : icmpspec",
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
"host : ROUTE STRING",
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
"state_opt_item : PFLOW",
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
"$$8 :",
"pool_opts : $$8 pool_opts_l",
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
"binatrule : no BINAT natpasslog interface af proto FROM host toipspec tag tagged rtable redirection",
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
#line 4397 "../../freebsd/contrib/pf/pfctl/parse.y"

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
		{ "cbq",		CBQ},
		{ "code",		CODE},
		{ "crop",		FRAGCROP},
		{ "debug",		DEBUG},
		{ "divert-reply",	DIVERTREPLY},
		{ "divert-to",		DIVERTTO},
		{ "drop",		DROP},
		{ "drop-ovl",		FRAGDROP},
		{ "dup-to",		DUPTO},
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
		{ "hostid",		HOSTID},
		{ "icmp-type",		ICMPTYPE},
		{ "icmp6-type",		ICMP6TYPE},
		{ "if-bound",		IFBOUND},
		{ "in",			IN},
		{ "include",		INCLUDE},
		{ "inet",		INET},
		{ "inet6",		INET6},
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
		{ "pflow",		PFLOW},
		{ "port",		PORT},
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

#ifndef __rtems__
char	*parsebuf;
int	 parseindex;
char	 pushback_buffer[MAXPUSHBACK];
int	 pushback_index = 0;
#else /* __rtems__ */
static char	*parsebuf;
static int	 parseindex;
static char	 pushback_buffer[MAXPUSHBACK];
static int	 pushback_index = 0;
#endif /* __rtems__ */

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
	 * in the grammer.
	 */
	return (fibs - 1);
#else
	return (RT_TABLEID_MAX);
#endif
}
#ifdef __rtems__
#include "parse-data.h"
#endif /* __rtems__ */
#line 4226 "pfctly.tab.c"

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
case 17:
#line 578 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ file->errors++; }
break;
case 18:
#line 581 "../../freebsd/contrib/pf/pfctl/parse.y"
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
#line 608 "../../freebsd/contrib/pf/pfctl/parse.y"
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
#line 622 "../../freebsd/contrib/pf/pfctl/parse.y"
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
#line 634 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!(pf->opts & PF_OPT_OPTIMIZE)) {
				pf->opts |= PF_OPT_OPTIMIZE;
				pf->optimize = yystack.l_mark[0].v.i;
			}
		}
break;
case 32:
#line 644 "../../freebsd/contrib/pf/pfctl/parse.y"
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
#line 656 "../../freebsd/contrib/pf/pfctl/parse.y"
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
#line 666 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_DROP;
		}
break;
case 35:
#line 673 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_RETURN;
		}
break;
case 36:
#line 680 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set require-order %s\n",
				    yystack.l_mark[0].v.number == 1 ? "yes" : "no");
			require_order = yystack.l_mark[0].v.number;
		}
break;
case 37:
#line 686 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 38:
#line 704 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 39:
#line 716 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 40:
#line 728 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (expand_skip_interface(yystack.l_mark[0].v.interface) != 0) {
				yyerror("error setting skip interface(s)");
				YYERROR;
			}
		}
break;
case 41:
#line 734 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (keep_state_defaults != NULL) {
				yyerror("cannot redefine state-defaults");
				YYERROR;
			}
			keep_state_defaults = yystack.l_mark[0].v.state_opt;
		}
break;
case 42:
#line 743 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 43:
#line 744 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if ((yyval.v.string = strdup("all")) == NULL) {
				err(1, "stringall: strdup");
			}
		}
break;
case 44:
#line 751 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 46:
#line 760 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 48:
#line 769 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			char	*s;
			if (asprintf(&s, "%lld", (long long)yystack.l_mark[0].v.number) == -1) {
				yyerror("string: asprintf");
				YYERROR;
			}
			yyval.v.string = s;
		}
break;
case 50:
#line 780 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("%s = \"%s\"\n", yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string);
			if (symset(yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string, 0) == -1)
				err(1, "cannot store variable %s", yystack.l_mark[-2].v.string);
			free(yystack.l_mark[-2].v.string);
			free(yystack.l_mark[0].v.string);
		}
break;
case 51:
#line 790 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 52:
#line 791 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.string = NULL; }
break;
case 57:
#line 801 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 58:
#line 818 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			pf->alast = pf->anchor;
			pf->asd--;
			pf->anchor = pf->astack[pf->asd];
		}
break;
case 60:
#line 828 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 61:
#line 949 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 62:
#line 970 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 63:
#line 1012 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 64:
#line 1049 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 65:
#line 1080 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = yyval.v.b.w = 0;
			if (yystack.l_mark[-1].v.i)
				yyval.v.b.b1 = PF_NOSCRUB;
			else
				yyval.v.b.b1 = PF_SCRUB;
		}
break;
case 66:
#line 1090 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 67:
#line 1147 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
				bzero(&scrub_opts, sizeof scrub_opts);
				scrub_opts.rtableid = -1;
			}
break;
case 68:
#line 1152 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.scrub_opts = scrub_opts; }
break;
case 69:
#line 1153 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&scrub_opts, sizeof scrub_opts);
			scrub_opts.rtableid = -1;
			yyval.v.scrub_opts = scrub_opts;
		}
break;
case 72:
#line 1164 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (scrub_opts.nodf) {
				yyerror("no-df cannot be respecified");
				YYERROR;
			}
			scrub_opts.nodf = 1;
		}
break;
case 73:
#line 1171 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 74:
#line 1183 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 75:
#line 1195 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (scrub_opts.marker & SOM_SETTOS) {
				yyerror("set-tos cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= SOM_SETTOS;
			scrub_opts.settos = yystack.l_mark[0].v.number;
		}
break;
case 76:
#line 1203 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (scrub_opts.marker & SOM_FRAGCACHE) {
				yyerror("fragcache cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= SOM_FRAGCACHE;
			scrub_opts.fragcache = yystack.l_mark[0].v.i;
		}
break;
case 77:
#line 1211 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 78:
#line 1225 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (scrub_opts.randomid) {
				yyerror("random-id cannot be respecified");
				YYERROR;
			}
			scrub_opts.randomid = 1;
		}
break;
case 79:
#line 1232 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			scrub_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 80:
#line 1239 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			scrub_opts.match_tag = yystack.l_mark[0].v.string;
			scrub_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
break;
case 81:
#line 1245 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = 0; /* default */ }
break;
case 82:
#line 1246 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PFRULE_FRAGCROP; }
break;
case 83:
#line 1247 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PFRULE_FRAGDROP; }
break;
case 84:
#line 1250 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 85:
#line 1337 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 86:
#line 1338 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 87:
#line 1341 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 88:
#line 1342 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
break;
case 89:
#line 1349 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 90:
#line 1350 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-1].v.interface->dynamic = 1;
			yyval.v.interface = yystack.l_mark[-1].v.interface;
		}
break;
case 91:
#line 1356 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
				bzero(&antispoof_opts, sizeof antispoof_opts);
				antispoof_opts.rtableid = -1;
			}
break;
case 92:
#line 1361 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.antispoof_opts = antispoof_opts; }
break;
case 93:
#line 1362 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&antispoof_opts, sizeof antispoof_opts);
			antispoof_opts.rtableid = -1;
			yyval.v.antispoof_opts = antispoof_opts;
		}
break;
case 96:
#line 1373 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (antispoof_opts.label) {
				yyerror("label cannot be redefined");
				YYERROR;
			}
			antispoof_opts.label = yystack.l_mark[0].v.string;
		}
break;
case 97:
#line 1380 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			antispoof_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 98:
#line 1389 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number = 1; }
break;
case 99:
#line 1390 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number = 0; }
break;
case 100:
#line 1393 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 101:
#line 1423 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
		}
break;
case 102:
#line 1428 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.table_opts = table_opts; }
break;
case 103:
#line 1430 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
			yyval.v.table_opts = table_opts;
		}
break;
case 106:
#line 1441 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 107:
#line 1455 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ table_opts.init_addr = 1; }
break;
case 108:
#line 1456 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 109:
#line 1496 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 110:
#line 1508 "../../freebsd/contrib/pf/pfctl/parse.y"
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
			if (yystack.l_mark[0].v.queue == NULL) {
				yyerror("no child queues specified");
				YYERROR;
			}
			if (expand_altq(&a, yystack.l_mark[-3].v.interface, yystack.l_mark[0].v.queue, yystack.l_mark[-2].v.queue_opts.queue_bwspec,
			    &yystack.l_mark[-2].v.queue_opts.scheduler))
				YYERROR;
		}
break;
case 111:
#line 1532 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 112:
#line 1569 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
		}
break;
case 113:
#line 1577 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.queue_opts = queue_opts; }
break;
case 114:
#line 1578 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
			yyval.v.queue_opts = queue_opts;
		}
break;
case 117:
#line 1592 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (queue_opts.marker & QOM_BWSPEC) {
				yyerror("bandwidth cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_BWSPEC;
			queue_opts.queue_bwspec = yystack.l_mark[0].v.queue_bwspec;
		}
break;
case 118:
#line 1600 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 119:
#line 1612 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 120:
#line 1624 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (queue_opts.marker & QOM_SCHEDULER) {
				yyerror("scheduler cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_SCHEDULER;
			queue_opts.scheduler = yystack.l_mark[0].v.queue_options;
		}
break;
case 121:
#line 1632 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (queue_opts.marker & QOM_TBRSIZE) {
				yyerror("tbrsize cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65535) {
				yyerror("tbrsize too big: max 65535");
				YYERROR;
			}
			queue_opts.marker |= QOM_TBRSIZE;
			queue_opts.tbrsize = yystack.l_mark[0].v.number;
		}
break;
case 122:
#line 1646 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			double	 bps;
			char	*cp;

			yyval.v.queue_bwspec.bw_percent = 0;

			bps = strtod(yystack.l_mark[0].v.string, &cp);
			if (cp != NULL) {
				if (!strcmp(cp, "b"))
					; /* nothing */
				else if (!strcmp(cp, "Kb"))
					bps *= 1000;
				else if (!strcmp(cp, "Mb"))
					bps *= 1000 * 1000;
				else if (!strcmp(cp, "Gb"))
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
			yyval.v.queue_bwspec.bw_absolute = (u_int32_t)bps;
		}
break;
case 123:
#line 1680 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > UINT_MAX) {
				yyerror("bandwidth number too big");
				YYERROR;
			}
			yyval.v.queue_bwspec.bw_percent = 0;
			yyval.v.queue_bwspec.bw_absolute = yystack.l_mark[0].v.number;
		}
break;
case 124:
#line 1690 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = 0;
		}
break;
case 125:
#line 1694 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = yystack.l_mark[-1].v.number;
		}
break;
case 126:
#line 1698 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = 0;
		}
break;
case 127:
#line 1702 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = yystack.l_mark[-1].v.number;
		}
break;
case 128:
#line 1706 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			bzero(&yyval.v.queue_options.data.hfsc_opts,
			    sizeof(struct node_hfsc_opts));
		}
break;
case 129:
#line 1711 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			yyval.v.queue_options.data.hfsc_opts = yystack.l_mark[-1].v.hfsc_opts;
		}
break;
case 130:
#line 1717 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 131:
#line 1718 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 132:
#line 1721 "../../freebsd/contrib/pf/pfctl/parse.y"
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
			else {
				yyerror("unknown cbq flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 133:
#line 1741 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 134:
#line 1742 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
break;
case 135:
#line 1745 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				yyval.v.number = PRCF_DEFAULTCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				yyval.v.number = PRCF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				yyval.v.number = PRCF_RED|PRCF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				yyval.v.number = PRCF_RIO;
			else {
				yyerror("unknown priq flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 136:
#line 1763 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
				bzero(&hfsc_opts,
				    sizeof(struct node_hfsc_opts));
			}
break;
case 137:
#line 1767 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.hfsc_opts = hfsc_opts;
		}
break;
case 140:
#line 1776 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (hfsc_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			hfsc_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.linkshare.used = 1;
		}
break;
case 141:
#line 1785 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 142:
#line 1799 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (hfsc_opts.realtime.used) {
				yyerror("realtime already specified");
				YYERROR;
			}
			hfsc_opts.realtime.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.realtime.used = 1;
		}
break;
case 143:
#line 1808 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 144:
#line 1822 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (hfsc_opts.upperlimit.used) {
				yyerror("upperlimit already specified");
				YYERROR;
			}
			hfsc_opts.upperlimit.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.upperlimit.used = 1;
		}
break;
case 145:
#line 1831 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 146:
#line 1845 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "default"))
				hfsc_opts.flags |= HFCF_DEFAULTCLASS;
			else if (!strcmp(yystack.l_mark[0].v.string, "red"))
				hfsc_opts.flags |= HFCF_RED;
			else if (!strcmp(yystack.l_mark[0].v.string, "ecn"))
				hfsc_opts.flags |= HFCF_RED|HFCF_ECN;
			else if (!strcmp(yystack.l_mark[0].v.string, "rio"))
				hfsc_opts.flags |= HFCF_RIO;
			else {
				yyerror("unknown hfsc flag \"%s\"", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 147:
#line 1863 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.queue = NULL; }
break;
case 148:
#line 1864 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.queue = yystack.l_mark[0].v.queue; }
break;
case 149:
#line 1865 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
break;
case 150:
#line 1868 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
break;
case 151:
#line 1869 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.queue->tail->next = yystack.l_mark[-1].v.queue;
			yystack.l_mark[-3].v.queue->tail = yystack.l_mark[-1].v.queue;
			yyval.v.queue = yystack.l_mark[-3].v.queue;
		}
break;
case 152:
#line 1876 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 153:
#line 1896 "../../freebsd/contrib/pf/pfctl/parse.y"
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
				case PF_STATE_OPT_PFLOW:
					if (r.rule_flag & PFRULE_PFLOW) {
						yyerror("state pflow "
						    "option: multiple "
						    "definitions");
						YYERROR;
					}
					r.rule_flag |= PFRULE_PFLOW;
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
			if (r.rt && r.rt != PF_FASTROUTE) {
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
case 154:
#line 2317 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
				filter_opts.rtableid = -1;
			}
break;
case 155:
#line 2322 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.filter_opts = filter_opts; }
break;
case 156:
#line 2323 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			filter_opts.rtableid = -1;
			yyval.v.filter_opts = filter_opts;
		}
break;
case 159:
#line 2334 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.uid)
				yystack.l_mark[0].v.uid->tail->next = filter_opts.uid;
			filter_opts.uid = yystack.l_mark[0].v.uid;
		}
break;
case 160:
#line 2339 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.gid)
				yystack.l_mark[0].v.gid->tail->next = filter_opts.gid;
			filter_opts.gid = yystack.l_mark[0].v.gid;
		}
break;
case 161:
#line 2344 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 162:
#line 2355 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.marker & FOM_ICMP) {
				yyerror("icmp-type cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_ICMP;
			filter_opts.icmpspec = yystack.l_mark[0].v.icmp;
		}
break;
case 163:
#line 2363 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.marker & FOM_TOS) {
				yyerror("tos cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_TOS;
			filter_opts.tos = yystack.l_mark[0].v.number;
		}
break;
case 164:
#line 2371 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 165:
#line 2380 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			filter_opts.fragment = 1;
		}
break;
case 166:
#line 2383 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			filter_opts.allowopts = 1;
		}
break;
case 167:
#line 2386 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.label) {
				yyerror("label cannot be redefined");
				YYERROR;
			}
			filter_opts.label = yystack.l_mark[0].v.string;
		}
break;
case 168:
#line 2393 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
break;
case 169:
#line 2400 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
break;
case 170:
#line 2403 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
break;
case 171:
#line 2407 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 172:
#line 2419 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			filter_opts.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 173:
#line 2426 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 174:
#line 2435 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 175:
#line 2453 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
#ifdef __FreeBSD__
			yyerror("divert-reply has no meaning in FreeBSD pf(4)");
			YYERROR;
#else
			filter_opts.divert.port = 1;	/* some random value */
#endif
		}
break;
case 176:
#line 2463 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 177:
#line 2479 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.probability = (double)yystack.l_mark[0].v.number;
		}
break;
case 178:
#line 2485 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = PF_PASS; yyval.v.b.b2 = yyval.v.b.w = 0; }
break;
case 179:
#line 2486 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b = yystack.l_mark[0].v.b; yyval.v.b.b1 = PF_DROP; }
break;
case 180:
#line 2489 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = blockpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 181:
#line 2494 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_DROP;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
break;
case 182:
#line 2499 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
break;
case 183:
#line 2504 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 184:
#line 2513 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 185:
#line 2518 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 186:
#line 2523 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = returnicmpdefault;
		}
break;
case 187:
#line 2528 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
break;
case 188:
#line 2533 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-3].v.number;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
break;
case 189:
#line 2538 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURN;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
break;
case 190:
#line 2545 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 191:
#line 2552 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 192:
#line 2564 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET6))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 193:
#line 2571 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 194:
#line 2583 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_INOUT; }
break;
case 195:
#line 2584 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_IN; }
break;
case 196:
#line 2585 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OUT; }
break;
case 197:
#line 2588 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.quick = 0; }
break;
case 198:
#line 2589 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.quick = 1; }
break;
case 199:
#line 2592 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.log = 0; yyval.v.logquick.quick = 0; yyval.v.logquick.logif = 0; }
break;
case 200:
#line 2593 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 0; }
break;
case 201:
#line 2594 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.quick = 1; yyval.v.logquick.log = 0; yyval.v.logquick.logif = 0; }
break;
case 202:
#line 2595 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick = yystack.l_mark[-1].v.logquick; yyval.v.logquick.quick = 1; }
break;
case 203:
#line 2596 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 1; }
break;
case 204:
#line 2599 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.log = PF_LOG; yyval.v.logquick.logif = 0; }
break;
case 205:
#line 2600 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.logquick.log = PF_LOG | yystack.l_mark[-1].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[-1].v.logquick.logif;
		}
break;
case 206:
#line 2606 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; }
break;
case 207:
#line 2607 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.logquick.log = yystack.l_mark[-2].v.logquick.log | yystack.l_mark[0].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[0].v.logquick.logif;
			if (yyval.v.logquick.logif == 0)
				yyval.v.logquick.logif = yystack.l_mark[-2].v.logquick.logif;
		}
break;
case 208:
#line 2615 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.log = PF_LOG_ALL; yyval.v.logquick.logif = 0; }
break;
case 209:
#line 2616 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
break;
case 210:
#line 2617 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
break;
case 211:
#line 2618 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 212:
#line 2639 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = NULL; }
break;
case 213:
#line 2640 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
break;
case 214:
#line 2641 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 215:
#line 2644 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
break;
case 216:
#line 2645 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
break;
case 217:
#line 2652 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; yyval.v.interface->not = yystack.l_mark[-1].v.number; }
break;
case 218:
#line 2655 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 219:
#line 2679 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = 0; }
break;
case 220:
#line 2680 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = AF_INET; }
break;
case 221:
#line 2681 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = AF_INET6; }
break;
case 222:
#line 2684 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.proto = NULL; }
break;
case 223:
#line 2685 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.proto = yystack.l_mark[0].v.proto; }
break;
case 224:
#line 2686 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
break;
case 225:
#line 2689 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
break;
case 226:
#line 2690 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.proto->tail->next = yystack.l_mark[-1].v.proto;
			yystack.l_mark[-3].v.proto->tail = yystack.l_mark[-1].v.proto;
			yyval.v.proto = yystack.l_mark[-3].v.proto;
		}
break;
case 227:
#line 2697 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 228:
#line 2714 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 229:
#line 2726 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
break;
case 230:
#line 2734 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.fromto.src.host = NULL;
			yyval.v.fromto.src.port = NULL;
			yyval.v.fromto.dst.host = NULL;
			yyval.v.fromto.dst.port = NULL;
			yyval.v.fromto.src_os = NULL;
		}
break;
case 231:
#line 2741 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.fromto.src = yystack.l_mark[-2].v.peer;
			yyval.v.fromto.src_os = yystack.l_mark[-1].v.os;
			yyval.v.fromto.dst = yystack.l_mark[0].v.peer;
		}
break;
case 232:
#line 2748 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.os = NULL; }
break;
case 233:
#line 2749 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.os = yystack.l_mark[0].v.os; }
break;
case 234:
#line 2750 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
break;
case 235:
#line 2753 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.os = calloc(1, sizeof(struct node_os));
			if (yyval.v.os == NULL)
				err(1, "os: calloc");
			yyval.v.os->os = yystack.l_mark[0].v.string;
			yyval.v.os->tail = yyval.v.os;
		}
break;
case 236:
#line 2762 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
break;
case 237:
#line 2763 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.os->tail->next = yystack.l_mark[-1].v.os;
			yystack.l_mark[-3].v.os->tail = yystack.l_mark[-1].v.os;
			yyval.v.os = yystack.l_mark[-3].v.os;
		}
break;
case 238:
#line 2770 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
break;
case 239:
#line 2774 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
break;
case 240:
#line 2779 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
break;
case 241:
#line 2783 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (disallow_urpf_failed(yystack.l_mark[0].v.peer.host, "\"urpf-failed\" is "
			    "not permitted in a destination address"))
				YYERROR;
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
break;
case 242:
#line 2791 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[0].v.host;
			yyval.v.peer.port = NULL;
		}
break;
case 243:
#line 2795 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[-2].v.host;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
break;
case 244:
#line 2799 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
break;
case 247:
#line 2809 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = NULL; }
break;
case 248:
#line 2810 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 249:
#line 2811 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 250:
#line 2814 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 251:
#line 2815 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = NULL; }
break;
case 252:
#line 2818 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 253:
#line 2819 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 254:
#line 2832 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			struct node_host	*n;

			for (n = yystack.l_mark[0].v.host; n != NULL; n = n->next)
				n->not = yystack.l_mark[-1].v.number;
			yyval.v.host = yystack.l_mark[0].v.host;
		}
break;
case 255:
#line 2839 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 256:
#line 2848 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 257:
#line 2859 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 258:
#line 2869 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 259:
#line 2902 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 260:
#line 2916 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 262:
#line 2935 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 263:
#line 2946 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 264:
#line 2964 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL) {
				free(yystack.l_mark[0].v.string);
				err(1, "host: calloc");
			}
			yyval.v.host->addr.type = PF_ADDR_RTLABEL;
			if (strlcpy(yyval.v.host->addr.v.rtlabelname, yystack.l_mark[0].v.string,
			    sizeof(yyval.v.host->addr.v.rtlabelname)) >=
			    sizeof(yyval.v.host->addr.v.rtlabelname)) {
				yyerror("route label too long, max %u chars",
				    sizeof(yyval.v.host->addr.v.rtlabelname) - 1);
				free(yystack.l_mark[0].v.string);
				free(yyval.v.host);
				YYERROR;
			}
			yyval.v.host->next = NULL;
			yyval.v.host->tail = yyval.v.host;
			free(yystack.l_mark[0].v.string);
		}
break;
case 266:
#line 2987 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 267:
#line 3000 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 268:
#line 3054 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.port = yystack.l_mark[0].v.port; }
break;
case 269:
#line 3055 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
break;
case 270:
#line 3058 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
break;
case 271:
#line 3059 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.port->tail->next = yystack.l_mark[-1].v.port;
			yystack.l_mark[-3].v.port->tail = yystack.l_mark[-1].v.port;
			yyval.v.port = yystack.l_mark[-3].v.port;
		}
break;
case 272:
#line 3066 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 273:
#line 3079 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 274:
#line 3094 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 275:
#line 3111 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, 0) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 276:
#line 3120 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 277:
#line 3129 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.uid = yystack.l_mark[0].v.uid; }
break;
case 278:
#line 3130 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
break;
case 279:
#line 3133 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
break;
case 280:
#line 3134 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.uid->tail->next = yystack.l_mark[-1].v.uid;
			yystack.l_mark[-3].v.uid->tail = yystack.l_mark[-1].v.uid;
			yyval.v.uid = yystack.l_mark[-3].v.uid;
		}
break;
case 281:
#line 3141 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 282:
#line 3151 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 283:
#line 3166 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 284:
#line 3183 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 285:
#line 3198 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= UID_MAX) {
				yyerror("illegal uid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
break;
case 286:
#line 3207 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.gid = yystack.l_mark[0].v.gid; }
break;
case 287:
#line 3208 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
break;
case 288:
#line 3211 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
break;
case 289:
#line 3212 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.gid->tail->next = yystack.l_mark[-1].v.gid;
			yystack.l_mark[-3].v.gid->tail = yystack.l_mark[-1].v.gid;
			yyval.v.gid = yystack.l_mark[-3].v.gid;
		}
break;
case 290:
#line 3219 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 291:
#line 3229 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 292:
#line 3244 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 293:
#line 3261 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 294:
#line 3276 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= GID_MAX) {
				yyerror("illegal gid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
break;
case 295:
#line 3285 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 296:
#line 3298 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = yystack.l_mark[-2].v.b.b1; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
break;
case 297:
#line 3299 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
break;
case 298:
#line 3300 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = 0; }
break;
case 299:
#line 3303 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
break;
case 300:
#line 3304 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 301:
#line 3305 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
break;
case 302:
#line 3306 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 303:
#line 3309 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 304:
#line 3310 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
break;
case 305:
#line 3317 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
break;
case 306:
#line 3318 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
break;
case 307:
#line 3325 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 308:
#line 3335 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 309:
#line 3354 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 310:
#line 3370 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 311:
#line 3380 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 312:
#line 3399 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 313:
#line 3415 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 314:
#line 3426 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
break;
case 315:
#line 3435 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 316:
#line 3447 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp6-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
break;
case 317:
#line 3456 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "lowdelay"))
				yyval.v.number = IPTOS_LOWDELAY;
			else if (!strcmp(yystack.l_mark[0].v.string, "throughput"))
				yyval.v.number = IPTOS_THROUGHPUT;
			else if (!strcmp(yystack.l_mark[0].v.string, "reliability"))
				yyval.v.number = IPTOS_RELIABILITY;
			else if (yystack.l_mark[0].v.string[0] == '0' && yystack.l_mark[0].v.string[1] == 'x')
				yyval.v.number = strtoul(yystack.l_mark[0].v.string, NULL, 16);
			else
				yyval.v.number = 0;		/* flag bad argument */
			if (!yyval.v.number || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.string);
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 318:
#line 3474 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.number = yystack.l_mark[0].v.number;
			if (!yyval.v.number || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.number);
				YYERROR;
			}
		}
break;
case 319:
#line 3483 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_SRCTRACK; }
break;
case 320:
#line 3484 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_SRCTRACK_GLOBAL; }
break;
case 321:
#line 3485 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_SRCTRACK_RULE; }
break;
case 322:
#line 3488 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.i = PFRULE_IFBOUND;
		}
break;
case 323:
#line 3491 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.i = 0;
		}
break;
case 324:
#line 3496 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.keep_state.action = 0;
			yyval.v.keep_state.options = NULL;
		}
break;
case 325:
#line 3500 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_NORMAL;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 326:
#line 3504 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_MODULATE;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 327:
#line 3508 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_SYNPROXY;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
break;
case 328:
#line 3514 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = 0; }
break;
case 329:
#line 3515 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_FLUSH; }
break;
case 330:
#line 3516 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.i = PF_FLUSH | PF_FLUSH_GLOBAL;
		}
break;
case 331:
#line 3521 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.state_opt = yystack.l_mark[-1].v.state_opt; }
break;
case 332:
#line 3522 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.state_opt = NULL; }
break;
case 333:
#line 3525 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.state_opt = yystack.l_mark[0].v.state_opt; }
break;
case 334:
#line 3526 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-2].v.state_opt->tail->next = yystack.l_mark[0].v.state_opt;
			yystack.l_mark[-2].v.state_opt->tail = yystack.l_mark[0].v.state_opt;
			yyval.v.state_opt = yystack.l_mark[-2].v.state_opt;
		}
break;
case 335:
#line 3533 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 336:
#line 3546 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_NOSYNC;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 337:
#line 3554 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 338:
#line 3567 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 339:
#line 3580 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 340:
#line 3595 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 341:
#line 3613 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 342:
#line 3626 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 343:
#line 3635 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 344:
#line 3644 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SLOPPY;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 345:
#line 3652 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_PFLOW;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
break;
case 346:
#line 3660 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 347:
#line 3692 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.string = yystack.l_mark[0].v.string;
		}
break;
case 348:
#line 3697 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
			yyval.v.qassign.pqname = NULL;
		}
break;
case 349:
#line 3701 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
			yyval.v.qassign.pqname = NULL;
		}
break;
case 350:
#line 3705 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-3].v.string;
			yyval.v.qassign.pqname = yystack.l_mark[-1].v.string;
		}
break;
case 351:
#line 3711 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = 0; }
break;
case 352:
#line 3712 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = 1; }
break;
case 353:
#line 3715 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE|PPORT_STAR) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
break;
case 354:
#line 3724 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 355:
#line 3725 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 356:
#line 3728 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 357:
#line 3729 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
			yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
			yyval.v.host = yystack.l_mark[-3].v.host;
		}
break;
case 358:
#line 3736 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.redirection = NULL; }
break;
case 359:
#line 3737 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
break;
case 360:
#line 3744 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
break;
case 361:
#line 3754 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 362:
#line 3764 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 363:
#line 3803 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ bzero(&pool_opts, sizeof pool_opts); }
break;
case 364:
#line 3805 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.pool_opts = pool_opts; }
break;
case 365:
#line 3806 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			bzero(&pool_opts, sizeof pool_opts);
			yyval.v.pool_opts = pool_opts;
		}
break;
case 368:
#line 3816 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type =  PF_POOL_BITMASK;
		}
break;
case 369:
#line 3823 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_RANDOM;
		}
break;
case 370:
#line 3830 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_SRCHASH;
			pool_opts.key = yystack.l_mark[0].v.hashkey;
		}
break;
case 371:
#line 3838 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_ROUNDROBIN;
		}
break;
case 372:
#line 3845 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (pool_opts.staticport) {
				yyerror("static-port cannot be redefined");
				YYERROR;
			}
			pool_opts.staticport = 1;
		}
break;
case 373:
#line 3852 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (filter_opts.marker & POM_STICKYADDRESS) {
				yyerror("sticky-address cannot be redefined");
				YYERROR;
			}
			pool_opts.marker |= POM_STICKYADDRESS;
			pool_opts.opts |= PF_POOL_STICKYADDR;
		}
break;
case 374:
#line 3862 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.redirection = NULL; }
break;
case 375:
#line 3863 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
break;
case 376:
#line 3870 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
break;
case 377:
#line 3879 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
break;
case 378:
#line 3880 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
break;
case 379:
#line 3881 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
break;
case 380:
#line 3882 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
break;
case 381:
#line 3885 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 382:
#line 3898 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 383:
#line 3915 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 384:
#line 4074 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 385:
#line 4239 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.string = NULL; }
break;
case 386:
#line 4240 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
break;
case 387:
#line 4243 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.tagged.neg = 0; yyval.v.tagged.name = NULL; }
break;
case 388:
#line 4244 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.tagged.neg = yystack.l_mark[-2].v.number; yyval.v.tagged.name = yystack.l_mark[0].v.string; }
break;
case 389:
#line 4247 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.rtableid = -1; }
break;
case 390:
#line 4248 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			yyval.v.rtableid = yystack.l_mark[0].v.number;
		}
break;
case 391:
#line 4257 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 392:
#line 4266 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.host = yystack.l_mark[-1].v.host;
			yyval.v.host->ifname = yystack.l_mark[-2].v.string;
		}
break;
case 393:
#line 4272 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 394:
#line 4273 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 395:
#line 4287 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
break;
case 396:
#line 4288 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
break;
case 397:
#line 4291 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
break;
case 398:
#line 4296 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.route.host = NULL;
			yyval.v.route.rt = PF_FASTROUTE;
			yyval.v.route.pool_opts = 0;
		}
break;
case 399:
#line 4301 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_ROUTETO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 400:
#line 4308 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_REPLYTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 401:
#line 4315 "../../freebsd/contrib/pf/pfctl/parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_DUPTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
break;
case 402:
#line 4325 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 405:
#line 4348 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 410:
#line 4374 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.number = 0; }
break;
case 411:
#line 4375 "../../freebsd/contrib/pf/pfctl/parse.y"
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
case 412:
#line 4388 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_EQ; }
break;
case 413:
#line 4389 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_NE; }
break;
case 414:
#line 4390 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_LE; }
break;
case 415:
#line 4391 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_LT; }
break;
case 416:
#line 4392 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_GE; }
break;
case 417:
#line 4393 "../../freebsd/contrib/pf/pfctl/parse.y"
	{ yyval.v.i = PF_OP_GT; }
break;
#line 8955 "pfctly.tab.c"
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
