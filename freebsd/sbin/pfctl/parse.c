/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
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
#include <machine/rtems-bsd-program.h>
#include <sys/limits.h>
#endif /* __rtems__ */

#include <sys/cdefs.h>
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

#include <assert.h>
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
	PFCTL_STATE_ETHER,
	PFCTL_STATE_SCRUB,
	PFCTL_STATE_QUEUE,
	PFCTL_STATE_NAT,
	PFCTL_STATE_FILTER
};

struct node_etherproto {
	u_int16_t		 proto;
	struct node_etherproto	*next;
	struct node_etherproto	*tail;
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
#define FOM_FLAGS	0x0001
#define FOM_ICMP	0x0002
#define FOM_TOS		0x0004
#define FOM_KEEP	0x0008
#define FOM_SRCTRACK	0x0010
#define FOM_MINTTL	0x0020
#define FOM_MAXMSS	0x0040
#define FOM_AFTO	0x0080 /* not yet implemmented */
#define FOM_SETTOS	0x0100
#define FOM_SCRUB_TCP	0x0200
#define FOM_SETPRIO	0x0400
#define FOM_ONCE	0x1000 /* not yet implemmented */
#define FOM_PRIO	0x2000
#define FOM_SETDELAY	0x4000
#define FOM_FRAGCACHE	0x8000 /* does not exist in OpenBSD */
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
	u_int32_t		 ridentifier;
	struct {
		int			 action;
		struct node_state_opt	*options;
	} keep;
	int			 fragment;
	int			 allowopts;
	char			*label[PF_RULE_MAX_LABEL_COUNT];
	int			 labelcount;
	struct node_qassign	 queues;
	char			*tag;
	char			*match_tag;
	u_int8_t		 match_tag_not;
	u_int16_t		 dnpipe;
	u_int16_t		 dnrpipe;
	u_int32_t		 free_flags;
	u_int			 rtableid;
	u_int8_t		 prio;
	u_int8_t		 set_prio[2];
	struct {
		struct node_host	*addr;
		u_int16_t		port;
	}			 divert;
	/* new-style scrub opts */
	int			 nodf;
	int			 minttl;
	int			 settos;
	int			 randomid;
	int			 max_mss;
} filter_opts;

static struct antispoof_opts {
	char			*label[PF_RULE_MAX_LABEL_COUNT];
	int			 labelcount;
	u_int32_t		 ridentifier;
	u_int			 rtableid;
} antispoof_opts;

static struct scrub_opts {
	int			 marker;
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
	struct pf_mape_portset	 mape;

} pool_opts;

static struct codel_opts	 codel_opts;
static struct node_hfsc_opts	 hfsc_opts;
static struct node_fairq_opts	 fairq_opts;
static struct node_state_opt	*keep_state_defaults = NULL;
static struct pfctl_watermarks	 syncookie_opts;

int		 disallow_table(struct node_host *, const char *);
int		 disallow_urpf_failed(struct node_host *, const char *);
int		 disallow_alias(struct node_host *, const char *);
int		 rule_consistent(struct pfctl_rule *, int);
int		 filter_consistent(struct pfctl_rule *, int);
int		 nat_consistent(struct pfctl_rule *);
int		 rdr_consistent(struct pfctl_rule *);
int		 process_tabledef(char *, struct table_opts *);
void		 expand_label_str(char *, size_t, const char *, const char *);
void		 expand_label_if(const char *, char *, size_t, const char *);
void		 expand_label_addr(const char *, char *, size_t, sa_family_t,
		    struct pf_rule_addr *);
void		 expand_label_port(const char *, char *, size_t,
		    struct pf_rule_addr *);
void		 expand_label_proto(const char *, char *, size_t, u_int8_t);
void		 expand_label_nr(const char *, char *, size_t,
		    struct pfctl_rule *);
void		 expand_eth_rule(struct pfctl_eth_rule *,
		    struct node_if *, struct node_etherproto *,
		    struct node_mac *, struct node_mac *,
		    struct node_host *, struct node_host *, const char *,
		    const char *);
void		 expand_rule(struct pfctl_rule *, struct node_if *,
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
int	 rule_label(struct pfctl_rule *, char *s[PF_RULE_MAX_LABEL_COUNT]);
int	 eth_rule_label(struct pfctl_eth_rule *, char *s[PF_RULE_MAX_LABEL_COUNT]);
int	 rt_tableid_max(void);

void	 mv_rules(struct pfctl_ruleset *, struct pfctl_ruleset *);
void	 mv_eth_rules(struct pfctl_eth_ruleset *, struct pfctl_eth_ruleset *);
void	 decide_address_family(struct node_host *, sa_family_t *);
void	 remove_invalid_hosts(struct node_host **, sa_family_t *);
int	 invalid_redirect(struct node_host *, sa_family_t);
u_int16_t parseicmpspec(char *, sa_family_t);
int	 kw_casecmp(const void *, const void *);
int	 map_tos(char *string, int *);
struct node_mac* node_mac_from_string(const char *);
struct node_mac* node_mac_from_string_masklen(const char *, int);
struct node_mac* node_mac_from_string_mask(const char *, const char *);

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
		struct node_etherproto	*etherproto;
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
			struct node_mac	*src;
			struct node_mac	*dst;
		}			 etherfromto;
		struct node_mac		*mac;
		struct {
			struct node_mac	*mac;
		} etheraddr;
		char			*bridge_to;
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
		struct pfctl_watermarks	*watermarks;
	} v;
	int lineno;
} YYSTYPE;

#define PPORT_RANGE	1
#define PPORT_STAR	2
int	parseport(char *, struct range *r, int);

#define DYNIF_MULTIADDR(addr) ((addr).type == PF_ADDR_DYNIFTL && \
	(!((addr).iflags & PFI_AFLAG_NOALIAS) ||		 \
	!isdigit((addr).v.ifname[strlen((addr).v.ifname)-1])))

#line 586 "parse.c"

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
#define MATCH 259
#define SCRUB 260
#define RETURN 261
#define IN 262
#define OS 263
#define OUT 264
#define LOG 265
#define QUICK 266
#define ON 267
#define FROM 268
#define TO 269
#define FLAGS 270
#define RETURNRST 271
#define RETURNICMP 272
#define RETURNICMP6 273
#define PROTO 274
#define INET 275
#define INET6 276
#define ALL 277
#define ANY 278
#define ICMPTYPE 279
#define ICMP6TYPE 280
#define CODE 281
#define KEEP 282
#define MODULATE 283
#define STATE 284
#define PORT 285
#define RDR 286
#define NAT 287
#define BINAT 288
#define ARROW 289
#define NODF 290
#define MINTTL 291
#define ERROR 292
#define ALLOWOPTS 293
#define FASTROUTE 294
#define FILENAME 295
#define ROUTETO 296
#define DUPTO 297
#define REPLYTO 298
#define NO 299
#define LABEL 300
#define NOROUTE 301
#define URPFFAILED 302
#define FRAGMENT 303
#define USER 304
#define GROUP 305
#define MAXMSS 306
#define MAXIMUM 307
#define TTL 308
#define TOS 309
#define DROP 310
#define TABLE 311
#define REASSEMBLE 312
#define ANCHOR 313
#define NATANCHOR 314
#define RDRANCHOR 315
#define BINATANCHOR 316
#define SET 317
#define OPTIMIZATION 318
#define TIMEOUT 319
#define LIMIT 320
#define LOGINTERFACE 321
#define BLOCKPOLICY 322
#define FAILPOLICY 323
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
#define KEEPCOUNTERS 335
#define SYNCOOKIES 336
#define L3 337
#define ETHER 338
#define BITMASK 339
#define RANDOM 340
#define SOURCEHASH 341
#define ROUNDROBIN 342
#define STATICPORT 343
#define PROBABILITY 344
#define MAPEPORTSET 345
#define ALTQ 346
#define CBQ 347
#define CODEL 348
#define PRIQ 349
#define HFSC 350
#define FAIRQ 351
#define BANDWIDTH 352
#define TBRSIZE 353
#define LINKSHARE 354
#define REALTIME 355
#define UPPERLIMIT 356
#define QUEUE 357
#define PRIORITY 358
#define QLIMIT 359
#define HOGS 360
#define BUCKETS 361
#define RTABLE 362
#define TARGET 363
#define INTERVAL 364
#define DNPIPE 365
#define DNQUEUE 366
#define RIDENTIFIER 367
#define LOAD 368
#define RULESET_OPTIMIZATION 369
#define PRIO 370
#define STICKYADDRESS 371
#define MAXSRCSTATES 372
#define MAXSRCNODES 373
#define SOURCETRACK 374
#define GLOBAL 375
#define RULE 376
#define MAXSRCCONN 377
#define MAXSRCCONNRATE 378
#define OVERLOAD 379
#define FLUSH 380
#define SLOPPY 381
#define TAGGED 382
#define TAG 383
#define IFBOUND 384
#define FLOATING 385
#define STATEPOLICY 386
#define STATEDEFAULTS 387
#define ROUTE 388
#define SETTOS 389
#define DIVERTTO 390
#define DIVERTREPLY 391
#define BRIDGE_TO 392
#define STRING 393
#define NUMBER 394
#define PORTBINARY 395
#define YYERRCODE 256
typedef int YYINT;
static const YYINT pfctlylhs[] = {                       -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  157,
  172,  172,  172,  172,  172,  172,   19,   13,   13,  158,
  158,  158,  158,  158,  158,  158,  158,  158,  158,  158,
  158,  158,  158,  158,  158,  158,  158,  158,  158,  158,
   20,  145,  178,  145,  179,  179,  181,   81,   81,   84,
   84,   85,   85,   86,   86,  169,   83,   83,  182,  182,
  182,  182,  184,  183,  183,  165,  165,  165,  165,  166,
   29,  159,  185,  185,  185,  185,  187,  186,  186,  160,
  188,  122,  122,  124,  124,  123,  123,  123,  123,  123,
  123,  123,  152,  152,  161,  189,  134,  134,  136,  136,
  135,  135,  135,  135,  135,  135,  135,  135,  135,   18,
   18,  170,   96,   96,   97,   97,   98,   98,  190,  128,
  128,  130,  130,  129,  129,  129,   11,   11,  171,  191,
  137,  137,  139,  139,  138,  138,  138,  138,  167,  168,
  192,  131,  131,  133,  133,  132,  132,  132,  132,  132,
  118,  118,  104,  104,  104,  104,  104,  104,  104,  104,
  104,  104,  105,  105,  106,  107,  107,  108,  193,  111,
  109,  109,  110,  110,  110,  110,  110,  110,  110,  194,
  114,  112,  112,  113,  113,  113,  113,  113,  195,  117,
  115,  115,  116,  116,  116,  101,  101,  101,  102,  102,
  103,  164,  196,  119,  119,  121,  121,  120,  120,  120,
  120,  120,  120,  120,  120,  120,  120,  120,  120,  120,
  120,  120,  120,  120,  120,  120,  120,  120,  120,  120,
  120,  120,  120,  120,  125,  125,  127,  127,  126,  126,
   33,   33,   14,   14,   26,   26,   26,   32,   32,   32,
   32,   32,   32,   32,   32,   32,   32,   47,   47,   48,
   48,   16,   16,   16,   92,   92,   91,   91,   91,   91,
   91,   93,   93,   94,   94,   95,   95,   95,   95,    1,
    1,    1,    2,    2,    3,    4,   17,   17,   17,  146,
  146,  146,  147,  147,  148,   25,   25,   38,   38,   38,
   39,   39,   40,   41,   41,   50,   50,  149,  149,  150,
  150,  151,  151,  154,  154,  153,  156,  156,  155,  155,
   49,   49,   65,   65,   65,   66,   67,   67,   52,   52,
   53,   53,   51,   51,   51,  174,  174,   54,   54,   54,
   55,   55,   59,   59,   56,   56,   56,   57,   57,   57,
   57,   57,   57,   57,    5,    5,   58,   68,   68,   69,
   69,   70,   70,   70,   34,   36,   71,   71,   72,   72,
   73,   73,   73,    8,    8,   74,   74,   75,   75,   76,
   76,   76,    9,    9,   31,   30,   30,   30,   42,   42,
   42,   42,   43,   43,   45,   45,   44,   44,   44,   46,
   46,   46,    6,    6,    7,    7,   10,   10,   21,   21,
   21,   24,   24,   87,   87,   87,   87,   22,   22,   22,
   88,   88,   89,   89,   90,   90,   90,   90,   90,   90,
   90,   90,   90,   90,   90,   80,  100,  100,   99,   99,
   99,   15,   15,   35,   61,   61,   60,   60,   79,   79,
   79,   37,   37,  197,  140,  140,  142,  142,  141,  141,
  141,  141,  141,  141,  141,   78,   78,   78,   28,   28,
   28,   28,   27,   27,  162,  163,   82,   82,  143,  143,
  144,  144,   62,   62,   63,   63,   64,   64,   77,   77,
   77,   77,   77,  173,  173,  175,  175,  176,  177,  177,
  180,  180,   12,   12,   23,   23,   23,   23,   23,   23,
};
static const YYINT pfctlylen[] = {                        2,
    0,    3,    2,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    4,    3,    2,
    2,    3,    3,    3,    3,    3,    1,    0,    1,    4,
    3,    3,    3,    6,    3,    6,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    2,    4,
    1,    0,    0,    4,    3,    1,    2,    1,    1,    2,
    1,    2,    1,    1,    1,    3,    1,    0,    0,    2,
    3,    3,    0,    5,    0,   10,    7,    7,    7,    5,
    2,   10,    0,    2,    3,    3,    0,    5,    0,   10,
    0,    2,    0,    2,    1,    1,    2,    1,    2,    3,
    2,    2,    0,    2,    8,    0,    2,    0,    3,    1,
    1,    2,    2,    2,    1,    2,    1,    2,    3,    2,
    3,    5,    2,    5,    2,    4,    1,    3,    0,    2,
    0,    2,    1,    1,    2,    2,    1,    0,    5,    0,
    2,    0,    2,    1,    1,    3,    4,    2,    5,    5,
    0,    2,    0,    2,    1,    2,    2,    2,    1,    2,
    1,    1,    1,    4,    1,    4,    1,    4,    1,    4,
    1,    4,    1,    3,    1,    1,    3,    1,    0,    2,
    1,    3,    2,    8,    2,    8,    2,    8,    1,    0,
    2,    1,    3,    2,    6,    2,    2,    1,    0,    2,
    1,    3,    2,    2,    1,    0,    1,    4,    2,    4,
    1,    9,    0,    2,    0,    2,    1,    2,    2,    1,
    1,    2,    2,    1,    2,    1,    1,    1,    1,    2,
    4,    6,    2,    6,    4,    2,    3,    2,    2,    2,
    4,    1,    4,    1,    4,    2,    3,    1,    1,    2,
    2,    6,    1,    1,    1,    1,    2,    0,    1,    1,
    5,    1,    1,    4,    4,    6,    1,    1,    1,    1,
    1,    0,    1,    1,    0,    1,    0,    1,    1,    2,
    2,    1,    4,    1,    3,    1,    1,    1,    2,    0,
    2,    5,    2,    4,    2,    1,    0,    1,    1,    0,
    2,    5,    2,    4,    1,    1,    1,    0,    2,    5,
    2,    4,    1,    1,    1,    0,    2,    1,    2,    0,
    2,    0,    2,    3,    1,    2,    1,    4,    2,    3,
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
    2,    1,    1,    1,    2,    2,    2,    4,    2,    4,
    6,    0,    1,    1,    1,    4,    2,    4,    0,    2,
    4,    0,    1,    0,    2,    0,    2,    1,    1,    1,
    2,    1,    1,    1,    6,    0,    2,    4,    0,    1,
    2,    1,    3,    3,   10,   13,    0,    2,    0,    3,
    0,    2,    1,    4,    2,    4,    1,    4,    0,    1,
    3,    3,    3,    2,    2,    4,    2,    2,    4,    2,
    1,    0,    1,    1,    1,    2,    2,    1,    2,    1,
};
static const YYINT pfctlydefred[] = {                     0,
    0,    0,    0,    0,  255,    0,  256,  453,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    3,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   21,    0,    0,    0,    0,    0,    0,   19,
  267,    0,    0,    0,  259,  257,    0,   67,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   49,    0,    0,    0,    0,
    0,    0,    0,    0,   20,    0,    0,    0,    0,    0,
    0,    0,   81,    0,    0,    0,  273,  274,    0,    0,
    0,    2,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,   17,   26,   18,   24,   23,
   25,   22,    0,    0,    0,    0,    0,   60,    0,    0,
    0,  513,  514,    0,   31,    0,    0,    0,   33,    0,
    0,   35,   59,   58,   37,   40,   39,   42,   41,   43,
   44,   46,   47,  366,  365,   38,   51,    0,   32,   27,
  422,  423,   45,    0,  436,    0,    0,    0,    0,    0,
    0,  444,    0,  442,  443,    0,  433,    0,  281,    0,
    0,  280,    0,    0,    0,  137,  291,    0,    0,    0,
    0,    0,   65,   64,   66,    0,    0,  484,  482,  483,
    0,    0,  298,  299,    0,    0,    0,  268,  269,    0,
  270,  271,    0,    0,  276,    0,    0,    0,    0,   29,
   30,  505,  504,    0,    0,  508,    0,   50,    0,  435,
  437,  441,  420,  421,  438,    0,    0,  445,  511,    0,
    0,  286,  287,  288,    0,  284,  296,    0,    0,  127,
  123,    0,    0,    0,    0,  295,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  159,  155,    0,    0,
    0,   62,  481,    0,    0,    0,    0,    0,    0,  264,
    0,  265,  139,    0,    0,    0,    0,    0,  346,    0,
    0,    0,    0,    0,    0,    0,  434,  289,  283,    0,
    0,    0,  122,    0,    0,    0,    0,    0,  211,    0,
  149,  207,    0,  199,    0,  179,  190,  161,  162,  156,
  160,  157,  158,  154,  150,   80,    0,  500,    0,    0,
    0,    0,  314,  315,    0,  309,  313,    0,  331,    0,
    0,    0,  261,    0,    0,  145,    0,  144,    0,    0,
    0,    0,    0,  507,   34,    0,  510,   36,    0,    0,
    0,   56,  439,    0,  285,    0,    0,  128,    0,    0,
    0,  134,  133,    0,    0,    0,    0,  292,    0,  293,
    0,  175,    0,  173,    0,    0,  178,    0,  176,    0,
    0,    0,    0,    0,  493,    0,    0,  497,    0,    0,
    0,    0,    0,  348,    0,    0,    0,  340,    0,  349,
    0,    0,    0,    0,    0,  266,  148,    0,  143,    0,
    0,   77,   78,   79,    0,    0,   57,   54,    0,    0,
  440,  124,    0,  125,  446,  136,  135,  132,    0,    0,
  104,    0,    0,    0,    0,  164,    0,  172,    0,    0,
  205,    0,  201,  166,    0,  168,    0,    0,    0,  189,
    0,  181,  170,    0,    0,    0,  198,    0,  192,    0,
    0,    0,  501,    0,  503,  502,    0,    0,    0,    0,
  515,    0,    0,    0,    0,    0,  345,  368,  376,    0,
  356,  357,    0,    0,    0,    0,  355,    0,    0,  488,
    0,    0,  336,    0,  334,    0,  332,    0,  146,    0,
    0,    0,  492,  506,  509,   55,  430,    0,  307,  306,
    0,  305,  301,    0,  318,    0,    0,    0,  294,  208,
    0,  209,  174,  204,  203,    0,  177,    0,  183,    0,
  185,    0,  187,    0,    0,  194,  196,  197,    0,    0,
    0,    0,    0,  469,  470,    0,  472,  473,    0,  474,
  468,    0,    0,  310,    0,  311,    0,  516,  517,  519,
  373,    0,    0,    0,    0,    0,    0,    0,    0,  344,
    0,    0,    0,  342,  105,    0,  353,  147,    0,    0,
    0,  126,    0,    0,    0,  327,  321,    0,    0,    0,
  319,    0,    0,  202,    0,    0,    0,  182,    0,  193,
    0,    0,  495,  498,    0,  494,  471,  463,    0,  467,
  212,    0,    0,    0,  374,  350,  360,  359,  361,  367,
  364,  363,  490,    0,    0,    0,    0,  111,    0,    0,
    0,    0,  117,    0,    0,    0,  115,  110,    0,    0,
   73,   76,    0,    0,    0,    0,    0,    0,  227,    0,
  226,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  242,    0,  220,  221,  228,
  224,  229,  217,    0,  244,    0,    0,    0,    0,  326,
  317,   87,   90,  323,   82,    0,  210,    0,    0,    0,
    0,  351,    0,    0,    0,  312,  369,    0,  370,    0,
  455,    0,  485,  337,  335,    0,  112,    0,  120,  113,
  116,  118,  417,  418,  114,    0,    0,  354,    0,    0,
  398,  395,    0,    0,  413,  414,    0,    0,  399,  415,
  416,    0,    0,  401,    0,    0,  424,  384,  385,    0,
    0,    0,  218,  377,  393,  394,    0,    0,    0,  219,
  386,  223,    0,    0,    0,  249,  246,    0,  253,  254,
  238,  449,    0,  239,    0,  230,    0,  233,  225,  222,
  236,    0,  240,  375,    0,  216,  302,    0,  303,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   98,
   96,   95,    0,    0,    0,    0,    0,    0,  496,    0,
    0,    0,    0,    0,  121,  119,  109,   69,    0,  397,
    0,    0,    0,    0,    0,    0,  425,  426,    0,    0,
  382,    0,    0,  391,  250,  251,    0,  248,    0,  427,
    0,    0,    0,    0,  237,    0,  329,  328,    0,  324,
   83,  447,    0,  101,  102,   97,   99,    0,   94,    0,
    0,    0,  195,    0,    0,  371,    0,    0,  461,  454,
  338,    0,  243,  396,    0,    0,  408,  409,    0,    0,
  411,  412,    0,    0,    0,  383,    0,    0,  392,    0,
  245,    0,  450,    0,  231,    0,  235,    0,  241,  304,
  330,    0,    0,  100,    0,    0,    0,    0,  486,  475,
  457,  456,    0,   70,   74,    0,    0,  400,    0,  403,
  402,    0,  405,  431,  378,    0,  379,  387,    0,  388,
    0,  247,    0,    0,    0,   84,   88,    0,    0,  448,
  184,  186,  188,    0,    0,   71,   72,    0,    0,    0,
    0,    0,  451,  232,  234,   85,   86,    0,  458,  404,
  406,  380,  389,  252,  478,
};
static const YYINT pfctlydgoto[] = {                      2,
   89,  307,  187,  250,  156,  738,  743,  751,  758,  725,
  407,  134,  221,  771,   23,   99,  205,  647,  159,  158,
  174,  431,  485,  175,  522,   24,   25,  198,   26,  678,
  734,   56,  766,  783,  869,  486,  617,  277,  478,  336,
  337,  679,  875,  739,  879,  744,  210,  213,  340,  599,
  408,  341,  507,  409,  612,  410,  497,  498,  511,  868,
  712,  398,  552,  399,  414,  505,  637,  487,  623,  488,
  753,  884,  754,  760,  887,  761,  332,  909,  635,  372,
  145,  412,   59,   61,  195,  489,  681,  827,  176,  177,
   83,  216,   84,  245,  246,  181,  366,  251,  682,  801,
  311,  444,  312,  267,  383,  384,  388,  389,  461,  462,
  390,  468,  469,  392,  452,  453,  385,  320,  590,  683,
  684,  695,  802,  803,  685,  767,  839,  303,  373,  374,
  189,  268,  269,  585,  648,  649,  283,  348,  349,  473,
  561,  562,  502,  422,  228,  440,  686,  523,  526,  527,
  601,  377,  596,  690,  791,  597,   27,   28,   29,   30,
   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
   41,    3,  139,  225,  291,  142,  293,  229,  361,  240,
  362,  872,  652,  729,  902,  693,  793,  696,  586,  304,
  284,  190,  391,  393,  386,  591,  474,
};
static const YYINT pfctlysindex[] = {                   -56,
    0,   24, 1954,   73,    0,  642,    0,    0,   33, -294,
 -284, -284, -284, 3396,    8, -164,  614,  -27, -114,   84,
  475,    0,  710,  471,  -27,  471,  563,  608,  663,  673,
  685,  706,  755,  778,  841,  867,  899,  934,  952,  954,
  982,  991,    0, 1000,  804, 1015, 1026, 1028, 1037,    0,
    0,  558,  610,  617,    0,    0,  315,    0,  471, -284,
  -27,  -27,  -27, -240,  388,  -59,  -68, -215, -196, -172,
 -240,  451,  557,  -27, -264,    0,  562, -284,  501, 3172,
 1012,  791,  769,  832,    0, -294,  471,  152,    0,  -27,
 -284,   71,    0,  104,  104,  104,    0,    0,    8,  616,
    8,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  817,  380,  581, 1055,  869,    0,  616,  616,
  616,    0,    0,  829,    0,  739,  766, 1139,    0,  768,
 1139,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  787,    0,  794,  812,  640,  827,  830,
 1115,    0,  835,    0,    0, 1154,    0,  474,    0,  -20,
  616,    0,  471,  869, 1139,    0,    0,  843,  880, 2453,
    0,  972,    0,    0,    0,   71,  791,    0,    0,    0,
  -27,  -27,    0,    0,  967,  -27,  849,    0,    0,  361,
    0,    0, 1201,    0,    0,  -27,  967,  967,  967,    0,
    0,    0,    0, 1139, -272,    0,  851,    0, 1208,    0,
    0,    0,    0,    0,    0, 1199,  859,    0,    0, 3172,
 -284,    0,    0,    0,  698,    0,    0, 1139,  843,    0,
    0,    0,  869,  -27, 1221,    0,  -66, 1215, 1220, 1224,
 1225, 1226,  669,  873,  874,  875,    0,    0, 2453,  -66,
 -284,    0,    0,  616,  848,  -62, -193,  616, 1229,    0,
  581,    0,    0,  -94,  616, -193, -193, -193,    0, 1139,
  179, 1139,  228,  878,  890, 1212,    0,    0,    0,  474,
   26, 1246,    0,  485,  -27,  896,  240, 1139,    0, 1139,
    0,    0,  897,    0,  898,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  967,    0,   57,   57,
   57,  616,    0,    0, 1139,    0,    0,   10,    0,  906,
 1032,  967,    0, 1253,  923,    0, 1139,    0,  -94,  967,
  937,  937,  937,    0,    0, -272,    0,    0,  851,  924,
  786,    0,    0,  938,    0,  405, 1139,    0,  931,  941,
 -264,    0,    0,  485, 1051,  939, 1051,    0, 1221,    0,
  943,    0,  861,    0, 1299,  579,    0,  905,    0, 1300,
  539, 1311, -108, 1083,    0, 1139,  960,    0,    0,    0,
    0,  967,  672,    0,   98, 1139,  109,    0, 1071,    0,
  964, 1221,  -51, 1092, -193,    0,    0,  526,    0, -193,
  970,    0,    0,    0, 1139, 1139,    0,    0,  878,  987,
    0,    0,   26,    0,    0,    0,    0,    0,  -46,  452,
    0,  452, 1139,  564, 1139,    0,  897,    0, -264, -264,
    0, 1154,    0,    0,  898,    0,   75,   77,   96,    0,
 1154,    0,    0,  100,  669, -264,    0, 1154,    0,   52,
   46,   41,    0,  940,    0,    0, -193,  571, 1139, 1139,
    0, 1304, 1305, 1306,   71,  979,    0,    0,    0,   52,
    0,    0,  795, 1330,  986,  996,    0, 1333,   98,    0,
 1008,  937,    0, 1139,    0,   10,    0,    0,    0, 1139,
  578,    0,    0,    0,    0,    0,    0, 1139,    0,    0,
 1139,    0,    0,  191,    0, 1062, 1131, 1062,    0,    0,
  943,    0,    0,    0,    0,  579,    0,  669,    0,  669,
    0,  669,    0,  539,  669,    0,    0,    0, -108, 1137,
 1139,  582, 1367,    0,    0, -284,    0,    0, -264,    0,
    0,  940,    0,    0,  672,    0,   63,    0,    0,    0,
    0,   71,  588, 1022, 1025, 1029, 1378, 1366, 1035,    0,
 -284, 1141, 1039,    0,    0, 1497,    0,    0,   52, 1313,
 5217,    0,  680, 1139, -284,    0,    0, -193, 1314,  191,
    0,    0, 1139,    0, 1154, 1154, 1154,    0, -264,    0,
   52,  906,    0,    0,   46,    0,    0,    0, 1387,    0,
    0, 1139,  602, 1139,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -10,    0, 1139,  603,    0, 1044,  186,
 1046, 1053,    0, 1050,  693, 1065,    0,    0, 1154, 1139,
    0,    0, 1409,  -21,  -44,  125, 1173, 1174,    0, 1180,
    0,  135,  203,  693,    1, 1181,  711,   80, 1073,  139,
  147, -264, 1074, -284,  715,    0, 1087,    0,    0,    0,
    0,    0,    0, 5217,    0,  611, 1139, 1221, 1423,    0,
    0,    0,    0,    0,    0,  834,    0, 1077, 1081, 1082,
  669,    0, 1221, 1139, -264,    0,    0,   63,    0, 1139,
    0, 1196,    0,    0,    0, 1039,    0, 1171,    0,    0,
    0,    0,    0,    0,    0, -284, 1497,    0, 1475,    0,
    0,    0, 1093, 1447,    0,    0, 1139, 1214,    0,    0,
    0, 1139, 1222,    0, 1461, 1461,    0,    0,    0, 1139,
 1107,  736,    0,    0,    0,    0, 1139, 1111,  771,    0,
    0,    0,  693,  -30,  -53,    0,    0, 1461,    0,    0,
    0,    0, 1114,    0, -264,    0, -264,    0,    0,    0,
    0, 1232,    0,    0, -284,    0,    0,  680,    0, 1139,
  643, 1124, 1509,   82, -264, -264, -264, -284, 1140,    0,
    0,    0,  834, 1154, 1154, 1154, 1480,  937,    0, 1476,
 1139,   41,   71, 1139,    0,    0,    0,    0, 1483,    0,
 1093,  776,  793,  796,  802, 3172,    0,    0,  132,  736,
    0,  173,  771,    0,    0,    0, 1133,    0,  932,    0,
  944,  945,  968,   71,    0, 1139,    0,    0, 1221,    0,
    0,    0, 1136,    0,    0,    0,    0, -284,    0,  669,
  669,  669,    0, 1242, -264,    0, 1139,  646,    0,    0,
    0, 2226,    0,    0,  647, 1139,    0,    0,  703, 1139,
    0,    0,  983,  725, 1139,    0,  792, 1139,    0, 1154,
    0,  -53,    0, 1144,    0, -264,    0, -264,    0,    0,
    0,   21, 1491,    0, 1492, 1493, 1495,   41,    0,    0,
    0,    0,   41,    0,    0, 1529, 1533,    0,  776,    0,
    0,  796,    0,    0,    0,  132,    0,    0,  173,    0,
 1150,    0, 1504, 1505, 1511,    0,    0, 1550, 1552,    0,
    0,    0,    0, 1281, 1139,    0,    0, 1139, 1139, 1139,
 1139, 1537,    0,    0,    0,    0,    0,   71,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT pfctlyrindex[] = {                   167,
    0,  824,  853,    0,    0, 1481,    0,    0,    0, 3259,
    0,    0,    0,    0, 1241,    0,    0, 2107,    0,    0,
    0,    0,    0, 2947, 5088, 3324,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, 1614, 1749, 1882,    0,    0,    0,    0, 3636, 2080,
 2041, 2041, 2041,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, 1565,    0,    0,    0,    0,    0,    0,
 2212, 2348,    0, 2466,    0, 2167, 1143, 1186, 2213, 1336,
    0,    0,    0, 3710, 3710,  622,    0,    0, 3384, 4272,
 4139,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, 3761,    0,  765,  765,
  765,    0,    0, 1571,    0,    0,    0,  389,    0,    0,
 1190,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   68,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  210,    0,    0,
    0,    0,    0,    0,    0,  949,    0,    0,    0,    0,
   66,    0, 1104, 1835,  -17,    0,    0,    0,    0,    0,
 1375,    0,    0,    0,    0, 1574, 5031,    0,    0,    0,
  783, 3510,    0,    0, 5122, 4779,    0,    0,    0,  809,
    0,    0,    0,   -4,    0, 3887,  731,  731,  731,    0,
    0,    0,    0, 1865,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  649,    0,    0,  111,    0,    0,
    0,  209, 1831, 1920, 1186,    0, 1576,  286,  956, 1103,
 1206, 1359,    0,    0,    0,    0,    0,    0,    3, 1576,
    0,    0,    0,  395, 3942,    0, 4389, 4998,    0,    0,
    0,    0,    0,    0, 4190,  344,  344,  344,    0,  362,
  552,  117, 1194,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, 1070, 1956,    7,   19,    0, 1190,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, 1320,    0,    0,    0,
    0, 4327,    0,    0,  811,    0,    0,  482,    0, 1487,
 2699, 2029,    0,    0,    0,    0,   20,    0, 1581, 4444,
 1585, 1585, 1585,    0,    0,    0,    0,    0,    0,    0,
 1194,    0,    0,  704,    0,  130,  172,    0,    0,    0,
    0,    0,    0, 1586, 1126,    0, 2319,    0, 1186,    0,
    0,    0, 1194,    0,    0,    0,    0, 1194,    0,    0,
    0,    0,    0,    0,    0,  111,    0,    0, 3002, 3002,
 3002, 4499,    0,    0,    0,  102,    0,    0, 2581,    0,
    0,  781,    0, 2814,  542,    0,    0,  482,    0, 4615,
    0,    0,    0,    0,  362,  117,    0,    0,    0,  722,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   18,
    0,  498,   19, 1194,  117,    0,    0,    0,    0,    0,
    0,  185,    0,    0,    0,    0,    0,    0,    0,    0,
  438,    0,    0,    0,    0,    0,    0,  653,    0,  482,
    0,    0,    0,    0,    0,    0, 4647,  809,  199,  206,
    0,    0,  821,  825,    0,  868,    0,    0,    0,  482,
    0,    0,  517,    0,    0,    0,    0,  678,    0,    0,
    0,  101,    0, 1190,    0,  482,    0,  368,    0,   14,
  268, 4740,    0,    0,    0,    0,    0,  172,    0,    0,
  811,    0,    0, 1186,    0,   58,  197,  245,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    5,
  172,  130,    0,    0,    0, 3127,    0,    0,    0,    0,
    0, 4066, 4959,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  268,    0,    0,    0,    0,    0,    0,    0,
    0,  419,    0,    0,    0, 1216,    0,    0,  482, 1590,
 1216,    0,    0,  -17,    0,    0,    0,   94, 1591, 1186,
    0,  319,  117,    0, 1210, 1210, 1210,    0,    0,    0,
  482,   13,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  199,  637,  122,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  580,  117, 1194,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, 1187,   14,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   11,    0,  809,  199, 1186, 1031,    0,
    0,    0,    0,    0,    0, 1216,    0,    0,    0,    0,
    0,    0,   17,  172,    0,    0,    0,    0,    0,  150,
    0,  594,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, 1216,    0,    0, 1638,
    0,    0,    0,    0,    0,    0,  811, 1268,    0,    0,
    0,  811, 1310,    0, 4834, 4834,    0,    0,    0,  206,
  993,    0,    0,    0,    0,    0,  206, 1122,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 4834,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, 4866,    0,    0,    0,    0,    0,    0,    0,   19,
    7,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    4,  809,  809,  809,    0,   29,    0,    0,
  122,    0,    0,  117,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  311,    0,
 1194,  809,  809,    0,    0,  199,    0,    0, 1186,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, 1592,    0,    0,  612,  162,    0,    0,
    0,    0,    0,    0,  809,  199,    0,    0,  809,  199,
    0,    0, 3553,  637,  122,    0,  637,  122,    0, 1210,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, 1596,  612,    0,    0,  199,  199,  122,
  122,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT pfctlygindex[] = {                     0,
 2637,    0, -161,  472, -359,    0,    0, -511,  -77, -554,
  -79, 1536,    0,    0, 1605,  494, 1534,    0,    0,    0,
    0,    0,  516, 1532,    0, 1599,    0, 1127,    0,    0,
 -583,    0,    0,  773,  660, -357,    0, 1255,    0, -361,
    0,    0,    0, -675,    0, -751,    0, 1339, 1254, 1094,
 1117,    0,    0,  235,    0,    0, -461,    0, 1138,    0,
    0, -426,    0,  901,    0, -488,    0, 1130,    0, -545,
    0,    0, -681,    0,    0, -680,    0,    0,    0,   50,
    0, 1009, 1540,   -9, 1435,  -90,    0, -509,  806, 1397,
  524,   92,  743,    0, 1340,    0,    0, -245,    0,    0,
 1369,    0, -333,    0,    0, 1195,    0, 1188,    0, 1097,
    0,    0, 1095,    0,    0, 1109,    0, -440, 1085,  962,
    0,    0,  846,    0,    0, -678,    0,    0, 1278,    0,
 1462, 1386,    0,  926,  933,    0,    0, 1317,    0, -364,
 1106,    0,  975, -345,    0, 1293,    0, -561, 1231,    0,
    0,    0, -644,    0,    0, 1090,    0,    0,  779,  780,
    0, 1688, 1693,   -3,   -2,    0,    0,    0,    0,    0,
    0,    0, -192, -136,    0, -156,    0,    0,    0, -175,
 1269,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 5608
static const YYINT pfctlytable[] = {                     48,
   49,  196,   62,   63,  227,  142,  423,  424,  188,  837,
  553,  437,  152,   92,  352,  347,  539,  541,  543,  249,
  214,  624,  487,  546,  547,  733,  489,  320,  347,  495,
  936,  687,  290,   22,  281,  475,  476,  352,  491,  512,
  765,  479,  186,  790,  551,  487,  347,  445,  255,  496,
  128,  347,  347,  347,  141,  367,  310,  347,  132,  347,
  335,  143,  347,  138,  146,  249,    1,  316,  160,  300,
  292,  504,  880,  347,  338,  297,  521,   52,  737,  347,
  495,  192,   50,  339,  186,  397,  838,  289,  148,  534,
  535,  136,   57,  308,  636,  482,  397,  605,   58,  606,
  496,  607,  248,  339,  609,  196,  548,   53,   60,  762,
  491,  301,  710,  147,  538,  356,  540,  359,  140,  773,
  137,  853,  483,  481,  484,  152,  339,  571,  154,  155,
  482,  379,  406,  214,  347,  542,  347,  149,  347,  545,
  320,  347,  347,  347,  347,  937,  876,  885,  495,  820,
  347,  888,  133,  354,  347,  357,  582,  483,  481,  484,
  347,  347,  811,  425,  482,  347,    1,  482,  496,  512,
  949,  380,  711,  381,  406,  188,    1,  144,  775,  396,
  316,  347,  347,  347,  186,  429,  777,  518,  704,  347,
  433,  483,  481,  484,  483,  481,  484,  603,  403,  619,
  345,  512,  426,  622,  901,  482,  322,  447,  835,  347,
  418,  347,  455,  932,  625,  347,  339,  443,  131,  419,
  480,  512,  239,  186,  347,  200,  846,  814,   85,  322,
  434,  298,  483,  481,  484,  482,  828,  874,  347,   88,
  831,  347,  347,  948,  950,  464,  347,  742,  951,  701,
  419,  465,  466,  419,  316,  763,  731,  750,  840,  471,
  807,  326,  483,  481,  484,  347,  347,  347,  531,  490,
  713,  239,   81,   82,  185,  254,  536,  316,   90,    4,
    5,    6,    7,  239,  467,  544,  320,  404,  514,  515,
  140,  347,  549,  352,  405,  163,  347,  347,  346,  188,
  512,  487,  565,  355,  136,  489,  529,  512,  532,  763,
  776,  778,  779,  594,  347,  347,  764,  491,  886,  322,
  347,  347,    8,  347,  140,  757,  309,  512,   93,  404,
  333,  334,  501,  137,    9,  589,   10,   11,   12,   13,
   14,  503,  566,  567,  305,  810,  519,  520,  735,  736,
  867,   91,  358,  339,  320,   15,  339,   16,   17,  152,
  197,   17,  339,  836,  378,  297,  352,  583,   81,   18,
  764,  732,  247,  587,  487,  347,  615,  108,  489,  347,
   19,  592,  493,  494,  593,  138,  352,  352,  140,  491,
  512,   20,  138,  339,  487,  152,   91,  589,  138,  512,
  106,  280,  347,  347,  239,  347,  347,  347,  163,  491,
  492,  347,  347,  347,  613,  842,   21,  843,  247,  905,
  906,  907,    1,    1,    1,    1,    1,  297,  459,  698,
  699,  700,  297,  493,  494,  854,  855,  856,  395,  491,
  491,  491,  491,  491,  595,  491,  944,  708,  239,  395,
  339,  945,    1,    1,    1,  193,  194,  688,  339,  339,
  339,  716,  864,  193,  194,    1,  697,  318,  319,  318,
  319,  491,  772,  727,  852,  339,  339,    1,  180,    1,
    1,    1,    1,    1,  718,  706,  347,  709,  318,  319,
  193,  194,  318,  319,  347,  347,  322,  719,    1,  714,
    1,  493,  494,  347,    1,  910,  646,  320,  129,  347,
  788,  677,    1,  728,  347,  347,  419,  740,  741,  101,
  595,  138,  512,    1,  748,  749,  358,  748,  749,  432,
  320,  154,  155,  322,    1,   92,  934,  419,  935,  154,
  155,  138,  347,  347,  316,  512,  618,  512,  512,  358,
  789,  339,  127,  322,  512,  512,  358,  358,  186,    1,
  358,  322,  322,  322,  347,  755,  756,  809,  512,  512,
  129,  633,  102,  812,  339,  129,  358,  512,  322,  322,
  184,  419,  419,  419,  784,  689,  419,  419,  419,  466,
  419,  347,  347,  419,  419,  755,  756,  123,  347,  347,
  822,  316,  419,  460,  677,  824,  339,  239,  595,  316,
  316,  316,  339,  829,  239,  849,  799,  103,   91,  512,
  832,  239,  202,  501,  206,  239,  316,  316,  860,  861,
  862,  239,  163,  163,  163,  163,  163,  163,  163,  358,
  680,  358,  163,  163,  163,  239,  239,  646,  406,  124,
  509,  347,  510,  847,  239,  347,  125,  106,  106,  256,
  512,  512,  297,  892,  781,  894,  896,  898,  297,  512,
  106,  347,  104,  106,  866,   91,  253,  871,  163,  106,
  512,  834,  105,   91,   91,   91,  239,  362,  530,  239,
  239,  106,  913,  191,  106,  564,  512,  512,  512,  919,
   91,   91,  588,  922,  550,  339,  614,  126,  926,  900,
  362,  929,  626,  428,  931,  107,  816,  362,  362,  524,
  302,  362,  870,  799,  510,  347,  707,  715,  525,  106,
  911,  429,   97,  680,   98,  787,  347,  362,  299,  920,
  308,  239,  241,  923,  428,  800,  239,  428,  927,  106,
  242,  930,  347,  784,  347,  889,  106,  459,  459,  459,
  459,  459,  429,  459,  108,  429,  320,  848,  239,  595,
  912,  918,  208,  209,  297,  845,  358,  243,  244,  358,
  135,  347,  138,  138,  369,  358,  358,  109,  857,  459,
  489,  512,  512,  512,  358,  358,  358,  320,  358,  358,
  362,  358,  362,  404,  339,  358,  358,  358,  959,  358,
  339,  960,  961,  962,  963,  358,  358,  358,  358,  358,
  358,  358,  358,  650,  179,  358,  428,  921,  358,  239,
  512,  339,  339,  358,  320,  239,  199,  199,  199,  575,
  358,  574,  358,  151,  339,  702,  370,  339,  904,  925,
  110,  371,  800,  339,  320,  358,  358,  358,  358,  358,
  358,  358,  320,  320,  320,  339,  186,  870,  916,  917,
    5,    6,    7,  358,  138,  138,  111,  372,  358,  320,
  320,  358,  358,  358,  161,  162,  358,  358,  479,  479,
  203,  204,  457,  458,  459,  479,  479,  479,  358,  358,
  372,  446,   51,  339,  239,  358,  358,  358,  112,  358,
  358,  372,   52,   53,   54,  512,  928,  512,  464,  464,
  464,  464,  464,  339,  464,  512,   86,  372,  372,  372,
  339,  460,  460,  460,  460,  460,  460,  362,  460,  273,
  362,  449,  450,  113,  512,  454,  362,  362,  239,  152,
  464,   55,  512,  512,  157,  362,  362,  362,   48,  362,
  362,  114,  362,  115,  460,  171,  362,  362,  362,   93,
  362,  451,  891,  211,  212,  239,  362,  362,  362,  362,
  362,  362,  362,  362,  893,  895,  362,  239,  239,  362,
  372,  116,  372,  308,  362,   94,   95,   96,  308,  308,
  117,  362,  381,  362,  347,  347,  512,  308,  897,  118,
  428,  239,  512,  512,  233,  234,  362,  362,  362,  362,
  362,  362,  362,  924,  119,  381,  239,  297,  429,  512,
  512,  428,  297,  297,  362,  120,  381,  121,  297,  362,
  325,  297,  362,  362,  362,  512,  122,  362,  362,  429,
  290,  178,  381,  381,  381,   81,  290,  290,  290,  362,
  362,  318,  319,  325,  333,  334,  362,  362,  362,  489,
  362,  362,  519,  520,  325,  428,  428,  428,  171,  290,
  428,  428,  428,  452,  428,  723,  724,  428,  428,   94,
   95,   96,  308,  429,  429,  429,  428,  182,  429,  429,
  429,  180,  429,  769,  770,  429,  429,  782,  194,  452,
  452,  452,  165,  272,  429,  381,  214,  381,  220,  489,
  489,  489,  489,  489,  207,  489,  297,  372,  748,  749,
  372,  390,  222,  369,  215,  300,  372,  372,  452,  452,
  452,  328,  489,  329,  330,  331,  372,  372,  224,  372,
  372,  489,  272,  325,  390,  325,  372,  372,  372,  223,
  372,  226,  138,  755,  756,  390,  372,  372,  735,  736,
  372,  372,  372,  372,  237,  272,  372,  752,  759,  372,
  230,  390,  390,  390,  372,  877,  878,  231,  740,  741,
  794,  372,  290,  372,  881,  882,  107,  239,  795,  796,
  797,  512,  512,  347,  347,  232,  372,  372,  372,  372,
  372,  372,  372,  518,  518,  167,  798,  520,  520,  512,
  235,  200,  201,  236,  372,  165,  272,  107,  238,  372,
  400,  401,  372,  372,  372,  247,  257,  372,  372,  271,
  276,  282,  279,  140,  390,  295,  390,  294,  300,  372,
  372,  296,  381,  186,  313,  512,  372,  372,  372,  314,
  372,  372,  381,  315,  316,  317,  321,  322,  323,  343,
  360,  381,  381,  364,  381,  381,  512,  407,  554,  555,
  556,  557,  558,  363,  559,  381,  368,  376,  411,  382,
  387,  381,  381,  416,  413,  381,  381,  381,  421,  325,
  407,  381,  171,  171,  171,  171,  171,  171,  171,  381,
  560,  407,  171,  171,  171,  417,  427,  430,  381,  410,
  512,  512,  512,  435,  439,  512,  512,  512,  167,  512,
  325,  441,  512,  512,  436,  309,  381,  290,  290,  448,
  456,  512,  410,  290,  752,  290,  290,  759,  171,  381,
  470,  463,  472,  410,  381,  499,  500,  381,  381,  381,
  506,  517,  381,  513,  568,  569,  570,  325,  169,  272,
  272,  272,  272,  572,  381,  381,  576,  272,  577,  579,
  272,  390,  381,  381,  153,  381,  381,  325,  578,  581,
  407,  390,  407,  300,  300,  325,  325,  325,  598,  600,
  390,  390,  300,  390,  390,  611,  290,  616,  272,  272,
  272,  272,  325,  325,  390,  627,  272,  628,  630,  272,
  390,  390,  629,  325,  390,  390,  390,  631,  632,  634,
  390,  503,  410,  705,  410,  651,  692,  717,  390,  720,
  272,  752,  272,  722,  759,  721,  726,  390,  730,  165,
  165,  165,  165,  165,  165,  165,  745,  746,  290,  165,
  165,  165,  300,  747,  768,  390,  774,  780,  785,  792,
  804,  286,  287,  288,  805,  806,  512,  512,  390,  272,
  813,  169,  815,  390,  818,  732,  390,  390,  390,  512,
  258,  390,  512,  821,  823,  165,  487,  153,  512,  272,
  826,  830,  825,  390,  390,  833,  841,  272,  272,  272,
  512,  390,  390,  258,  390,  390,  844,  850,  851,  487,
  863,  858,  865,  873,  272,  272,  890,  407,  903,  186,
  908,  940,  941,  942,  272,  943,  933,  407,  946,  351,
  352,  353,  947,  952,  953,  954,  407,  407,  512,  407,
  407,  955,  167,  167,  167,  167,  167,  167,  167,  956,
  407,  957,  167,  167,  167,  958,  407,  407,  512,  410,
  407,  407,  407,  277,  290,  512,  407,  964,  138,  410,
   28,  394,  347,   63,  407,  206,  512,  308,  410,  410,
  141,  410,  410,  407,  491,  130,  415,  138,  167,   75,
   89,  476,  410,  512,  420,  477,  150,   45,  410,  410,
  163,  407,  410,  410,  410,   87,  899,  965,  410,  344,
  703,  602,  584,  260,  407,  183,  410,  573,  580,  407,
  272,  883,  407,  407,  407,  410,  297,  407,  325,  365,
  608,  533,  537,  610,  604,  786,  260,  621,  859,  407,
  407,  438,  270,  410,  324,  819,  477,  407,  407,  817,
  407,  407,  217,  218,  219,  419,  410,  620,  508,  442,
  106,  410,  528,  512,  410,  410,  410,  808,  108,  410,
  938,  939,  290,  290,  290,  290,  290,  290,  290,  694,
   46,  410,  410,  290,  290,   47,    0,  516,    0,  410,
  410,    0,  410,  410,    0,  169,  169,  169,  169,  169,
  169,  169,    0,    0,  252,  169,  169,  169,    0,    0,
    0,  151,  151,  151,  151,  151,  151,  151,  290,    0,
  563,    0,  151,  151,    0,    0,    0,    0,    0,    0,
  258,    0,  258,  258,  258,  258,  258,  258,  258,  258,
  258,  169,    0,    0,  258,  258,  258,  258,  262,  258,
  258,    0,  258,  258,    0,    0,    0,  153,    0,    0,
    0,    0,    0,  258,  258,  487,  258,  258,  258,  258,
  258,  262,    0,  258,  258,  258,  638,  639,    0,  258,
    0,    0,    0,    0,    0,    0,    0,  258,    0,  640,
    0,    0,  641,    0,    0,    0,  258,  327,  642,    0,
    0,  342,    0,    0,    0,    0,    0,  258,  350,    0,
  643,    0,    0,    0,  258,  487,  487,  487,  487,  487,
    0,  487,    0,    0,    0,    0,    0,  258,    0,    0,
  275,    0,  258,    0,  275,  258,  258,  258,  487,    0,
  258,  691,    0,    0,    0,    0,    0,  487,  644,    0,
    0,    0,  258,  258,    0,  402,    0,  275,  487,    0,
  258,  258,  258,  260,    0,  260,  260,  260,  260,  260,
  260,  260,  260,  260,    0,  645,    0,  260,  260,  260,
  260,  263,  260,  260,    0,  260,  260,  347,    0,    0,
    0,    0,    0,    0,  347,    0,  260,  260,  347,  260,
  260,  260,  260,  260,  263,    0,  260,  260,  260,    0,
    0,    0,  260,    0,  347,  347,  347,  106,  106,  290,
  260,    0,    0,    0,    0,    0,    0,    0,    0,  260,
  106,    0,    0,  106,    0,    0,    0,    0,    0,  106,
  260,    0,  290,  275,    0,    0,    0,  260,    0,    0,
    0,  106,    0,   43,    0,  103,    0,    0,    0,    0,
  260,    0,    0,    0,    0,  260,    0,    0,  260,  260,
  260,    0,    0,  260,    0,    0,    0,  347,  103,  347,
    0,    0,    0,    0,    0,  260,  260,    0,    0,  106,
    0,    0,    0,  260,  260,  260,    0,    0,  262,    0,
  262,  262,  262,  262,  262,  262,  262,  262,  262,  106,
    0,    0,  262,  262,  262,  262,  106,  262,  262,    0,
  262,  262,    0,    0,    0,    0,    0,    0,  308,    0,
    0,  262,  262,    0,  262,  262,  262,  262,  262,    0,
  290,  262,  262,  262,    0,    0,    0,  262,    0,    0,
    0,  308,    0,    0,    0,  262,    0,    0,    0,    0,
    0,    0,    0,    0,  262,    0,    0,    0,   44,    0,
    0,    0,    0,    0,    0,  262,    0,    0,    0,   61,
    0,    0,  262,    0,    0,    0,    0,  275,  275,  275,
    0,  275,  275,  275,  275,  262,    0,  275,  275,    0,
  262,  275,   61,  262,  262,  262,    0,    0,  262,    0,
   61,    0,    0,   61,    0,    0,   61,    0,    0,    0,
  262,  262,    0,    0,  275,    0,    0,    0,  262,  262,
  262,  263,  347,  263,  263,  263,  263,  263,  263,  263,
  263,  263,    0,    0,    0,  263,  263,  263,  263,    0,
  263,  263,    0,  263,  263,  347,  347,  275,    0,    0,
    0,  275,    0,    0,  263,  263,   68,  263,  263,  263,
  263,  263,    0,    0,  263,  263,  263,  290,  290,    0,
  263,  275,    0,  290,    0,    0,  290,    0,  263,  275,
  275,  275,   61,    0,   61,    0,    0,  263,    0,   42,
    5,    6,    7,    0,    0,    0,  275,  275,  263,  290,
    0,  282,    0,  103,  103,  263,  275,    0,  347,  103,
    0,    0,  103,    0,    0,  914,    0,    0,  263,    0,
    0,    0,    0,  263,  282,    0,  263,  263,  263,    0,
    0,  263,    8,    0,    0,  103,  290,  347,  347,    0,
    0,    0,    0,  263,  263,    0,   10,   11,   12,   13,
    0,  263,  263,  263,    0,    0,  290,    0,    0,    0,
    0,    0,    0,    0,  290,  290,  290,    0,    0,   68,
    0,  308,  103,    0,    0,    0,  308,  308,    0,    0,
    0,  290,  290,  290,    0,  308,    0,    0,  290,  290,
    0,  290,  103,    0,  290,  290,  290,  290,  308,  308,
  103,  103,  103,    0,    0,    0,    0,    0,  300,    0,
    0,  308,    0,    0,  308,    0,    0,  103,  103,   61,
  308,    0,   61,    0,    0,    0,   61,   61,   61,   61,
  915,  300,  308,   61,   61,   61,   61,  279,   61,   61,
    0,   61,   61,    0,    0,    0,    0,    0,   61,   61,
   61,    0,   61,    0,    0,    0,    0,    0,   61,   61,
  279,    0,   61,   61,   61,   61,    0,    0,   61,    0,
  308,   61,    0,    0,    0,    0,   61,    0,    0,    0,
    0,    0,  290,   61,    0,   61,    0,    0,    0,    0,
  308,    0,    0,    0,    0,    0,   61,  308,   61,   61,
   61,   61,   61,   61,   61,    0,    0,    0,   68,    0,
   68,    0,   68,   68,   68,   68,   61,    0,    0,    0,
   68,   61,    0,   68,   61,   61,   61,    0,    0,   61,
   61,    0,    0,  290,  290,  290,  290,  290,  290,  290,
    0,   61,   61,  290,  290,  290,    0,    0,   61,   61,
   61,  282,    0,    0,  282,  278,    0,  282,  282,  282,
  282,  282,    5,    6,    7,  282,  282,  282,  282,    0,
  282,  282,    0,  282,  282,    0,    0,    0,  278,    0,
  282,  282,  282,   68,  282,  282,    0,  282,  282,  282,
  282,  282,    0,    0,  282,  282,  282,  282,    0,    0,
  282,    0,    0,  282,    0,    0,    0,    0,  282,    0,
    0,    0,    0,    0,    0,  282,    0,  282,   10,   11,
   12,   13,    0,    0,  282,    0,    0,    0,    0,    0,
  282,  282,  282,  282,  282,  282,  282,    0,    0,  151,
  151,  151,  151,  151,  151,  151,    0,    0,  282,  153,
  151,  151,    0,  282,    0,    0,  282,  282,  282,    0,
    0,  282,  282,    0,    0,    0,  300,  300,    0,    0,
  343,    0,    0,  282,  282,  300,    0,    0,    0,    0,
  282,  282,  282,    0,    0,    0,    0,  279,    0,    0,
  279,    0,    0,  343,  279,  279,  279,  279,  300,    0,
    0,  279,  279,  279,  279,    0,  279,  279,    0,  279,
  279,    0,    0,    0,    0,    0,    0,  279,  279,    0,
  279,  279,    0,  279,  279,  279,  279,  279,    0,    0,
  279,  279,  279,  279,    0,  300,  279,    0,    0,  279,
    0,  100,    0,    0,  279,    0,    0,    0,    0,    0,
    0,  279,    0,  279,    0,  300,    0,    0,    0,    0,
  279,    0,    0,  300,  300,  300,    0,    0,    0,    0,
    0,  279,    0,    0,    0,    0,    0,  129,  130,  131,
  300,  300,    0,  343,  279,    0,    0,    0,  333,  279,
  153,    0,  279,  279,  279,    0,    0,  279,    0,    0,
    0,    0,    0,    0,    0,  278,  191,    0,  278,  279,
  279,  333,  278,  278,  278,  278,  279,  279,  279,  278,
  278,  278,  278,    0,  278,  278,    0,  278,  278,    0,
    0,    0,    0,    0,    0,  278,  278,    0,  278,  278,
    0,  278,  278,  278,  278,  278,    0,    0,  278,  278,
  278,  278,    0,    0,  278,    0,    0,  278,    0,    0,
    0,    0,  278,    0,    0,    0,    0,    0,    0,  278,
    0,  278,    0,    0,    0,    0,    0,    0,  278,  258,
  259,  260,  261,  262,  263,  264,    0,    0,    0,  278,
  265,  266,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  333,  278,  341,    0,    0,    0,  278,    0,    0,
  278,  278,  278,    0,    0,  278,    0,  274,  275,    0,
  343,    0,  278,  343,    0,    0,  341,  278,  278,  343,
  343,    0,  285,    0,  278,  278,  278,    0,    0,  343,
  343,    0,  343,  343,    0,    0,    0,    0,    0,  343,
  343,  343,    0,  343,    0,    0,    0,    0,    0,  343,
  343,    0,    0,  343,  343,  343,  343,    0,    0,  343,
  306,    0,  343,    0,    0,    0,    0,  343,    0,    0,
    0,    0,    0,    0,  343,    0,  343,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  343,
  343,  343,  343,  343,  343,  343,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  341,  343,    0,    0,
    0,  375,  343,    0,    0,  343,  343,  343,    0,    0,
  343,  343,    0,    0,    0,    0,  272,    0,  333,    0,
    0,    0,  343,  343,    0,    0,    0,  333,  333,  343,
  343,  343,    0,    0,    0,    0,    0,  333,  333,  272,
  333,  333,    0,    0,    0,    0,    0,  333,  333,  333,
    0,  333,    0,    0,    0,    0,    0,  333,  333,    0,
    0,  333,  333,  333,  333,    0,    0,  333,    0,    0,
  333,  466,    0,    0,    0,  333,    0,    0,    0,    0,
    0,    0,  333,    0,  333,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  466,    0,    0,  333,  333,  333,
  333,  333,  333,  333,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  333,    0,    0,    0,    0,
  333,    0,    0,  333,  333,  333,    0,    0,  333,  333,
    0,    0,    0,  341,    0,    0,    0,    0,    0,    0,
  333,  333,    0,  341,    0,    0,    0,  333,  333,  333,
    0,    0,  341,  341,    0,  341,  341,    0,    0,    0,
    0,    0,  341,  341,  341,    0,  341,    0,    0,    0,
    0,    0,  341,  341,    0,    0,  341,  341,  341,  341,
    0,    0,  341,    0,    0,  341,    0,    0,    0,    0,
  341,    0,    0,    0,    0,    0,  462,  341,    0,  341,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  341,  341,  341,  341,  341,  341,  341,  462,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  341,    0,    0,    0,    0,  341,    0,    0,  341,  341,
  341,    0,    0,  341,  341,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  341,  341,    0,    0,    0,
    0,    0,  341,  341,  341,    0,  272,    0,    0,  272,
    0,  272,  272,  272,  272,  272,  272,    0,    0,    0,
  272,  272,  272,  272,    0,  272,  272,    0,  272,  272,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  272,
  272,    0,  272,  272,  272,  272,  272,    0,    0,  272,
  272,  272,    0,    0,    0,  272,    0,    0,    0,    0,
    0,  466,    0,  272,  466,    0,    0,    0,   68,  466,
  466,  466,  272,    0,    0,  466,  466,  466,  466,    0,
  466,  466,    0,  466,  466,    0,    0,    0,    0,    0,
  272,   68,    0,    0,  466,    0,    0,    0,    0,    0,
  466,  466,    0,  272,  466,  466,  466,    0,  272,    0,
  466,  272,  272,  272,    0,    0,  272,    0,  466,    0,
    0,    0,    0,    0,    0,    0,    0,  466,  272,  272,
    0,    0,    0,  272,    0,    0,  272,  272,    0,    0,
  464,  464,  464,  464,  464,  466,  464,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  272,    0,  466,    0,
    0,    0,    0,  466,    0,    0,  466,  466,  466,    0,
    0,  466,  464,    0,    0,    0,    0,    0,    0,    0,
    0,   68,    0,  466,  466,    0,  462,    0,    0,  462,
    0,  466,  466,  277,  462,  462,  462,    0,    0,    0,
  462,  462,  462,  462,    0,  462,  462,    0,  462,  462,
    0,    0,    0,    0,    0,    0,  277,    0,    0,  462,
    0,    0,    0,    0,    0,  462,  462,    0,    0,  462,
  462,  462,    0,    0,    0,  462,    0,    0,    0,    0,
    0,    0,    0,  462,    0,    0,    0,    0,    0,    0,
    0,    0,  462,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  462,  462,  462,  462,  462,
  462,  462,    0,    0,    0,    0,    0,    0,  164,    0,
    0,    0,    0,  462,    0,    0,    0,    0,  462,    0,
    0,  462,  462,  462,    0,    0,  462,  462,    0,  165,
    0,    0,    0,    0,    0,    0,    0,    0,  462,  462,
    0,    0,    0,    0,    0,    0,  462,  462,   68,  290,
   68,   68,   68,    0,   68,   68,   68,   68,   68,    0,
    0,    0,   68,   68,   68,   68,    0,   68,   68,    0,
   68,   68,  290,  166,  167,  168,    0,    0,  169,  170,
  171,   68,  172,    0,    0,  161,  162,   68,   68,    0,
    0,   68,   68,   68,  173,    0,    0,   68,    0,    0,
    0,    0,    0,    0,    0,   68,    0,    0,    0,    0,
    0,    0,    0,    0,   68,    0,  272,    0,  272,  272,
  272,  272,  272,    0,    0,    0,    0,  272,  272,  272,
  272,    0,   68,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  272,  272,   68,    0,    0,    0,    0,
   68,    0,    0,   68,   68,   68,  272,    0,   68,  272,
    0,    0,    0,    0,    0,  272,    0,    0,    0,    0,
   68,   68,    0,  277,    0,  272,  277,  272,   68,   68,
  277,  277,  277,  277,    0,    0,    0,  277,  277,  277,
  277,    0,  277,  277,    0,  277,  277,    0,  272,    0,
    0,    0,    0,    0,    0,    0,  277,  277,    0,  277,
  277,  277,  277,  277,    0,  272,  277,  277,  277,    0,
    0,    0,  277,    0,    0,    0,    0,    0,    0,    0,
  277,    0,    0,    0,    0,  272,    0,   64,    0,  277,
    0,    0,  272,   65,   66,   67,   68,   69,   70,  479,
   71,    0,   72,    0,   73,   74,   75,  277,    0,    0,
   76,   77,    0,    0,    0,    0,    0,    0,    0,    0,
  277,    0,  479,    0,    0,  277,    0,    0,  277,  277,
  277,    0,    0,  277,    0,    0,    0,    0,  272,    0,
    0,    0,    0,    0,   78,  277,  277,    0,    0,  290,
  275,    0,  290,  277,  277,    0,    0,  290,  290,  290,
    0,   79,   80,  290,  290,  290,  290,    0,  290,  290,
    0,  290,  290,  275,    0,    0,    0,    0,    0,    0,
    0,    0,  290,  290,    0,  290,  290,  290,  290,  290,
    0,    0,  290,  290,  290,    0,    0,    0,  290,    0,
    0,    0,    0,    0,    0,    0,  290,    0,    0,    0,
    0,    0,    0,    0,    0,  290,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  290,    0,    0,    0,    0,    0,  512,
    0,    0,    0,    0,    0,    0,  290,    0,    0,    0,
    0,  290,    0,    0,  290,  290,  290,    0,    0,  290,
  512,    0,    0,  275,    0,    0,    0,    0,    0,    0,
    0,  290,  290,    0,    0,  272,  290,    0,  272,  290,
  290,  272,  272,  272,  272,  272,    0,    0,    0,  272,
  272,  272,  272,    0,  272,  272,    0,  272,  272,  290,
    0,    0,    0,    0,  512,  512,  512,    0,  272,  512,
  512,  512,    0,  512,  272,  272,  512,  512,  272,  272,
  272,    0,    0,    0,  272,  512,    0,    0,    0,    0,
    0,  499,  272,    0,    0,    0,    0,    0,    0,    0,
    0,  272,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  479,    0,  499,    0,  479,  479,  479,  272,
    0,    0,    0,  479,  479,  479,  479,    0,    0,    0,
    0,    0,  272,    0,    0,    0,    0,  272,  479,    0,
  272,  272,  272,    0,    0,  272,    0,    0,    0,  290,
    0,    0,    0,    0,    0,    0,    0,  272,  272,    0,
  275,    0,    0,  275,    0,  272,  272,  275,  275,  275,
  275,    0,    0,    0,  275,  275,  275,  275,    0,  275,
  275,    0,  275,  275,    0,    0,    0,    0,  479,  479,
  479,  479,  479,  275,  479,    0,    0,    0,    0,  275,
  275,    0,    0,  275,  275,  275,    0,    0,    0,  275,
    0,  479,    0,    0,    0,  465,    0,  275,    0,    0,
  479,    0,    0,    0,    0,    0,  275,    0,    0,    0,
    0,  479,  479,    0,    0,    0,    0,    0,  465,    0,
    0,    0,    0,    0,  275,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  275,    0,    0,
    0,    0,  275,    0,    0,  275,  275,  275,    0,    0,
  275,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  275,  275,    0,    0,  290,    0,  277,  290,
  275,  275,    0,    0,  290,  290,  290,    0,    0,    0,
  290,  290,  290,  290,    0,  290,  290,    0,  290,  290,
    0,  277,    0,    0,    0,    0,    0,    0,    0,  290,
    0,    0,    0,    0,    0,  290,  290,    0,    0,  290,
  290,  290,    0,    0,    0,  290,    0,    0,    0,  297,
    0,  499,    0,  290,  499,    0,    0,    0,    0,  499,
  499,  499,  290,    0,    0,  499,  499,  499,  499,    0,
  499,  499,  297,  499,  499,    0,    0,    0,    0,    0,
  290,    0,    0,    0,  499,    0,    0,    0,    0,    0,
  499,  499,    0,  290,  499,  499,  499,    0,  290,    0,
  499,  290,  290,  290,    0,    0,  290,    0,  499,    0,
    0,    0,    0,    0,    0,    0,    0,  499,  290,  290,
    0,    0,    0,    0,    0,    0,  290,  290,    0,    0,
    0,  297,    0,    0,    0,  499,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  499,    0,
    0,    0,    0,  499,  297,    0,  499,  499,  499,    0,
    0,  499,  297,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  499,  499,  465,    0,    0,  465,    0,
    0,  499,  499,  465,  465,  465,  297,    0,    0,  465,
  465,  465,  465,    0,  465,  465,    0,  465,  465,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  465,  297,
    0,    0,    0,    0,  465,  465,    0,    0,  465,  465,
  465,    0,    0,    0,  465,    0,    0,    0,    0,    0,
    0,    0,  465,    0,    0,    0,    0,    0,    0,    0,
    0,  465,    0,    0,    0,    0,    0,    0,  339,    0,
    0,  277,    0,    0,    0,  277,  277,  277,    0,  465,
    0,    0,  277,  277,  277,  277,    0,    0,    0,    0,
    0,  339,  465,    0,    0,    0,    0,  465,  277,  277,
  465,  465,  465,    0,    0,  465,    0,    0,    0,    0,
    0,  277,    0,    0,  277,    0,    0,  465,  465,  297,
  277,    0,  297,  308,    0,  465,  465,  297,  297,  297,
    0,    0,  277,  297,    0,    0,  297,    0,  297,  297,
    0,  297,  297,    0,    0,    0,  308,    0,    0,    0,
    0,    0,  297,    0,    0,    0,    0,    0,  297,  297,
    0,    0,  297,  297,  297,    0,    0,    0,  297,    0,
  277,    0,    0,    0,    0,    0,  297,    0,  308,    0,
    0,    0,    0,    0,    0,  297,    0,    0,    0,    0,
  277,    0,    0,    0,    0,    0,    0,  277,    0,    0,
    0,  308,    0,  297,  297,    0,    0,    0,    0,  297,
  297,    0,    0,    0,    0,  297,  297,    0,  297,    0,
    0,  297,    0,    0,  297,  297,  297,    0,    0,  297,
  297,    0,    0,    0,    0,    0,  308,    0,    0,    0,
    0,  297,  297,    0,    0,    0,    0,    0,    0,  297,
  297,    0,    0,    0,    0,    0,  297,    0,    0,  297,
    0,    0,    0,    0,  297,  297,  297,    0,    0,    0,
  297,    0,    0,  297,    0,  297,  297,    0,  297,  297,
  297,  297,  297,  297,  297,    0,  297,    0,    0,  297,
    0,    0,    0,    0,  339,  297,  297,    0,    0,  297,
  297,  297,    0,  297,    0,  297,    0,    0,    0,    0,
    0,    0,  297,  297,    0,    0,    0,  339,    0,    0,
    0,  339,  297,  297,  297,    0,  339,  339,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  297,    0,    0,    0,    0,    0,    0,  339,    0,  339,
    0,    0,    0,  297,    0,    0,    0,    0,  297,    0,
    0,  297,  297,  297,    0,    0,  297,    0,    0,    0,
    0,    0,    0,  308,    0,    0,  308,    0,  297,  297,
    0,  308,  308,  308,    0,    0,  297,  297,    0,    0,
  308,    0,  308,  308,    0,  308,  308,  339,  339,  339,
  339,  339,    0,  339,    0,    0,  308,  339,    0,    0,
    0,    0,  308,  308,    0,    0,  308,  308,  308,  215,
  339,    0,  308,    0,    0,    0,    0,    0,  308,  339,
  308,  308,    0,    0,    0,    0,  308,  308,  308,  308,
  339,  339,  213,    0,    0,  308,    0,  308,  308,    0,
  308,  308,    0,    0,    0,    0,    0,  308,  290,    0,
    0,  308,    0,    0,    0,    0,    0,  308,  308,    0,
  308,  308,  308,  308,    0,  308,    0,  308,  308,  308,
  308,  290,    0,  308,    0,  308,    0,    0,    0,    0,
    0,    0,    0,    0,  308,  308,  308,    0,    0,    0,
    0,    0,    0,  308,  308,    0,    0,    0,    0,    0,
    0,    0,  308,  432,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  308,    0,    0,    0,    0,
  308,    0,  215,  308,  308,  308,  432,    0,  308,    0,
    0,    0,    0,    0,  339,   65,    0,  339,    0,    0,
  308,  308,    0,  339,  339,    0,    0,    0,  308,  308,
    0,    0,    0,  339,  339,    0,  339,  339,   65,    0,
    0,    0,    0,    0,    0,    0,  339,  339,    0,  339,
    0,    0,    0,  339,  339,  339,  339,  339,  339,  339,
    0,    0,    0,  339,    0,  339,  339,    0,  339,  339,
    0,  339,    0,    0,    0,    0,    0,    0,    0,  339,
  339,    0,    0,    0,    0,  339,  339,    0,    0,  339,
  339,  339,    0,    0,    0,  339,  432,    0,  339,    0,
    0,    0,    0,  339,    0,    0,    0,    0,  215,    0,
    0,  339,  339,    0,    0,    0,  339,    0,    0,  339,
  339,  339,    0,    0,  339,    0,    0,    0,   65,    0,
  339,  213,    0,    0,    0,    0,  339,  339,    0,  213,
    0,    0,    0,  339,  339,  339,    0,  297,  339,  213,
    0,  339,  339,  339,    0,    0,  339,    0,  213,  213,
    0,  213,  213,    0,    0,    0,    0,    0,  339,  339,
  297,    0,  213,    0,    0,    0,  339,  339,  213,  213,
  480,  290,  213,  213,  213,    0,  290,  290,  213,    0,
    0,    0,  290,  290,  290,  290,  213,    0,    0,    0,
    0,    0,    0,  480,    0,  213,    0,    0,  290,  290,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  290,    0,  213,  290,    0,    0,    0,    0,    0,
  290,    0,    0,  432,    0,    0,  213,  290,    0,    0,
    0,  213,  290,  432,  213,  213,  213,    0,    0,  213,
    0,    0,  432,  432,    0,  432,  432,    0,    0,    0,
  290,  213,  213,    0,    0,   65,  432,    0,    0,  213,
  213,  308,  432,  432,    0,   65,  432,  432,  432,    0,
  290,    0,  432,    0,   65,   65,    0,   65,   65,    0,
  432,    0,    0,    0,  308,    0,    0,    0,   65,  432,
  290,    0,    0,    0,   65,   65,    0,  290,   65,   65,
   65,    0,    0,    0,   65,    0,    0,  432,    0,    0,
    0,    0,   65,    0,    0,    0,    0,    0,    0,    0,
  432,   65,    0,    0,    0,  432,    0,    0,  432,  432,
  432,    0,    0,  432,    0,    0,    0,    0,    0,   65,
    0,    0,    0,    0,    0,  432,  432,    0,  213,    0,
    0,    0,   65,  432,  432,    0,    0,   65,  213,    0,
   65,   65,   65,    0,    0,   65,    0,  213,  213,    0,
  213,  213,    0,    0,    0,    0,    0,   65,   65,  186,
    0,  213,    0,    0,    0,   65,   65,  213,  213,    0,
  297,  213,  213,  213,    0,  297,  297,  213,    0,    0,
    0,  297,    0,    0,  297,  213,    0,    0,    0,    0,
    0,    0,    0,    0,  213,    0,    0,  297,  297,    0,
    0,    0,    0,  480,    0,    0,    0,  480,  480,  480,
  297,    0,  213,  297,  480,  480,  480,  480,    0,  297,
    0,    0,    0,    0,    0,  213,    0,    0,    0,  480,
  213,  297,    0,  213,  213,  213,    0,    0,  213,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  213,  213,    0,    0,    0,    0,    0,    0,  213,  213,
  290,    0,    0,    0,    0,  290,  290,    0,    0,  297,
    0,  290,  290,  290,  290,    0,    0,    0,    0,  480,
  480,  480,  480,  480,    0,  480,  290,    0,    0,  297,
    0,    0,    0,    0,  308,    0,  297,    0,    0,  308,
  308,    0,  480,    0,    0,    0,    0,    0,  308,    0,
    0,  480,    0,    0,    0,    0,    0,    0,    0,    0,
  308,    0,  480,  480,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  290,  290,  290,  290,
  290,    0,  290,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  290,
    0,    0,    0,    0,    0,    0,    0,    0,  290,    0,
  308,  308,  308,  308,  308,    0,  308,    0,    0,  290,
  290,    0,    0,    0,    0,    0,  653,    0,    0,    0,
    0,    0,    0,  308,    0,    0,  654,    0,    0,    0,
    0,    0,  308,    0,    0,  655,  656,    0,  657,  658,
    0,    0,    0,  308,  308,    0,    0,    0,    0,  659,
    0,    0,    0,    0,    0,  660,  369,    0,    0,  661,
  662,  663,    0,    0,    0,  664,    0,    0,    0,    0,
    0,    0,    0,  665,    0,    0,    0,    0,    0,    0,
    0,    0,  666,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  667,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  668,    0,    0,    0,    0,  669,    0,
    0,  670,  671,  672,    0,    0,  673,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  674,
    0,    0,    0,    0,    0,    0,  675,  676,
};
static const YYINT pfctlycheck[] = {                      3,
    3,   92,   12,   13,  141,   10,  352,  353,   88,   40,
  472,  371,   10,   10,   10,   33,  457,  458,  459,   40,
   10,  567,   10,  464,  465,   47,   10,   10,  123,   40,
   10,  593,  225,   10,  210,  400,  401,   33,   10,   33,
   40,  403,   33,  688,  471,   33,   33,  381,  185,   60,
   60,   33,   33,   40,  123,  301,  123,   44,  299,   40,
  123,  277,   44,  123,  261,   40,  123,   10,   78,  245,
  227,  123,  824,   60,  268,   10,  123,   10,  123,   60,
   40,   91,   10,  277,   33,   40,  765,  224,  261,  449,
  450,  364,   60,  255,  583,   33,   40,  538,  393,  540,
   60,  542,  123,   10,  545,  196,  466,   40,  393,  664,
   10,  248,  123,  310,   40,  291,   40,  293,  123,   40,
  393,   40,   60,   61,   62,  123,   33,  485,  393,  394,
   33,  307,  123,  123,   33,   40,  123,  310,  125,   40,
  123,   40,  123,  125,  125,  125,  822,  829,   40,  733,
   40,  832,  393,  290,   33,  292,  502,   60,   61,   62,
   44,   60,  708,  356,   33,   44,    0,   33,   60,   40,
  922,  308,  634,  310,  123,  255,   10,  393,   40,  123,
  123,   60,   61,   62,   33,  361,   40,  433,  615,   40,
  366,   60,   61,   62,   60,   61,   62,  531,  335,  559,
  295,   40,  359,  565,  849,   33,   10,  383,  763,   60,
  347,   40,  388,  892,  572,   44,  123,  379,   10,   10,
  123,   60,   44,   33,  123,   41,  788,  716,  393,   33,
  367,  241,   60,   61,   62,   33,  746,  821,   33,  267,
  752,  125,   44,  919,  926,  354,  125,  123,  929,  609,
   41,  360,  361,   44,   10,  309,  278,  123,  768,  396,
  701,  271,   60,   61,   62,   60,   61,   62,  444,  406,
  635,   44,  265,  266,  123,  184,  452,   33,  393,  256,
  257,  258,  259,   44,  393,  461,  269,  278,  425,  426,
  295,  278,  468,  289,  285,   10,  125,  278,  393,  379,
   33,  289,  478,  125,  364,  289,  443,   40,  445,  309,
  670,  671,  672,  123,  301,  302,  370,  289,  830,  123,
  301,  302,  299,  125,  393,  123,  393,   60,   10,  278,
  393,  394,  412,  393,  311,  511,  313,  314,  315,  316,
  317,  393,  479,  480,  253,  705,  393,  394,  393,  394,
  812,   33,  125,   10,  337,  332,  263,  334,  338,  357,
  257,  338,  269,  394,  125,  300,  362,  504,  265,  346,
  370,  393,  393,  510,  362,  393,  552,   10,  362,  278,
  357,  518,  393,  394,  521,  382,  382,  383,  393,  289,
  123,  368,  382,  300,  382,  393,  313,  573,  382,  393,
   33,   41,  301,  302,   44,   44,  393,  394,  123,  301,
  302,  393,  393,  394,  551,  775,  393,  777,  393,  860,
  861,  862,  256,  257,  258,  259,  260,  362,   10,  605,
  606,  607,  367,  393,  394,  795,  796,  797,  393,  339,
  340,  341,  342,  343,  524,  345,  908,  623,   44,  393,
  357,  913,  286,  287,  288,  393,  394,  594,  365,  366,
  367,  637,  808,  393,  394,  299,  603,  393,  394,  393,
  394,  371,  393,  649,  393,  382,  383,  311,   41,  313,
  314,  315,  316,  317,  299,  622,  125,  624,  393,  394,
  393,  394,  393,  394,  393,  394,  300,  312,  332,  636,
  334,  393,  394,  393,  338,  865,  586,   10,  300,  393,
  686,  591,  346,  650,  393,  394,  307,  393,  394,   26,
  600,   40,  393,  357,  393,  394,   10,  393,  394,  125,
   33,  393,  394,  337,  368,   61,  896,  328,  898,  393,
  394,   60,  393,  394,  300,  278,  556,  363,  364,   33,
  687,   10,   59,  357,  393,  394,   40,   41,   33,  393,
   44,  365,  366,  367,  393,  393,  394,  704,  301,  302,
  362,  581,   10,  710,   33,  367,   60,  393,  382,  383,
   87,  372,  373,  374,  675,  595,  377,  378,  379,   10,
  381,  393,  394,  384,  385,  393,  394,   40,  393,  394,
  737,  357,  393,   10,  684,  742,  263,   44,  688,  365,
  366,  367,  269,  750,   44,  791,  696,   10,  300,  309,
  757,   44,   99,  703,  101,   44,  382,  383,  804,  805,
  806,   44,  347,  348,  349,  350,  351,  352,  353,  123,
  591,  125,  357,  358,  359,   44,   44,  727,  123,   40,
  125,   40,  418,  790,   44,   44,   40,  290,  291,  188,
  393,  394,  268,  839,  674,  841,  842,  843,  274,   33,
  303,   60,   10,  306,  811,  357,  183,  814,  393,  312,
  370,  759,   10,  365,  366,  367,   44,   10,  125,   44,
   44,  324,  868,   41,   10,  125,   60,   61,   62,  875,
  382,  383,  125,  879,  470,  362,  125,  393,  884,  846,
   33,  887,  125,   10,  890,   10,  726,   40,   41,  268,
  249,   44,  813,  803,  490,  364,  125,  125,  277,  362,
  867,   10,  262,  684,  264,  125,  125,   60,   41,  876,
   10,   44,  269,  880,   41,  696,   44,   44,  885,  382,
  277,  888,  364,  844,  393,  833,  389,  339,  340,  341,
  342,  343,   41,  345,   10,   44,  269,  125,   44,  849,
  125,  125,  393,  394,   10,  785,  260,  304,  305,  263,
  393,  393,  301,  302,  300,  269,  270,   10,  798,  371,
   10,  354,  355,  356,  278,  279,  280,  300,  282,  283,
  123,  285,  125,  278,  263,  289,  290,  291,  945,  293,
  269,  948,  949,  950,  951,  299,  300,  301,  302,  303,
  304,  305,  306,  589,   82,  309,   41,  125,  312,   44,
  393,  290,  291,  317,  337,   44,   94,   95,   96,   45,
  324,   47,  326,  393,  303,  611,  362,  306,  858,  125,
   10,  367,  803,  312,  357,  339,  340,  341,  342,  343,
  344,  345,  365,  366,  367,  324,   33,  958,  872,  872,
  257,  258,  259,  357,  393,  394,   10,   10,  362,  382,
  383,  365,  366,  367,  384,  385,  370,  371,  267,  268,
  275,  276,  354,  355,  356,  274,  275,  276,  382,  383,
   33,   41,  261,  362,   44,  389,  390,  391,   10,  393,
  394,   44,  271,  272,  273,  364,  125,  269,  339,  340,
  341,  342,  343,  382,  345,  277,  313,   60,   61,   62,
  389,  393,  339,  340,  341,  342,  343,  260,  345,  197,
  263,  363,  364,   10,  393,   41,  269,  270,   44,  393,
  371,  310,  304,  305,  393,  278,  279,  280,   10,  282,
  283,   10,  285,   10,  371,   10,  289,  290,  291,  260,
  293,  393,   41,  393,  394,   44,  299,  300,  301,  302,
  303,  304,  305,  306,   41,   41,  309,   44,   44,  312,
  123,   10,  125,  263,  317,  286,  287,  288,  268,  269,
   10,  324,   10,  326,  393,  394,  354,  277,   41,   10,
  307,   44,  360,  361,  375,  376,  339,  340,  341,  342,
  343,  344,  345,   41,   10,   33,   44,  263,  307,  393,
  394,  328,  268,  269,  357,   10,   44,   10,  274,  362,
   10,  277,  365,  366,  367,  393,   10,  370,  371,  328,
  268,   40,   60,   61,   62,  265,  274,  275,  276,  382,
  383,  393,  394,   33,  393,  394,  389,  390,  391,  289,
  393,  394,  393,  394,   44,  372,  373,  374,  123,   10,
  377,  378,  379,  260,  381,  393,  394,  384,  385,  286,
  287,  288,  362,  372,  373,  374,  393,  266,  377,  378,
  379,  333,  381,  393,  394,  384,  385,  393,  394,  286,
  287,  288,   10,   10,  393,  123,   62,  125,  290,  339,
  340,  341,  342,  343,  308,  345,  362,  260,  393,  394,
  263,   10,  394,  300,  266,   10,  269,  270,  286,  287,
  288,  294,  362,  296,  297,  298,  279,  280,   10,  282,
  283,  371,   10,  123,   33,  125,  289,  290,  291,  394,
  293,  394,  382,  393,  394,   44,  299,  300,  393,  394,
  303,  304,  305,  306,   60,   33,  309,  662,  663,  312,
  394,   60,   61,   62,  317,  393,  394,  394,  393,  394,
  357,  324,  123,  326,  393,  394,   10,   44,  365,  366,
  367,  393,  394,  393,  394,  394,  339,  340,  341,  342,
  343,  344,  345,  393,  394,   10,  383,  393,  394,   33,
  394,   95,   96,  394,  357,  123,  123,   41,  394,  362,
  330,  331,  365,  366,  367,  393,  357,  370,  371,  268,
  274,   41,  394,  393,  123,   47,  125,   40,  123,  382,
  383,  393,  260,   33,   40,  307,  389,  390,  391,   40,
  393,  394,  270,   40,   40,   40,  394,  394,  394,   41,
  393,  279,  280,   62,  282,  283,  328,   10,  339,  340,
  341,  342,  343,  394,  345,  293,   41,  392,  383,  393,
  393,  299,  300,   41,  263,  303,  304,  305,  362,  269,
   33,  309,  347,  348,  349,  350,  351,  352,  353,  317,
  371,   44,  357,  358,  359,  393,  393,  380,  326,   10,
  372,  373,  374,  393,  274,  377,  378,  379,  123,  381,
  300,  393,  384,  385,  394,  393,  344,  268,  269,   41,
   41,  393,   33,  274,  829,   10,  277,  832,  393,  357,
  268,   41,  393,   44,  362,  285,  393,  365,  366,  367,
  269,  375,  370,  394,   61,   61,   61,  337,   10,  266,
  267,  268,  269,  395,  382,  383,   47,  274,  393,   47,
  277,  260,  390,  391,   10,  393,  394,  357,  393,  382,
  123,  270,  125,  268,  269,  365,  366,  367,  337,  269,
  279,  280,  277,  282,  283,  269,  337,   41,  266,  267,
  268,  269,  382,  383,  293,  394,  274,  393,   41,  277,
  299,  300,  394,  393,  303,  304,  305,   62,  394,  289,
  309,  393,  123,   47,  125,  123,  123,  394,  317,  394,
  337,  926,  300,  394,  929,  393,  382,  326,   40,  347,
  348,  349,  350,  351,  352,  353,  284,  284,  123,  357,
  358,  359,  337,  284,  284,  344,  394,  394,  382,   47,
  394,  217,  218,  219,  394,  394,  290,  291,  357,  337,
  285,  123,  312,  362,   10,  393,  365,  366,  367,  303,
   10,  370,  306,   47,  281,  393,   10,  123,  312,  357,
   40,  395,  281,  382,  383,  395,  393,  365,  366,  367,
  324,  390,  391,   33,  393,  394,  285,  394,   10,   33,
   41,  382,   47,   41,  382,  383,  394,  260,  393,   33,
  289,   41,   41,   41,  392,   41,  393,  270,   10,  286,
  287,  288,   10,  394,   41,   41,  279,  280,  362,  282,
  283,   41,  347,  348,  349,  350,  351,  352,  353,   10,
  293,   10,  357,  358,  359,  285,  299,  300,  382,  260,
  303,  304,  305,  333,   10,  389,  309,   41,  393,  270,
   10,  327,  393,   10,  317,   10,  393,  268,  279,  280,
   10,  282,  283,  326,   10,   10,  342,  382,  393,   10,
   10,   10,  293,  394,  350,   10,   71,    3,  299,  300,
   79,  344,  303,  304,  305,   17,  844,  958,  309,  281,
  612,  528,  506,   10,  357,   86,  317,  490,  499,  362,
  196,  826,  365,  366,  367,  326,  240,  370,  270,  300,
  544,  447,  455,  549,  536,  684,   33,  563,  803,  382,
  383,  374,  191,  344,  269,  730,  402,  390,  391,  727,
  393,  394,  129,  130,  131,  349,  357,  562,  415,  377,
   33,  362,  442,  420,  365,  366,  367,  703,   41,  370,
  902,  902,  347,  348,  349,  350,  351,  352,  353,  600,
    3,  382,  383,  358,  359,    3,   -1,  429,   -1,  390,
  391,   -1,  393,  394,   -1,  347,  348,  349,  350,  351,
  352,  353,   -1,   -1,  181,  357,  358,  359,   -1,   -1,
   -1,  347,  348,  349,  350,  351,  352,  353,  393,   -1,
  477,   -1,  358,  359,   -1,   -1,   -1,   -1,   -1,   -1,
  260,   -1,  262,  263,  264,  265,  266,  267,  268,  269,
  270,  393,   -1,   -1,  274,  275,  276,  277,   10,  279,
  280,   -1,  282,  283,   -1,   -1,   -1,  393,   -1,   -1,
   -1,   -1,   -1,  293,  294,  289,  296,  297,  298,  299,
  300,   33,   -1,  303,  304,  305,  290,  291,   -1,  309,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  317,   -1,  303,
   -1,   -1,  306,   -1,   -1,   -1,  326,  274,  312,   -1,
   -1,  278,   -1,   -1,   -1,   -1,   -1,  337,  285,   -1,
  324,   -1,   -1,   -1,  344,  339,  340,  341,  342,  343,
   -1,  345,   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,
   10,   -1,  362,   -1,   10,  365,  366,  367,  362,   -1,
  370,  598,   -1,   -1,   -1,   -1,   -1,  371,  362,   -1,
   -1,   -1,  382,  383,   -1,  332,   -1,   33,  382,   -1,
  390,  391,  392,  260,   -1,  262,  263,  264,  265,  266,
  267,  268,  269,  270,   -1,  389,   -1,  274,  275,  276,
  277,   10,  279,  280,   -1,  282,  283,   33,   -1,   -1,
   -1,   -1,   -1,   -1,   40,   -1,  293,  294,   44,  296,
  297,  298,  299,  300,   33,   -1,  303,  304,  305,   -1,
   -1,   -1,  309,   -1,   60,   61,   62,  290,  291,   10,
  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,
  303,   -1,   -1,  306,   -1,   -1,   -1,   -1,   -1,  312,
  337,   -1,   33,  123,   -1,   -1,   -1,  344,   -1,   -1,
   -1,  324,   -1,   10,   -1,   10,   -1,   -1,   -1,   -1,
  357,   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,  366,
  367,   -1,   -1,  370,   -1,   -1,   -1,  123,   33,  125,
   -1,   -1,   -1,   -1,   -1,  382,  383,   -1,   -1,  362,
   -1,   -1,   -1,  390,  391,  392,   -1,   -1,  260,   -1,
  262,  263,  264,  265,  266,  267,  268,  269,  270,  382,
   -1,   -1,  274,  275,  276,  277,  389,  279,  280,   -1,
  282,  283,   -1,   -1,   -1,   -1,   -1,   -1,   10,   -1,
   -1,  293,  294,   -1,  296,  297,  298,  299,  300,   -1,
   10,  303,  304,  305,   -1,   -1,   -1,  309,   -1,   -1,
   -1,   33,   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,  125,   -1,
   -1,   -1,   -1,   -1,   -1,  337,   -1,   -1,   -1,   10,
   -1,   -1,  344,   -1,   -1,   -1,   -1,  267,  268,  269,
   -1,  267,  268,  269,  274,  357,   -1,  277,  274,   -1,
  362,  277,   33,  365,  366,  367,   -1,   -1,  370,   -1,
   41,   -1,   -1,   44,   -1,   -1,   47,   -1,   -1,   -1,
  382,  383,   -1,   -1,  300,   -1,   -1,   -1,  390,  391,
  392,  260,  278,  262,  263,  264,  265,  266,  267,  268,
  269,  270,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
  279,  280,   -1,  282,  283,  301,  302,  337,   -1,   -1,
   -1,  337,   -1,   -1,  293,  294,   10,  296,  297,  298,
  299,  300,   -1,   -1,  303,  304,  305,  268,  269,   -1,
  309,  357,   -1,  274,   -1,   -1,  277,   -1,  317,  365,
  366,  367,  123,   -1,  125,   -1,   -1,  326,   -1,  256,
  257,  258,  259,   -1,   -1,   -1,  382,  383,  337,  300,
   -1,   10,   -1,  268,  269,  344,  392,   -1,  364,  274,
   -1,   -1,  277,   -1,   -1,   10,   -1,   -1,  357,   -1,
   -1,   -1,   -1,  362,   33,   -1,  365,  366,  367,   -1,
   -1,  370,  299,   -1,   -1,  300,  337,  393,  394,   -1,
   -1,   -1,   -1,  382,  383,   -1,  313,  314,  315,  316,
   -1,  390,  391,  392,   -1,   -1,  357,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  365,  366,  367,   -1,   -1,  123,
   -1,  263,  337,   -1,   -1,   -1,  268,  269,   -1,   -1,
   -1,  382,  383,  263,   -1,  277,   -1,   -1,  268,  269,
   -1,  392,  357,   -1,  274,  275,  276,  277,  290,  291,
  365,  366,  367,   -1,   -1,   -1,   -1,   -1,   10,   -1,
   -1,  303,   -1,   -1,  306,   -1,   -1,  382,  383,  260,
  312,   -1,  263,   -1,   -1,   -1,  267,  268,  269,  270,
  125,   33,  324,  274,  275,  276,  277,   10,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,  289,  290,
  291,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,
   33,   -1,  303,  304,  305,  306,   -1,   -1,  309,   -1,
  362,  312,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,
   -1,   -1,  362,  324,   -1,  326,   -1,   -1,   -1,   -1,
  382,   -1,   -1,   -1,   -1,   -1,  337,  389,  339,  340,
  341,  342,  343,  344,  345,   -1,   -1,   -1,  262,   -1,
  264,   -1,  266,  267,  268,  269,  357,   -1,   -1,   -1,
  274,  362,   -1,  277,  365,  366,  367,   -1,   -1,  370,
  371,   -1,   -1,  347,  348,  349,  350,  351,  352,  353,
   -1,  382,  383,  357,  358,  359,   -1,   -1,  389,  390,
  391,  260,   -1,   -1,  263,   10,   -1,  266,  267,  268,
  269,  270,  257,  258,  259,  274,  275,  276,  277,   -1,
  279,  280,   -1,  282,  283,   -1,   -1,   -1,   33,   -1,
  289,  290,  291,  337,  293,  294,   -1,  296,  297,  298,
  299,  300,   -1,   -1,  303,  304,  305,  306,   -1,   -1,
  309,   -1,   -1,  312,   -1,   -1,   -1,   -1,  317,   -1,
   -1,   -1,   -1,   -1,   -1,  324,   -1,  326,  313,  314,
  315,  316,   -1,   -1,  333,   -1,   -1,   -1,   -1,   -1,
  339,  340,  341,  342,  343,  344,  345,   -1,   -1,  347,
  348,  349,  350,  351,  352,  353,   -1,   -1,  357,  357,
  358,  359,   -1,  362,   -1,   -1,  365,  366,  367,   -1,
   -1,  370,  371,   -1,   -1,   -1,  268,  269,   -1,   -1,
   10,   -1,   -1,  382,  383,  277,   -1,   -1,   -1,   -1,
  389,  390,  391,   -1,   -1,   -1,   -1,  260,   -1,   -1,
  263,   -1,   -1,   33,  267,  268,  269,  270,  300,   -1,
   -1,  274,  275,  276,  277,   -1,  279,  280,   -1,  282,
  283,   -1,   -1,   -1,   -1,   -1,   -1,  290,  291,   -1,
  293,  294,   -1,  296,  297,  298,  299,  300,   -1,   -1,
  303,  304,  305,  306,   -1,  337,  309,   -1,   -1,  312,
   -1,   25,   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,
   -1,  324,   -1,  326,   -1,  357,   -1,   -1,   -1,   -1,
  333,   -1,   -1,  365,  366,  367,   -1,   -1,   -1,   -1,
   -1,  344,   -1,   -1,   -1,   -1,   -1,   61,   62,   63,
  382,  383,   -1,  123,  357,   -1,   -1,   -1,   10,  362,
   74,   -1,  365,  366,  367,   -1,   -1,  370,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  260,   90,   -1,  263,  382,
  383,   33,  267,  268,  269,  270,  389,  390,  391,  274,
  275,  276,  277,   -1,  279,  280,   -1,  282,  283,   -1,
   -1,   -1,   -1,   -1,   -1,  290,  291,   -1,  293,  294,
   -1,  296,  297,  298,  299,  300,   -1,   -1,  303,  304,
  305,  306,   -1,   -1,  309,   -1,   -1,  312,   -1,   -1,
   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,  324,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,  333,  347,
  348,  349,  350,  351,  352,  353,   -1,   -1,   -1,  344,
  358,  359,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  123,  357,   10,   -1,   -1,   -1,  362,   -1,   -1,
  365,  366,  367,   -1,   -1,  370,   -1,  201,  202,   -1,
  260,   -1,  206,  263,   -1,   -1,   33,  382,  383,  269,
  270,   -1,  216,   -1,  389,  390,  391,   -1,   -1,  279,
  280,   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,  289,
  290,  291,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,
  300,   -1,   -1,  303,  304,  305,  306,   -1,   -1,  309,
  254,   -1,  312,   -1,   -1,   -1,   -1,  317,   -1,   -1,
   -1,   -1,   -1,   -1,  324,   -1,  326,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  339,
  340,  341,  342,  343,  344,  345,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  123,  357,   -1,   -1,
   -1,  305,  362,   -1,   -1,  365,  366,  367,   -1,   -1,
  370,  371,   -1,   -1,   -1,   -1,   10,   -1,  260,   -1,
   -1,   -1,  382,  383,   -1,   -1,   -1,  269,  270,  389,
  390,  391,   -1,   -1,   -1,   -1,   -1,  279,  280,   33,
  282,  283,   -1,   -1,   -1,   -1,   -1,  289,  290,  291,
   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,
   -1,  303,  304,  305,  306,   -1,   -1,  309,   -1,   -1,
  312,   10,   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,
   -1,   -1,  324,   -1,  326,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   33,   -1,   -1,  339,  340,  341,
  342,  343,  344,  345,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,
  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,  371,
   -1,   -1,   -1,  260,   -1,   -1,   -1,   -1,   -1,   -1,
  382,  383,   -1,  270,   -1,   -1,   -1,  389,  390,  391,
   -1,   -1,  279,  280,   -1,  282,  283,   -1,   -1,   -1,
   -1,   -1,  289,  290,  291,   -1,  293,   -1,   -1,   -1,
   -1,   -1,  299,  300,   -1,   -1,  303,  304,  305,  306,
   -1,   -1,  309,   -1,   -1,  312,   -1,   -1,   -1,   -1,
  317,   -1,   -1,   -1,   -1,   -1,   10,  324,   -1,  326,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  339,  340,  341,  342,  343,  344,  345,   33,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  357,   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,  366,
  367,   -1,   -1,  370,  371,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  382,  383,   -1,   -1,   -1,
   -1,   -1,  389,  390,  391,   -1,  260,   -1,   -1,  263,
   -1,  265,  266,  267,  268,  269,  270,   -1,   -1,   -1,
  274,  275,  276,  277,   -1,  279,  280,   -1,  282,  283,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,
  294,   -1,  296,  297,  298,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,
   -1,  260,   -1,  317,  263,   -1,   -1,   -1,   10,  268,
  269,  270,  326,   -1,   -1,  274,  275,  276,  277,   -1,
  279,  280,   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,
  344,   33,   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,
  299,  300,   -1,  357,  303,  304,  305,   -1,  362,   -1,
  309,  365,  366,  367,   -1,   -1,  370,   -1,  317,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,  382,  383,
   -1,   -1,   -1,   10,   -1,   -1,  390,  391,   -1,   -1,
  339,  340,  341,  342,  343,  344,  345,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   33,   -1,  357,   -1,
   -1,   -1,   -1,  362,   -1,   -1,  365,  366,  367,   -1,
   -1,  370,  371,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  123,   -1,  382,  383,   -1,  260,   -1,   -1,  263,
   -1,  390,  391,   10,  268,  269,  270,   -1,   -1,   -1,
  274,  275,  276,  277,   -1,  279,  280,   -1,  282,  283,
   -1,   -1,   -1,   -1,   -1,   -1,   33,   -1,   -1,  293,
   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  339,  340,  341,  342,  343,
  344,  345,   -1,   -1,   -1,   -1,   -1,   -1,  307,   -1,
   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,
   -1,  365,  366,  367,   -1,   -1,  370,  371,   -1,  328,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  382,  383,
   -1,   -1,   -1,   -1,   -1,   -1,  390,  391,  260,   10,
  262,  263,  264,   -1,  266,  267,  268,  269,  270,   -1,
   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,   -1,
  282,  283,   33,  372,  373,  374,   -1,   -1,  377,  378,
  379,  293,  381,   -1,   -1,  384,  385,  299,  300,   -1,
   -1,  303,  304,  305,  393,   -1,   -1,  309,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  326,   -1,  263,   -1,  265,  266,
  267,  268,  269,   -1,   -1,   -1,   -1,  274,  275,  276,
  277,   -1,  344,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  290,  291,  357,   -1,   -1,   -1,   -1,
  362,   -1,   -1,  365,  366,  367,  303,   -1,  370,  306,
   -1,   -1,   -1,   -1,   -1,  312,   -1,   -1,   -1,   -1,
  382,  383,   -1,  260,   -1,   10,  263,  324,  390,  391,
  267,  268,  269,  270,   -1,   -1,   -1,  274,  275,  276,
  277,   -1,  279,  280,   -1,  282,  283,   -1,   33,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  293,  294,   -1,  296,
  297,  298,  299,  300,   -1,  362,  303,  304,  305,   -1,
   -1,   -1,  309,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  317,   -1,   -1,   -1,   -1,  382,   -1,  312,   -1,  326,
   -1,   -1,  389,  318,  319,  320,  321,  322,  323,   10,
  325,   -1,  327,   -1,  329,  330,  331,  344,   -1,   -1,
  335,  336,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  357,   -1,   33,   -1,   -1,  362,   -1,   -1,  365,  366,
  367,   -1,   -1,  370,   -1,   -1,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,  369,  382,  383,   -1,   -1,  260,
   10,   -1,  263,  390,  391,   -1,   -1,  268,  269,  270,
   -1,  386,  387,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   33,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  293,  294,   -1,  296,  297,  298,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  344,   -1,   -1,   -1,   -1,   -1,  307,
   -1,   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,   -1,
   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
  328,   -1,   -1,  123,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  382,  383,   -1,   -1,  260,   10,   -1,  263,  390,
  391,  266,  267,  268,  269,  270,   -1,   -1,   -1,  274,
  275,  276,  277,   -1,  279,  280,   -1,  282,  283,   33,
   -1,   -1,   -1,   -1,  372,  373,  374,   -1,  293,  377,
  378,  379,   -1,  381,  299,  300,  384,  385,  303,  304,
  305,   -1,   -1,   -1,  309,  393,   -1,   -1,   -1,   -1,
   -1,   10,  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  263,   -1,   33,   -1,  267,  268,  269,  344,
   -1,   -1,   -1,  274,  275,  276,  277,   -1,   -1,   -1,
   -1,   -1,  357,   -1,   -1,   -1,   -1,  362,  289,   -1,
  365,  366,  367,   -1,   -1,  370,   -1,   -1,   -1,  123,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  382,  383,   -1,
  260,   -1,   -1,  263,   -1,  390,  391,  267,  268,  269,
  270,   -1,   -1,   -1,  274,  275,  276,  277,   -1,  279,
  280,   -1,  282,  283,   -1,   -1,   -1,   -1,  339,  340,
  341,  342,  343,  293,  345,   -1,   -1,   -1,   -1,  299,
  300,   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,
   -1,  362,   -1,   -1,   -1,   10,   -1,  317,   -1,   -1,
  371,   -1,   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,
   -1,  382,  383,   -1,   -1,   -1,   -1,   -1,   33,   -1,
   -1,   -1,   -1,   -1,  344,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,
   -1,   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,
  370,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  382,  383,   -1,   -1,  260,   -1,   10,  263,
  390,  391,   -1,   -1,  268,  269,  270,   -1,   -1,   -1,
  274,  275,  276,  277,   -1,  279,  280,   -1,  282,  283,
   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,
   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   10,
   -1,  260,   -1,  317,  263,   -1,   -1,   -1,   -1,  268,
  269,  270,  326,   -1,   -1,  274,  275,  276,  277,   -1,
  279,  280,   33,  282,  283,   -1,   -1,   -1,   -1,   -1,
  344,   -1,   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,
  299,  300,   -1,  357,  303,  304,  305,   -1,  362,   -1,
  309,  365,  366,  367,   -1,   -1,  370,   -1,  317,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,  382,  383,
   -1,   -1,   -1,   -1,   -1,   -1,  390,  391,   -1,   -1,
   -1,   10,   -1,   -1,   -1,  344,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  357,   -1,
   -1,   -1,   -1,  362,   33,   -1,  365,  366,  367,   -1,
   -1,  370,  123,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  382,  383,  260,   -1,   -1,  263,   -1,
   -1,  390,  391,  268,  269,  270,   10,   -1,   -1,  274,
  275,  276,  277,   -1,  279,  280,   -1,  282,  283,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,   33,
   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,  304,
  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   10,   -1,
   -1,  263,   -1,   -1,   -1,  267,  268,  269,   -1,  344,
   -1,   -1,  274,  275,  276,  277,   -1,   -1,   -1,   -1,
   -1,   33,  357,   -1,   -1,   -1,   -1,  362,  290,  291,
  365,  366,  367,   -1,   -1,  370,   -1,   -1,   -1,   -1,
   -1,  303,   -1,   -1,  306,   -1,   -1,  382,  383,  260,
  312,   -1,  263,   10,   -1,  390,  391,  268,  269,  270,
   -1,   -1,  324,  274,   -1,   -1,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   33,   -1,   -1,   -1,
   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,   -1,
  362,   -1,   -1,   -1,   -1,   -1,  317,   -1,   10,   -1,
   -1,   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,   -1,
  382,   -1,   -1,   -1,   -1,   -1,   -1,  389,   -1,   -1,
   -1,   33,   -1,  344,  263,   -1,   -1,   -1,   -1,  268,
  269,   -1,   -1,   -1,   -1,  274,  357,   -1,  277,   -1,
   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
  289,   -1,   -1,   -1,   -1,   -1,  123,   -1,   -1,   -1,
   -1,  382,  383,   -1,   -1,   -1,   -1,   -1,   -1,  390,
  391,   -1,   -1,   -1,   -1,   -1,  260,   -1,   -1,  263,
   -1,   -1,   -1,   -1,  268,  269,  270,   -1,   -1,   -1,
  274,   -1,   -1,  277,   -1,  279,  280,   -1,  282,  283,
  339,  340,  341,  342,  343,   -1,  345,   -1,   -1,  293,
   -1,   -1,   -1,   -1,   10,  299,  300,   -1,   -1,  303,
  304,  305,   -1,  362,   -1,  309,   -1,   -1,   -1,   -1,
   -1,   -1,  371,  317,   -1,   -1,   -1,   33,   -1,   -1,
   -1,  263,  326,  382,  383,   -1,   10,  269,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  344,   -1,   -1,   -1,   -1,   -1,   -1,  289,   -1,   33,
   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,
   -1,  365,  366,  367,   -1,   -1,  370,   -1,   -1,   -1,
   -1,   -1,   -1,  260,   -1,   -1,  263,   -1,  382,  383,
   -1,  268,  269,  270,   -1,   -1,  390,  391,   -1,   -1,
  277,   -1,  279,  280,   -1,  282,  283,  339,  340,  341,
  342,  343,   -1,  345,   -1,   -1,  293,  123,   -1,   -1,
   -1,   -1,  299,  300,   -1,   -1,  303,  304,  305,   10,
  362,   -1,  309,   -1,   -1,   -1,   -1,   -1,  260,  371,
  317,  263,   -1,   -1,   -1,   -1,  268,  269,  270,  326,
  382,  383,   33,   -1,   -1,  277,   -1,  279,  280,   -1,
  282,  283,   -1,   -1,   -1,   -1,   -1,  344,   10,   -1,
   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,
  357,  303,  304,  305,   -1,  362,   -1,  309,  365,  366,
  367,   33,   -1,  370,   -1,  317,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  326,  382,  383,   -1,   -1,   -1,
   -1,   -1,   -1,  390,  391,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  344,   10,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,
  362,   -1,  123,  365,  366,  367,   33,   -1,  370,   -1,
   -1,   -1,   -1,   -1,  260,   10,   -1,  263,   -1,   -1,
  382,  383,   -1,  269,  270,   -1,   -1,   -1,  390,  391,
   -1,   -1,   -1,  279,  280,   -1,  282,  283,   33,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  260,  293,   -1,  263,
   -1,   -1,   -1,  299,  300,  269,  270,  303,  304,  305,
   -1,   -1,   -1,  309,   -1,  279,  280,   -1,  282,  283,
   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,
  326,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,  123,   -1,  344,   -1,
   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,   10,   -1,
   -1,  357,  326,   -1,   -1,   -1,  362,   -1,   -1,  365,
  366,  367,   -1,   -1,  370,   -1,   -1,   -1,  123,   -1,
  344,   33,   -1,   -1,   -1,   -1,  382,  383,   -1,  260,
   -1,   -1,   -1,  357,  390,  391,   -1,   10,  362,  270,
   -1,  365,  366,  367,   -1,   -1,  370,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,  382,  383,
   33,   -1,  293,   -1,   -1,   -1,  390,  391,  299,  300,
   10,  263,  303,  304,  305,   -1,  268,  269,  309,   -1,
   -1,   -1,  274,  275,  276,  277,  317,   -1,   -1,   -1,
   -1,   -1,   -1,   33,   -1,  326,   -1,   -1,  290,  291,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  303,   -1,  344,  306,   -1,   -1,   -1,   -1,   -1,
  312,   -1,   -1,  260,   -1,   -1,  357,   10,   -1,   -1,
   -1,  362,  324,  270,  365,  366,  367,   -1,   -1,  370,
   -1,   -1,  279,  280,   -1,  282,  283,   -1,   -1,   -1,
   33,  382,  383,   -1,   -1,  260,  293,   -1,   -1,  390,
  391,   10,  299,  300,   -1,  270,  303,  304,  305,   -1,
  362,   -1,  309,   -1,  279,  280,   -1,  282,  283,   -1,
  317,   -1,   -1,   -1,   33,   -1,   -1,   -1,  293,  326,
  382,   -1,   -1,   -1,  299,  300,   -1,  389,  303,  304,
  305,   -1,   -1,   -1,  309,   -1,   -1,  344,   -1,   -1,
   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  357,  326,   -1,   -1,   -1,  362,   -1,   -1,  365,  366,
  367,   -1,   -1,  370,   -1,   -1,   -1,   -1,   -1,  344,
   -1,   -1,   -1,   -1,   -1,  382,  383,   -1,  260,   -1,
   -1,   -1,  357,  390,  391,   -1,   -1,  362,  270,   -1,
  365,  366,  367,   -1,   -1,  370,   -1,  279,  280,   -1,
  282,  283,   -1,   -1,   -1,   -1,   -1,  382,  383,   33,
   -1,  293,   -1,   -1,   -1,  390,  391,  299,  300,   -1,
  263,  303,  304,  305,   -1,  268,  269,  309,   -1,   -1,
   -1,  274,   -1,   -1,  277,  317,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  326,   -1,   -1,  290,  291,   -1,
   -1,   -1,   -1,  263,   -1,   -1,   -1,  267,  268,  269,
  303,   -1,  344,  306,  274,  275,  276,  277,   -1,  312,
   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,   -1,  289,
  362,  324,   -1,  365,  366,  367,   -1,   -1,  370,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  382,  383,   -1,   -1,   -1,   -1,   -1,   -1,  390,  391,
  263,   -1,   -1,   -1,   -1,  268,  269,   -1,   -1,  362,
   -1,  274,  275,  276,  277,   -1,   -1,   -1,   -1,  339,
  340,  341,  342,  343,   -1,  345,  289,   -1,   -1,  382,
   -1,   -1,   -1,   -1,  263,   -1,  389,   -1,   -1,  268,
  269,   -1,  362,   -1,   -1,   -1,   -1,   -1,  277,   -1,
   -1,  371,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  289,   -1,  382,  383,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  339,  340,  341,  342,
  343,   -1,  345,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  362,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  371,   -1,
  339,  340,  341,  342,  343,   -1,  345,   -1,   -1,  382,
  383,   -1,   -1,   -1,   -1,   -1,  260,   -1,   -1,   -1,
   -1,   -1,   -1,  362,   -1,   -1,  270,   -1,   -1,   -1,
   -1,   -1,  371,   -1,   -1,  279,  280,   -1,  282,  283,
   -1,   -1,   -1,  382,  383,   -1,   -1,   -1,   -1,  293,
   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  344,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,
   -1,  365,  366,  367,   -1,   -1,  370,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  383,
   -1,   -1,   -1,   -1,   -1,   -1,  390,  391,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 395
#define YYUNDFTOKEN 595
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
0,0,0,0,0,0,0,0,"PASS","BLOCK","MATCH","SCRUB","RETURN","IN","OS","OUT","LOG",
"QUICK","ON","FROM","TO","FLAGS","RETURNRST","RETURNICMP","RETURNICMP6","PROTO",
"INET","INET6","ALL","ANY","ICMPTYPE","ICMP6TYPE","CODE","KEEP","MODULATE",
"STATE","PORT","RDR","NAT","BINAT","ARROW","NODF","MINTTL","ERROR","ALLOWOPTS",
"FASTROUTE","FILENAME","ROUTETO","DUPTO","REPLYTO","NO","LABEL","NOROUTE",
"URPFFAILED","FRAGMENT","USER","GROUP","MAXMSS","MAXIMUM","TTL","TOS","DROP",
"TABLE","REASSEMBLE","ANCHOR","NATANCHOR","RDRANCHOR","BINATANCHOR","SET",
"OPTIMIZATION","TIMEOUT","LIMIT","LOGINTERFACE","BLOCKPOLICY","FAILPOLICY",
"RANDOMID","REQUIREORDER","SYNPROXY","FINGERPRINTS","NOSYNC","DEBUG","SKIP",
"HOSTID","ANTISPOOF","FOR","INCLUDE","KEEPCOUNTERS","SYNCOOKIES","L3","ETHER",
"BITMASK","RANDOM","SOURCEHASH","ROUNDROBIN","STATICPORT","PROBABILITY",
"MAPEPORTSET","ALTQ","CBQ","CODEL","PRIQ","HFSC","FAIRQ","BANDWIDTH","TBRSIZE",
"LINKSHARE","REALTIME","UPPERLIMIT","QUEUE","PRIORITY","QLIMIT","HOGS",
"BUCKETS","RTABLE","TARGET","INTERVAL","DNPIPE","DNQUEUE","RIDENTIFIER","LOAD",
"RULESET_OPTIMIZATION","PRIO","STICKYADDRESS","MAXSRCSTATES","MAXSRCNODES",
"SOURCETRACK","GLOBAL","RULE","MAXSRCCONN","MAXSRCCONNRATE","OVERLOAD","FLUSH",
"SLOPPY","TAGGED","TAG","IFBOUND","FLOATING","STATEPOLICY","STATEDEFAULTS",
"ROUTE","SETTOS","DIVERTTO","DIVERTREPLY","BRIDGE_TO","STRING","NUMBER",
"PORTBINARY",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"illegal-symbol",
};
static const char *const pfctlyrule[] = {
"$accept : ruleset",
"ruleset :",
"ruleset : ruleset include '\\n'",
"ruleset : ruleset '\\n'",
"ruleset : ruleset option '\\n'",
"ruleset : ruleset etherrule '\\n'",
"ruleset : ruleset etheranchorrule '\\n'",
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
"optnodf :",
"optnodf : NODF",
"option : SET REASSEMBLE yesno optnodf",
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
"option : SET KEEPCOUNTERS",
"option : SET SYNCOOKIES syncookie_val syncookie_opts",
"syncookie_val : STRING",
"syncookie_opts :",
"$$1 :",
"syncookie_opts : $$1 '(' syncookie_opt_l ')'",
"syncookie_opt_l : syncookie_opt_l comma syncookie_opt",
"syncookie_opt_l : syncookie_opt",
"syncookie_opt : STRING STRING",
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
"$$2 :",
"pfa_anchor : '{' $$2 '\\n' pfa_anchorlist '}'",
"pfa_anchor :",
"anchorrule : ANCHOR anchorname dir quick interface af proto fromto filter_opts pfa_anchor",
"anchorrule : NATANCHOR string interface af proto fromto rtable",
"anchorrule : RDRANCHOR string interface af proto fromto rtable",
"anchorrule : BINATANCHOR string interface af proto fromto rtable",
"loadrule : LOAD ANCHOR string FROM string",
"scrubaction : no SCRUB",
"etherrule : ETHER action dir quick interface bridge etherproto etherfromto l3fromto etherfilter_opts",
"etherpfa_anchorlist :",
"etherpfa_anchorlist : etherpfa_anchorlist '\\n'",
"etherpfa_anchorlist : etherpfa_anchorlist etherrule '\\n'",
"etherpfa_anchorlist : etherpfa_anchorlist etheranchorrule '\\n'",
"$$3 :",
"etherpfa_anchor : '{' $$3 '\\n' etherpfa_anchorlist '}'",
"etherpfa_anchor :",
"etheranchorrule : ETHER ANCHOR anchorname dir quick interface etherproto etherfromto l3fromto etherpfa_anchor",
"$$4 :",
"etherfilter_opts : $$4 etherfilter_opts_l",
"etherfilter_opts :",
"etherfilter_opts_l : etherfilter_opts_l etherfilter_opt",
"etherfilter_opts_l : etherfilter_opt",
"etherfilter_opt : etherqname",
"etherfilter_opt : RIDENTIFIER number",
"etherfilter_opt : label",
"etherfilter_opt : TAG string",
"etherfilter_opt : not TAGGED string",
"etherfilter_opt : DNPIPE number",
"etherfilter_opt : DNQUEUE number",
"bridge :",
"bridge : BRIDGE_TO STRING",
"scrubrule : scrubaction dir logquick interface af proto fromto scrub_opts",
"$$5 :",
"scrub_opts : $$5 scrub_opts_l",
"scrub_opts :",
"scrub_opts_l : scrub_opts_l comma scrub_opt",
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
"fragcache : FRAGMENT NO REASSEMBLE",
"antispoof : ANTISPOOF logquick antispoof_ifspc af antispoof_opts",
"antispoof_ifspc : FOR antispoof_if",
"antispoof_ifspc : FOR '{' optnl antispoof_iflst '}'",
"antispoof_iflst : antispoof_if optnl",
"antispoof_iflst : antispoof_iflst comma antispoof_if optnl",
"antispoof_if : if_item",
"antispoof_if : '(' if_item ')'",
"$$6 :",
"antispoof_opts : $$6 antispoof_opts_l",
"antispoof_opts :",
"antispoof_opts_l : antispoof_opts_l antispoof_opt",
"antispoof_opts_l : antispoof_opt",
"antispoof_opt : label",
"antispoof_opt : RIDENTIFIER number",
"antispoof_opt : RTABLE NUMBER",
"not : '!'",
"not :",
"tabledef : TABLE '<' STRING '>' table_opts",
"$$7 :",
"table_opts : $$7 table_opts_l",
"table_opts :",
"table_opts_l : table_opts_l table_opt",
"table_opts_l : table_opt",
"table_opt : STRING",
"table_opt : '{' optnl '}'",
"table_opt : '{' optnl host_list '}'",
"table_opt : FILENAME STRING",
"altqif : ALTQ interface queue_opts QUEUE qassign",
"queuespec : QUEUE STRING interface queue_opts qassign",
"$$8 :",
"queue_opts : $$8 queue_opts_l",
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
"$$9 :",
"hfsc_opts : $$9 hfscopts_list",
"hfscopts_list : hfscopts_item",
"hfscopts_list : hfscopts_list comma hfscopts_item",
"hfscopts_item : LINKSHARE bandwidth",
"hfscopts_item : LINKSHARE '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : REALTIME bandwidth",
"hfscopts_item : REALTIME '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : UPPERLIMIT bandwidth",
"hfscopts_item : UPPERLIMIT '(' bandwidth comma NUMBER comma bandwidth ')'",
"hfscopts_item : STRING",
"$$10 :",
"fairq_opts : $$10 fairqopts_list",
"fairqopts_list : fairqopts_item",
"fairqopts_list : fairqopts_list comma fairqopts_item",
"fairqopts_item : LINKSHARE bandwidth",
"fairqopts_item : LINKSHARE '(' bandwidth number bandwidth ')'",
"fairqopts_item : HOGS bandwidth",
"fairqopts_item : BUCKETS number",
"fairqopts_item : STRING",
"$$11 :",
"codel_opts : $$11 codelopts_list",
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
"$$12 :",
"filter_opts : $$12 filter_opts_l",
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
"filter_opt : RIDENTIFIER number",
"filter_opt : FRAGMENT",
"filter_opt : ALLOWOPTS",
"filter_opt : label",
"filter_opt : qname",
"filter_opt : DNPIPE number",
"filter_opt : DNPIPE '(' number ')'",
"filter_opt : DNPIPE '(' number comma number ')'",
"filter_opt : DNQUEUE number",
"filter_opt : DNQUEUE '(' number comma number ')'",
"filter_opt : DNQUEUE '(' number ')'",
"filter_opt : TAG string",
"filter_opt : not TAGGED string",
"filter_opt : PROBABILITY probability",
"filter_opt : RTABLE NUMBER",
"filter_opt : DIVERTTO portplain",
"filter_opt : DIVERTTO STRING PORT portplain",
"filter_opt : DIVERTREPLY",
"filter_opt : SCRUB '(' scrub_opts ')'",
"filter_opt : filter_sets",
"filter_sets : SET '(' filter_sets_l ')'",
"filter_sets : SET filter_set",
"filter_sets_l : filter_sets_l comma filter_set",
"filter_sets_l : filter_set",
"filter_set : prio",
"filter_set : TOS tos",
"prio : PRIO NUMBER",
"prio : PRIO '(' NUMBER comma NUMBER ')'",
"probability : STRING",
"probability : NUMBER",
"action : PASS",
"action : MATCH",
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
"etherproto :",
"etherproto : PROTO etherproto_item",
"etherproto : PROTO '{' optnl etherproto_list '}'",
"etherproto_list : etherproto_item optnl",
"etherproto_list : etherproto_list comma etherproto_item optnl",
"etherproto_item : etherprotoval",
"etherprotoval : NUMBER",
"etherprotoval : STRING",
"proto :",
"proto : PROTO proto_item",
"proto : PROTO '{' optnl proto_list '}'",
"proto_list : proto_item optnl",
"proto_list : proto_list comma proto_item optnl",
"proto_item : protoval",
"protoval : STRING",
"protoval : NUMBER",
"l3fromto :",
"l3fromto : L3 fromto",
"etherfromto : ALL",
"etherfromto : etherfrom etherto",
"etherfrom :",
"etherfrom : FROM macspec",
"etherto :",
"etherto : TO macspec",
"mac : string '/' NUMBER",
"mac : string",
"xmac : not mac",
"macspec : xmac",
"macspec : '{' optnl mac_list '}'",
"mac_list : xmac optnl",
"mac_list : mac_list comma xmac",
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
"etherqname : QUEUE STRING",
"etherqname : QUEUE '(' STRING ')'",
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
"$$13 :",
"pool_opts : $$13 pool_opts_l",
"pool_opts :",
"pool_opts_l : pool_opts_l pool_opt",
"pool_opts_l : pool_opt",
"pool_opt : BITMASK",
"pool_opt : RANDOM",
"pool_opt : SOURCEHASH hashkey",
"pool_opt : ROUNDROBIN",
"pool_opt : STATICPORT",
"pool_opt : STICKYADDRESS",
"pool_opt : MAPEPORTSET number '/' number '/' number",
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
#line 5227 "parse.y"
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
rule_consistent(struct pfctl_rule *r, int anchor_call)
{
	int	problems = 0;

	switch (r->action) {
	case PF_PASS:
	case PF_MATCH:
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
filter_consistent(struct pfctl_rule *r, int anchor_call)
{
	int	problems = 0;

	if (r->proto != IPPROTO_TCP && r->proto != IPPROTO_UDP &&
	    r->proto != IPPROTO_SCTP &&
	    (r->src.port_op || r->dst.port_op)) {
		yyerror("port only applies to tcp/udp/sctp");
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
	if (r->action != PF_PASS && r->keep_state) {
		yyerror("keep state is great, but only for pass rules");
		problems++;
	}
	if (r->rule_flag & PFRULE_STATESLOPPY &&
	    (r->keep_state == PF_STATE_MODULATE ||
	    r->keep_state == PF_STATE_SYNPROXY)) {
		yyerror("sloppy state matching cannot be used with "
		    "synproxy state or modulate state");
		problems++;
	}
	/* match rules rules */
	if (r->action == PF_MATCH) {
		if (r->divert.port) {
			yyerror("divert is not supported on match rules");
			problems++;
		}
		if (r->rt) {
			yyerror("route-to, reply-to, dup-to and fastroute "
			   "must not be used on match rules");
			problems++;
		}
	}
	return (-problems);
}

int
nat_consistent(struct pfctl_rule *r)
{
	return (0);	/* yeah! */
}

int
rdr_consistent(struct pfctl_rule *r)
{
	int			 problems = 0;

	if (r->proto != IPPROTO_TCP && r->proto != IPPROTO_UDP &&
	    r->proto != IPPROTO_SCTP) {
		if (r->src.port_op) {
			yyerror("src port only applies to tcp/udp/sctp");
			problems++;
		}
		if (r->dst.port_op) {
			yyerror("dst port only applies to tcp/udp/sctp");
			problems++;
		}
		if (r->rpool.proxy_port[0]) {
			yyerror("rpool port only applies to tcp/udp/sctp");
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
	unsigned long		 maxcount;
	size_t			 s = sizeof(maxcount);

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

		if (sysctlbyname("net.pf.request_maxcount", &maxcount, &s,
		    NULL, 0) == -1)
			maxcount = 65535;

		if (ab.pfrb_size > maxcount)
			yyerror("cannot define table %s: too many elements.\n"
			    "Consider increasing net.pf.request_maxcount.",
			    name);
		else
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
    struct pf_rule_addr *addr)
{
	char tmp[64], tmp_not[66];

	if (strstr(label, name) != NULL) {
		switch (addr->addr.type) {
		case PF_ADDR_DYNIFTL:
			snprintf(tmp, sizeof(tmp), "(%s)", addr->addr.v.ifname);
			break;
		case PF_ADDR_TABLE:
			snprintf(tmp, sizeof(tmp), "<%s>", addr->addr.v.tblname);
			break;
		case PF_ADDR_NOROUTE:
			snprintf(tmp, sizeof(tmp), "no-route");
			break;
		case PF_ADDR_URPFFAILED:
			snprintf(tmp, sizeof(tmp), "urpf-failed");
			break;
		case PF_ADDR_ADDRMASK:
			if (!af || (PF_AZERO(&addr->addr.v.a.addr, af) &&
			    PF_AZERO(&addr->addr.v.a.mask, af)))
				snprintf(tmp, sizeof(tmp), "any");
			else {
				char	a[48];
				int	bits;

				if (inet_ntop(af, &addr->addr.v.a.addr, a,
				    sizeof(a)) == NULL)
					snprintf(tmp, sizeof(tmp), "?");
				else {
					bits = unmask(&addr->addr.v.a.mask, af);
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

		if (addr->neg) {
			snprintf(tmp_not, sizeof(tmp_not), "! %s", tmp);
			expand_label_str(label, len, name, tmp_not);
		} else
			expand_label_str(label, len, name, tmp);
	}
}

void
expand_label_port(const char *name, char *label, size_t len,
    struct pf_rule_addr *addr)
{
	char	 a1[6], a2[6], op[13] = "";

	if (strstr(label, name) != NULL) {
		snprintf(a1, sizeof(a1), "%u", ntohs(addr->port[0]));
		snprintf(a2, sizeof(a2), "%u", ntohs(addr->port[1]));
		if (!addr->port_op)
			;
		else if (addr->port_op == PF_OP_IRG)
			snprintf(op, sizeof(op), "%s><%s", a1, a2);
		else if (addr->port_op == PF_OP_XRG)
			snprintf(op, sizeof(op), "%s<>%s", a1, a2);
		else if (addr->port_op == PF_OP_EQ)
			snprintf(op, sizeof(op), "%s", a1);
		else if (addr->port_op == PF_OP_NE)
			snprintf(op, sizeof(op), "!=%s", a1);
		else if (addr->port_op == PF_OP_LT)
			snprintf(op, sizeof(op), "<%s", a1);
		else if (addr->port_op == PF_OP_LE)
			snprintf(op, sizeof(op), "<=%s", a1);
		else if (addr->port_op == PF_OP_GT)
			snprintf(op, sizeof(op), ">%s", a1);
		else if (addr->port_op == PF_OP_GE)
			snprintf(op, sizeof(op), ">=%s", a1);
		expand_label_str(label, len, name, op);
	}
}

void
expand_label_proto(const char *name, char *label, size_t len, u_int8_t proto)
{
	const char *protoname;
	char n[4];

	if (strstr(label, name) != NULL) {
		protoname = pfctl_proto2name(proto);
		if (protoname != NULL)
			expand_label_str(label, len, name, protoname);
		else {
			snprintf(n, sizeof(n), "%u", proto);
			expand_label_str(label, len, name, n);
		}
	}
}

void
expand_label_nr(const char *name, char *label, size_t len,
    struct pfctl_rule *r)
{
	char n[11];

	if (strstr(label, name) != NULL) {
		snprintf(n, sizeof(n), "%u", r->nr);
		expand_label_str(label, len, name, n);
	}
}

void
expand_label(char *label, size_t len, struct pfctl_rule *r)
{
	expand_label_if("$if", label, len, r->ifname);
	expand_label_addr("$srcaddr", label, len, r->af, &r->src);
	expand_label_addr("$dstaddr", label, len, r->af, &r->dst);
	expand_label_port("$srcport", label, len, &r->src);
	expand_label_port("$dstport", label, len, &r->dst);
	expand_label_proto("$proto", label, len, r->proto);
	expand_label_nr("$nr", label, len, r);
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
			    pa.scheduler == ALTQT_HFSC ||
			    pa.scheduler == ALTQT_FAIRQ) {
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
				    pa.scheduler == ALTQT_HFSC ||
				    pa.scheduler == ALTQT_FAIRQ)
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

static int
pf_af_to_proto(sa_family_t af)
{
	if (af == AF_INET)
		return (ETHERTYPE_IP);
	if (af == AF_INET6)
		return (ETHERTYPE_IPV6);

	return (0);
}

void
expand_eth_rule(struct pfctl_eth_rule *r,
    struct node_if *interfaces, struct node_etherproto *protos,
    struct node_mac *srcs, struct node_mac *dsts,
    struct node_host *ipsrcs, struct node_host *ipdsts,
    const char *bridge_to, const char *anchor_call)
{
	char tagname[PF_TAG_NAME_SIZE];
	char match_tagname[PF_TAG_NAME_SIZE];
	char qname[PF_QNAME_SIZE];

	if (strlcpy(tagname, r->tagname, sizeof(tagname)) >= sizeof(tagname))
		errx(1, "expand_eth_rule: tagname");
	if (strlcpy(match_tagname, r->match_tagname, sizeof(match_tagname)) >=
	    sizeof(match_tagname))
		errx(1, "expand_eth_rule: match_tagname");
	if (strlcpy(qname, r->qname, sizeof(qname)) >= sizeof(qname))
		errx(1, "expand_eth_rule: qname");

	LOOP_THROUGH(struct node_if, interface, interfaces,
	LOOP_THROUGH(struct node_etherproto, proto, protos,
	LOOP_THROUGH(struct node_mac, src, srcs,
	LOOP_THROUGH(struct node_mac, dst, dsts,
	LOOP_THROUGH(struct node_host, ipsrc, ipsrcs,
	LOOP_THROUGH(struct node_host, ipdst, ipdsts,
		strlcpy(r->ifname, interface->ifname,
		    sizeof(r->ifname));
		r->ifnot = interface->not;
		r->proto = proto->proto;
		if (!r->proto && ipsrc->af)
			r->proto = pf_af_to_proto(ipsrc->af);
		else if (!r->proto && ipdst->af)
			r->proto = pf_af_to_proto(ipdst->af);
		bcopy(src->mac, r->src.addr, ETHER_ADDR_LEN);
		bcopy(src->mask, r->src.mask, ETHER_ADDR_LEN);
		r->src.neg = src->neg;
		r->src.isset = src->isset;
		r->ipsrc.addr = ipsrc->addr;
		r->ipsrc.neg = ipsrc->not;
		r->ipdst.addr = ipdst->addr;
		r->ipdst.neg = ipdst->not;
		bcopy(dst->mac, r->dst.addr, ETHER_ADDR_LEN);
		bcopy(dst->mask, r->dst.mask, ETHER_ADDR_LEN);
		r->dst.neg = dst->neg;
		r->dst.isset = dst->isset;
		r->nr = pf->eastack[pf->asd]->match++;

		if (strlcpy(r->tagname, tagname, sizeof(r->tagname)) >=
		    sizeof(r->tagname))
			errx(1, "expand_eth_rule: r->tagname");
		if (strlcpy(r->match_tagname, match_tagname,
		    sizeof(r->match_tagname)) >= sizeof(r->match_tagname))
			errx(1, "expand_eth_rule: r->match_tagname");
		if (strlcpy(r->qname, qname, sizeof(r->qname)) >= sizeof(r->qname))
			errx(1, "expand_eth_rule: r->qname");

		if (bridge_to)
			strlcpy(r->bridge_to, bridge_to, sizeof(r->bridge_to));

		pfctl_append_eth_rule(pf, r, anchor_call);
	))))));

	FREE_LIST(struct node_if, interfaces);
	FREE_LIST(struct node_etherproto, protos);
	FREE_LIST(struct node_mac, srcs);
	FREE_LIST(struct node_mac, dsts);
	FREE_LIST(struct node_host, ipsrcs);
	FREE_LIST(struct node_host, ipdsts);
}

void
expand_rule(struct pfctl_rule *r,
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
	char			 label[PF_RULE_MAX_LABEL_COUNT][PF_RULE_LABEL_SIZE];
	char			 tagname[PF_TAG_NAME_SIZE];
	char			 match_tagname[PF_TAG_NAME_SIZE];
	struct pf_pooladdr	*pa;
	struct node_host	*h, *osrch, *odsth;
	u_int8_t		 flags, flagset, keep_state;

	memcpy(label, r->label, sizeof(r->label));
	assert(sizeof(r->label) == sizeof(label));
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

		memcpy(r->label, label, sizeof(r->label));
		if (strlcpy(r->tagname, tagname, sizeof(r->tagname)) >=
		    sizeof(r->tagname))
			errx(1, "expand_rule: strlcpy");
		if (strlcpy(r->match_tagname, match_tagname,
		    sizeof(r->match_tagname)) >= sizeof(r->match_tagname))
			errx(1, "expand_rule: strlcpy");

		osrch = odsth = NULL;
		if (src_host->addr.type == PF_ADDR_DYNIFTL) {
			osrch = src_host;
			if ((src_host = gen_dynnode(src_host, r->af)) == NULL)
				err(1, "expand_rule: calloc");
		}
		if (dst_host->addr.type == PF_ADDR_DYNIFTL) {
			odsth = dst_host;
			if ((dst_host = gen_dynnode(dst_host, r->af)) == NULL)
				err(1, "expand_rule: calloc");
		}

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
			pfctl_append_rule(pf, r, anchor_call);
			added++;
		}

		if (osrch && src_host->addr.type == PF_ADDR_DYNIFTL) {
			free(src_host);
			src_host = osrch;
		}
		if (odsth && dst_host->addr.type == PF_ADDR_DYNIFTL) {
			free(dst_host);
			dst_host = odsth;
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
		yyerror("Rules must be in order: options, ethernet, "
		    "normalization, queueing, translation, filtering");
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
		{ "bridge-to",		BRIDGE_TO},
		{ "buckets",		BUCKETS},
		{ "cbq",		CBQ},
		{ "code",		CODE},
		{ "codelq",		CODEL},
		{ "debug",		DEBUG},
		{ "divert-reply",	DIVERTREPLY},
		{ "divert-to",		DIVERTTO},
		{ "dnpipe",		DNPIPE},
		{ "dnqueue",		DNQUEUE},
		{ "drop",		DROP},
		{ "dup-to",		DUPTO},
		{ "ether",		ETHER},
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
		{ "keepcounters",	KEEPCOUNTERS},
		{ "l3",			L3},
		{ "label",		LABEL},
		{ "limit",		LIMIT},
		{ "linkshare",		LINKSHARE},
		{ "load",		LOAD},
		{ "log",		LOG},
		{ "loginterface",	LOGINTERFACE},
		{ "map-e-portset",	MAPEPORTSET},
		{ "match",		MATCH},
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
		{ "ridentifier",	RIDENTIFIER},
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
		{ "syncookies",         SYNCOOKIES},
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
				else if (next == '\n') {
					file->lineno++;
					continue;
				}
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
mv_rules(struct pfctl_ruleset *src, struct pfctl_ruleset *dst)
{
	int i;
	struct pfctl_rule *r;

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
mv_eth_rules(struct pfctl_eth_ruleset *src, struct pfctl_eth_ruleset *dst)
{
	struct pfctl_eth_rule *r;

	while ((r = TAILQ_FIRST(&src->rules)) != NULL) {
		TAILQ_REMOVE(&src->rules, r, entries);
		TAILQ_INSERT_TAIL(&dst->rules, r, entries);
		dst->anchor->match++;
	}
	src->anchor->match = 0;
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
		if (s == NULL)
			s = getservbyname(n, "sctp");
		if (s == NULL) {
			yyerror("unknown port %s", n);
			return (-1);
		}
		return (s->s_port);
	}
}

int
rule_label(struct pfctl_rule *r, char *s[PF_RULE_MAX_LABEL_COUNT])
{
	for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++) {
		if (s[i] == NULL)
			return (0);

		if (strlcpy(r->label[i], s[i], sizeof(r->label[0])) >=
		    sizeof(r->label[0])) {
			yyerror("rule label too long (max %d chars)",
			    sizeof(r->label[0])-1);
			return (-1);
		}
	}
	return (0);
}

int
eth_rule_label(struct pfctl_eth_rule *r, char *s[PF_RULE_MAX_LABEL_COUNT])
{
	for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++) {
		if (s[i] == NULL)
			return (0);

		if (strlcpy(r->label[i], s[i], sizeof(r->label[0])) >=
		    sizeof(r->label[0])) {
			yyerror("rule label too long (max %d chars)",
			    sizeof(r->label[0])-1);
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
		{ "throughput",		IPTOS_THROUGHPUT },
		{ "va",			IPTOS_DSCP_VA }
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

struct node_mac*
node_mac_from_string(const char *str)
{
	struct node_mac *m;

	m = calloc(1, sizeof(struct node_mac));
	if (m == NULL)
		err(1, "mac: calloc");

	if (sscanf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
	    &m->mac[0], &m->mac[1], &m->mac[2], &m->mac[3], &m->mac[4],
	    &m->mac[5]) != 6) {
		free(m);
		yyerror("invalid MAC address");
		return (NULL);
	}

	memset(m->mask, 0xff, ETHER_ADDR_LEN);
	m->isset = true;
	m->next = NULL;
	m->tail = m;

	return (m);
}

struct node_mac*
node_mac_from_string_masklen(const char *str, int masklen)
{
	struct node_mac *m;

	if (masklen < 0 || masklen > (ETHER_ADDR_LEN * 8)) {
		yyerror("invalid MAC mask length");
		return (NULL);
	}

	m = node_mac_from_string(str);
	if (m == NULL)
		return (NULL);

	memset(m->mask, 0, ETHER_ADDR_LEN);
	for (int i = 0; i < masklen; i++)
		m->mask[i / 8] |= 1 << (i % 8);

	return (m);
}

struct node_mac*
node_mac_from_string_mask(const char *str, const char *mask)
{
	struct node_mac *m;

	m = node_mac_from_string(str);
	if (m == NULL)
		return (NULL);

	if (sscanf(mask, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
	    &m->mask[0], &m->mask[1], &m->mask[2], &m->mask[3], &m->mask[4],
	    &m->mask[5]) != 6) {
		free(m);
		yyerror("invalid MAC mask");
		return (NULL);
	}

	return (m);
}
#line 4970 "parse.c"

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
case 19:
#line 618 "parse.y"
	{ file->errors++; }
#line 5172 "parse.c"
break;
case 20:
#line 621 "parse.y"
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
#line 5189 "parse.c"
break;
case 27:
#line 648 "parse.y"
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
#line 5205 "parse.c"
break;
case 28:
#line 662 "parse.y"
	{ yyval.v.number = 0; }
#line 5210 "parse.c"
break;
case 29:
#line 663 "parse.y"
	{ yyval.v.number = 1; }
#line 5215 "parse.c"
break;
case 30:
#line 666 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			pfctl_set_reassembly(pf, yystack.l_mark[-1].v.number, yystack.l_mark[0].v.number);
		}
#line 5224 "parse.c"
break;
case 31:
#line 671 "parse.y"
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
#line 5240 "parse.c"
break;
case 32:
#line 683 "parse.y"
	{
			if (!(pf->opts & PF_OPT_OPTIMIZE)) {
				pf->opts |= PF_OPT_OPTIMIZE;
				pf->optimize = yystack.l_mark[0].v.i;
			}
		}
#line 5250 "parse.c"
break;
case 37:
#line 693 "parse.y"
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
#line 5266 "parse.c"
break;
case 38:
#line 705 "parse.y"
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
#line 5280 "parse.c"
break;
case 39:
#line 715 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_DROP;
		}
#line 5291 "parse.c"
break;
case 40:
#line 722 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_RETURN;
		}
#line 5302 "parse.c"
break;
case 41:
#line 729 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_DROP;
		}
#line 5313 "parse.c"
break;
case 42:
#line 736 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_RETURN;
		}
#line 5324 "parse.c"
break;
case 43:
#line 743 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set require-order %s\n",
				    yystack.l_mark[0].v.number == 1 ? "yes" : "no");
			require_order = yystack.l_mark[0].v.number;
		}
#line 5334 "parse.c"
break;
case 44:
#line 749 "parse.y"
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
#line 5356 "parse.c"
break;
case 45:
#line 767 "parse.y"
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
#line 5372 "parse.c"
break;
case 46:
#line 779 "parse.y"
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
#line 5388 "parse.c"
break;
case 47:
#line 791 "parse.y"
	{
			if (expand_skip_interface(yystack.l_mark[0].v.interface) != 0) {
				yyerror("error setting skip interface(s)");
				YYERROR;
			}
		}
#line 5398 "parse.c"
break;
case 48:
#line 797 "parse.y"
	{
			if (keep_state_defaults != NULL) {
				yyerror("cannot redefine state-defaults");
				YYERROR;
			}
			keep_state_defaults = yystack.l_mark[0].v.state_opt;
		}
#line 5409 "parse.c"
break;
case 49:
#line 804 "parse.y"
	{
			pf->keep_counters = true;
		}
#line 5416 "parse.c"
break;
case 50:
#line 807 "parse.y"
	{
			if (pfctl_cfg_syncookies(pf, yystack.l_mark[-1].v.i, yystack.l_mark[0].v.watermarks)) {
				yyerror("error setting syncookies");
				YYERROR;
			}
		}
#line 5426 "parse.c"
break;
case 51:
#line 815 "parse.y"
	{
			if (!strcmp(yystack.l_mark[0].v.string, "never"))
				yyval.v.i = PFCTL_SYNCOOKIES_NEVER;
			else if (!strcmp(yystack.l_mark[0].v.string, "adaptive"))
				yyval.v.i = PFCTL_SYNCOOKIES_ADAPTIVE;
			else if (!strcmp(yystack.l_mark[0].v.string, "always"))
				yyval.v.i = PFCTL_SYNCOOKIES_ALWAYS;
			else {
				yyerror("illegal value for syncookies");
				YYERROR;
			}
		}
#line 5442 "parse.c"
break;
case 52:
#line 828 "parse.y"
	{ yyval.v.watermarks = NULL; }
#line 5447 "parse.c"
break;
case 53:
#line 829 "parse.y"
	{
			memset(&syncookie_opts, 0, sizeof(syncookie_opts));
		  }
#line 5454 "parse.c"
break;
case 54:
#line 831 "parse.y"
	{ yyval.v.watermarks = &syncookie_opts; }
#line 5459 "parse.c"
break;
case 57:
#line 838 "parse.y"
	{
			double   val;
			char    *cp;

			val = strtod(yystack.l_mark[0].v.string, &cp);
			if (cp == NULL || strcmp(cp, "%"))
				YYERROR;
			if (val <= 0 || val > 100) {
				yyerror("illegal percentage value");
				YYERROR;
			}
			if (!strcmp(yystack.l_mark[-1].v.string, "start")) {
				syncookie_opts.hi = val;
			} else if (!strcmp(yystack.l_mark[-1].v.string, "end")) {
				syncookie_opts.lo = val;
			} else {
				yyerror("illegal syncookie option");
				YYERROR;
			}
		}
#line 5483 "parse.c"
break;
case 58:
#line 860 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 5488 "parse.c"
break;
case 59:
#line 861 "parse.y"
	{
			if ((yyval.v.string = strdup("all")) == NULL) {
				err(1, "stringall: strdup");
			}
		}
#line 5497 "parse.c"
break;
case 60:
#line 868 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
#line 5507 "parse.c"
break;
case 62:
#line 877 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
#line 5517 "parse.c"
break;
case 64:
#line 886 "parse.y"
	{
			char	*s;
			if (asprintf(&s, "%lld", (long long)yystack.l_mark[0].v.number) == -1) {
				yyerror("string: asprintf");
				YYERROR;
			}
			yyval.v.string = s;
		}
#line 5529 "parse.c"
break;
case 66:
#line 897 "parse.y"
	{
			char *s = yystack.l_mark[-2].v.string;
			if (pf->opts & PF_OPT_VERBOSE)
				printf("%s = \"%s\"\n", yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string);
			while (*s++) {
				if (isspace((unsigned char)*s)) {
					yyerror("macro name cannot contain "
					   "whitespace");
					YYERROR;
				}
			}
			if (symset(yystack.l_mark[-2].v.string, yystack.l_mark[0].v.string, 0) == -1)
				err(1, "cannot store variable %s", yystack.l_mark[-2].v.string);
			free(yystack.l_mark[-2].v.string);
			free(yystack.l_mark[0].v.string);
		}
#line 5549 "parse.c"
break;
case 67:
#line 915 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 5554 "parse.c"
break;
case 68:
#line 916 "parse.y"
	{ yyval.v.string = NULL; }
#line 5559 "parse.c"
break;
case 73:
#line 926 "parse.y"
	{
			char ta[PF_ANCHOR_NAME_SIZE];
			struct pfctl_ruleset *rs;

			/* stepping into a brace anchor */
			pf->asd++;
			pf->bn++;

			/*
			* Anchor contents are parsed before the anchor rule
			* production completes, so we don't know the real
			* location yet. Create a holding ruleset in the root;
			* contents will be moved afterwards.
			*/
			snprintf(ta, PF_ANCHOR_NAME_SIZE, "_%d", pf->bn);
			rs = pf_find_or_create_ruleset(ta);
			if (rs == NULL)
				err(1, "pfa_anchor: pf_find_or_create_ruleset");
			pf->astack[pf->asd] = rs->anchor;
			pf->anchor = rs->anchor;
		}
#line 5584 "parse.c"
break;
case 74:
#line 947 "parse.y"
	{
			pf->alast = pf->anchor;
			pf->asd--;
			pf->anchor = pf->astack[pf->asd];
		}
#line 5593 "parse.c"
break;
case 76:
#line 957 "parse.y"
	{
			struct pfctl_rule	r;
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
				if (yystack.l_mark[-8].v.string && strchr(yystack.l_mark[-8].v.string, '/') != NULL) {
					free(yystack.l_mark[-8].v.string);
					yyerror("anchor paths containing '/' "
					   "cannot be used for inline anchors.");
					YYERROR;
				}

				/* Move inline rules into relative location. */
				pfctl_anchor_setup(&r,
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
			r.ridentifier = yystack.l_mark[-1].v.filter_opts.ridentifier;

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
			for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++)
				free(yystack.l_mark[-1].v.filter_opts.label[i]);
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
#line 5738 "parse.c"
break;
case 77:
#line 1098 "parse.y"
	{
			struct pfctl_rule	r;

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
#line 5763 "parse.c"
break;
case 78:
#line 1119 "parse.y"
	{
			struct pfctl_rule	r;

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
#line 5809 "parse.c"
break;
case 79:
#line 1161 "parse.y"
	{
			struct pfctl_rule	r;

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

			pfctl_append_rule(pf, &r, yystack.l_mark[-5].v.string);
			free(yystack.l_mark[-5].v.string);
		}
#line 5848 "parse.c"
break;
case 80:
#line 1198 "parse.y"
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
#line 5882 "parse.c"
break;
case 81:
#line 1229 "parse.y"
	{
			yyval.v.b.b2 = yyval.v.b.w = 0;
			if (yystack.l_mark[-1].v.i)
				yyval.v.b.b1 = PF_NOSCRUB;
			else
				yyval.v.b.b1 = PF_SCRUB;
		}
#line 5893 "parse.c"
break;
case 82:
#line 1239 "parse.y"
	{
			struct pfctl_eth_rule	r;

			bzero(&r, sizeof(r));

			if (check_rulestate(PFCTL_STATE_ETHER))
				YYERROR;

			r.action = yystack.l_mark[-8].v.b.b1;
			r.direction = yystack.l_mark[-7].v.i;
			r.quick = yystack.l_mark[-6].v.logquick.quick;
			if (yystack.l_mark[0].v.filter_opts.tag != NULL)
				memcpy(&r.tagname, yystack.l_mark[0].v.filter_opts.tag, sizeof(r.tagname));
			if (yystack.l_mark[0].v.filter_opts.match_tag)
				if (strlcpy(r.match_tagname, yystack.l_mark[0].v.filter_opts.match_tag,
				    PF_TAG_NAME_SIZE) >= PF_TAG_NAME_SIZE) {
					yyerror("tag too long, max %u chars",
					    PF_TAG_NAME_SIZE - 1);
					YYERROR;
				}
			r.match_tag_not = yystack.l_mark[0].v.filter_opts.match_tag_not;
			if (yystack.l_mark[0].v.filter_opts.queues.qname != NULL)
				memcpy(&r.qname, yystack.l_mark[0].v.filter_opts.queues.qname, sizeof(r.qname));
			r.dnpipe = yystack.l_mark[0].v.filter_opts.dnpipe;
			r.dnflags = yystack.l_mark[0].v.filter_opts.free_flags;
			if (eth_rule_label(&r, yystack.l_mark[0].v.filter_opts.label))
				YYERROR;
			for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++)
				free(yystack.l_mark[0].v.filter_opts.label[i]);
			r.ridentifier = yystack.l_mark[0].v.filter_opts.ridentifier;

			expand_eth_rule(&r, yystack.l_mark[-5].v.interface, yystack.l_mark[-3].v.etherproto, yystack.l_mark[-2].v.etherfromto.src, yystack.l_mark[-2].v.etherfromto.dst,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-4].v.bridge_to, "");
		}
#line 5931 "parse.c"
break;
case 87:
#line 1282 "parse.y"
	{
			char ta[PF_ANCHOR_NAME_SIZE];
			struct pfctl_eth_ruleset *rs;

			/* steping into a brace anchor */
			pf->asd++;
			pf->bn++;

			/* create a holding ruleset in the root */
			snprintf(ta, PF_ANCHOR_NAME_SIZE, "_%d", pf->bn);
			rs = pf_find_or_create_eth_ruleset(ta);
			if (rs == NULL)
				err(1, "etherpfa_anchor: pf_find_or_create_eth_ruleset");
			pf->eastack[pf->asd] = rs->anchor;
			pf->eanchor = rs->anchor;
		}
#line 5951 "parse.c"
break;
case 88:
#line 1298 "parse.y"
	{
			pf->ealast = pf->eanchor;
			pf->asd--;
			pf->eanchor = pf->eastack[pf->asd];
		}
#line 5960 "parse.c"
break;
case 90:
#line 1307 "parse.y"
	{
			struct pfctl_eth_rule	r;

			if (check_rulestate(PFCTL_STATE_ETHER)) {
				free(yystack.l_mark[-7].v.string);
				YYERROR;
			}

			if (yystack.l_mark[-7].v.string && (yystack.l_mark[-7].v.string[0] == '_' || strstr(yystack.l_mark[-7].v.string, "/_") != NULL)) {
				free(yystack.l_mark[-7].v.string);
				yyerror("anchor names beginning with '_' "
				    "are reserved for internal use");
				YYERROR;
			}

			memset(&r, 0, sizeof(r));
			if (pf->eastack[pf->asd + 1]) {
				if (yystack.l_mark[-7].v.string && strchr(yystack.l_mark[-7].v.string, '/') != NULL) {
					free(yystack.l_mark[-7].v.string);
					yyerror("anchor paths containing '/' "
					   "cannot be used for inline anchors.");
					YYERROR;
				}

				/* Move inline rules into relative location. */
				pfctl_eth_anchor_setup(pf, &r,
				    &pf->eastack[pf->asd]->ruleset,
				    yystack.l_mark[-7].v.string ? yystack.l_mark[-7].v.string : pf->ealast->name);
				if (r.anchor == NULL)
					err(1, "etheranchorrule: unable to "
					    "create ruleset");

				if (pf->ealast != r.anchor) {
					if (r.anchor->match) {
						yyerror("inline anchor '%s' "
						    "already exists",
						    r.anchor->name);
						YYERROR;
					}
					mv_eth_rules(&pf->ealast->ruleset,
					    &r.anchor->ruleset);
				}
				pf_remove_if_empty_eth_ruleset(&pf->ealast->ruleset);
				pf->ealast = r.anchor;
			} else {
				if (!yystack.l_mark[-7].v.string) {
					yyerror("anchors without explicit "
					    "rules must specify a name");
					YYERROR;
				}
			}

			r.direction = yystack.l_mark[-6].v.i;
			r.quick = yystack.l_mark[-5].v.logquick.quick;

			expand_eth_rule(&r, yystack.l_mark[-4].v.interface, yystack.l_mark[-3].v.etherproto, yystack.l_mark[-2].v.etherfromto.src, yystack.l_mark[-2].v.etherfromto.dst,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.dst.host, NULL,
			    pf->eastack[pf->asd + 1] ? pf->ealast->name : yystack.l_mark[-7].v.string);

			free(yystack.l_mark[-7].v.string);
			pf->eastack[pf->asd + 1] = NULL;
		}
#line 6026 "parse.c"
break;
case 91:
#line 1371 "parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
			}
#line 6033 "parse.c"
break;
case 92:
#line 1375 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 6038 "parse.c"
break;
case 93:
#line 1376 "parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			yyval.v.filter_opts = filter_opts;
		}
#line 6046 "parse.c"
break;
case 96:
#line 1385 "parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
#line 6057 "parse.c"
break;
case 97:
#line 1392 "parse.y"
	{
			filter_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 6064 "parse.c"
break;
case 98:
#line 1395 "parse.y"
	{
			if (filter_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			filter_opts.label[filter_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 6075 "parse.c"
break;
case 99:
#line 1402 "parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
#line 6082 "parse.c"
break;
case 100:
#line 1405 "parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 6090 "parse.c"
break;
case 101:
#line 1409 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 6098 "parse.c"
break;
case 102:
#line 1413 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 6106 "parse.c"
break;
case 103:
#line 1419 "parse.y"
	{
			yyval.v.bridge_to = NULL;
		}
#line 6113 "parse.c"
break;
case 104:
#line 1422 "parse.y"
	{
			yyval.v.bridge_to = strdup(yystack.l_mark[0].v.string);
		}
#line 6120 "parse.c"
break;
case 105:
#line 1428 "parse.y"
	{
			struct pfctl_rule	r;

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
			if (yystack.l_mark[0].v.scrub_opts.marker & FOM_SETTOS) {
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
#line 6179 "parse.c"
break;
case 106:
#line 1485 "parse.y"
	{
				bzero(&scrub_opts, sizeof scrub_opts);
				scrub_opts.rtableid = -1;
			}
#line 6187 "parse.c"
break;
case 107:
#line 1490 "parse.y"
	{ yyval.v.scrub_opts = scrub_opts; }
#line 6192 "parse.c"
break;
case 108:
#line 1491 "parse.y"
	{
			bzero(&scrub_opts, sizeof scrub_opts);
			scrub_opts.rtableid = -1;
			yyval.v.scrub_opts = scrub_opts;
		}
#line 6201 "parse.c"
break;
case 111:
#line 1502 "parse.y"
	{
			if (scrub_opts.nodf) {
				yyerror("no-df cannot be respecified");
				YYERROR;
			}
			scrub_opts.nodf = 1;
		}
#line 6212 "parse.c"
break;
case 112:
#line 1509 "parse.y"
	{
			if (scrub_opts.marker & FOM_MINTTL) {
				yyerror("min-ttl cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal min-ttl value %d", yystack.l_mark[0].v.number);
				YYERROR;
			}
			scrub_opts.marker |= FOM_MINTTL;
			scrub_opts.minttl = yystack.l_mark[0].v.number;
		}
#line 6228 "parse.c"
break;
case 113:
#line 1521 "parse.y"
	{
			if (scrub_opts.marker & FOM_MAXMSS) {
				yyerror("max-mss cannot be respecified");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65535) {
				yyerror("illegal max-mss value %d", yystack.l_mark[0].v.number);
				YYERROR;
			}
			scrub_opts.marker |= FOM_MAXMSS;
			scrub_opts.maxmss = yystack.l_mark[0].v.number;
		}
#line 6244 "parse.c"
break;
case 114:
#line 1533 "parse.y"
	{
			if (scrub_opts.marker & FOM_SETTOS) {
				yyerror("set-tos cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= FOM_SETTOS;
			scrub_opts.settos = yystack.l_mark[0].v.number;
		}
#line 6256 "parse.c"
break;
case 115:
#line 1541 "parse.y"
	{
			if (scrub_opts.marker & FOM_FRAGCACHE) {
				yyerror("fragcache cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= FOM_FRAGCACHE;
			scrub_opts.fragcache = yystack.l_mark[0].v.i;
		}
#line 6268 "parse.c"
break;
case 116:
#line 1549 "parse.y"
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
#line 6286 "parse.c"
break;
case 117:
#line 1563 "parse.y"
	{
			if (scrub_opts.randomid) {
				yyerror("random-id cannot be respecified");
				YYERROR;
			}
			scrub_opts.randomid = 1;
		}
#line 6297 "parse.c"
break;
case 118:
#line 1570 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			scrub_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 6308 "parse.c"
break;
case 119:
#line 1577 "parse.y"
	{
			scrub_opts.match_tag = yystack.l_mark[0].v.string;
			scrub_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 6316 "parse.c"
break;
case 120:
#line 1583 "parse.y"
	{ yyval.v.i = 0; /* default */ }
#line 6321 "parse.c"
break;
case 121:
#line 1584 "parse.y"
	{ yyval.v.i = PFRULE_FRAGMENT_NOREASS; }
#line 6326 "parse.c"
break;
case 122:
#line 1587 "parse.y"
	{
			struct pfctl_rule	 r;
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
				r.ridentifier = yystack.l_mark[0].v.antispoof_opts.ridentifier;
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
					r.ridentifier = yystack.l_mark[0].v.antispoof_opts.ridentifier;
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
			for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++)
				free(yystack.l_mark[0].v.antispoof_opts.label[i]);
		}
#line 6418 "parse.c"
break;
case 123:
#line 1677 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 6423 "parse.c"
break;
case 124:
#line 1678 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 6428 "parse.c"
break;
case 125:
#line 1681 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 6433 "parse.c"
break;
case 126:
#line 1682 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
#line 6442 "parse.c"
break;
case 127:
#line 1689 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 6447 "parse.c"
break;
case 128:
#line 1690 "parse.y"
	{
			yystack.l_mark[-1].v.interface->dynamic = 1;
			yyval.v.interface = yystack.l_mark[-1].v.interface;
		}
#line 6455 "parse.c"
break;
case 129:
#line 1696 "parse.y"
	{
				bzero(&antispoof_opts, sizeof antispoof_opts);
				antispoof_opts.rtableid = -1;
			}
#line 6463 "parse.c"
break;
case 130:
#line 1701 "parse.y"
	{ yyval.v.antispoof_opts = antispoof_opts; }
#line 6468 "parse.c"
break;
case 131:
#line 1702 "parse.y"
	{
			bzero(&antispoof_opts, sizeof antispoof_opts);
			antispoof_opts.rtableid = -1;
			yyval.v.antispoof_opts = antispoof_opts;
		}
#line 6477 "parse.c"
break;
case 134:
#line 1713 "parse.y"
	{
			if (antispoof_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			antispoof_opts.label[antispoof_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 6488 "parse.c"
break;
case 135:
#line 1720 "parse.y"
	{
			antispoof_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 6495 "parse.c"
break;
case 136:
#line 1723 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			antispoof_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 6506 "parse.c"
break;
case 137:
#line 1732 "parse.y"
	{ yyval.v.number = 1; }
#line 6511 "parse.c"
break;
case 138:
#line 1733 "parse.y"
	{ yyval.v.number = 0; }
#line 6516 "parse.c"
break;
case 139:
#line 1736 "parse.y"
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
#line 6548 "parse.c"
break;
case 140:
#line 1766 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
		}
#line 6556 "parse.c"
break;
case 141:
#line 1771 "parse.y"
	{ yyval.v.table_opts = table_opts; }
#line 6561 "parse.c"
break;
case 142:
#line 1773 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
			yyval.v.table_opts = table_opts;
		}
#line 6570 "parse.c"
break;
case 145:
#line 1784 "parse.y"
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
#line 6588 "parse.c"
break;
case 146:
#line 1798 "parse.y"
	{ table_opts.init_addr = 1; }
#line 6593 "parse.c"
break;
case 147:
#line 1799 "parse.y"
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
#line 6637 "parse.c"
break;
case 148:
#line 1839 "parse.y"
	{
			struct node_tinit	*ti;

			if (!(ti = calloc(1, sizeof(*ti))))
				err(1, "table_opt: calloc");
			ti->file = yystack.l_mark[0].v.string;
			SIMPLEQ_INSERT_TAIL(&table_opts.init_nodes, ti,
			    entries);
			table_opts.init_addr = 1;
		}
#line 6651 "parse.c"
break;
case 149:
#line 1851 "parse.y"
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
#line 6677 "parse.c"
break;
case 150:
#line 1875 "parse.y"
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
#line 6716 "parse.c"
break;
case 151:
#line 1912 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
		}
#line 6727 "parse.c"
break;
case 152:
#line 1920 "parse.y"
	{ yyval.v.queue_opts = queue_opts; }
#line 6732 "parse.c"
break;
case 153:
#line 1921 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
			yyval.v.queue_opts = queue_opts;
		}
#line 6744 "parse.c"
break;
case 156:
#line 1935 "parse.y"
	{
			if (queue_opts.marker & QOM_BWSPEC) {
				yyerror("bandwidth cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_BWSPEC;
			queue_opts.queue_bwspec = yystack.l_mark[0].v.queue_bwspec;
		}
#line 6756 "parse.c"
break;
case 157:
#line 1943 "parse.y"
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
#line 6772 "parse.c"
break;
case 158:
#line 1955 "parse.y"
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
#line 6788 "parse.c"
break;
case 159:
#line 1967 "parse.y"
	{
			if (queue_opts.marker & QOM_SCHEDULER) {
				yyerror("scheduler cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_SCHEDULER;
			queue_opts.scheduler = yystack.l_mark[0].v.queue_options;
		}
#line 6800 "parse.c"
break;
case 160:
#line 1975 "parse.y"
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
#line 6816 "parse.c"
break;
case 161:
#line 1989 "parse.y"
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
#line 6863 "parse.c"
break;
case 162:
#line 2032 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= LLONG_MAX) {
				yyerror("bandwidth number too big");
				YYERROR;
			}
			yyval.v.queue_bwspec.bw_percent = 0;
			yyval.v.queue_bwspec.bw_absolute = yystack.l_mark[0].v.number;
		}
#line 6875 "parse.c"
break;
case 163:
#line 2042 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = 0;
		}
#line 6883 "parse.c"
break;
case 164:
#line 2046 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = yystack.l_mark[-1].v.number;
		}
#line 6891 "parse.c"
break;
case 165:
#line 2050 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = 0;
		}
#line 6899 "parse.c"
break;
case 166:
#line 2054 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = yystack.l_mark[-1].v.number;
		}
#line 6907 "parse.c"
break;
case 167:
#line 2058 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			bzero(&yyval.v.queue_options.data.hfsc_opts,
			    sizeof(struct node_hfsc_opts));
		}
#line 6916 "parse.c"
break;
case 168:
#line 2063 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			yyval.v.queue_options.data.hfsc_opts = yystack.l_mark[-1].v.hfsc_opts;
		}
#line 6924 "parse.c"
break;
case 169:
#line 2067 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			bzero(&yyval.v.queue_options.data.fairq_opts,
				sizeof(struct node_fairq_opts));
		}
#line 6933 "parse.c"
break;
case 170:
#line 2072 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			yyval.v.queue_options.data.fairq_opts = yystack.l_mark[-1].v.fairq_opts;
		}
#line 6941 "parse.c"
break;
case 171:
#line 2076 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			bzero(&yyval.v.queue_options.data.codel_opts,
				sizeof(struct codel_opts));
		}
#line 6950 "parse.c"
break;
case 172:
#line 2081 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			yyval.v.queue_options.data.codel_opts = yystack.l_mark[-1].v.codel_opts;
		}
#line 6958 "parse.c"
break;
case 173:
#line 2087 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6963 "parse.c"
break;
case 174:
#line 2088 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6968 "parse.c"
break;
case 175:
#line 2091 "parse.y"
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
#line 6992 "parse.c"
break;
case 176:
#line 2113 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6997 "parse.c"
break;
case 177:
#line 2114 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 7002 "parse.c"
break;
case 178:
#line 2117 "parse.y"
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
#line 7024 "parse.c"
break;
case 179:
#line 2137 "parse.y"
	{
				bzero(&hfsc_opts,
				    sizeof(struct node_hfsc_opts));
			}
#line 7032 "parse.c"
break;
case 180:
#line 2141 "parse.y"
	{
			yyval.v.hfsc_opts = hfsc_opts;
		}
#line 7039 "parse.c"
break;
case 183:
#line 2150 "parse.y"
	{
			if (hfsc_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			hfsc_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.linkshare.used = 1;
		}
#line 7051 "parse.c"
break;
case 184:
#line 2159 "parse.y"
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
#line 7069 "parse.c"
break;
case 185:
#line 2173 "parse.y"
	{
			if (hfsc_opts.realtime.used) {
				yyerror("realtime already specified");
				YYERROR;
			}
			hfsc_opts.realtime.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.realtime.used = 1;
		}
#line 7081 "parse.c"
break;
case 186:
#line 2182 "parse.y"
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
#line 7099 "parse.c"
break;
case 187:
#line 2196 "parse.y"
	{
			if (hfsc_opts.upperlimit.used) {
				yyerror("upperlimit already specified");
				YYERROR;
			}
			hfsc_opts.upperlimit.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.upperlimit.used = 1;
		}
#line 7111 "parse.c"
break;
case 188:
#line 2205 "parse.y"
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
#line 7129 "parse.c"
break;
case 189:
#line 2219 "parse.y"
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
#line 7151 "parse.c"
break;
case 190:
#line 2239 "parse.y"
	{
				bzero(&fairq_opts,
				    sizeof(struct node_fairq_opts));
			}
#line 7159 "parse.c"
break;
case 191:
#line 2243 "parse.y"
	{
			yyval.v.fairq_opts = fairq_opts;
		}
#line 7166 "parse.c"
break;
case 194:
#line 2252 "parse.y"
	{
			if (fairq_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			fairq_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			fairq_opts.linkshare.used = 1;
		}
#line 7178 "parse.c"
break;
case 195:
#line 2260 "parse.y"
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
#line 7192 "parse.c"
break;
case 196:
#line 2270 "parse.y"
	{
			fairq_opts.hogs_bw = yystack.l_mark[0].v.queue_bwspec;
		}
#line 7199 "parse.c"
break;
case 197:
#line 2273 "parse.y"
	{
			fairq_opts.nbuckets = yystack.l_mark[0].v.number;
		}
#line 7206 "parse.c"
break;
case 198:
#line 2276 "parse.y"
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
#line 7228 "parse.c"
break;
case 199:
#line 2296 "parse.y"
	{
				bzero(&codel_opts,
				    sizeof(struct codel_opts));
			}
#line 7236 "parse.c"
break;
case 200:
#line 2300 "parse.y"
	{
			yyval.v.codel_opts = codel_opts;
		}
#line 7243 "parse.c"
break;
case 203:
#line 2309 "parse.y"
	{
			if (codel_opts.interval) {
				yyerror("interval already specified");
				YYERROR;
			}
			codel_opts.interval = yystack.l_mark[0].v.number;
		}
#line 7254 "parse.c"
break;
case 204:
#line 2316 "parse.y"
	{
			if (codel_opts.target) {
				yyerror("target already specified");
				YYERROR;
			}
			codel_opts.target = yystack.l_mark[0].v.number;
		}
#line 7265 "parse.c"
break;
case 205:
#line 2323 "parse.y"
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
#line 7279 "parse.c"
break;
case 206:
#line 2335 "parse.y"
	{ yyval.v.queue = NULL; }
#line 7284 "parse.c"
break;
case 207:
#line 2336 "parse.y"
	{ yyval.v.queue = yystack.l_mark[0].v.queue; }
#line 7289 "parse.c"
break;
case 208:
#line 2337 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
#line 7294 "parse.c"
break;
case 209:
#line 2340 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
#line 7299 "parse.c"
break;
case 210:
#line 2341 "parse.y"
	{
			yystack.l_mark[-3].v.queue->tail->next = yystack.l_mark[-1].v.queue;
			yystack.l_mark[-3].v.queue->tail = yystack.l_mark[-1].v.queue;
			yyval.v.queue = yystack.l_mark[-3].v.queue;
		}
#line 7308 "parse.c"
break;
case 211:
#line 2348 "parse.y"
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
#line 7328 "parse.c"
break;
case 212:
#line 2368 "parse.y"
	{
			struct pfctl_rule	 r;
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

			if (yystack.l_mark[0].v.filter_opts.nodf)
				r.scrub_flags |= PFSTATE_NODF;
			if (yystack.l_mark[0].v.filter_opts.randomid)
				r.scrub_flags |= PFSTATE_RANDOMID;
			if (yystack.l_mark[0].v.filter_opts.minttl)
				r.min_ttl = yystack.l_mark[0].v.filter_opts.minttl;
			if (yystack.l_mark[0].v.filter_opts.max_mss)
				r.max_mss = yystack.l_mark[0].v.filter_opts.max_mss;
			if (yystack.l_mark[0].v.filter_opts.marker & FOM_SETTOS) {
				r.scrub_flags |= PFSTATE_SETTOS;
				r.set_tos = yystack.l_mark[0].v.filter_opts.settos;
			}
			if (yystack.l_mark[0].v.filter_opts.marker & FOM_SCRUB_TCP)
				r.scrub_flags |= PFSTATE_SCRUB_TCP;

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
			for (int i = 0; i < PF_RULE_MAX_LABEL_COUNT; i++)
				free(yystack.l_mark[0].v.filter_opts.label[i]);
			r.ridentifier = yystack.l_mark[0].v.filter_opts.ridentifier;
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

			if (yystack.l_mark[0].v.filter_opts.dnpipe || yystack.l_mark[0].v.filter_opts.dnrpipe) {
				r.dnpipe = yystack.l_mark[0].v.filter_opts.dnpipe;
				r.dnrpipe = yystack.l_mark[0].v.filter_opts.dnrpipe;
				if (yystack.l_mark[0].v.filter_opts.free_flags & PFRULE_DN_IS_PIPE)
					r.free_flags |= PFRULE_DN_IS_PIPE;
				else
					r.free_flags |= PFRULE_DN_IS_QUEUE;
			}

			expand_rule(&r, yystack.l_mark[-5].v.interface, yystack.l_mark[-4].v.route.host, yystack.l_mark[-2].v.proto, yystack.l_mark[-1].v.fromto.src_os,
			    yystack.l_mark[-1].v.fromto.src.host, yystack.l_mark[-1].v.fromto.src.port, yystack.l_mark[-1].v.fromto.dst.host, yystack.l_mark[-1].v.fromto.dst.port,
			    yystack.l_mark[0].v.filter_opts.uid, yystack.l_mark[0].v.filter_opts.gid, yystack.l_mark[0].v.filter_opts.icmpspec, "");
		}
#line 7780 "parse.c"
break;
case 213:
#line 2818 "parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
				filter_opts.rtableid = -1;
			}
#line 7788 "parse.c"
break;
case 214:
#line 2823 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 7793 "parse.c"
break;
case 215:
#line 2824 "parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			filter_opts.rtableid = -1;
			yyval.v.filter_opts = filter_opts;
		}
#line 7802 "parse.c"
break;
case 218:
#line 2835 "parse.y"
	{
			if (filter_opts.uid)
				yystack.l_mark[0].v.uid->tail->next = filter_opts.uid;
			filter_opts.uid = yystack.l_mark[0].v.uid;
		}
#line 7811 "parse.c"
break;
case 219:
#line 2840 "parse.y"
	{
			if (filter_opts.gid)
				yystack.l_mark[0].v.gid->tail->next = filter_opts.gid;
			filter_opts.gid = yystack.l_mark[0].v.gid;
		}
#line 7820 "parse.c"
break;
case 220:
#line 2845 "parse.y"
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
#line 7835 "parse.c"
break;
case 221:
#line 2856 "parse.y"
	{
			if (filter_opts.marker & FOM_ICMP) {
				yyerror("icmp-type cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_ICMP;
			filter_opts.icmpspec = yystack.l_mark[0].v.icmp;
		}
#line 7847 "parse.c"
break;
case 222:
#line 2864 "parse.y"
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
#line 7863 "parse.c"
break;
case 223:
#line 2876 "parse.y"
	{
			if (filter_opts.marker & FOM_TOS) {
				yyerror("tos cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_TOS;
			filter_opts.tos = yystack.l_mark[0].v.number;
		}
#line 7875 "parse.c"
break;
case 224:
#line 2884 "parse.y"
	{
			if (filter_opts.marker & FOM_KEEP) {
				yyerror("modulate or keep cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_KEEP;
			filter_opts.keep.action = yystack.l_mark[0].v.keep_state.action;
			filter_opts.keep.options = yystack.l_mark[0].v.keep_state.options;
		}
#line 7888 "parse.c"
break;
case 225:
#line 2893 "parse.y"
	{
			filter_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 7895 "parse.c"
break;
case 226:
#line 2896 "parse.y"
	{
			filter_opts.fragment = 1;
		}
#line 7902 "parse.c"
break;
case 227:
#line 2899 "parse.y"
	{
			filter_opts.allowopts = 1;
		}
#line 7909 "parse.c"
break;
case 228:
#line 2902 "parse.y"
	{
			if (filter_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			filter_opts.label[filter_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 7920 "parse.c"
break;
case 229:
#line 2909 "parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
#line 7931 "parse.c"
break;
case 230:
#line 2916 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7939 "parse.c"
break;
case 231:
#line 2920 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[-1].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7947 "parse.c"
break;
case 232:
#line 2924 "parse.y"
	{
			filter_opts.dnrpipe = yystack.l_mark[-1].v.number;
			filter_opts.dnpipe = yystack.l_mark[-3].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7956 "parse.c"
break;
case 233:
#line 2929 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7964 "parse.c"
break;
case 234:
#line 2933 "parse.y"
	{
			filter_opts.dnrpipe = yystack.l_mark[-1].v.number;
			filter_opts.dnpipe = yystack.l_mark[-3].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7973 "parse.c"
break;
case 235:
#line 2938 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[-1].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7981 "parse.c"
break;
case 236:
#line 2942 "parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
#line 7988 "parse.c"
break;
case 237:
#line 2945 "parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 7996 "parse.c"
break;
case 238:
#line 2949 "parse.y"
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
#line 8012 "parse.c"
break;
case 239:
#line 2961 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			filter_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 8023 "parse.c"
break;
case 240:
#line 2968 "parse.y"
	{
#ifdef __FreeBSD__
			filter_opts.divert.port = yystack.l_mark[0].v.range.a;
			if (!filter_opts.divert.port) {
				yyerror("invalid divert port: %u", ntohs(yystack.l_mark[0].v.range.a));
				YYERROR;
			}
#endif
		}
#line 8036 "parse.c"
break;
case 241:
#line 2977 "parse.y"
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
#line 8058 "parse.c"
break;
case 242:
#line 2995 "parse.y"
	{
#ifdef __FreeBSD__
			yyerror("divert-reply has no meaning in FreeBSD pf(4)");
			YYERROR;
#else
			filter_opts.divert.port = 1;	/* some random value */
#endif
		}
#line 8070 "parse.c"
break;
case 243:
#line 3003 "parse.y"
	{
			filter_opts.nodf = yystack.l_mark[-1].v.scrub_opts.nodf;
			filter_opts.minttl = yystack.l_mark[-1].v.scrub_opts.minttl;
			if (yystack.l_mark[-1].v.scrub_opts.marker & FOM_SETTOS) {
				/* Old style rules are "scrub set-tos 0x42"
				 * New style are "set tos 0x42 scrub (...)"
				 * What is in "scrub(...)"" is unfortunately the
				 * original scrub syntax so it would overwrite
				 * "set tos" of a pass/match rule.
				 */
				filter_opts.settos = yystack.l_mark[-1].v.scrub_opts.settos;
			}
			filter_opts.randomid = yystack.l_mark[-1].v.scrub_opts.randomid;
			filter_opts.max_mss = yystack.l_mark[-1].v.scrub_opts.maxmss;
			if (yystack.l_mark[-1].v.scrub_opts.reassemble_tcp)
				filter_opts.marker |= FOM_SCRUB_TCP;
			filter_opts.marker |= yystack.l_mark[-1].v.scrub_opts.marker;
		}
#line 8092 "parse.c"
break;
case 245:
#line 3024 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 8097 "parse.c"
break;
case 246:
#line 3025 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 8102 "parse.c"
break;
case 249:
#line 3032 "parse.y"
	{
			if (filter_opts.marker & FOM_SETPRIO) {
				yyerror("prio cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_SETPRIO;
			filter_opts.set_prio[0] = yystack.l_mark[0].v.b.b1;
			filter_opts.set_prio[1] = yystack.l_mark[0].v.b.b2;
		}
#line 8115 "parse.c"
break;
case 250:
#line 3041 "parse.y"
	{
			if (filter_opts.marker & FOM_SETTOS) {
				yyerror("tos cannot be respecified");
				YYERROR;
			}
			filter_opts.marker |= FOM_SETTOS;
			filter_opts.settos = yystack.l_mark[0].v.number;
		}
#line 8127 "parse.c"
break;
case 251:
#line 3049 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yyval.v.b.b2 = yystack.l_mark[0].v.number;
		}
#line 8138 "parse.c"
break;
case 252:
#line 3056 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > PF_PRIO_MAX ||
			    yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yystack.l_mark[-3].v.number;
			yyval.v.b.b2 = yystack.l_mark[-1].v.number;
		}
#line 8151 "parse.c"
break;
case 253:
#line 3067 "parse.y"
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
#line 8171 "parse.c"
break;
case 254:
#line 3083 "parse.y"
	{
			yyval.v.probability = (double)yystack.l_mark[0].v.number;
		}
#line 8178 "parse.c"
break;
case 255:
#line 3089 "parse.y"
	{
			yyval.v.b.b1 = PF_PASS;
			yyval.v.b.b2 = failpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8188 "parse.c"
break;
case 256:
#line 3095 "parse.y"
	{ yyval.v.b.b1 = PF_MATCH; yyval.v.b.b2 = yyval.v.b.w = 0; }
#line 8193 "parse.c"
break;
case 257:
#line 3096 "parse.y"
	{ yyval.v.b = yystack.l_mark[0].v.b; yyval.v.b.b1 = PF_DROP; }
#line 8198 "parse.c"
break;
case 258:
#line 3099 "parse.y"
	{
			yyval.v.b.b2 = blockpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8207 "parse.c"
break;
case 259:
#line 3104 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_DROP;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
#line 8216 "parse.c"
break;
case 260:
#line 3109 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
#line 8225 "parse.c"
break;
case 261:
#line 3114 "parse.y"
	{
			if (yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > 255) {
				yyerror("illegal ttl value %d", yystack.l_mark[-1].v.number);
				YYERROR;
			}
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = 0;
		}
#line 8238 "parse.c"
break;
case 262:
#line 3123 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8247 "parse.c"
break;
case 263:
#line 3128 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8256 "parse.c"
break;
case 264:
#line 3133 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = returnicmpdefault;
		}
#line 8265 "parse.c"
break;
case 265:
#line 3138 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
#line 8274 "parse.c"
break;
case 266:
#line 3143 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-3].v.number;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
#line 8283 "parse.c"
break;
case 267:
#line 3148 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURN;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8292 "parse.c"
break;
case 268:
#line 3155 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 8303 "parse.c"
break;
case 269:
#line 3162 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmpdefault >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
#line 8317 "parse.c"
break;
case 270:
#line 3174 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET6))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 8328 "parse.c"
break;
case 271:
#line 3181 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmp6default >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
#line 8342 "parse.c"
break;
case 272:
#line 3193 "parse.y"
	{ yyval.v.i = PF_INOUT; }
#line 8347 "parse.c"
break;
case 273:
#line 3194 "parse.y"
	{ yyval.v.i = PF_IN; }
#line 8352 "parse.c"
break;
case 274:
#line 3195 "parse.y"
	{ yyval.v.i = PF_OUT; }
#line 8357 "parse.c"
break;
case 275:
#line 3198 "parse.y"
	{ yyval.v.logquick.quick = 0; }
#line 8362 "parse.c"
break;
case 276:
#line 3199 "parse.y"
	{ yyval.v.logquick.quick = 1; }
#line 8367 "parse.c"
break;
case 277:
#line 3202 "parse.y"
	{ yyval.v.logquick.log = 0; yyval.v.logquick.quick = 0; yyval.v.logquick.logif = 0; }
#line 8372 "parse.c"
break;
case 278:
#line 3203 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 0; }
#line 8377 "parse.c"
break;
case 279:
#line 3204 "parse.y"
	{ yyval.v.logquick.quick = 1; yyval.v.logquick.log = 0; yyval.v.logquick.logif = 0; }
#line 8382 "parse.c"
break;
case 280:
#line 3205 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[-1].v.logquick; yyval.v.logquick.quick = 1; }
#line 8387 "parse.c"
break;
case 281:
#line 3206 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 1; }
#line 8392 "parse.c"
break;
case 282:
#line 3209 "parse.y"
	{ yyval.v.logquick.log = PF_LOG; yyval.v.logquick.logif = 0; }
#line 8397 "parse.c"
break;
case 283:
#line 3210 "parse.y"
	{
			yyval.v.logquick.log = PF_LOG | yystack.l_mark[-1].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[-1].v.logquick.logif;
		}
#line 8405 "parse.c"
break;
case 284:
#line 3216 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; }
#line 8410 "parse.c"
break;
case 285:
#line 3217 "parse.y"
	{
			yyval.v.logquick.log = yystack.l_mark[-2].v.logquick.log | yystack.l_mark[0].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[0].v.logquick.logif;
			if (yyval.v.logquick.logif == 0)
				yyval.v.logquick.logif = yystack.l_mark[-2].v.logquick.logif;
		}
#line 8420 "parse.c"
break;
case 286:
#line 3225 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_ALL; yyval.v.logquick.logif = 0; }
#line 8425 "parse.c"
break;
case 287:
#line 3226 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
#line 8430 "parse.c"
break;
case 288:
#line 3227 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
#line 8435 "parse.c"
break;
case 289:
#line 3228 "parse.y"
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
#line 8458 "parse.c"
break;
case 290:
#line 3249 "parse.y"
	{ yyval.v.interface = NULL; }
#line 8463 "parse.c"
break;
case 291:
#line 3250 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 8468 "parse.c"
break;
case 292:
#line 3251 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 8473 "parse.c"
break;
case 293:
#line 3254 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 8478 "parse.c"
break;
case 294:
#line 3255 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
#line 8487 "parse.c"
break;
case 295:
#line 3262 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; yyval.v.interface->not = yystack.l_mark[-1].v.number; }
#line 8492 "parse.c"
break;
case 296:
#line 3265 "parse.y"
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
#line 8518 "parse.c"
break;
case 297:
#line 3289 "parse.y"
	{ yyval.v.i = 0; }
#line 8523 "parse.c"
break;
case 298:
#line 3290 "parse.y"
	{ yyval.v.i = AF_INET; }
#line 8528 "parse.c"
break;
case 299:
#line 3291 "parse.y"
	{ yyval.v.i = AF_INET6; }
#line 8533 "parse.c"
break;
case 300:
#line 3294 "parse.y"
	{ yyval.v.etherproto = NULL; }
#line 8538 "parse.c"
break;
case 301:
#line 3295 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[0].v.etherproto; }
#line 8543 "parse.c"
break;
case 302:
#line 3296 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[-1].v.etherproto; }
#line 8548 "parse.c"
break;
case 303:
#line 3299 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[-1].v.etherproto; }
#line 8553 "parse.c"
break;
case 304:
#line 3300 "parse.y"
	{
			yystack.l_mark[-3].v.etherproto->tail->next = yystack.l_mark[-1].v.etherproto;
			yystack.l_mark[-3].v.etherproto->tail = yystack.l_mark[-1].v.etherproto;
			yyval.v.etherproto = yystack.l_mark[-3].v.etherproto;
		}
#line 8562 "parse.c"
break;
case 305:
#line 3307 "parse.y"
	{
			u_int16_t	pr;

			pr = (u_int16_t)yystack.l_mark[0].v.i;
			if (pr == 0) {
				yyerror("proto 0 cannot be used");
				YYERROR;
			}
			yyval.v.etherproto = calloc(1, sizeof(struct node_proto));
			if (yyval.v.etherproto == NULL)
				err(1, "proto_item: calloc");
			yyval.v.etherproto->proto = pr;
			yyval.v.etherproto->next = NULL;
			yyval.v.etherproto->tail = yyval.v.etherproto;
		}
#line 8581 "parse.c"
break;
case 306:
#line 3324 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65565) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
#line 8591 "parse.c"
break;
case 307:
#line 3331 "parse.y"
	{
			if (!strncmp(yystack.l_mark[0].v.string, "0x", 2)) {
				if (sscanf(yystack.l_mark[0].v.string, "0x%4x", &yyval.v.i) != 1) {
					free(yystack.l_mark[0].v.string);
					yyerror("invalid EtherType hex");
					YYERROR;
				}
			} else {
				yyerror("Symbolic EtherType not yet supported");
			}
		}
#line 8606 "parse.c"
break;
case 308:
#line 3344 "parse.y"
	{ yyval.v.proto = NULL; }
#line 8611 "parse.c"
break;
case 309:
#line 3345 "parse.y"
	{ yyval.v.proto = yystack.l_mark[0].v.proto; }
#line 8616 "parse.c"
break;
case 310:
#line 3346 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
#line 8621 "parse.c"
break;
case 311:
#line 3349 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
#line 8626 "parse.c"
break;
case 312:
#line 3350 "parse.y"
	{
			yystack.l_mark[-3].v.proto->tail->next = yystack.l_mark[-1].v.proto;
			yystack.l_mark[-3].v.proto->tail = yystack.l_mark[-1].v.proto;
			yyval.v.proto = yystack.l_mark[-3].v.proto;
		}
#line 8635 "parse.c"
break;
case 313:
#line 3357 "parse.y"
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
#line 8654 "parse.c"
break;
case 314:
#line 3374 "parse.y"
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
#line 8670 "parse.c"
break;
case 315:
#line 3386 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
#line 8680 "parse.c"
break;
case 316:
#line 3394 "parse.y"
	{
			bzero(&yyval.v.fromto, sizeof(yyval.v.fromto));
		}
#line 8687 "parse.c"
break;
case 317:
#line 3397 "parse.y"
	{
			if (yystack.l_mark[0].v.fromto.src.host != NULL &&
			    yystack.l_mark[0].v.fromto.src.host->addr.type != PF_ADDR_ADDRMASK &&
			    yystack.l_mark[0].v.fromto.src.host->addr.type != PF_ADDR_TABLE) {
				yyerror("from must be an address or table");
				YYERROR;
			}
			if (yystack.l_mark[0].v.fromto.dst.host != NULL &&
			    yystack.l_mark[0].v.fromto.dst.host->addr.type != PF_ADDR_ADDRMASK &&
			    yystack.l_mark[0].v.fromto.dst.host->addr.type != PF_ADDR_TABLE) {
				yyerror("to must be an address or table");
				YYERROR;
			}
			yyval.v.fromto = yystack.l_mark[0].v.fromto;
		}
#line 8706 "parse.c"
break;
case 318:
#line 3413 "parse.y"
	{
			yyval.v.etherfromto.src = NULL;
			yyval.v.etherfromto.dst = NULL;
		}
#line 8714 "parse.c"
break;
case 319:
#line 3417 "parse.y"
	{
			yyval.v.etherfromto.src = yystack.l_mark[-1].v.etheraddr.mac;
			yyval.v.etherfromto.dst = yystack.l_mark[0].v.etheraddr.mac;
		}
#line 8722 "parse.c"
break;
case 320:
#line 3423 "parse.y"
	{
			bzero(&yyval.v.etheraddr, sizeof(yyval.v.etheraddr));
		}
#line 8729 "parse.c"
break;
case 321:
#line 3426 "parse.y"
	{
			yyval.v.etheraddr.mac = yystack.l_mark[0].v.mac;
		}
#line 8736 "parse.c"
break;
case 322:
#line 3431 "parse.y"
	{
			bzero(&yyval.v.etheraddr, sizeof(yyval.v.etheraddr));
		}
#line 8743 "parse.c"
break;
case 323:
#line 3434 "parse.y"
	{
			yyval.v.etheraddr.mac = yystack.l_mark[0].v.mac;
		}
#line 8750 "parse.c"
break;
case 324:
#line 3439 "parse.y"
	{
			yyval.v.mac = node_mac_from_string_masklen(yystack.l_mark[-2].v.string, yystack.l_mark[0].v.number);
			free(yystack.l_mark[-2].v.string);
			if (yyval.v.mac == NULL)
				YYERROR;
		}
#line 8760 "parse.c"
break;
case 325:
#line 3445 "parse.y"
	{
			if (strchr(yystack.l_mark[0].v.string, '&')) {
				/* mac&mask */
				char *mac = strtok(yystack.l_mark[0].v.string, "&");
				char *mask = strtok(NULL, "&");
				yyval.v.mac = node_mac_from_string_mask(mac, mask);
			} else {
				yyval.v.mac = node_mac_from_string(yystack.l_mark[0].v.string);
			}
			free(yystack.l_mark[0].v.string);
			if (yyval.v.mac == NULL)
				YYERROR;

		}
#line 8778 "parse.c"
break;
case 326:
#line 3459 "parse.y"
	{
			struct node_mac	*n;

			for (n = yystack.l_mark[0].v.mac; n != NULL; n = n->next)
				n->neg = yystack.l_mark[-1].v.number;
			yyval.v.mac = yystack.l_mark[0].v.mac;
		}
#line 8789 "parse.c"
break;
case 327:
#line 3467 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[0].v.mac;
		}
#line 8796 "parse.c"
break;
case 328:
#line 3471 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[-1].v.mac;
		}
#line 8803 "parse.c"
break;
case 329:
#line 3475 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[-1].v.mac;
		}
#line 8810 "parse.c"
break;
case 330:
#line 3478 "parse.y"
	{
			if (yystack.l_mark[0].v.mac == NULL)
				yyval.v.mac = yystack.l_mark[-2].v.mac;
			else if (yystack.l_mark[-2].v.mac == NULL)
				yyval.v.mac = yystack.l_mark[0].v.mac;
			else {
				yystack.l_mark[-2].v.mac->tail->next = yystack.l_mark[0].v.mac;
				yystack.l_mark[-2].v.mac->tail = yystack.l_mark[0].v.mac->tail;
				yyval.v.mac = yystack.l_mark[-2].v.mac;
			}
		}
#line 8825 "parse.c"
break;
case 331:
#line 3490 "parse.y"
	{
			yyval.v.fromto.src.host = NULL;
			yyval.v.fromto.src.port = NULL;
			yyval.v.fromto.dst.host = NULL;
			yyval.v.fromto.dst.port = NULL;
			yyval.v.fromto.src_os = NULL;
		}
#line 8836 "parse.c"
break;
case 332:
#line 3497 "parse.y"
	{
			yyval.v.fromto.src = yystack.l_mark[-2].v.peer;
			yyval.v.fromto.src_os = yystack.l_mark[-1].v.os;
			yyval.v.fromto.dst = yystack.l_mark[0].v.peer;
		}
#line 8845 "parse.c"
break;
case 333:
#line 3504 "parse.y"
	{ yyval.v.os = NULL; }
#line 8850 "parse.c"
break;
case 334:
#line 3505 "parse.y"
	{ yyval.v.os = yystack.l_mark[0].v.os; }
#line 8855 "parse.c"
break;
case 335:
#line 3506 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
#line 8860 "parse.c"
break;
case 336:
#line 3509 "parse.y"
	{
			yyval.v.os = calloc(1, sizeof(struct node_os));
			if (yyval.v.os == NULL)
				err(1, "os: calloc");
			yyval.v.os->os = yystack.l_mark[0].v.string;
			yyval.v.os->tail = yyval.v.os;
		}
#line 8871 "parse.c"
break;
case 337:
#line 3518 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
#line 8876 "parse.c"
break;
case 338:
#line 3519 "parse.y"
	{
			yystack.l_mark[-3].v.os->tail->next = yystack.l_mark[-1].v.os;
			yystack.l_mark[-3].v.os->tail = yystack.l_mark[-1].v.os;
			yyval.v.os = yystack.l_mark[-3].v.os;
		}
#line 8885 "parse.c"
break;
case 339:
#line 3526 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
#line 8893 "parse.c"
break;
case 340:
#line 3530 "parse.y"
	{
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
#line 8900 "parse.c"
break;
case 341:
#line 3535 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
#line 8908 "parse.c"
break;
case 342:
#line 3539 "parse.y"
	{
			if (disallow_urpf_failed(yystack.l_mark[0].v.peer.host, "\"urpf-failed\" is "
			    "not permitted in a destination address"))
				YYERROR;
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
#line 8918 "parse.c"
break;
case 343:
#line 3547 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[0].v.host;
			yyval.v.peer.port = NULL;
		}
#line 8926 "parse.c"
break;
case 344:
#line 3551 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[-2].v.host;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
#line 8934 "parse.c"
break;
case 345:
#line 3555 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
#line 8942 "parse.c"
break;
case 348:
#line 3565 "parse.y"
	{ yyval.v.host = NULL; }
#line 8947 "parse.c"
break;
case 349:
#line 3566 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 8952 "parse.c"
break;
case 350:
#line 3567 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 8957 "parse.c"
break;
case 351:
#line 3570 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 8962 "parse.c"
break;
case 352:
#line 3571 "parse.y"
	{ yyval.v.host = NULL; }
#line 8967 "parse.c"
break;
case 353:
#line 3574 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 8972 "parse.c"
break;
case 354:
#line 3575 "parse.y"
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
#line 8987 "parse.c"
break;
case 355:
#line 3588 "parse.y"
	{
			struct node_host	*n;

			for (n = yystack.l_mark[0].v.host; n != NULL; n = n->next)
				n->not = yystack.l_mark[-1].v.number;
			yyval.v.host = yystack.l_mark[0].v.host;
		}
#line 8998 "parse.c"
break;
case 356:
#line 3595 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_NOROUTE;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
#line 9011 "parse.c"
break;
case 357:
#line 3604 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_URPFFAILED;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
#line 9024 "parse.c"
break;
case 358:
#line 3615 "parse.y"
	{
			if ((yyval.v.host = host(yystack.l_mark[0].v.string)) == NULL)	{
				/* error. "any" is handled elsewhere */
				free(yystack.l_mark[0].v.string);
				yyerror("could not parse host specification");
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);

		}
#line 9038 "parse.c"
break;
case 359:
#line 3625 "parse.y"
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
#line 9075 "parse.c"
break;
case 360:
#line 3658 "parse.y"
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
#line 9093 "parse.c"
break;
case 361:
#line 3672 "parse.y"
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
#line 9115 "parse.c"
break;
case 363:
#line 3691 "parse.y"
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
#line 9130 "parse.c"
break;
case 364:
#line 3702 "parse.y"
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
#line 9152 "parse.c"
break;
case 366:
#line 3723 "parse.y"
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
#line 9167 "parse.c"
break;
case 367:
#line 3736 "parse.y"
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
#line 9223 "parse.c"
break;
case 368:
#line 3790 "parse.y"
	{ yyval.v.port = yystack.l_mark[0].v.port; }
#line 9228 "parse.c"
break;
case 369:
#line 3791 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
#line 9233 "parse.c"
break;
case 370:
#line 3794 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
#line 9238 "parse.c"
break;
case 371:
#line 3795 "parse.y"
	{
			yystack.l_mark[-3].v.port->tail->next = yystack.l_mark[-1].v.port;
			yystack.l_mark[-3].v.port->tail = yystack.l_mark[-1].v.port;
			yyval.v.port = yystack.l_mark[-3].v.port;
		}
#line 9247 "parse.c"
break;
case 372:
#line 3802 "parse.y"
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
#line 9264 "parse.c"
break;
case 373:
#line 3815 "parse.y"
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
#line 9283 "parse.c"
break;
case 374:
#line 3830 "parse.y"
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
#line 9302 "parse.c"
break;
case 375:
#line 3847 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, 0) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 9313 "parse.c"
break;
case 376:
#line 3856 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 9324 "parse.c"
break;
case 377:
#line 3865 "parse.y"
	{ yyval.v.uid = yystack.l_mark[0].v.uid; }
#line 9329 "parse.c"
break;
case 378:
#line 3866 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
#line 9334 "parse.c"
break;
case 379:
#line 3869 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
#line 9339 "parse.c"
break;
case 380:
#line 3870 "parse.y"
	{
			yystack.l_mark[-3].v.uid->tail->next = yystack.l_mark[-1].v.uid;
			yystack.l_mark[-3].v.uid->tail = yystack.l_mark[-1].v.uid;
			yyval.v.uid = yystack.l_mark[-3].v.uid;
		}
#line 9348 "parse.c"
break;
case 381:
#line 3877 "parse.y"
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
#line 9362 "parse.c"
break;
case 382:
#line 3887 "parse.y"
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
#line 9381 "parse.c"
break;
case 383:
#line 3902 "parse.y"
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
#line 9400 "parse.c"
break;
case 384:
#line 3919 "parse.y"
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
#line 9419 "parse.c"
break;
case 385:
#line 3934 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= UID_MAX) {
				yyerror("illegal uid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
#line 9430 "parse.c"
break;
case 386:
#line 3943 "parse.y"
	{ yyval.v.gid = yystack.l_mark[0].v.gid; }
#line 9435 "parse.c"
break;
case 387:
#line 3944 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
#line 9440 "parse.c"
break;
case 388:
#line 3947 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
#line 9445 "parse.c"
break;
case 389:
#line 3948 "parse.y"
	{
			yystack.l_mark[-3].v.gid->tail->next = yystack.l_mark[-1].v.gid;
			yystack.l_mark[-3].v.gid->tail = yystack.l_mark[-1].v.gid;
			yyval.v.gid = yystack.l_mark[-3].v.gid;
		}
#line 9454 "parse.c"
break;
case 390:
#line 3955 "parse.y"
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
#line 9468 "parse.c"
break;
case 391:
#line 3965 "parse.y"
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
#line 9487 "parse.c"
break;
case 392:
#line 3980 "parse.y"
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
#line 9506 "parse.c"
break;
case 393:
#line 3997 "parse.y"
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
#line 9525 "parse.c"
break;
case 394:
#line 4012 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= GID_MAX) {
				yyerror("illegal gid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
#line 9536 "parse.c"
break;
case 395:
#line 4021 "parse.y"
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
#line 9551 "parse.c"
break;
case 396:
#line 4034 "parse.y"
	{ yyval.v.b.b1 = yystack.l_mark[-2].v.b.b1; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
#line 9556 "parse.c"
break;
case 397:
#line 4035 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
#line 9561 "parse.c"
break;
case 398:
#line 4036 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = 0; }
#line 9566 "parse.c"
break;
case 399:
#line 4039 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
#line 9571 "parse.c"
break;
case 400:
#line 4040 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9576 "parse.c"
break;
case 401:
#line 4041 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
#line 9581 "parse.c"
break;
case 402:
#line 4042 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9586 "parse.c"
break;
case 403:
#line 4045 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9591 "parse.c"
break;
case 404:
#line 4046 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
#line 9600 "parse.c"
break;
case 405:
#line 4053 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9605 "parse.c"
break;
case 406:
#line 4054 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
#line 9614 "parse.c"
break;
case 407:
#line 4061 "parse.y"
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
#line 9628 "parse.c"
break;
case 408:
#line 4071 "parse.y"
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
#line 9651 "parse.c"
break;
case 409:
#line 4090 "parse.y"
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
#line 9669 "parse.c"
break;
case 410:
#line 4106 "parse.y"
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
#line 9683 "parse.c"
break;
case 411:
#line 4116 "parse.y"
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
#line 9706 "parse.c"
break;
case 412:
#line 4135 "parse.y"
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
#line 9724 "parse.c"
break;
case 413:
#line 4151 "parse.y"
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
#line 9739 "parse.c"
break;
case 414:
#line 4162 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
#line 9750 "parse.c"
break;
case 415:
#line 4171 "parse.y"
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
#line 9766 "parse.c"
break;
case 416:
#line 4183 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp6-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
#line 9777 "parse.c"
break;
case 417:
#line 4192 "parse.y"
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
#line 9801 "parse.c"
break;
case 418:
#line 4212 "parse.y"
	{
			yyval.v.number = yystack.l_mark[0].v.number;
			if (yyval.v.number < 0 || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.number);
				YYERROR;
			}
		}
#line 9812 "parse.c"
break;
case 419:
#line 4221 "parse.y"
	{ yyval.v.i = PF_SRCTRACK; }
#line 9817 "parse.c"
break;
case 420:
#line 4222 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_GLOBAL; }
#line 9822 "parse.c"
break;
case 421:
#line 4223 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_RULE; }
#line 9827 "parse.c"
break;
case 422:
#line 4226 "parse.y"
	{
			yyval.v.i = PFRULE_IFBOUND;
		}
#line 9834 "parse.c"
break;
case 423:
#line 4229 "parse.y"
	{
			yyval.v.i = 0;
		}
#line 9841 "parse.c"
break;
case 424:
#line 4234 "parse.y"
	{
			yyval.v.keep_state.action = 0;
			yyval.v.keep_state.options = NULL;
		}
#line 9849 "parse.c"
break;
case 425:
#line 4238 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_NORMAL;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9857 "parse.c"
break;
case 426:
#line 4242 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_MODULATE;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9865 "parse.c"
break;
case 427:
#line 4246 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_SYNPROXY;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9873 "parse.c"
break;
case 428:
#line 4252 "parse.y"
	{ yyval.v.i = 0; }
#line 9878 "parse.c"
break;
case 429:
#line 4253 "parse.y"
	{ yyval.v.i = PF_FLUSH; }
#line 9883 "parse.c"
break;
case 430:
#line 4254 "parse.y"
	{
			yyval.v.i = PF_FLUSH | PF_FLUSH_GLOBAL;
		}
#line 9890 "parse.c"
break;
case 431:
#line 4259 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[-1].v.state_opt; }
#line 9895 "parse.c"
break;
case 432:
#line 4260 "parse.y"
	{ yyval.v.state_opt = NULL; }
#line 9900 "parse.c"
break;
case 433:
#line 4263 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[0].v.state_opt; }
#line 9905 "parse.c"
break;
case 434:
#line 4264 "parse.y"
	{
			yystack.l_mark[-2].v.state_opt->tail->next = yystack.l_mark[0].v.state_opt;
			yystack.l_mark[-2].v.state_opt->tail = yystack.l_mark[0].v.state_opt;
			yyval.v.state_opt = yystack.l_mark[-2].v.state_opt;
		}
#line 9914 "parse.c"
break;
case 435:
#line 4271 "parse.y"
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
#line 9931 "parse.c"
break;
case 436:
#line 4284 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_NOSYNC;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 9943 "parse.c"
break;
case 437:
#line 4292 "parse.y"
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
#line 9960 "parse.c"
break;
case 438:
#line 4305 "parse.y"
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
#line 9977 "parse.c"
break;
case 439:
#line 4318 "parse.y"
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
#line 9996 "parse.c"
break;
case 440:
#line 4333 "parse.y"
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
#line 10018 "parse.c"
break;
case 441:
#line 4351 "parse.y"
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
#line 10035 "parse.c"
break;
case 442:
#line 4364 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SRCTRACK;
			yyval.v.state_opt->data.src_track = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10048 "parse.c"
break;
case 443:
#line 4373 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_STATELOCK;
			yyval.v.state_opt->data.statelock = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10061 "parse.c"
break;
case 444:
#line 4382 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SLOPPY;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10073 "parse.c"
break;
case 445:
#line 4390 "parse.y"
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
#line 10107 "parse.c"
break;
case 446:
#line 4422 "parse.y"
	{
			yyval.v.string = yystack.l_mark[0].v.string;
		}
#line 10114 "parse.c"
break;
case 447:
#line 4427 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
		}
#line 10121 "parse.c"
break;
case 448:
#line 4430 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
		}
#line 10128 "parse.c"
break;
case 449:
#line 4435 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
			yyval.v.qassign.pqname = NULL;
		}
#line 10136 "parse.c"
break;
case 450:
#line 4439 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
			yyval.v.qassign.pqname = NULL;
		}
#line 10144 "parse.c"
break;
case 451:
#line 4443 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-3].v.string;
			yyval.v.qassign.pqname = yystack.l_mark[-1].v.string;
		}
#line 10152 "parse.c"
break;
case 452:
#line 4449 "parse.y"
	{ yyval.v.i = 0; }
#line 10157 "parse.c"
break;
case 453:
#line 4450 "parse.y"
	{ yyval.v.i = 1; }
#line 10162 "parse.c"
break;
case 454:
#line 4453 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE|PPORT_STAR) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 10173 "parse.c"
break;
case 455:
#line 4462 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 10178 "parse.c"
break;
case 456:
#line 4463 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10183 "parse.c"
break;
case 457:
#line 4466 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10188 "parse.c"
break;
case 458:
#line 4467 "parse.y"
	{
			yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
			yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
			yyval.v.host = yystack.l_mark[-3].v.host;
		}
#line 10197 "parse.c"
break;
case 459:
#line 4474 "parse.y"
	{ yyval.v.redirection = NULL; }
#line 10202 "parse.c"
break;
case 460:
#line 4475 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
#line 10213 "parse.c"
break;
case 461:
#line 4482 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
#line 10224 "parse.c"
break;
case 462:
#line 4492 "parse.y"
	{
			yyval.v.hashkey = calloc(1, sizeof(struct pf_poolhashkey));
			if (yyval.v.hashkey == NULL)
				err(1, "hashkey: calloc");
			yyval.v.hashkey->key32[0] = arc4random();
			yyval.v.hashkey->key32[1] = arc4random();
			yyval.v.hashkey->key32[2] = arc4random();
			yyval.v.hashkey->key32[3] = arc4random();
		}
#line 10237 "parse.c"
break;
case 463:
#line 4502 "parse.y"
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
#line 10278 "parse.c"
break;
case 464:
#line 4541 "parse.y"
	{ bzero(&pool_opts, sizeof pool_opts); }
#line 10283 "parse.c"
break;
case 465:
#line 4543 "parse.y"
	{ yyval.v.pool_opts = pool_opts; }
#line 10288 "parse.c"
break;
case 466:
#line 4544 "parse.y"
	{
			bzero(&pool_opts, sizeof pool_opts);
			yyval.v.pool_opts = pool_opts;
		}
#line 10296 "parse.c"
break;
case 469:
#line 4554 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type =  PF_POOL_BITMASK;
		}
#line 10307 "parse.c"
break;
case 470:
#line 4561 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_RANDOM;
		}
#line 10318 "parse.c"
break;
case 471:
#line 4568 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_SRCHASH;
			pool_opts.key = yystack.l_mark[0].v.hashkey;
		}
#line 10330 "parse.c"
break;
case 472:
#line 4576 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_ROUNDROBIN;
		}
#line 10341 "parse.c"
break;
case 473:
#line 4583 "parse.y"
	{
			if (pool_opts.staticport) {
				yyerror("static-port cannot be redefined");
				YYERROR;
			}
			pool_opts.staticport = 1;
		}
#line 10352 "parse.c"
break;
case 474:
#line 4590 "parse.y"
	{
			if (pool_opts.marker & POM_STICKYADDRESS) {
				yyerror("sticky-address cannot be redefined");
				YYERROR;
			}
			pool_opts.marker |= POM_STICKYADDRESS;
			pool_opts.opts |= PF_POOL_STICKYADDR;
		}
#line 10364 "parse.c"
break;
case 475:
#line 4598 "parse.y"
	{
			if (pool_opts.mape.offset) {
				yyerror("map-e-portset cannot be redefined");
				YYERROR;
			}
			if (pool_opts.type) {
				yyerror("map-e-portset cannot be used with "
					"address pools");
				YYERROR;
			}
			if (yystack.l_mark[-4].v.number <= 0 || yystack.l_mark[-4].v.number >= 16) {
				yyerror("MAP-E PSID offset must be 1-15");
				YYERROR;
			}
			if (yystack.l_mark[-2].v.number < 0 || yystack.l_mark[-2].v.number >= 16 || yystack.l_mark[-4].v.number + yystack.l_mark[-2].v.number > 16) {
				yyerror("Invalid MAP-E PSID length");
				YYERROR;
			} else if (yystack.l_mark[-2].v.number == 0) {
				yyerror("PSID Length = 0: this means"
				    " you do not need MAP-E");
				YYERROR;
			}
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65535) {
				yyerror("Invalid MAP-E PSID");
				YYERROR;
			}
			pool_opts.mape.offset = yystack.l_mark[-4].v.number;
			pool_opts.mape.psidlen = yystack.l_mark[-2].v.number;
			pool_opts.mape.psid = yystack.l_mark[0].v.number;
		}
#line 10398 "parse.c"
break;
case 476:
#line 4630 "parse.y"
	{ yyval.v.redirection = NULL; }
#line 10403 "parse.c"
break;
case 477:
#line 4631 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
#line 10414 "parse.c"
break;
case 478:
#line 4638 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
#line 10425 "parse.c"
break;
case 479:
#line 4647 "parse.y"
	{ yyval.v.b.b1 = yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
#line 10430 "parse.c"
break;
case 480:
#line 4648 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
#line 10435 "parse.c"
break;
case 481:
#line 4649 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
#line 10440 "parse.c"
break;
case 482:
#line 4650 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
#line 10445 "parse.c"
break;
case 483:
#line 4653 "parse.y"
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
#line 10462 "parse.c"
break;
case 484:
#line 4666 "parse.y"
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
#line 10479 "parse.c"
break;
case 485:
#line 4683 "parse.y"
	{
			struct pfctl_rule	r;

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
				if (yystack.l_mark[-1].v.redirection->host->addr.type == PF_ADDR_DYNIFTL) {
					if ((yystack.l_mark[-1].v.redirection->host = gen_dynnode(yystack.l_mark[-1].v.redirection->host, r.af)) == NULL)
						err(1, "calloc");
				}
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

			if (yystack.l_mark[0].v.pool_opts.mape.offset) {
				if (r.action != PF_NAT) {
					yyerror("the 'map-e-portset' option is"
					    " only valid with nat rules");
					YYERROR;
				}
				if (yystack.l_mark[0].v.pool_opts.staticport) {
					yyerror("the 'map-e-portset' option"
					    " can't be used 'static-port'");
					YYERROR;
				}
				if (r.rpool.proxy_port[0] !=
				    PF_NAT_PROXY_PORT_LOW &&
				    r.rpool.proxy_port[1] !=
				    PF_NAT_PROXY_PORT_HIGH) {
					yyerror("the 'map-e-portset' option"
					    " can't be used when specifying"
					    " a port range");
					YYERROR;
				}
				r.rpool.mape = yystack.l_mark[0].v.pool_opts.mape;
			}

			expand_rule(&r, yystack.l_mark[-8].v.interface, yystack.l_mark[-1].v.redirection == NULL ? NULL : yystack.l_mark[-1].v.redirection->host, yystack.l_mark[-6].v.proto,
			    yystack.l_mark[-5].v.fromto.src_os, yystack.l_mark[-5].v.fromto.src.host, yystack.l_mark[-5].v.fromto.src.port, yystack.l_mark[-5].v.fromto.dst.host,
			    yystack.l_mark[-5].v.fromto.dst.port, 0, 0, 0, "");
			free(yystack.l_mark[-1].v.redirection);
		}
#line 10665 "parse.c"
break;
case 486:
#line 4869 "parse.y"
	{
			struct pfctl_rule	binat;
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
				if (yystack.l_mark[-5].v.host->addr.type == PF_ADDR_DYNIFTL) {
					if ((yystack.l_mark[-5].v.host = gen_dynnode(yystack.l_mark[-5].v.host, binat.af)) == NULL)
						err(1, "calloc");
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
				if (yystack.l_mark[-4].v.host->addr.type == PF_ADDR_DYNIFTL) {
					if ((yystack.l_mark[-4].v.host = gen_dynnode(yystack.l_mark[-4].v.host, binat.af)) == NULL)
						err(1, "calloc");
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
				if (yystack.l_mark[0].v.redirection->host->addr.type == PF_ADDR_DYNIFTL) {
					if ((yystack.l_mark[0].v.redirection->host = gen_dynnode(yystack.l_mark[0].v.redirection->host, binat.af)) == NULL)
						err(1, "calloc");
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

			pfctl_append_rule(pf, &binat, "");
		}
#line 10844 "parse.c"
break;
case 487:
#line 5046 "parse.y"
	{ yyval.v.string = NULL; }
#line 10849 "parse.c"
break;
case 488:
#line 5047 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 10854 "parse.c"
break;
case 489:
#line 5050 "parse.y"
	{ yyval.v.tagged.neg = 0; yyval.v.tagged.name = NULL; }
#line 10859 "parse.c"
break;
case 490:
#line 5051 "parse.y"
	{ yyval.v.tagged.neg = yystack.l_mark[-2].v.number; yyval.v.tagged.name = yystack.l_mark[0].v.string; }
#line 10864 "parse.c"
break;
case 491:
#line 5054 "parse.y"
	{ yyval.v.rtableid = -1; }
#line 10869 "parse.c"
break;
case 492:
#line 5055 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			yyval.v.rtableid = yystack.l_mark[0].v.number;
		}
#line 10880 "parse.c"
break;
case 493:
#line 5064 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "route_host: calloc");
			if (strlen(yystack.l_mark[0].v.string) >= IFNAMSIZ) {
				yyerror("interface name too long");
				YYERROR;
			}
			yyval.v.host->ifname = strdup(yystack.l_mark[0].v.string);
			set_ipmask(yyval.v.host, 128);
			yyval.v.host->next = NULL;
			yyval.v.host->tail = yyval.v.host;
		}
#line 10897 "parse.c"
break;
case 494:
#line 5077 "parse.y"
	{
			struct node_host *n;

			yyval.v.host = yystack.l_mark[-1].v.host;
			for (n = yystack.l_mark[-1].v.host; n != NULL; n = n->next) {
				if (strlen(yystack.l_mark[-2].v.string) >= IFNAMSIZ) {
					yyerror("interface name too long");
					YYERROR;
				}
				n->ifname = strdup(yystack.l_mark[-2].v.string);
			}
		}
#line 10913 "parse.c"
break;
case 495:
#line 5091 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10918 "parse.c"
break;
case 496:
#line 5092 "parse.y"
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
#line 10934 "parse.c"
break;
case 497:
#line 5106 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 10939 "parse.c"
break;
case 498:
#line 5107 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10944 "parse.c"
break;
case 499:
#line 5110 "parse.y"
	{
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
#line 10953 "parse.c"
break;
case 500:
#line 5115 "parse.y"
	{
			/* backwards-compat */
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
#line 10963 "parse.c"
break;
case 501:
#line 5121 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_ROUTETO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 10974 "parse.c"
break;
case 502:
#line 5128 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_REPLYTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 10985 "parse.c"
break;
case 503:
#line 5135 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_DUPTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 10996 "parse.c"
break;
case 504:
#line 5145 "parse.y"
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
#line 11016 "parse.c"
break;
case 505:
#line 5161 "parse.y"
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
#line 11030 "parse.c"
break;
case 508:
#line 5178 "parse.y"
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
#line 11050 "parse.c"
break;
case 513:
#line 5204 "parse.y"
	{ yyval.v.number = 0; }
#line 11055 "parse.c"
break;
case 514:
#line 5205 "parse.y"
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
#line 11070 "parse.c"
break;
case 515:
#line 5218 "parse.y"
	{ yyval.v.i = PF_OP_EQ; }
#line 11075 "parse.c"
break;
case 516:
#line 5219 "parse.y"
	{ yyval.v.i = PF_OP_NE; }
#line 11080 "parse.c"
break;
case 517:
#line 5220 "parse.y"
	{ yyval.v.i = PF_OP_LE; }
#line 11085 "parse.c"
break;
case 518:
#line 5221 "parse.y"
	{ yyval.v.i = PF_OP_LT; }
#line 11090 "parse.c"
break;
case 519:
#line 5222 "parse.y"
	{ yyval.v.i = PF_OP_GE; }
#line 11095 "parse.c"
break;
case 520:
#line 5223 "parse.y"
	{ yyval.v.i = PF_OP_GT; }
#line 11100 "parse.c"
break;
#line 11102 "parse.c"
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
