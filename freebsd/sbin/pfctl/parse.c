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

/* Provided by kernel-space modules */
#define pf_find_or_create_ruleset _bsd_pf_find_or_create_kruleset
#define pf_anchor_setup _bsd_pf_kanchor_setup
#define pf_remove_if_empty_ruleset _bsd_pf_remove_if_empty_kruleset

#include <machine/rtems-bsd-program.h>
#endif /* __rtems__ */

#include <sys/cdefs.h>
#define PFIOC_USE_LATEST

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif
#ifdef __rtems__
#include <sys/limits.h>
#endif /* __rtems__ */
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
	    PF_STATE_OPT_TIMEOUT, PF_STATE_OPT_SLOPPY,
	    PF_STATE_OPT_ALLOW_RELATED };

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

#line 590 "parse.c"

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
#define ALLOW_RELATED 382
#define TAGGED 383
#define TAG 384
#define IFBOUND 385
#define FLOATING 386
#define STATEPOLICY 387
#define STATEDEFAULTS 388
#define ROUTE 389
#define SETTOS 390
#define DIVERTTO 391
#define DIVERTREPLY 392
#define BRIDGE_TO 393
#define STRING 394
#define NUMBER 395
#define PORTBINARY 396
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
   90,   90,   90,   90,   90,   90,   80,  100,  100,   99,
   99,   99,   15,   15,   35,   61,   61,   60,   60,   79,
   79,   79,   37,   37,  197,  140,  140,  142,  142,  141,
  141,  141,  141,  141,  141,  141,   78,   78,   78,   28,
   28,   28,   28,   27,   27,  162,  163,   82,   82,  143,
  143,  144,  144,   62,   62,   63,   63,   64,   64,   77,
   77,   77,   77,   77,  173,  173,  175,  175,  176,  177,
  177,  180,  180,   12,   12,   23,   23,   23,   23,   23,
   23,
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
    2,    1,    1,    1,    1,    2,    2,    2,    4,    2,
    4,    6,    0,    1,    1,    1,    4,    2,    4,    0,
    2,    4,    0,    1,    0,    2,    0,    2,    1,    1,
    1,    2,    1,    1,    1,    6,    0,    2,    4,    0,
    1,    2,    1,    3,    3,   10,   13,    0,    2,    0,
    3,    0,    2,    1,    4,    2,    4,    1,    4,    0,
    1,    3,    3,    3,    2,    2,    4,    2,    2,    4,
    2,    1,    0,    1,    1,    1,    2,    2,    1,    2,
    1,
};
static const YYINT pfctlydefred[] = {                     0,
    0,    0,    0,    0,  255,    0,  256,  454,    0,    0,
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
    0,  514,  515,    0,   31,    0,    0,    0,   33,    0,
    0,   35,   59,   58,   37,   40,   39,   42,   41,   43,
   44,   46,   47,  366,  365,   38,   51,    0,   32,   27,
  422,  423,   45,    0,  436,    0,    0,    0,    0,    0,
    0,  444,  445,    0,  442,  443,    0,  433,    0,  281,
    0,    0,  280,    0,    0,    0,  137,  291,    0,    0,
    0,    0,    0,   65,   64,   66,    0,    0,  485,  483,
  484,    0,    0,  298,  299,    0,    0,    0,  268,  269,
    0,  270,  271,    0,    0,  276,    0,    0,    0,    0,
   29,   30,  506,  505,    0,    0,  509,    0,   50,    0,
  435,  437,  441,  420,  421,  438,    0,    0,  446,  512,
    0,    0,  286,  287,  288,    0,  284,  296,    0,    0,
  127,  123,    0,    0,    0,    0,  295,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  159,  155,    0,
    0,    0,   62,  482,    0,    0,    0,    0,    0,    0,
  264,    0,  265,  139,    0,    0,    0,    0,    0,  346,
    0,    0,    0,    0,    0,    0,    0,  434,  289,  283,
    0,    0,    0,  122,    0,    0,    0,    0,    0,  211,
    0,  149,  207,    0,  199,    0,  179,  190,  161,  162,
  156,  160,  157,  158,  154,  150,   80,    0,  501,    0,
    0,    0,    0,  314,  315,    0,  309,  313,    0,  331,
    0,    0,    0,  261,    0,    0,  145,    0,  144,    0,
    0,    0,    0,    0,  508,   34,    0,  511,   36,    0,
    0,    0,   56,  439,    0,  285,    0,    0,  128,    0,
    0,    0,  134,  133,    0,    0,    0,    0,  292,    0,
  293,    0,  175,    0,  173,    0,    0,  178,    0,  176,
    0,    0,    0,    0,    0,  494,    0,    0,  498,    0,
    0,    0,    0,    0,  348,    0,    0,    0,  340,    0,
  349,    0,    0,    0,    0,    0,  266,  148,    0,  143,
    0,    0,   77,   78,   79,    0,    0,   57,   54,    0,
    0,  440,  124,    0,  125,  447,  136,  135,  132,    0,
    0,  104,    0,    0,    0,    0,  164,    0,  172,    0,
    0,  205,    0,  201,  166,    0,  168,    0,    0,    0,
  189,    0,  181,  170,    0,    0,    0,  198,    0,  192,
    0,    0,    0,  502,    0,  504,  503,    0,    0,    0,
    0,  516,    0,    0,    0,    0,    0,  345,  368,  376,
    0,  356,  357,    0,    0,    0,    0,  355,    0,    0,
  489,    0,    0,  336,    0,  334,    0,  332,    0,  146,
    0,    0,    0,  493,  507,  510,   55,  430,    0,  307,
  306,    0,  305,  301,    0,  318,    0,    0,    0,  294,
  208,    0,  209,  174,  204,  203,    0,  177,    0,  183,
    0,  185,    0,  187,    0,    0,  194,  196,  197,    0,
    0,    0,    0,    0,  470,  471,    0,  473,  474,    0,
  475,  469,    0,    0,  310,    0,  311,    0,  517,  518,
  520,  373,    0,    0,    0,    0,    0,    0,    0,    0,
  344,    0,    0,    0,  342,  105,    0,  353,  147,    0,
    0,    0,  126,    0,    0,    0,  327,  321,    0,    0,
    0,  319,    0,    0,  202,    0,    0,    0,  182,    0,
  193,    0,    0,  496,  499,    0,  495,  472,  464,    0,
  468,  212,    0,    0,    0,  374,  350,  360,  359,  361,
  367,  364,  363,  491,    0,    0,    0,    0,  111,    0,
    0,    0,    0,  117,    0,    0,    0,  115,  110,    0,
    0,   73,   76,    0,    0,    0,    0,    0,    0,  227,
    0,  226,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  242,    0,  220,  221,
  228,  224,  229,  217,    0,  244,    0,    0,    0,    0,
  326,  317,   87,   90,  323,   82,    0,  210,    0,    0,
    0,    0,  351,    0,    0,    0,  312,  369,    0,  370,
    0,  456,    0,  486,  337,  335,    0,  112,    0,  120,
  113,  116,  118,  417,  418,  114,    0,    0,  354,    0,
    0,  398,  395,    0,    0,  413,  414,    0,    0,  399,
  415,  416,    0,    0,  401,    0,    0,  424,  384,  385,
    0,    0,    0,  218,  377,  393,  394,    0,    0,    0,
  219,  386,  223,    0,    0,    0,  249,  246,    0,  253,
  254,  238,  450,    0,  239,    0,  230,    0,  233,  225,
  222,  236,    0,  240,  375,    0,  216,  302,    0,  303,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   98,   96,   95,    0,    0,    0,    0,    0,    0,  497,
    0,    0,    0,    0,    0,  121,  119,  109,   69,    0,
  397,    0,    0,    0,    0,    0,    0,  425,  426,    0,
    0,  382,    0,    0,  391,  250,  251,    0,  248,    0,
  427,    0,    0,    0,    0,  237,    0,  329,  328,    0,
  324,   83,  448,    0,  101,  102,   97,   99,    0,   94,
    0,    0,    0,  195,    0,    0,  371,    0,    0,  462,
  455,  338,    0,  243,  396,    0,    0,  408,  409,    0,
    0,  411,  412,    0,    0,    0,  383,    0,    0,  392,
    0,  245,    0,  451,    0,  231,    0,  235,    0,  241,
  304,  330,    0,    0,  100,    0,    0,    0,    0,  487,
  476,  458,  457,    0,   70,   74,    0,    0,  400,    0,
  403,  402,    0,  405,  431,  378,    0,  379,  387,    0,
  388,    0,  247,    0,    0,    0,   84,   88,    0,    0,
  449,  184,  186,  188,    0,    0,   71,   72,    0,    0,
    0,    0,    0,  452,  232,  234,   85,   86,    0,  459,
  404,  406,  380,  389,  252,  479,
};
static const YYINT pfctlydgoto[] = {                      2,
   89,  308,  188,  251,  156,  739,  744,  752,  759,  726,
  408,  134,  222,  772,   23,   99,  206,  648,  159,  158,
  175,  432,  486,  176,  523,   24,   25,  199,   26,  679,
  735,   56,  767,  784,  870,  487,  618,  278,  479,  337,
  338,  680,  876,  740,  880,  745,  211,  214,  341,  600,
  409,  342,  508,  410,  613,  411,  498,  499,  512,  869,
  713,  399,  553,  400,  415,  506,  638,  488,  624,  489,
  754,  885,  755,  761,  888,  762,  333,  910,  636,  373,
  145,  413,   59,   61,  196,  490,  682,  828,  177,  178,
   83,  217,   84,  246,  247,  182,  367,  252,  683,  802,
  312,  445,  313,  268,  384,  385,  389,  390,  462,  463,
  391,  469,  470,  393,  453,  454,  386,  321,  591,  684,
  685,  696,  803,  804,  686,  768,  840,  304,  374,  375,
  190,  269,  270,  586,  649,  650,  284,  349,  350,  474,
  562,  563,  503,  423,  229,  441,  687,  524,  527,  528,
  602,  378,  597,  691,  792,  598,   27,   28,   29,   30,
   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
   41,    3,  139,  226,  292,  142,  294,  230,  362,  241,
  363,  873,  653,  730,  903,  694,  794,  697,  587,  305,
  285,  191,  392,  394,  387,  592,  475,
};
static const YYINT pfctlysindex[] = {                   -46,
    0,  699, 2452,   78,    0,  903,    0,    0,   45, -313,
 -149, -149, -149, 4131,  434, -144,  430,  -48, -128,  -31,
  224,    0,  714, -164,  -48, -164,  305,  315,  340,  350,
  382,  435,  459,  506,  536,  556,  564,  595,  602,  638,
  643,  664,    0,  707,  852,  720,  730,  752,  757,    0,
    0,  584,  704,  756,    0,    0,  380,    0, -164, -149,
  -48,  -48,  -48, -181,  407,  -96,  -94, -187,  217,  229,
 -181,  429,  464,  -48,  363,    0,  478, -149,  522, 2100,
  835,  469,  553,  628,    0, -313, -164,   25,    0,  -48,
 -149,  394,    0, -163, -163, -163,    0,    0,  434,  743,
  434,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  616,  656,  724,  830,  678,    0,  743,  743,
  743,    0,    0,  660,    0,  551,  573,  968,    0,  585,
  968,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  625,    0,  645,  651,  694,  659,  680,
  899,    0,    0,  688,    0,    0,  953,    0,  981,    0,
  105,  743,    0, -164,  678,  968,    0,    0,  615,  654,
 1370,    0,  770,    0,    0,    0,  394,  469,    0,    0,
    0,  -48,  -48,    0,    0,  820,  -48,  708,    0,    0,
  298,    0,    0, 1001,    0,    0,  -48,  820,  820,  820,
    0,    0,    0,    0,  968, -301,    0,  713,    0, 1089,
    0,    0,    0,    0,    0,    0, 1094,  741,    0,    0,
 2100, -149,    0,    0,    0,  722,    0,    0,  968,  615,
    0,    0,    0,  678,  -48, 1113,    0,  -74, 1109, 1125,
 1127, 1132, 1141,  733,  788,  809,  815,    0,    0, 1370,
  -74, -149,    0,    0,  743,  995,  -68,  -29,  743, 1171,
    0,  724,    0,    0,   79,  743,  -29,  -29,  -29,    0,
  968,   81,  968,  193,  829,  819, 1165,    0,    0,    0,
  981,  -32, 1192,    0,  371,  -48,  846,  526,  968,    0,
  968,    0,    0,  848,    0,  853,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  820,    0,  123,
  123,  123,  743,    0,    0,  968,    0,    0,   63,    0,
  875, 1005,  820,    0, 1225,  876,    0,  968,    0,   79,
  820,  916,  916,  916,    0,    0, -301,    0,    0,  713,
  888,  734,    0,    0,  924,    0,  572,  968,    0,  900,
  958,  363,    0,    0,  371, 1025,  961, 1025,    0, 1113,
    0,  964,    0,  762,    0, 1320,  -99,    0,  792,    0,
 1325, -271, 1336,  367, 1118,    0,  968,  996,    0,    0,
    0,    0,  820,  761,    0,   76,  968,  601,    0, 1110,
    0,  999, 1113,  -61, 1128,  -29,    0,    0,   27,    0,
  -29, 1004,    0,    0,    0,  968,  968,    0,    0,  829,
 1019,    0,    0,  -32,    0,    0,    0,    0,    0,  -59,
    9,    0,    9,  968,  579,  968,    0,  848,    0,  363,
  363,    0,  953,    0,    0,  853,    0,   46,   57,   82,
    0,  953,    0,    0,  202,  733,  363,    0,  953,    0,
  -11,   86,  148,    0, 1280,    0,    0,  -29,  614,  968,
  968,    0, 1335, 1337, 1348,  394, 1015,    0,    0,    0,
  -11,    0,    0,  843, 1369, 1023, 1027,    0, 1377,   76,
    0, 1045,  916,    0,  968,    0,   63,    0,    0,    0,
  968,  626,    0,    0,    0,    0,    0,    0,  968,    0,
    0,  968,    0,    0,   97,    0, 1092, 1161, 1092,    0,
    0,  964,    0,    0,    0,    0,  -99,    0,  733,    0,
  733,    0,  733,    0, -271,  733,    0,    0,    0,  367,
 1167,  968,  647, 1393,    0,    0, -149,    0,    0,  363,
    0,    0, 1280,    0,    0,  761,    0,  109,    0,    0,
    0,    0,  394,  675, 1043, 1052, 1046, 1406, 1389, 1058,
    0, -149, 1166, 1060,    0,    0, 1563,    0,    0,  -11,
 1333, 5133,    0,  823,  968, -149,    0,    0,  -29, 1340,
   97,    0,    0,  968,    0,  953,  953,  953,    0,  363,
    0,  -11,  875,    0,    0,   86,    0,    0,    0, 1411,
    0,    0,  968,  702,  968,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   35,    0,  968,  710,    0, 1064,
  330, 1065, 1082,    0, 1085,  857, 1100,    0,    0,  953,
  968,    0,    0, 1437,  304,  115,  150, 1204, 1205,    0,
 1206,    0,  129,  458,  857,  241, 1208,  878,  127, 1098,
  232,  252,  363, 1101, -149,  893,    0, 1121,    0,    0,
    0,    0,    0,    0, 5133,    0,  732,  968, 1113, 1454,
    0,    0,    0,    0,    0,    0, 1003,    0, 1114, 1116,
 1117,  733,    0, 1113,  968,  363,    0,    0,  109,    0,
  968,    0, 1218,    0,    0,    0, 1060,    0, 1201,    0,
    0,    0,    0,    0,    0,    0, -149, 1563,    0, 1505,
    0,    0,    0, 1122, 1471,    0,    0,  968, 1239,    0,
    0,    0,  968, 1240,    0, 1482, 1482,    0,    0,    0,
  968, 1129,  914,    0,    0,    0,    0,  968, 1131,  918,
    0,    0,    0,  857,   40, -231,    0,    0, 1482,    0,
    0,    0,    0, 1134,    0,  363,    0,  363,    0,    0,
    0,    0, 1244,    0,    0, -149,    0,    0,  823,    0,
  968,  768, 1135, 1523,  136,  363,  363,  363, -149, 1153,
    0,    0,    0, 1003,  953,  953,  953, 1497,  916,    0,
 1496,  968,  148,  394,  968,    0,    0,    0,    0, 1509,
    0, 1122,  921,  925,  930,  945, 2100,    0,    0,  214,
  914,    0,  472,  918,    0,    0,    0, 1151,    0,  805,
    0,  832,  891,  949,  394,    0,  968,    0,    0, 1113,
    0,    0,    0, 1157,    0,    0,    0,    0, -149,    0,
  733,  733,  733,    0, 1264,  363,    0,  968,  793,    0,
    0,    0, 1699,    0,    0,  818,  968,    0,    0,  841,
  968,    0,    0,  991,  851,  968,    0,  867,  968,    0,
  953,    0, -231,    0, 1162,    0,  363,    0,  363,    0,
    0,    0,   29, 1514,    0, 1518, 1519, 1521,  148,    0,
    0,    0,    0,  148,    0,    0, 1554, 1557,    0,  921,
    0,    0,  930,    0,    0,    0,  214,    0,    0,  472,
    0, 1174,    0, 1529, 1530, 1531,    0,    0, 1568, 1569,
    0,    0,    0,    0, 1295,  968,    0,    0,  968,  968,
  968,  968, 1540,    0,    0,    0,    0,    0,  394,    0,
    0,    0,    0,    0,    0,    0,
};
static const YYINT pfctlyrindex[] = {                   168,
    0,  919, 1047,    0,    0, 1398,    0,    0,    0, 3253,
    0,    0,    0,    0, 1250,    0,    0, 2492,    0,    0,
    0,    0,    0, 2860, 5082, 2907,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, 1535, 1672, 1809,    0,    0,    0,    0, 3565, 2007,
 1464, 1464, 1464,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, 1577,    0,    0,    0,    0,    0,    0,
 2126, 2245,    0, 2371,    0, 2318, 1484, 1194, 2610, 1054,
    0,    0,    0, 4998, 4998,  673,    0,    0, 3380, 2523,
 4671,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, 3620,    0,  557,  557,
  557,    0,    0, 1582,    0,    0,    0, -141,    0,    0,
 1207,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  215,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1212,    0,    0,
    0,    0,    0,    0,    0,    0,  727,    0,    0,    0,
    0,  231,    0,  794, 1848,  -23,    0,    0,    0,    0,
    0, 1120,    0,    0,    0,    0, 1585, 5034,    0,    0,
    0, 1108, 3435,    0,    0, 2714, 4932,    0,    0,    0,
  948,    0,    0,    0,    4,    0, 3750, 1088, 1088, 1088,
    0,    0,    0,    0,  196,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, 1006,    0,    0,  141,    0,
    0,    0,  328, 1158, 1860, 1194,    0, 1589,    6,  206,
  286,  738,  979,    0,    0,    0,    0,    0,    0,   11,
 1589,    0,    0,    0,  218, 3805,    0, 4217, 4980,    0,
    0,    0,    0,    0,    0, 4056,  278,  278,  278,    0,
  527, -134,   70, 1211,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, 1375, 1873,    2,   21,    0,
 1207,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1332,    0,    0,
    0,    0, 4111,    0,    0,  950,    0,    0,  860,    0,
 1404, 2609, 3181,    0,    0,    0,    0,  814,    0, 1600,
 4228, 1604, 1604, 1604,    0,    0,    0,    0,    0,    0,
    0, 1211,    0,    0, 1230,    0,  143,   26,    0,    0,
    0,    0,    0,    0, 1607,  591,    0, 3918,    0, 1194,
    0,    0,    0, 1211,    0,    0,    0,    0, 1211,    0,
    0,    0,    0,    0,    0,    0,  141,    0,    0, 2979,
 2979, 2979, 4353,    0,    0,    0,  194,    0,    0, 2490,
    0,    0, 1661,    0, 2725,  879,    0,    0,  860,    0,
 4479,    0,    0,    0,    0,  527,   70,    0,    0,    0,
 1254,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  774,    0,   77,   21, 1211,   70,    0,    0,    0,    0,
    0,    0,   94,    0,    0,    0,    0,    0,    0,    0,
    0,  -24,    0,    0,    0,    0,    0,    0,  609,    0,
  860,    0,    0,    0,    0,    0,    0, 4507,  948,  610,
  559,    0,    0,  977,  980,    0,  854,    0,    0,    0,
  860,    0,    0,  539,    0,    0,    0,    0,  682,    0,
    0,    0,  739,    0, 1207,    0,  860,    0, 1251,    0,
   71,  283, 4627,    0,    0,    0,    0,    0,   26,    0,
    0,  950,    0,    0, 1194,    0,   49, 1972,   28,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   20,   26,  143,    0,    0,    0, 3112,    0,    0,    0,
    0,    0, 3930, 4885,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  283,    0,    0,    0,    0,    0,    0,
    0,    0,  440,    0,    0,    0, 1235,    0,    0,  860,
 1619, 1235,    0,    0,  -23,    0,    0,    0, 1797, 1620,
 1194,    0,  211,   70,    0, 1242, 1242, 1242,    0,    0,
    0,  860,   14,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  610,  731,   99,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  889,   70, 1211,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  369,
   71,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    1,    0,  948,  610, 1194,   24,
    0,    0,    0,    0,    0,    0, 1235,    0,    0,    0,
    0,    0,    0,   18,   26,    0,    0,    0,    0,    0,
  209,    0, 1007,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, 1235,    0,    0,
 1605,    0,    0,    0,    0,    0,    0,  950,  403,    0,
    0,    0,  950, 1182,    0, 4742, 4742,    0,    0,    0,
  559,  997,    0,    0,    0,    0,    0,  559, 1140,    0,
    0,    0,    0,    0,    0,    0,    0,    0, 4742,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, 4770,    0,    0,    0,    0,    0,    0,    0,
   21,    2,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    5,  948,  948,  948,    0,   22,    0,
    0,   99,    0,    0,   70,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -230,
    0, 1211,  948,  948,    0,    0,  610,    0,    0, 1194,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, 1632,    0,    0,  262,  219,    0,
    0,    0,    0,    0,    0,  948,  610,    0,    0,  948,
  610,    0,    0, 2953,  731,   99,    0,  731,   99,    0,
 1242,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, 1635,  262,    0,    0,  610,  610,
   99,   99,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
static const YYINT pfctlygindex[] = {                     0,
 3551,    0, -183,   33, -359,    0,    0, -542, -550, -623,
  -79, 1576,    0,    0, 1647,  156, 2443,    0,    0,    0,
    0,    0,  488, 1574,    0, 1637,    0, 1091,    0,    0,
 -658,    0,    0,  810,  697, -342,    0, 1839,    0, -363,
    0,    0,    0, -767,    0, -758,    0, 1387,  901, 1130,
 1150,    0,    0,  265,    0,    0, -461,    0, 1179,    0,
    0, -432,    0,  904,    0, -551,    0, 1176,    0, -524,
    0,    0, -710,    0,    0, -712,    0,    0,    0, -569,
    0, 1066, 1597,   -9, 1487,  -90,    0, -218,  858, 1445,
  837,  112,  831,    0, 1386,    0,    0, -254,    0,    0,
 1417,    0, -339,    0,    0, 1241,    0, 1234,    0, 1154,
    0,    0, 1156,    0,    0, 1163,    0, -440, 1144, 1026,
    0,    0,  906,    0,    0, -714,    0,    0, 1329,    0,
 1522, 1443,    0,  985,  998,    0,    0, 1380,    0, -356,
 1168,    0, 1021, -347,    0, 1356,    0, -563, 1293,    0,
    0,    0, -652,    0,    0, 1136,    0,    0,  845,  856,
    0, 1751, 1753,   -3,   -2,    0,    0,    0,    0,    0,
    0,    0, -152, -136,    0, -156,    0,    0,    0, -175,
 1327,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 5525
static const YYINT pfctlytable[] = {                     48,
   49,  197,   62,   63,  228,  424,  425,  250,  189,  347,
  214,  554,  438,  142,   92,  163,  180,  540,  542,  544,
  152,  187,  681,  488,  547,  548,  138,  490,  141,  352,
  688,  492,  637,  325,  513,  282,  791,  316,  937,  552,
  480,  763,  446,  625,  476,  477,  488,  368,  311,  256,
  128,  839,  352,  347,  336,  877,  325,  187,  316,  187,
  316,  505,  136,  522,  347,  347,  881,  325,  160,  347,
  301,  293,  309,  291,  496,  821,    1,  764,  513,  838,
   58,  193,  458,  459,  460,  539,  320,   50,  290,  143,
  535,  536,  137,  198,  497,  187,  541,   97,  606,   98,
  607,   81,  608,  347,   57,  610,  197,  549,  483,  320,
  347,  407,  302,  347,  347,  681,  357,  132,  360,  886,
  889,  543,  461,  214,  240,  398,  140,  801,  163,  187,
  347,  347,  380,  152,  200,  484,  482,  485,  765,  513,
  836,  483,  347,  572,  250,  347,  325,  186,  325,  407,
  347,  510,  949,  938,  355,  583,  358,  711,  347,  347,
  347,  483,  398,  875,  950,  815,  774,    1,  484,  482,
  485,  316,  381,  712,  382,  854,  189,    1,  933,  519,
  347,  101,  513,  705,  812,  407,  430,  496,  484,  482,
  485,  434,  604,  347,  347,  347,  444,  902,  481,  404,
  620,  348,  623,  427,  426,  356,  144,  497,  448,  835,
  832,  419,  133,  456,  127,  171,  951,  952,   88,  595,
   93,  257,  347,  347,   52,  847,  347,  249,  347,  513,
  626,  435,  299,  347,  801,  347,  240,  738,  339,  347,
  297,  546,  185,   91,   60,  397,  483,  340,  347,   85,
  702,  751,  347,  347,   53,  347,  347,  347,  513,  513,
  472,  808,  327,  450,  451,   90,  405,  136,  347,  532,
  491,  776,  743,  484,  482,  485,  525,  537,  513,  714,
  766,   91,  303,  890,   92,  526,  545,  339,  887,  515,
  516,  778,  325,  550,  452,  165,  255,  137,  140,  140,
  189,  347,  488,  566,  405,  347,  490,  530,  352,  533,
  492,  777,  779,  780,  102,  513,  347,  359,  347,  310,
  347,  347,  513,  325,  103,  334,  335,  316,  171,  513,
  513,  513,  504,  502,  520,  521,  590,  131,  281,  254,
  405,  240,  513,  567,  568,  320,  811,  406,  347,  104,
  734,  868,  163,  163,  163,  163,  163,  163,  163,  105,
  325,  248,  163,  163,  163,  306,   17,  152,  584,  513,
  347,  347,  347,  346,  588,  488,  320,  616,  107,  490,
  325,  352,  593,  138,  316,  594,  347,  138,  325,  325,
  325,  106,  316,  316,  316,  513,  488,  140,  590,  163,
  138,  513,  352,  352,  152,  513,  325,  325,  165,  107,
  316,  316,  407,  320,  347,  614,  843,  325,  844,  347,
  906,  907,  908,    1,    1,    1,    1,    1,  494,  495,
  699,  700,  701,  320,  837,  407,  855,  856,  857,  319,
  320,  320,  320,  320,  107,  596,  407,  945,  709,  460,
  319,  320,  946,    1,    1,    1,  513,  513,  689,  320,
  320,  865,  717,  347,  347,  347,    1,  698,  108,  194,
  195,  347,  347,  347,  728,  319,  320,  146,    1,  396,
    1,    1,    1,    1,    1,  297,  707,  513,  710,  148,
  483,  297,  347,  347,  347,  347,  347,  347,  248,    1,
  715,    1,  194,  195,  483,    1,  911,  647,  736,  737,
   91,  789,  678,    1,  729,  109,  396,  484,  482,  485,
  773,  596,  749,  750,    1,  407,  147,  407,  829,  853,
  297,  484,  482,  485,  347,    1,  513,  935,  149,  936,
  339,  494,  495,  741,  742,  110,  339,  619,  358,  764,
  841,  790,  171,  171,  171,  171,  171,  171,  171,  347,
  513,    1,  171,  171,  171,  111,  297,   91,  810,  240,
  347,  358,  634,  112,  813,   91,   91,   91,  358,  358,
  758,  732,  358,  513,  513,  785,  690,  347,  347,  347,
  347,  347,  297,   91,   91,  319,  320,  297,  358,  171,
  300,  823,  347,  347,  113,  678,  825,  749,  750,  596,
  765,  114,  513,  513,  830,  240,  850,  800,  347,  347,
  347,  833,  240,  123,  502,  154,  155,  129,  719,  861,
  862,  863,  165,  165,  165,  165,  165,  165,  165,  339,
  496,  720,  165,  165,  165,  154,  155,  115,  647,  191,
  379,  347,  116,  347,  848,  347,  347,  240,  513,  513,
  497,  358,  407,  358,  893,  782,  895,  897,  899,  240,
  370,  513,  407,  117,  513,  867,  513,  513,  872,  165,
  513,  407,  407,  511,  407,  407,    5,    6,    7,  129,
  240,  362,  513,  914,  129,  407,  433,  733,   81,   82,
  920,  407,  407,  531,  923,  407,  407,  407,   22,  927,
  901,  407,  930,  300,  362,  932,  118,  817,  240,  407,
  465,  362,  362,  871,  800,  362,  466,  467,  407,  119,
  513,  912,  371,   81,  347,  551,   48,  372,  565,  120,
  921,  362,   86,  124,  924,  240,  407,  167,  492,  928,
  589,  513,  931,  240,  785,  511,  154,  155,  513,  407,
  468,  121,  300,  513,  407,  240,  122,  407,  407,  407,
  596,  615,  407,  126,  429,  240,  846,  240,  460,  460,
  460,  460,  460,  320,  460,  407,  407,  194,  195,  858,
  513,  513,  513,  407,  407,  125,  407,  407,  358,  627,
  135,  358,  447,  272,  362,  240,  362,  358,  358,  960,
  460,  240,  961,  962,  963,  964,  358,  358,  358,  297,
  358,  358,  151,  358,  297,  297,  708,  358,  358,  358,
  297,  358,  455,  297,  716,  240,  240,  358,  358,  358,
  358,  358,  358,  358,  358,  892,  347,  358,  240,  905,
  358,  756,  757,  347,  651,  358,  788,  152,  300,  300,
  167,  240,  358,  372,  358,  756,  757,  300,  871,  917,
  918,  157,  894,  347,  179,  240,  703,  358,  358,  358,
  358,  358,  358,  358,  240,  181,  372,  576,  339,  575,
  347,  215,  849,  183,  240,  358,  320,  372,  467,  138,
  358,  492,  493,  358,  358,  358,  161,  162,  358,  358,
  240,  339,  180,  372,  372,  372,  272,  913,  297,  138,
  347,  358,  358,  208,  200,  200,  200,  300,  358,  358,
  358,  896,  358,  358,  240,  203,  347,  207,  347,  480,
  480,  362,  919,  216,  362,  223,  480,  480,  480,  221,
  362,  362,  347,  347,    4,    5,    6,    7,  238,  362,
  362,  362,  513,  362,  362,  922,  362,  224,  513,  513,
  362,  362,  362,   93,  362,  926,  372,  225,  372,  227,
  362,  362,  362,  362,  362,  362,  362,  362,  169,  898,
  362,  929,  240,  362,  494,  495,  240,    8,  362,   94,
   95,   96,  513,  347,  347,  362,  381,  362,  248,    9,
  258,   10,   11,   12,   13,   14,  461,  204,  205,  231,
  362,  362,  362,  362,  362,  362,  362,  492,  274,  381,
   15,  925,   16,  513,  240,  187,   17,  272,  362,  232,
  381,  283,  320,  362,   18,  233,  362,  362,  362,  209,
  210,  362,  362,  236,  513,   19,  381,  381,  381,  272,
  272,  272,  272,  290,  362,  362,   20,  272,  234,  235,
  272,  362,  362,  362,  237,  362,  362,  492,  492,  492,
  492,  492,  239,  492,  167,  167,  167,  167,  167,  167,
  167,  347,   21,  277,  167,  167,  167,  308,  513,  513,
  513,  169,  280,  513,  513,  513,  140,  513,  513,  492,
  320,  513,  513,  372,  347,  347,  372,  212,  213,  381,
  513,  381,  372,  372,  513,  513,  319,  320,  295,  153,
  272,  167,  372,  372,  297,  372,  372,   94,   95,   96,
  296,  339,  372,  372,  372,  187,  372,  339,  314,  390,
  753,  760,  372,  372,  334,  335,  372,  372,  372,  372,
  138,  138,  372,   51,  315,  372,  316,  275,  339,  339,
  372,  317,  390,   52,   53,   54,  290,  372,  453,  372,
  318,  339,  322,  390,  339,  201,  202,  352,  353,  354,
  339,  410,  372,  372,  372,  372,  372,  372,  372,  390,
  390,  390,  339,  323,  453,  453,  453,  347,  347,  324,
  372,  344,   55,  364,  410,  372,  520,  521,  372,  372,
  372,  419,  361,  372,  372,  410,  365,  465,  465,  465,
  465,  465,  369,  465,  401,  402,  372,  372,  377,  428,
  339,  383,  153,  372,  372,  372,  388,  372,  372,  242,
  724,  725,  419,  138,  138,  419,  381,  243,  412,  465,
  108,  339,  390,  429,  390,  417,  381,  414,  339,  418,
  428,  770,  771,  428,  513,  381,  381,  422,  381,  381,
  275,  428,  513,  106,  244,  245,  783,  195,  329,  381,
  330,  331,  332,  436,  429,  381,  381,  429,  440,  381,
  381,  381,  370,  431,  410,  381,  410,  749,  750,  513,
  513,  756,  757,  381,  736,  737,  509,  753,  878,  879,
  760,  513,  381,  741,  742,  169,  169,  169,  169,  169,
  169,  169,  453,  453,  453,  169,  169,  169,  882,  883,
  381,  513,  513,  347,  347,  461,  461,  461,  461,  461,
  308,  461,  437,  381,  442,  308,  308,  310,  381,  795,
  449,  381,  381,  381,  308,  457,  381,  796,  797,  798,
  519,  519,  169,  521,  521,  290,  464,  461,  564,  381,
  381,  290,  290,  290,  290,  471,  799,  381,  381,  473,
  381,  381,  501,  518,  500,  569,  507,  570,  514,  390,
  290,  290,  290,  290,  290,  290,  290,  258,  571,  390,
  573,  290,  290,  488,  753,  577,  578,  760,  390,  390,
  579,  390,  390,  580,  275,  275,  275,  582,  599,  601,
  258,  275,  390,  617,  275,  612,  488,  628,  390,  390,
  630,  410,  390,  390,  390,  629,  631,  290,  390,  308,
  632,  410,  633,  504,  635,  652,  390,  706,  718,  721,
  410,  410,  693,  410,  410,  390,  151,  151,  151,  151,
  151,  151,  151,  290,  410,  722,  731,  151,  151,  723,
  410,  410,  727,  390,  410,  410,  410,  746,  747,  748,
  410,  769,  775,  272,  275,  781,  390,  290,  410,  692,
  793,  390,  814,  786,  390,  390,  390,  410,  805,  390,
  806,  807,  816,  153,  819,  733,  272,  822,  419,  824,
  826,  827,  390,  390,  831,  410,  834,  842,  845,  851,
  390,  390,  852,  390,  390,  859,  428,  864,  410,  419,
  106,  106,  866,  410,  260,  891,  410,  410,  410,  874,
  904,  410,  909,  106,  941,  934,  106,  428,  942,  943,
  429,  944,  106,  947,  410,  410,  948,  260,  953,  954,
  955,  956,  410,  410,  106,  410,  410,  957,  958,  959,
  965,  429,  277,  419,  419,  419,  290,  138,  419,  419,
  419,   28,  419,  419,   63,  187,  419,  419,  206,  308,
  347,  428,  428,  428,  513,  419,  428,  428,  428,  141,
  428,  428,  106,  492,  428,  428,  130,  138,  555,  556,
  557,  558,  559,  428,  560,  429,  429,  429,   75,   89,
  429,  429,  429,  106,  429,  429,  513,  106,  429,  429,
  106,  477,  290,  290,  478,  108,  150,  429,  290,   45,
  561,  290,  163,   87,  900,  966,  585,  258,  603,  258,
  258,  258,  258,  258,  258,  258,  258,  258,  345,  574,
  490,  258,  258,  258,  258,  581,  258,  258,  704,  258,
  258,  262,  184,  273,  884,  298,  366,  326,  534,  538,
  258,  258,  488,  258,  258,  258,  258,  258,  609,  605,
  258,  258,  258,  439,  262,  611,  258,  622,  915,  860,
  787,  290,  325,  271,  258,  820,  259,  260,  261,  262,
  263,  264,  265,  258,  809,  818,  290,  266,  267,  420,
  621,  290,  290,  443,  258,  529,  695,  290,  290,  290,
  290,  258,  488,  488,  488,  488,  488,  939,  488,  272,
  272,  272,  272,   46,  258,   47,  517,  272,  940,  258,
  272,    0,  258,  258,  258,  488,    0,  258,    0,    0,
    0,    0,    0,    0,  488,    0,    0,    0,    0,    0,
  258,  258,    0,  272,    0,    0,  488,    0,  258,  258,
  258,    0,    0,    0,  260,    0,  260,  260,  260,  260,
  260,  260,  260,  260,  260,    0,  339,    0,  260,  260,
  260,  260,    0,  260,  260,    0,  260,  260,  263,    0,
  272,    0,    0,  916,    0,  290,    0,  260,  260,  339,
  260,  260,  260,  260,  260,    0,    0,  260,  260,  260,
  272,  263,    0,  260,    0,    0,    0,    0,  272,  272,
  272,  260,  639,  640,    0,    0,    0,  275,    0,    0,
  260,    0,    0,    0,    0,  641,  272,  272,  642,  290,
    0,  260,    0,    0,  643,    0,  272,    0,  260,    0,
  275,    0,  103,    0,    0,    0,  644,    0,    0,    0,
    0,  260,  290,    0,  106,  106,  260,    0,    0,  260,
  260,  260,    0,    0,  260,  103,    0,  106,    0,    0,
  106,    0,    0,    0,    0,    0,  106,  260,  260,  339,
    0,    0,    0,    0,  645,  260,  260,  260,  106,    0,
    0,  262,    0,  262,  262,  262,  262,  262,  262,  262,
  262,  262,    0,    0,    0,  262,  262,  262,  262,  490,
  262,  262,  646,  262,  262,    5,    6,    7,    0,    0,
    0,    0,    0,    0,  262,  262,  106,  262,  262,  262,
  262,  262,    0,    0,  262,  262,  262,    0,    0,    0,
  262,  322,    0,    0,    0,    0,    0,  106,  262,    0,
    0,    0,    0,    0,  106,    0,    0,  262,    0,  490,
  490,  490,  490,  490,  322,  490,    0,    0,  262,    0,
    0,   10,   11,   12,   13,  262,   61,    0,    0,    0,
    0,    0,  490,    0,    0,    0,    0,    0,  262,    0,
    0,  490,    0,  262,    0,    0,  262,  262,  262,   61,
    0,  262,    0,  138,    0,    0,    0,   61,    0,    0,
   61,    0,    0,   61,  262,  262,  287,  288,  289,  339,
    0,    0,  262,  262,  262,  339,    0,    0,  263,    0,
  263,  263,  263,  263,  263,  263,  263,  263,  263,    0,
    0,    0,  263,  263,  263,  263,    0,  263,  263,    0,
  263,  263,    0,    0,  322,    0,  339,    0,    0,    0,
    0,  263,  263,    0,  263,  263,  263,  263,  263,    0,
    0,  263,  263,  263,  275,  275,  275,  263,    0,    0,
    0,  275,    0,    0,  275,  263,    0,  290,  290,   61,
    0,   61,    0,  290,  263,  282,  290,    0,    0,    0,
  103,  103,    0,    0,    0,  263,  103,  275,    0,  103,
    0,    0,  263,  339,    0,    0,    0,    0,  282,  290,
    0,  339,  339,  339,    0,  263,  395,    0,    0,    0,
  263,    0,  103,  263,  263,  263,    0,    0,  263,  339,
  339,  416,    0,    0,  275,    0,    0,    0,    0,  421,
    0,  263,  263,    0,    0,    0,  290,    0,    0,  263,
  263,  263,    0,    0,  275,    0,    0,    0,    0,  103,
    0,    0,  275,  275,  275,    0,  290,    0,    0,    0,
    0,    0,    0,    0,  290,  290,  290,    0,    0,  103,
  275,  275,    0,    0,    0,    0,    0,  103,  103,  103,
  275,  478,  290,  290,    0,    0,    0,    0,    0,    0,
    0,    0,  290,    0,  279,  103,  103,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   61,    0,    0,   61,
    0,  322,    0,   61,   61,   61,   61,  279,    0,    0,
   61,   61,   61,   61,    0,   61,   61,    0,   61,   61,
    0,    0,    0,    0,    0,   61,   61,   61,    0,   61,
    0,    0,    0,    0,    0,   61,   61,    0,  322,   61,
   61,   61,   61,    0,    0,   61,    0,    0,   61,    0,
    0,    0,    0,   61,    0,    0,    0,   68,  322,    0,
   61,    0,   61,    0,    0,    0,  322,  322,  322,    0,
    0,    0,    0,   61,    0,   61,   61,   61,   61,   61,
   61,   61,    0,    0,  322,  322,    0,    0,    0,    0,
    0,    0,    0,   61,    0,    0,    0,    0,   61,    0,
    0,   61,   61,   61,    0,    0,   61,   61,    0,    0,
  278,    0,    0,    0,    0,  282,    0,    0,  282,   61,
   61,  282,  282,  282,  282,  282,   61,   61,   61,  282,
  282,  282,  282,  278,  282,  282,  164,  282,  282,    0,
    0,    0,    0,    0,  282,  282,  282,    0,  282,  282,
    0,  282,  282,  282,  282,  282,    0,  165,  282,  282,
  282,  282,    0,    0,  282,    0,    0,  282,    0,    0,
   68,    0,  282,    0,    0,    0,    0,    0,    0,  282,
    0,  282,    0,    0,    0,    0,    0,    0,  282,    0,
    0,   43,    0,    0,  282,  282,  282,  282,  282,  282,
  282,  166,  167,  168,    0,    0,  169,  170,  171,    0,
  172,  173,  282,    0,  161,  162,    0,  282,    0,    0,
  282,  282,  282,  174,    0,  282,  282,    0,    0,  343,
    0,    0,    0,    0,  279,    0,    0,  279,  282,  282,
    0,  279,  279,  279,  279,  282,  282,  282,  279,  279,
  279,  279,  343,  279,  279,    0,  279,  279,    0,    0,
    0,    0,  297,    0,  279,  279,    0,  279,  279,    0,
  279,  279,  279,  279,  279,    0,    0,  279,  279,  279,
  279,    0,    0,  279,    0,  297,  279,    0,    0,    0,
    0,  279,    0,    0,    0,    0,    0,    0,  279,    0,
  279,  218,  219,  220,    0,    0,   44,  279,    0,   68,
    0,   68,    0,   68,   68,   68,   68,    0,  279,    0,
    0,   68,    0,    0,   68,    0,    0,    0,    0,    0,
    0,  279,    0,    0,    0,    0,  279,    0,    0,  279,
  279,  279,  343,    0,  279,    0,    0,    0,  333,    0,
    0,    0,    0,    0,  253,    0,    0,  279,  279,    0,
  278,    0,    0,  278,  279,  279,  279,  278,  278,  278,
  278,  333,    0,    0,  278,  278,  278,  278,    0,  278,
  278,    0,  278,  278,   68,    0,    0,    0,    0,    0,
  278,  278,    0,  278,  278,    0,  278,  278,  278,  278,
  278,    0,    0,  278,  278,  278,  278,    0,    0,  278,
    0,    0,  278,    0,    0,    0,    0,  278,    0,    0,
    0,    0,    0,    0,  278,    0,  278,    0,    0,    0,
    0,    0,    0,  278,    0,    0,    0,   42,    5,    6,
    7,    0,    0,    0,  278,    0,    0,  328,    0,    0,
    0,  343,    0,  308,    0,    0,    0,  278,  351,    0,
    0,  333,  278,    0,  341,  278,  278,  278,    0,    0,
  278,    0,    0,    0,    0,    0,  308,    0,    0,  343,
    8,    0,  343,  278,  278,    0,    0,  341,  343,  343,
  278,  278,  278,    0,   10,   11,   12,   13,  343,  343,
    0,  343,  343,    0,    0,  403,    0,    0,  343,  343,
  343,    0,  343,    0,    0,  297,    0,    0,  343,  343,
  297,  297,  343,  343,  343,  343,  297,    0,  343,  297,
    0,  343,    0,    0,    0,    0,  343,    0,    0,    0,
    0,  297,    0,  343,    0,  343,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  343,  343,
  343,  343,  343,  343,  343,    0,    0,    0,  290,  290,
  290,  290,  290,  290,  290,    0,  343,  341,  290,  290,
  290,  343,    0,    0,  343,  343,  343,    0,    0,  343,
  343,  297,  297,  297,  297,  297,    0,  297,  333,  272,
    0,    0,  343,  343,    0,    0,    0,  333,  333,  343,
  343,  343,    0,    0,  297,    0,    0,  333,  333,    0,
  333,  333,  272,  297,    0,    0,    0,  333,  333,  333,
    0,  333,    0,    0,    0,  297,  297,  333,  333,    0,
    0,  333,  333,  333,  333,    0,  272,  333,    0,    0,
  333,    0,    0,    0,    0,  333,    0,    0,    0,    0,
    0,    0,  333,    0,  333,    0,    0,    0,    0,  272,
    0,    0,    0,    0,    0,    0,    0,  333,  333,  333,
  333,  333,  333,  333,    0,    0,  151,  151,  151,  151,
  151,  151,  151,    0,    0,  333,  153,  151,  151,    0,
  333,    0,    0,  333,  333,  333,  308,    0,  333,  333,
    0,  308,  308,    0,  341,    0,    0,    0,  467,    0,
  308,  333,  333,    0,  341,    0,    0,    0,  333,  333,
  333,    0,  308,  341,  341,    0,  341,  341,    0,    0,
    0,  467,    0,  341,  341,  341,    0,  341,    0,    0,
    0,    0,    0,  341,  341,    0,    0,  341,  341,  341,
  341,    0,    0,  341,    0,    0,  341,    0,    0,    0,
    0,  341,    0,    0,    0,    0,    0,    0,  341,    0,
  341,    0,  308,  308,  308,  308,  308,    0,  308,    0,
    0,    0,    0,  341,  341,  341,  341,  341,  341,  341,
    0,    0,    0,    0,    0,  308,    0,    0,    0,    0,
    0,  341,    0,    0,  308,    0,  341,    0,    0,  341,
  341,  341,    0,    0,  341,  341,  308,  308,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  341,  341,    0,
    0,    0,    0,    0,  341,  341,  341,    0,    0,  272,
    0,  463,  272,    0,  272,  272,  272,  272,  272,  272,
    0,    0,    0,  272,  272,  272,  272,    0,  272,  272,
    0,  272,  272,    0,  463,    0,    0,    0,    0,    0,
    0,    0,  272,  272,    0,  272,  272,  272,  272,  272,
    0,    0,  272,  272,  272,    0,    0,    0,  272,  272,
    0,  272,  272,  272,  272,  272,  272,    0,    0,    0,
  272,  272,  272,  272,    0,  272,    0,    0,    0,    0,
  308,    0,    0,    0,    0,    0,  272,  272,    0,    0,
    0,    0,    0,  272,    0,    0,    0,    0,    0,  272,
    0,    0,  272,  308,    0,    0,  272,    0,  272,    0,
    0,  272,    0,    0,  272,  272,  272,    0,    0,  272,
  272,    0,    0,    0,    0,    0,    0,    0,  467,    0,
    0,  467,  272,  272,    0,    0,  467,  467,  467,    0,
  272,  272,  467,  467,  467,  467,    0,  467,  467,  513,
  467,  467,   68,    0,    0,    0,    0,    0,  272,    0,
    0,  467,    0,    0,    0,    0,    0,  467,  467,    0,
  513,  467,  467,  467,    0,   68,    0,  467,    0,  272,
    0,    0,    0,    0,    0,  467,  272,    0,    0,    0,
    0,    0,    0,    0,  467,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  465,  465,  465,
  465,  465,  467,  465,  513,  513,  513,    0,    0,  513,
  513,  513,    0,  513,  513,  467,    0,  513,  513,    0,
  467,    0,    0,  467,  467,  467,  513,    0,  467,  465,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  467,  467,    0,    0,    0,    0,    0,    0,  467,
  467,  463,    0,    0,  463,   68,    0,    0,    0,  463,
  463,  463,    0,    0,    0,  463,  463,  463,  463,  277,
  463,  463,    0,  463,  463,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  463,    0,    0,    0,    0,    0,
  463,  463,  277,    0,  463,  463,  463,    0,    0,    0,
  463,    0,    0,    0,    0,    0,    0,    0,  463,    0,
    0,    0,    0,    0,    0,    0,    0,  463,    0,    0,
    0,    0,    0,  308,  290,    0,    0,    0,  308,  308,
  463,  463,  463,  463,  463,  463,  463,  308,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  290,  463,    0,
  308,  308,    0,  463,    0,    0,  463,  463,  463,    0,
    0,  463,  463,  308,    0,    0,  308,    0,    0,    0,
    0,    0,  308,    0,  463,  463,    0,    0,    0,    0,
    0,    0,  463,  463,  308,    0,    0,    0,    0,    0,
    0,    0,   68,    0,   68,   68,   68,    0,   68,   68,
   68,   68,   68,    0,    0,    0,   68,   68,   68,   68,
    0,   68,   68,    0,   68,   68,    0,    0,    0,    0,
    0,    0,  308,    0,    0,   68,    0,    0,    0,    0,
    0,   68,   68,    0,    0,   68,   68,   68,    0,    0,
    0,   68,    0,  308,    0,    0,    0,    0,    0,   68,
  308,    0,    0,    0,  272,  100,    0,    0,   68,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   68,  272,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   68,
    0,  129,  130,  131,   68,    0,    0,   68,   68,   68,
    0,    0,   68,    0,  153,    0,    0,    0,    0,  275,
    0,    0,    0,    0,    0,   68,   68,    0,    0,  277,
  192,    0,  277,   68,   68,    0,  277,  277,  277,  277,
    0,    0,  275,  277,  277,  277,  277,    0,  277,  277,
    0,  277,  277,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  277,  277,    0,  277,  277,  277,  277,  277,
    0,    0,  277,  277,  277,    0,    0,  272,  277,    0,
    0,    0,    0,    0,  290,    0,  277,  290,    0,    0,
    0,    0,  290,  290,  290,  277,    0,    0,  290,  290,
  290,  290,    0,  290,  290,    0,  290,  290,    0,    0,
    0,    0,    0,  277,    0,    0,    0,  290,  290,    0,
  290,  290,  290,  290,  290,    0,  277,  290,  290,  290,
    0,  277,  275,  290,  277,  277,  277,    0,    0,  277,
    0,  290,  275,  276,    0,    0,    0,  279,    0,  290,
  290,    0,  277,  277,    0,    0,    0,  286,    0,    0,
  277,  277,    0,    0,    0,    0,    0,    0,  290,    0,
    0,    0,  290,    0,    0,    0,    0,    0,    0,    0,
    0,  290,    0,    0,    0,    0,  290,    0,    0,  290,
  290,  290,    0,    0,  290,  307,    0,    0,    0,    0,
    0,    0,    0,    0,  500,    0,    0,  290,  290,    0,
    0,    0,    0,    0,  272,  290,  290,  272,    0,    0,
  272,  272,  272,  272,  272,    0,    0,  500,  272,  272,
  272,  272,    0,  272,  272,    0,  272,  272,    0,    0,
    0,    0,    0,    0,    0,    0,  376,  272,    0,    0,
    0,    0,    0,  272,  272,    0,    0,  272,  272,  272,
    0,    0,  290,  272,    0,    0,    0,    0,    0,  275,
    0,  272,  275,    0,    0,    0,  275,  275,  275,  275,
  272,    0,    0,  275,  275,  275,  275,    0,  275,  275,
    0,  275,  275,    0,    0,    0,    0,    0,  272,    0,
    0,    0,  275,    0,    0,    0,    0,    0,  275,  275,
    0,  272,  275,  275,  275,    0,  272,  300,  275,  272,
  272,  272,    0,    0,  272,    0,  275,    0,    0,  466,
    0,    0,    0,    0,    0,  275,    0,  272,  272,    0,
  300,    0,    0,    0,    0,  272,  272,    0,    0,    0,
    0,    0,  466,  275,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  275,    0,    0,    0,
    0,  275,    0,    0,  275,  275,  275,    0,    0,  275,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  275,  275,    0,    0,    0,    0,    0,  290,
  275,  275,  290,    0,    0,    0,    0,  290,  290,  290,
    0,    0,    0,  290,  290,  290,  290,    0,  290,  290,
    0,  290,  290,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  290,    0,    0,    0,    0,    0,  290,  290,
    0,    0,  290,  290,  290,    0,    0,    0,  290,    0,
    0,    0,    0,    0,  500,  297,  290,  500,    0,    0,
    0,    0,  500,  500,  500,  290,    0,    0,  500,  500,
  500,  500,    0,  500,  500,    0,  500,  500,  297,    0,
    0,    0,    0,  290,    0,    0,    0,  500,    0,    0,
    0,    0,    0,  500,  500,    0,  290,  500,  500,  500,
    0,  290,    0,  500,  290,  290,  290,    0,    0,  290,
  297,  500,    0,    0,    0,    0,    0,    0,    0,    0,
  500,    0,  290,  290,    0,    0,    0,    0,    0,    0,
  290,  290,    0,  297,    0,    0,    0,    0,  500,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  500,    0,    0,    0,    0,  500,    0,    0,  500,
  500,  500,    0,    0,  500,    0,    0,    0,  297,    0,
    0,    0,    0,    0,    0,  300,  300,  500,  500,  466,
    0,    0,  466,    0,  300,  500,  500,  466,  466,  466,
    0,    0,    0,  466,  466,  466,  466,    0,  466,  466,
    0,  466,  466,    0,    0,    0,    0,  300,    0,    0,
    0,    0,  466,    0,    0,    0,  339,    0,  466,  466,
    0,    0,  466,  466,  466,    0,    0,  308,  466,    0,
    0,    0,    0,    0,    0,    0,  466,    0,    0,  339,
    0,    0,    0,    0,  300,  466,    0,    0,    0,    0,
  308,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  466,  300,    0,    0,    0,    0,    0,
    0,    0,  300,  300,  300,    0,  466,    0,    0,    0,
    0,  466,    0,    0,  466,  466,  466,    0,    0,  466,
  300,  300,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  466,  466,    0,  297,    0,    0,  297,    0,
  466,  466,    0,  297,  297,  297,    0,    0,    0,  297,
    0,    0,  297,    0,  297,  297,    0,  297,  297,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  297,    0,
  308,    0,    0,    0,  297,  297,    0,    0,  297,  297,
  297,    0,  308,    0,  297,    0,    0,    0,    0,    0,
  297,    0,  297,  297,    0,    0,    0,    0,  297,  297,
  297,  297,    0,    0,  297,  308,    0,  297,    0,  297,
  297,    0,  297,  297,    0,    0,    0,    0,    0,  297,
    0,    0,    0,  297,    0,    0,    0,    0,    0,  297,
  297,    0,  297,  297,  297,  297,    0,  297,    0,  297,
  297,  297,  297,    0,    0,  297,    0,  297,    0,    0,
    0,    0,    0,    0,    0,    0,  297,    0,  297,  297,
    0,    0,   64,    0,    0,    0,  297,  297,   65,   66,
   67,   68,   69,   70,  297,   71,    0,   72,    0,   73,
   74,   75,    0,    0,    0,   76,   77,  297,    0,    0,
    0,    0,  297,    0,    0,  297,  297,  297,    0,  339,
  297,    0,    0,    0,    0,  339,    0,  308,  339,    0,
  308,    0,    0,  297,  297,  308,  308,  308,    0,   78,
    0,  297,  297,    0,  308,  339,  308,  308,    0,  308,
  308,  339,    0,    0,    0,    0,  339,   79,   80,    0,
  308,    0,    0,    0,    0,    0,  308,  308,    0,    0,
  308,  308,  308,    0,    0,    0,  308,    0,    0,  339,
    0,    0,    0,    0,  308,    0,    0,    0,    0,    0,
    0,    0,    0,  308,    0,  339,  339,  339,  339,  339,
    0,  339,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  308,    0,    0,    0,    0,    0,    0,  339,    0,
    0,    0,    0,    0,  308,    0,    0,  339,    0,  308,
    0,    0,  308,  308,  308,    0,    0,  308,    0,  339,
  339,  339,    0,    0,    0,    0,    0,    0,    0,    0,
  308,  308,  308,    0,    0,  308,    0,    0,  308,  308,
  308,  308,  308,    0,    0,    0,    0,    0,    0,  308,
    0,  308,  308,    0,  308,  308,  215,    0,    0,    0,
    0,    0,    0,    0,    0,  308,    0,    0,    0,    0,
    0,  308,  308,    0,    0,  308,  308,  308,    0,  213,
    0,  308,    0,    0,    0,    0,    0,    0,    0,  308,
    0,    0,    0,    0,    0,    0,    0,    0,  308,    0,
  277,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  308,    0,    0,    0,
    0,    0,    0,  277,    0,    0,    0,    0,    0,  308,
    0,    0,    0,    0,  308,    0,    0,  308,  308,  308,
    0,    0,  308,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  308,  308,    0,  339,    0,
    0,  339,    0,  308,  308,    0,    0,  339,  339,  215,
    0,  432,    0,    0,    0,    0,    0,  339,  339,    0,
  339,  339,    0,    0,    0,    0,  339,    0,    0,  339,
    0,  339,    0,    0,  432,  339,  339,  339,  339,   65,
    0,  339,  339,  339,    0,  339,  339,  339,  339,  339,
    0,    0,    0,    0,    0,  339,    0,    0,    0,  339,
    0,    0,   65,    0,  339,  339,  339,    0,    0,  339,
  339,  339,    0,    0,    0,  339,    0,    0,    0,    0,
    0,    0,  339,  339,    0,    0,    0,    0,    0,    0,
    0,    0,  339,    0,    0,  339,    0,    0,    0,    0,
  339,    0,    0,  339,  339,  339,    0,    0,  339,    0,
  339,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  339,  339,  339,  432,    0,    0,    0,  339,  339,
  339,  339,  339,  339,    0,    0,  339,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  213,    0,    0,  339,
  339,    0,   65,    0,  215,    0,  213,  339,  339,    0,
    0,    0,    0,    0,    0,  213,  213,    0,  213,  213,
    0,    0,    0,    0,    0,    0,    0,  213,    0,  213,
    0,    0,    0,    0,    0,  213,  213,    0,    0,  213,
  213,  213,    0,  277,    0,  213,    0,  277,  277,  277,
    0,  290,    0,  213,  277,  277,  277,  277,    0,    0,
    0,    0,  213,    0,    0,    0,    0,    0,    0,    0,
  277,  277,    0,    0,  290,    0,    0,    0,    0,    0,
  213,    0,    0,  277,    0,    0,  277,    0,    0,    0,
    0,    0,  277,  213,    0,    0,    0,    0,  213,  297,
    0,  213,  213,  213,  277,    0,  213,    0,    0,    0,
    0,  432,    0,    0,    0,    0,    0,  480,    0,  213,
  213,  432,  297,    0,    0,    0,    0,  213,  213,    0,
  432,  432,    0,  432,  432,    0,    0,    0,    0,   65,
  480,    0,  277,    0,  432,    0,    0,    0,    0,   65,
  432,  432,    0,  481,  432,  432,  432,    0,   65,   65,
  432,   65,   65,  277,    0,    0,    0,    0,  432,    0,
  277,    0,   65,    0,    0,    0,  481,  432,   65,   65,
    0,    0,   65,   65,   65,    0,    0,    0,   65,    0,
    0,    0,    0,    0,    0,  432,   65,    0,    0,    0,
    0,  290,    0,    0,    0,   65,    0,    0,  432,    0,
    0,    0,    0,  432,    0,    0,  432,  432,  432,    0,
    0,  432,    0,   65,  290,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  432,  432,   65,    0,    0,    0,
    0,   65,  432,  432,   65,   65,   65,    0,    0,   65,
    0,    0,    0,    0,  213,    0,    0,    0,    0,    0,
    0,    0,   65,   65,  213,    0,    0,    0,    0,    0,
   65,   65,    0,  213,  213,  187,  213,  213,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  213,    0,    0,
    0,    0,    0,  213,  213,    0,    0,  213,  213,  213,
    0,    0,    0,  213,  290,    0,    0,    0,    0,  290,
  290,  213,    0,    0,    0,  290,  290,  290,  290,    0,
  213,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  290,  290,    0,    0,    0,    0,    0,  213,    0,
    0,    0,    0,    0,  290,    0,    0,  290,    0,    0,
    0,  213,  297,  290,    0,    0,  213,  297,  297,  213,
  213,  213,    0,  297,  213,  290,  297,    0,    0,    0,
  480,    0,    0,    0,  480,  480,  480,  213,  213,  297,
  297,  480,  480,  480,  480,  213,  213,    0,    0,    0,
    0,    0,  297,    0,    0,  297,  480,    0,    0,    0,
    0,  297,    0,  290,    0,    0,  481,    0,    0,    0,
  481,  481,  481,  297,    0,    0,    0,  481,  481,  481,
  481,    0,    0,    0,  290,    0,    0,    0,    0,    0,
    0,  290,  481,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  480,  480,  480,  480,
  480,  297,  480,    0,  290,    0,    0,    0,    0,  290,
  290,    0,    0,    0,    0,  290,  290,  290,  290,  480,
    0,    0,  297,    0,    0,    0,    0,    0,  480,  297,
  290,    0,  481,  481,  481,  481,  481,    0,  481,    0,
  480,  480,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  654,    0,    0,  481,    0,    0,    0,    0,
    0,    0,  655,    0,  481,    0,    0,    0,    0,    0,
    0,  656,  657,    0,  658,  659,  481,  481,    0,    0,
  290,  290,  290,  290,  290,  660,  290,    0,    0,    0,
    0,  661,  370,    0,    0,  662,  663,  664,    0,    0,
    0,  665,    0,  290,    0,    0,    0,    0,    0,  666,
    0,    0,  290,    0,    0,    0,    0,    0,  667,    0,
    0,    0,    0,    0,  290,  290,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  668,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  669,
    0,    0,    0,    0,  670,    0,    0,  671,  672,  673,
    0,    0,  674,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  675,    0,    0,    0,
    0,    0,    0,  676,  677,
};
static const YYINT pfctlycheck[] = {                      3,
    3,   92,   12,   13,  141,  353,  354,   40,   88,   33,
   10,  473,  372,   10,   10,   10,   41,  458,  459,  460,
   10,   33,  592,   10,  465,  466,  123,   10,  123,   10,
  594,   10,  584,   10,   33,  211,  689,   10,   10,  472,
  404,  665,  382,  568,  401,  402,   33,  302,  123,  186,
   60,  766,   33,   33,  123,  823,   33,   33,   10,   33,
   33,  123,  364,  123,   44,   40,  825,   44,   78,   44,
  246,  228,  256,  226,   40,  734,  123,  309,  309,   40,
  394,   91,  354,  355,  356,   40,   10,   10,  225,  277,
  450,  451,  394,  257,   60,   33,   40,  262,  539,  264,
  541,  265,  543,   33,   60,  546,  197,  467,   33,   33,
   40,  123,  249,   44,   44,  685,  292,  299,  294,  830,
  833,   40,  394,  123,   44,   40,  123,  697,  123,   33,
   60,   33,  308,  123,   41,   60,   61,   62,  370,  370,
  764,   33,   44,  486,   40,  125,  123,  123,  125,  123,
  125,  125,  920,  125,  291,  503,  293,  123,   60,   61,
   62,   33,   40,  822,  923,  717,   40,    0,   60,   61,
   62,  123,  309,  635,  311,   40,  256,   10,  893,  434,
   40,   26,   40,  616,  709,  123,  362,   40,   60,   61,
   62,  367,  532,  123,  125,  125,  380,  850,  123,  336,
  560,  123,  566,  360,  357,  125,  394,   60,  384,  760,
  753,  348,  394,  389,   59,   10,  927,  930,  267,  123,
   10,  189,  364,  125,   10,  789,   33,  123,   33,  364,
  573,  368,  242,   40,  804,   40,   44,  123,  268,   44,
   10,   40,   87,   33,  394,  123,   33,  277,   40,  394,
  610,  123,  394,   60,   40,   60,   61,   62,   40,  394,
  397,  702,  272,  363,  364,  394,  278,  364,   60,  445,
  407,   40,  123,   60,   61,   62,  268,  453,   60,  636,
   40,  313,  250,  834,   61,  277,  462,   10,  831,  426,
  427,   40,  269,  469,  394,   10,  185,  394,  295,  394,
  380,   40,  289,  479,  278,   44,  289,  444,  289,  446,
  289,  671,  672,  673,   10,   33,  123,  125,  123,  394,
  125,   60,   40,  300,   10,  394,  395,  300,  123,  354,
  355,  356,  394,  413,  394,  395,  512,   10,   41,  184,
  278,   44,   60,  480,  481,  269,  706,  285,  278,   10,
   47,  813,  347,  348,  349,  350,  351,  352,  353,   10,
  337,  394,  357,  358,  359,  254,  338,  357,  505,  394,
  394,  301,  302,  295,  511,  362,  300,  553,   10,  362,
  357,  362,  519,  383,  357,  522,  125,  383,  365,  366,
  367,   10,  365,  366,  367,  394,  383,  394,  574,  394,
  383,   33,  383,  384,  394,  123,  383,  384,  123,   41,
  383,  384,   10,  337,  394,  552,  776,  394,  778,  394,
  861,  862,  863,  256,  257,  258,  259,  260,  394,  395,
  606,  607,  608,  357,  395,   33,  796,  797,  798,  394,
  395,  365,  366,  367,   10,  525,   44,  909,  624,   10,
  394,  395,  914,  286,  287,  288,  363,  364,  595,  383,
  384,  809,  638,  394,  394,  395,  299,  604,   10,  394,
  395,  278,  394,  278,  650,  394,  395,  261,  311,  394,
  313,  314,  315,  316,  317,  268,  623,  394,  625,  261,
   33,  274,  394,  395,  301,  302,  301,  302,  394,  332,
  637,  334,  394,  395,   33,  338,  866,  587,  394,  395,
  300,  687,  592,  346,  651,   10,  394,   60,   61,   62,
  394,  601,  394,  395,  357,  123,  310,  125,  747,  394,
  300,   60,   61,   62,  394,  368,  394,  897,  310,  899,
  263,  394,  395,  394,  395,   10,  269,  557,   10,  309,
  769,  688,  347,  348,  349,  350,  351,  352,  353,  364,
  278,  394,  357,  358,  359,   10,   10,  357,  705,   44,
   44,   33,  582,   10,  711,  365,  366,  367,   40,   41,
  123,  278,   44,  301,  302,  676,  596,  394,  395,  394,
  395,   33,  362,  383,  384,  394,  395,  367,   60,  394,
   10,  738,  394,  395,   10,  685,  743,  394,  395,  689,
  370,   10,  394,  395,  751,   44,  792,  697,   60,   61,
   62,  758,   44,   40,  704,  394,  395,  300,  299,  805,
  806,  807,  347,  348,  349,  350,  351,  352,  353,  362,
   40,  312,  357,  358,  359,  394,  395,   10,  728,   41,
  125,  125,   10,   44,  791,  394,  395,   44,  290,  291,
   60,  123,  260,  125,  840,  675,  842,  843,  844,   44,
  300,  303,  270,   10,  306,  812,  394,  395,  815,  394,
  312,  279,  280,  419,  282,  283,  257,  258,  259,  362,
   44,   10,  324,  869,  367,  293,  125,  394,  265,  266,
  876,  299,  300,  125,  880,  303,  304,  305,   10,  885,
  847,  309,  888,  123,   33,  891,   10,  727,   44,  317,
  354,   40,   41,  814,  804,   44,  360,  361,  326,   10,
  362,  868,  362,  265,  125,  471,   10,  367,  125,   10,
  877,   60,  313,   40,  881,   44,  344,   10,   10,  886,
  125,  383,  889,   44,  845,  491,  394,  395,  390,  357,
  394,   10,   41,   33,  362,   44,   10,  365,  366,  367,
  850,  125,  370,  394,   41,   44,  786,   44,  339,  340,
  341,  342,  343,   10,  345,  383,  384,  394,  395,  799,
   60,   61,   62,  391,  392,   40,  394,  395,  260,  125,
  394,  263,   41,   10,  123,   44,  125,  269,  270,  946,
  371,   44,  949,  950,  951,  952,  278,  279,  280,  263,
  282,  283,  394,  285,  268,  269,  125,  289,  290,  291,
  274,  293,   41,  277,  125,   44,   44,  299,  300,  301,
  302,  303,  304,  305,  306,   41,   33,  309,   44,  859,
  312,  394,  395,   40,  590,  317,  125,  394,  268,  269,
  123,   44,  324,   10,  326,  394,  395,  277,  959,  873,
  873,  394,   41,   60,   40,   44,  612,  339,  340,  341,
  342,  343,  344,  345,   44,  333,   33,   45,   10,   47,
  364,   62,  125,  266,   44,  357,  123,   44,   10,   40,
  362,  301,  302,  365,  366,  367,  385,  386,  370,  371,
   44,   33,   82,   60,   61,   62,  123,  125,  362,   60,
  394,  383,  384,  308,   94,   95,   96,  337,  390,  391,
  392,   41,  394,  395,   44,   99,  123,  101,  125,  267,
  268,  260,  125,  266,  263,  395,  274,  275,  276,  290,
  269,  270,  394,  395,  256,  257,  258,  259,   60,  278,
  279,  280,  354,  282,  283,  125,  285,  395,  360,  361,
  289,  290,  291,  260,  293,  125,  123,   10,  125,  395,
  299,  300,  301,  302,  303,  304,  305,  306,   10,   41,
  309,  125,   44,  312,  394,  395,   44,  299,  317,  286,
  287,  288,  394,  394,  395,  324,   10,  326,  394,  311,
  357,  313,  314,  315,  316,  317,   10,  275,  276,  395,
  339,  340,  341,  342,  343,  344,  345,  289,  198,   33,
  332,   41,  334,  307,   44,   33,  338,  268,  357,  395,
   44,   41,  269,  362,  346,  395,  365,  366,  367,  394,
  395,  370,  371,  395,  328,  357,   60,   61,   62,  266,
  267,  268,  269,   10,  383,  384,  368,  274,  375,  376,
  277,  390,  391,  392,  395,  394,  395,  339,  340,  341,
  342,  343,  395,  345,  347,  348,  349,  350,  351,  352,
  353,  278,  394,  274,  357,  358,  359,   10,  372,  373,
  374,  123,  395,  377,  378,  379,  394,  381,  382,  371,
  337,  385,  386,  260,  301,  302,  263,  394,  395,  123,
  394,  125,  269,  270,  394,  395,  394,  395,   40,   10,
  337,  394,  279,  280,  394,  282,  283,  286,  287,  288,
   47,  263,  289,  290,  291,   33,  293,  269,   40,   10,
  663,  664,  299,  300,  394,  395,  303,  304,  305,  306,
  301,  302,  309,  261,   40,  312,   40,   10,  290,  291,
  317,   40,   33,  271,  272,  273,  123,  324,  260,  326,
   40,  303,  395,   44,  306,   95,   96,  287,  288,  289,
  312,   10,  339,  340,  341,  342,  343,  344,  345,   60,
   61,   62,  324,  395,  286,  287,  288,  394,  395,  395,
  357,   41,  310,  395,   33,  362,  394,  395,  365,  366,
  367,   10,  394,  370,  371,   44,   62,  339,  340,  341,
  342,  343,   41,  345,  331,  332,  383,  384,  393,   10,
  362,  394,  123,  390,  391,  392,  394,  394,  395,  269,
  394,  395,   41,  394,  395,   44,  260,  277,  384,  371,
   10,  383,  123,   10,  125,   41,  270,  263,  390,  394,
   41,  394,  395,   44,  269,  279,  280,  362,  282,  283,
  123,  394,  277,   33,  304,  305,  394,  395,  294,  293,
  296,  297,  298,  394,   41,  299,  300,   44,  274,  303,
  304,  305,  300,  380,  123,  309,  125,  394,  395,  304,
  305,  394,  395,  317,  394,  395,  416,  830,  394,  395,
  833,  421,  326,  394,  395,  347,  348,  349,  350,  351,
  352,  353,  286,  287,  288,  357,  358,  359,  394,  395,
  344,  394,  395,  394,  395,  339,  340,  341,  342,  343,
  263,  345,  395,  357,  394,  268,  269,  394,  362,  357,
   41,  365,  366,  367,  277,   41,  370,  365,  366,  367,
  394,  395,  394,  394,  395,  268,   41,  371,  478,  383,
  384,  274,  275,  276,   10,  268,  384,  391,  392,  394,
  394,  395,  394,  375,  285,   61,  269,   61,  395,  260,
  347,  348,  349,  350,  351,  352,  353,   10,   61,  270,
  396,  358,  359,   10,  927,   47,  394,  930,  279,  280,
  394,  282,  283,   47,  267,  268,  269,  383,  337,  269,
   33,  274,  293,   41,  277,  269,   33,  395,  299,  300,
  395,  260,  303,  304,  305,  394,   41,  394,  309,  362,
   62,  270,  395,  394,  289,  123,  317,   47,  395,  395,
  279,  280,  123,  282,  283,  326,  347,  348,  349,  350,
  351,  352,  353,   10,  293,  394,   40,  358,  359,  395,
  299,  300,  383,  344,  303,  304,  305,  284,  284,  284,
  309,  284,  395,   10,  337,  395,  357,  123,  317,  599,
   47,  362,  285,  383,  365,  366,  367,  326,  395,  370,
  395,  395,  312,  394,   10,  394,   33,   47,  307,  281,
  281,   40,  383,  384,  396,  344,  396,  394,  285,  395,
  391,  392,   10,  394,  395,  383,  307,   41,  357,  328,
  290,  291,   47,  362,   10,  395,  365,  366,  367,   41,
  394,  370,  289,  303,   41,  394,  306,  328,   41,   41,
  307,   41,  312,   10,  383,  384,   10,   33,  395,   41,
   41,   41,  391,  392,  324,  394,  395,   10,   10,  285,
   41,  328,  333,  372,  373,  374,   10,  394,  377,  378,
  379,   10,  381,  382,   10,   33,  385,  386,   10,  268,
  394,  372,  373,  374,  394,  394,  377,  378,  379,   10,
  381,  382,  362,   10,  385,  386,   10,  383,  339,  340,
  341,  342,  343,  394,  345,  372,  373,  374,   10,   10,
  377,  378,  379,  383,  381,  382,  395,   33,  385,  386,
  390,   10,  268,  269,   10,   41,   71,  394,  274,    3,
  371,  277,   79,   17,  845,  959,  507,  260,  529,  262,
  263,  264,  265,  266,  267,  268,  269,  270,  282,  491,
   10,  274,  275,  276,  277,  500,  279,  280,  613,  282,
  283,   10,   86,  197,  827,  241,  301,  271,  448,  456,
  293,  294,  289,  296,  297,  298,  299,  300,  545,  537,
  303,  304,  305,  375,   33,  550,  309,  564,   10,  804,
  685,  337,  270,  192,  317,  731,  347,  348,  349,  350,
  351,  352,  353,  326,  704,  728,  263,  358,  359,  350,
  563,  268,  269,  378,  337,  443,  601,  274,  275,  276,
  277,  344,  339,  340,  341,  342,  343,  903,  345,  266,
  267,  268,  269,    3,  357,    3,  430,  274,  903,  362,
  277,   -1,  365,  366,  367,  362,   -1,  370,   -1,   -1,
   -1,   -1,   -1,   -1,  371,   -1,   -1,   -1,   -1,   -1,
  383,  384,   -1,  300,   -1,   -1,  383,   -1,  391,  392,
  393,   -1,   -1,   -1,  260,   -1,  262,  263,  264,  265,
  266,  267,  268,  269,  270,   -1,   10,   -1,  274,  275,
  276,  277,   -1,  279,  280,   -1,  282,  283,   10,   -1,
  337,   -1,   -1,  125,   -1,  362,   -1,  293,  294,   33,
  296,  297,  298,  299,  300,   -1,   -1,  303,  304,  305,
  357,   33,   -1,  309,   -1,   -1,   -1,   -1,  365,  366,
  367,  317,  290,  291,   -1,   -1,   -1,   10,   -1,   -1,
  326,   -1,   -1,   -1,   -1,  303,  383,  384,  306,   10,
   -1,  337,   -1,   -1,  312,   -1,  393,   -1,  344,   -1,
   33,   -1,   10,   -1,   -1,   -1,  324,   -1,   -1,   -1,
   -1,  357,   33,   -1,  290,  291,  362,   -1,   -1,  365,
  366,  367,   -1,   -1,  370,   33,   -1,  303,   -1,   -1,
  306,   -1,   -1,   -1,   -1,   -1,  312,  383,  384,  123,
   -1,   -1,   -1,   -1,  362,  391,  392,  393,  324,   -1,
   -1,  260,   -1,  262,  263,  264,  265,  266,  267,  268,
  269,  270,   -1,   -1,   -1,  274,  275,  276,  277,  289,
  279,  280,  390,  282,  283,  257,  258,  259,   -1,   -1,
   -1,   -1,   -1,   -1,  293,  294,  362,  296,  297,  298,
  299,  300,   -1,   -1,  303,  304,  305,   -1,   -1,   -1,
  309,   10,   -1,   -1,   -1,   -1,   -1,  383,  317,   -1,
   -1,   -1,   -1,   -1,  390,   -1,   -1,  326,   -1,  339,
  340,  341,  342,  343,   33,  345,   -1,   -1,  337,   -1,
   -1,  313,  314,  315,  316,  344,   10,   -1,   -1,   -1,
   -1,   -1,  362,   -1,   -1,   -1,   -1,   -1,  357,   -1,
   -1,  371,   -1,  362,   -1,   -1,  365,  366,  367,   33,
   -1,  370,   -1,  383,   -1,   -1,   -1,   41,   -1,   -1,
   44,   -1,   -1,   47,  383,  384,  218,  219,  220,  263,
   -1,   -1,  391,  392,  393,  269,   -1,   -1,  260,   -1,
  262,  263,  264,  265,  266,  267,  268,  269,  270,   -1,
   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,   -1,
  282,  283,   -1,   -1,  123,   -1,  300,   -1,   -1,   -1,
   -1,  293,  294,   -1,  296,  297,  298,  299,  300,   -1,
   -1,  303,  304,  305,  267,  268,  269,  309,   -1,   -1,
   -1,  274,   -1,   -1,  277,  317,   -1,  268,  269,  123,
   -1,  125,   -1,  274,  326,   10,  277,   -1,   -1,   -1,
  268,  269,   -1,   -1,   -1,  337,  274,  300,   -1,  277,
   -1,   -1,  344,  357,   -1,   -1,   -1,   -1,   33,  300,
   -1,  365,  366,  367,   -1,  357,  328,   -1,   -1,   -1,
  362,   -1,  300,  365,  366,  367,   -1,   -1,  370,  383,
  384,  343,   -1,   -1,  337,   -1,   -1,   -1,   -1,  351,
   -1,  383,  384,   -1,   -1,   -1,  337,   -1,   -1,  391,
  392,  393,   -1,   -1,  357,   -1,   -1,   -1,   -1,  337,
   -1,   -1,  365,  366,  367,   -1,  357,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  365,  366,  367,   -1,   -1,  357,
  383,  384,   -1,   -1,   -1,   -1,   -1,  365,  366,  367,
  393,  403,  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  393,   -1,   10,  383,  384,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  260,   -1,   -1,  263,
   -1,  300,   -1,  267,  268,  269,  270,   33,   -1,   -1,
  274,  275,  276,  277,   -1,  279,  280,   -1,  282,  283,
   -1,   -1,   -1,   -1,   -1,  289,  290,  291,   -1,  293,
   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,  337,  303,
  304,  305,  306,   -1,   -1,  309,   -1,   -1,  312,   -1,
   -1,   -1,   -1,  317,   -1,   -1,   -1,   10,  357,   -1,
  324,   -1,  326,   -1,   -1,   -1,  365,  366,  367,   -1,
   -1,   -1,   -1,  337,   -1,  339,  340,  341,  342,  343,
  344,  345,   -1,   -1,  383,  384,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,
   -1,  365,  366,  367,   -1,   -1,  370,  371,   -1,   -1,
   10,   -1,   -1,   -1,   -1,  260,   -1,   -1,  263,  383,
  384,  266,  267,  268,  269,  270,  390,  391,  392,  274,
  275,  276,  277,   33,  279,  280,  307,  282,  283,   -1,
   -1,   -1,   -1,   -1,  289,  290,  291,   -1,  293,  294,
   -1,  296,  297,  298,  299,  300,   -1,  328,  303,  304,
  305,  306,   -1,   -1,  309,   -1,   -1,  312,   -1,   -1,
  123,   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,  324,
   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,  333,   -1,
   -1,   10,   -1,   -1,  339,  340,  341,  342,  343,  344,
  345,  372,  373,  374,   -1,   -1,  377,  378,  379,   -1,
  381,  382,  357,   -1,  385,  386,   -1,  362,   -1,   -1,
  365,  366,  367,  394,   -1,  370,  371,   -1,   -1,   10,
   -1,   -1,   -1,   -1,  260,   -1,   -1,  263,  383,  384,
   -1,  267,  268,  269,  270,  390,  391,  392,  274,  275,
  276,  277,   33,  279,  280,   -1,  282,  283,   -1,   -1,
   -1,   -1,   10,   -1,  290,  291,   -1,  293,  294,   -1,
  296,  297,  298,  299,  300,   -1,   -1,  303,  304,  305,
  306,   -1,   -1,  309,   -1,   33,  312,   -1,   -1,   -1,
   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,  324,   -1,
  326,  129,  130,  131,   -1,   -1,  125,  333,   -1,  262,
   -1,  264,   -1,  266,  267,  268,  269,   -1,  344,   -1,
   -1,  274,   -1,   -1,  277,   -1,   -1,   -1,   -1,   -1,
   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,
  366,  367,  123,   -1,  370,   -1,   -1,   -1,   10,   -1,
   -1,   -1,   -1,   -1,  182,   -1,   -1,  383,  384,   -1,
  260,   -1,   -1,  263,  390,  391,  392,  267,  268,  269,
  270,   33,   -1,   -1,  274,  275,  276,  277,   -1,  279,
  280,   -1,  282,  283,  337,   -1,   -1,   -1,   -1,   -1,
  290,  291,   -1,  293,  294,   -1,  296,  297,  298,  299,
  300,   -1,   -1,  303,  304,  305,  306,   -1,   -1,  309,
   -1,   -1,  312,   -1,   -1,   -1,   -1,  317,   -1,   -1,
   -1,   -1,   -1,   -1,  324,   -1,  326,   -1,   -1,   -1,
   -1,   -1,   -1,  333,   -1,   -1,   -1,  256,  257,  258,
  259,   -1,   -1,   -1,  344,   -1,   -1,  275,   -1,   -1,
   -1,  279,   -1,   10,   -1,   -1,   -1,  357,  286,   -1,
   -1,  123,  362,   -1,   10,  365,  366,  367,   -1,   -1,
  370,   -1,   -1,   -1,   -1,   -1,   33,   -1,   -1,  260,
  299,   -1,  263,  383,  384,   -1,   -1,   33,  269,  270,
  390,  391,  392,   -1,  313,  314,  315,  316,  279,  280,
   -1,  282,  283,   -1,   -1,  333,   -1,   -1,  289,  290,
  291,   -1,  293,   -1,   -1,  263,   -1,   -1,  299,  300,
  268,  269,  303,  304,  305,  306,  274,   -1,  309,  277,
   -1,  312,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,
   -1,  289,   -1,  324,   -1,  326,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  339,  340,
  341,  342,  343,  344,  345,   -1,   -1,   -1,  347,  348,
  349,  350,  351,  352,  353,   -1,  357,  123,  357,  358,
  359,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
  371,  339,  340,  341,  342,  343,   -1,  345,  260,   10,
   -1,   -1,  383,  384,   -1,   -1,   -1,  269,  270,  390,
  391,  392,   -1,   -1,  362,   -1,   -1,  279,  280,   -1,
  282,  283,   33,  371,   -1,   -1,   -1,  289,  290,  291,
   -1,  293,   -1,   -1,   -1,  383,  384,  299,  300,   -1,
   -1,  303,  304,  305,  306,   -1,   10,  309,   -1,   -1,
  312,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,
   -1,   -1,  324,   -1,  326,   -1,   -1,   -1,   -1,   33,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  339,  340,  341,
  342,  343,  344,  345,   -1,   -1,  347,  348,  349,  350,
  351,  352,  353,   -1,   -1,  357,  357,  358,  359,   -1,
  362,   -1,   -1,  365,  366,  367,  263,   -1,  370,  371,
   -1,  268,  269,   -1,  260,   -1,   -1,   -1,   10,   -1,
  277,  383,  384,   -1,  270,   -1,   -1,   -1,  390,  391,
  392,   -1,  289,  279,  280,   -1,  282,  283,   -1,   -1,
   -1,   33,   -1,  289,  290,  291,   -1,  293,   -1,   -1,
   -1,   -1,   -1,  299,  300,   -1,   -1,  303,  304,  305,
  306,   -1,   -1,  309,   -1,   -1,  312,   -1,   -1,   -1,
   -1,  317,   -1,   -1,   -1,   -1,   -1,   -1,  324,   -1,
  326,   -1,  339,  340,  341,  342,  343,   -1,  345,   -1,
   -1,   -1,   -1,  339,  340,  341,  342,  343,  344,  345,
   -1,   -1,   -1,   -1,   -1,  362,   -1,   -1,   -1,   -1,
   -1,  357,   -1,   -1,  371,   -1,  362,   -1,   -1,  365,
  366,  367,   -1,   -1,  370,  371,  383,  384,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  383,  384,   -1,
   -1,   -1,   -1,   -1,  390,  391,  392,   -1,   -1,  260,
   -1,   10,  263,   -1,  265,  266,  267,  268,  269,  270,
   -1,   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   33,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  293,  294,   -1,  296,  297,  298,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,  263,
   -1,  265,  266,  267,  268,  269,  317,   -1,   -1,   -1,
  274,  275,  276,  277,   -1,  326,   -1,   -1,   -1,   -1,
   10,   -1,   -1,   -1,   -1,   -1,  290,  291,   -1,   -1,
   -1,   -1,   -1,  344,   -1,   -1,   -1,   -1,   -1,  303,
   -1,   -1,  306,   33,   -1,   -1,  357,   -1,  312,   -1,
   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
  324,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  260,   -1,
   -1,  263,  383,  384,   -1,   -1,  268,  269,  270,   -1,
  391,  392,  274,  275,  276,  277,   -1,  279,  280,  307,
  282,  283,   10,   -1,   -1,   -1,   -1,   -1,  362,   -1,
   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,
  328,  303,  304,  305,   -1,   33,   -1,  309,   -1,  383,
   -1,   -1,   -1,   -1,   -1,  317,  390,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  339,  340,  341,
  342,  343,  344,  345,  372,  373,  374,   -1,   -1,  377,
  378,  379,   -1,  381,  382,  357,   -1,  385,  386,   -1,
  362,   -1,   -1,  365,  366,  367,  394,   -1,  370,  371,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,  391,
  392,  260,   -1,   -1,  263,  123,   -1,   -1,   -1,  268,
  269,  270,   -1,   -1,   -1,  274,  275,  276,  277,   10,
  279,  280,   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,
  299,  300,   33,   -1,  303,  304,  305,   -1,   -1,   -1,
  309,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  317,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,   -1,   -1,
   -1,   -1,   -1,  263,   10,   -1,   -1,   -1,  268,  269,
  339,  340,  341,  342,  343,  344,  345,  277,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   33,  357,   -1,
  290,  291,   -1,  362,   -1,   -1,  365,  366,  367,   -1,
   -1,  370,  371,  303,   -1,   -1,  306,   -1,   -1,   -1,
   -1,   -1,  312,   -1,  383,  384,   -1,   -1,   -1,   -1,
   -1,   -1,  391,  392,  324,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  260,   -1,  262,  263,  264,   -1,  266,  267,
  268,  269,  270,   -1,   -1,   -1,  274,  275,  276,  277,
   -1,  279,  280,   -1,  282,  283,   -1,   -1,   -1,   -1,
   -1,   -1,  362,   -1,   -1,  293,   -1,   -1,   -1,   -1,
   -1,  299,  300,   -1,   -1,  303,  304,  305,   -1,   -1,
   -1,  309,   -1,  383,   -1,   -1,   -1,   -1,   -1,  317,
  390,   -1,   -1,   -1,   10,   25,   -1,   -1,  326,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  344,   33,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  357,
   -1,   61,   62,   63,  362,   -1,   -1,  365,  366,  367,
   -1,   -1,  370,   -1,   74,   -1,   -1,   -1,   -1,   10,
   -1,   -1,   -1,   -1,   -1,  383,  384,   -1,   -1,  260,
   90,   -1,  263,  391,  392,   -1,  267,  268,  269,  270,
   -1,   -1,   33,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  293,  294,   -1,  296,  297,  298,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,  123,  309,   -1,
   -1,   -1,   -1,   -1,  260,   -1,  317,  263,   -1,   -1,
   -1,   -1,  268,  269,  270,  326,   -1,   -1,  274,  275,
  276,  277,   -1,  279,  280,   -1,  282,  283,   -1,   -1,
   -1,   -1,   -1,  344,   -1,   -1,   -1,  293,  294,   -1,
  296,  297,  298,  299,  300,   -1,  357,  303,  304,  305,
   -1,  362,  123,  309,  365,  366,  367,   -1,   -1,  370,
   -1,  317,  202,  203,   -1,   -1,   -1,  207,   -1,   10,
  326,   -1,  383,  384,   -1,   -1,   -1,  217,   -1,   -1,
  391,  392,   -1,   -1,   -1,   -1,   -1,   -1,  344,   -1,
   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,
  366,  367,   -1,   -1,  370,  255,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   10,   -1,   -1,  383,  384,   -1,
   -1,   -1,   -1,   -1,  260,  391,  392,  263,   -1,   -1,
  266,  267,  268,  269,  270,   -1,   -1,   33,  274,  275,
  276,  277,   -1,  279,  280,   -1,  282,  283,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  306,  293,   -1,   -1,
   -1,   -1,   -1,  299,  300,   -1,   -1,  303,  304,  305,
   -1,   -1,  123,  309,   -1,   -1,   -1,   -1,   -1,  260,
   -1,  317,  263,   -1,   -1,   -1,  267,  268,  269,  270,
  326,   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,  344,   -1,
   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,
   -1,  357,  303,  304,  305,   -1,  362,   10,  309,  365,
  366,  367,   -1,   -1,  370,   -1,  317,   -1,   -1,   10,
   -1,   -1,   -1,   -1,   -1,  326,   -1,  383,  384,   -1,
   33,   -1,   -1,   -1,   -1,  391,  392,   -1,   -1,   -1,
   -1,   -1,   33,  344,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  357,   -1,   -1,   -1,
   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  383,  384,   -1,   -1,   -1,   -1,   -1,  260,
  391,  392,  263,   -1,   -1,   -1,   -1,  268,  269,  270,
   -1,   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,   -1,
   -1,   -1,   -1,   -1,  260,   10,  317,  263,   -1,   -1,
   -1,   -1,  268,  269,  270,  326,   -1,   -1,  274,  275,
  276,  277,   -1,  279,  280,   -1,  282,  283,   33,   -1,
   -1,   -1,   -1,  344,   -1,   -1,   -1,  293,   -1,   -1,
   -1,   -1,   -1,  299,  300,   -1,  357,  303,  304,  305,
   -1,  362,   -1,  309,  365,  366,  367,   -1,   -1,  370,
   10,  317,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  326,   -1,  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,
  391,  392,   -1,   33,   -1,   -1,   -1,   -1,  344,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  357,   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,
  366,  367,   -1,   -1,  370,   -1,   -1,   -1,  123,   -1,
   -1,   -1,   -1,   -1,   -1,  268,  269,  383,  384,  260,
   -1,   -1,  263,   -1,  277,  391,  392,  268,  269,  270,
   -1,   -1,   -1,  274,  275,  276,  277,   -1,  279,  280,
   -1,  282,  283,   -1,   -1,   -1,   -1,  300,   -1,   -1,
   -1,   -1,  293,   -1,   -1,   -1,   10,   -1,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   10,  309,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  317,   -1,   -1,   33,
   -1,   -1,   -1,   -1,  337,  326,   -1,   -1,   -1,   -1,
   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  344,  357,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  365,  366,  367,   -1,  357,   -1,   -1,   -1,
   -1,  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,
  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  383,  384,   -1,  260,   -1,   -1,  263,   -1,
  391,  392,   -1,  268,  269,  270,   -1,   -1,   -1,  274,
   -1,   -1,  277,   -1,  279,  280,   -1,  282,  283,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,   -1,
  123,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,  304,
  305,   -1,   10,   -1,  309,   -1,   -1,   -1,   -1,   -1,
  260,   -1,  317,  263,   -1,   -1,   -1,   -1,  268,  269,
  270,  326,   -1,   -1,  274,   33,   -1,  277,   -1,  279,
  280,   -1,  282,  283,   -1,   -1,   -1,   -1,   -1,  344,
   -1,   -1,   -1,  293,   -1,   -1,   -1,   -1,   -1,  299,
  300,   -1,  357,  303,  304,  305,   -1,  362,   -1,  309,
  365,  366,  367,   -1,   -1,  370,   -1,  317,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  326,   -1,  383,  384,
   -1,   -1,  312,   -1,   -1,   -1,  391,  392,  318,  319,
  320,  321,  322,  323,  344,  325,   -1,  327,   -1,  329,
  330,  331,   -1,   -1,   -1,  335,  336,  357,   -1,   -1,
   -1,   -1,  362,   -1,   -1,  365,  366,  367,   -1,  263,
  370,   -1,   -1,   -1,   -1,  269,   -1,  260,   10,   -1,
  263,   -1,   -1,  383,  384,  268,  269,  270,   -1,  369,
   -1,  391,  392,   -1,  277,  289,  279,  280,   -1,  282,
  283,   33,   -1,   -1,   -1,   -1,   10,  387,  388,   -1,
  293,   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,
  303,  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   33,
   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  326,   -1,  339,  340,  341,  342,  343,
   -1,  345,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  344,   -1,   -1,   -1,   -1,   -1,   -1,  362,   -1,
   -1,   -1,   -1,   -1,  357,   -1,   -1,  371,   -1,  362,
   -1,   -1,  365,  366,  367,   -1,   -1,  370,   -1,  383,
  384,  123,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  383,  384,  260,   -1,   -1,  263,   -1,   -1,  391,  392,
  268,  269,  270,   -1,   -1,   -1,   -1,   -1,   -1,  277,
   -1,  279,  280,   -1,  282,  283,   10,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  293,   -1,   -1,   -1,   -1,
   -1,  299,  300,   -1,   -1,  303,  304,  305,   -1,   33,
   -1,  309,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  317,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  326,   -1,
   10,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  344,   -1,   -1,   -1,
   -1,   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,  357,
   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,  366,  367,
   -1,   -1,  370,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  383,  384,   -1,  260,   -1,
   -1,  263,   -1,  391,  392,   -1,   -1,  269,  270,  123,
   -1,   10,   -1,   -1,   -1,   -1,   -1,  279,  280,   -1,
  282,  283,   -1,   -1,   -1,   -1,  260,   -1,   -1,  263,
   -1,  293,   -1,   -1,   33,  269,  270,  299,  300,   10,
   -1,  303,  304,  305,   -1,  279,  280,  309,  282,  283,
   -1,   -1,   -1,   -1,   -1,  317,   -1,   -1,   -1,  293,
   -1,   -1,   33,   -1,  326,  299,  300,   -1,   -1,  303,
  304,  305,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,
   -1,   -1,  344,  317,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  326,   -1,   -1,  357,   -1,   -1,   -1,   -1,
  362,   -1,   -1,  365,  366,  367,   -1,   -1,  370,   -1,
  344,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  383,  384,  357,  123,   -1,   -1,   -1,  362,  391,
  392,  365,  366,  367,   -1,   -1,  370,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  260,   -1,   -1,  383,
  384,   -1,  123,   -1,   10,   -1,  270,  391,  392,   -1,
   -1,   -1,   -1,   -1,   -1,  279,  280,   -1,  282,  283,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   33,   -1,  293,
   -1,   -1,   -1,   -1,   -1,  299,  300,   -1,   -1,  303,
  304,  305,   -1,  263,   -1,  309,   -1,  267,  268,  269,
   -1,   10,   -1,  317,  274,  275,  276,  277,   -1,   -1,
   -1,   -1,  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  290,  291,   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,
  344,   -1,   -1,  303,   -1,   -1,  306,   -1,   -1,   -1,
   -1,   -1,  312,  357,   -1,   -1,   -1,   -1,  362,   10,
   -1,  365,  366,  367,  324,   -1,  370,   -1,   -1,   -1,
   -1,  260,   -1,   -1,   -1,   -1,   -1,   10,   -1,  383,
  384,  270,   33,   -1,   -1,   -1,   -1,  391,  392,   -1,
  279,  280,   -1,  282,  283,   -1,   -1,   -1,   -1,  260,
   33,   -1,  362,   -1,  293,   -1,   -1,   -1,   -1,  270,
  299,  300,   -1,   10,  303,  304,  305,   -1,  279,  280,
  309,  282,  283,  383,   -1,   -1,   -1,   -1,  317,   -1,
  390,   -1,  293,   -1,   -1,   -1,   33,  326,  299,  300,
   -1,   -1,  303,  304,  305,   -1,   -1,   -1,  309,   -1,
   -1,   -1,   -1,   -1,   -1,  344,  317,   -1,   -1,   -1,
   -1,   10,   -1,   -1,   -1,  326,   -1,   -1,  357,   -1,
   -1,   -1,   -1,  362,   -1,   -1,  365,  366,  367,   -1,
   -1,  370,   -1,  344,   33,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  383,  384,  357,   -1,   -1,   -1,
   -1,  362,  391,  392,  365,  366,  367,   -1,   -1,  370,
   -1,   -1,   -1,   -1,  260,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  383,  384,  270,   -1,   -1,   -1,   -1,   -1,
  391,  392,   -1,  279,  280,   33,  282,  283,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  293,   -1,   -1,
   -1,   -1,   -1,  299,  300,   -1,   -1,  303,  304,  305,
   -1,   -1,   -1,  309,  263,   -1,   -1,   -1,   -1,  268,
  269,  317,   -1,   -1,   -1,  274,  275,  276,  277,   -1,
  326,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  290,  291,   -1,   -1,   -1,   -1,   -1,  344,   -1,
   -1,   -1,   -1,   -1,  303,   -1,   -1,  306,   -1,   -1,
   -1,  357,  263,  312,   -1,   -1,  362,  268,  269,  365,
  366,  367,   -1,  274,  370,  324,  277,   -1,   -1,   -1,
  263,   -1,   -1,   -1,  267,  268,  269,  383,  384,  290,
  291,  274,  275,  276,  277,  391,  392,   -1,   -1,   -1,
   -1,   -1,  303,   -1,   -1,  306,  289,   -1,   -1,   -1,
   -1,  312,   -1,  362,   -1,   -1,  263,   -1,   -1,   -1,
  267,  268,  269,  324,   -1,   -1,   -1,  274,  275,  276,
  277,   -1,   -1,   -1,  383,   -1,   -1,   -1,   -1,   -1,
   -1,  390,  289,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  339,  340,  341,  342,
  343,  362,  345,   -1,  263,   -1,   -1,   -1,   -1,  268,
  269,   -1,   -1,   -1,   -1,  274,  275,  276,  277,  362,
   -1,   -1,  383,   -1,   -1,   -1,   -1,   -1,  371,  390,
  289,   -1,  339,  340,  341,  342,  343,   -1,  345,   -1,
  383,  384,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  260,   -1,   -1,  362,   -1,   -1,   -1,   -1,
   -1,   -1,  270,   -1,  371,   -1,   -1,   -1,   -1,   -1,
   -1,  279,  280,   -1,  282,  283,  383,  384,   -1,   -1,
  339,  340,  341,  342,  343,  293,  345,   -1,   -1,   -1,
   -1,  299,  300,   -1,   -1,  303,  304,  305,   -1,   -1,
   -1,  309,   -1,  362,   -1,   -1,   -1,   -1,   -1,  317,
   -1,   -1,  371,   -1,   -1,   -1,   -1,   -1,  326,   -1,
   -1,   -1,   -1,   -1,  383,  384,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  344,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  357,
   -1,   -1,   -1,   -1,  362,   -1,   -1,  365,  366,  367,
   -1,   -1,  370,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  384,   -1,   -1,   -1,
   -1,   -1,   -1,  391,  392,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 396
#define YYUNDFTOKEN 596
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
"SLOPPY","ALLOW_RELATED","TAGGED","TAG","IFBOUND","FLOATING","STATEPOLICY",
"STATEDEFAULTS","ROUTE","SETTOS","DIVERTTO","DIVERTREPLY","BRIDGE_TO","STRING",
"NUMBER","PORTBINARY",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
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
"state_opt_item : ALLOW_RELATED",
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
#line 5247 "parse.y"
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
		{ "allow-related",	ALLOW_RELATED},
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
#line 4963 "parse.c"

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
#line 622 "parse.y"
	{ file->errors++; }
#line 5165 "parse.c"
break;
case 20:
#line 625 "parse.y"
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
#line 5182 "parse.c"
break;
case 27:
#line 652 "parse.y"
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
#line 5198 "parse.c"
break;
case 28:
#line 666 "parse.y"
	{ yyval.v.number = 0; }
#line 5203 "parse.c"
break;
case 29:
#line 667 "parse.y"
	{ yyval.v.number = 1; }
#line 5208 "parse.c"
break;
case 30:
#line 670 "parse.y"
	{
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			pfctl_set_reassembly(pf, yystack.l_mark[-1].v.number, yystack.l_mark[0].v.number);
		}
#line 5217 "parse.c"
break;
case 31:
#line 675 "parse.y"
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
#line 5233 "parse.c"
break;
case 32:
#line 687 "parse.y"
	{
			if (!(pf->opts & PF_OPT_OPTIMIZE)) {
				pf->opts |= PF_OPT_OPTIMIZE;
				pf->optimize = yystack.l_mark[0].v.i;
			}
		}
#line 5243 "parse.c"
break;
case 37:
#line 697 "parse.y"
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
#line 5259 "parse.c"
break;
case 38:
#line 709 "parse.y"
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
#line 5273 "parse.c"
break;
case 39:
#line 719 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_DROP;
		}
#line 5284 "parse.c"
break;
case 40:
#line 726 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set block-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			blockpolicy = PFRULE_RETURN;
		}
#line 5295 "parse.c"
break;
case 41:
#line 733 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy drop\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_DROP;
		}
#line 5306 "parse.c"
break;
case 42:
#line 740 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set fail-policy return\n");
			if (check_rulestate(PFCTL_STATE_OPTION))
				YYERROR;
			failpolicy = PFRULE_RETURN;
		}
#line 5317 "parse.c"
break;
case 43:
#line 747 "parse.y"
	{
			if (pf->opts & PF_OPT_VERBOSE)
				printf("set require-order %s\n",
				    yystack.l_mark[0].v.number == 1 ? "yes" : "no");
			require_order = yystack.l_mark[0].v.number;
		}
#line 5327 "parse.c"
break;
case 44:
#line 753 "parse.y"
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
#line 5349 "parse.c"
break;
case 45:
#line 771 "parse.y"
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
#line 5365 "parse.c"
break;
case 46:
#line 783 "parse.y"
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
#line 5381 "parse.c"
break;
case 47:
#line 795 "parse.y"
	{
			if (expand_skip_interface(yystack.l_mark[0].v.interface) != 0) {
				yyerror("error setting skip interface(s)");
				YYERROR;
			}
		}
#line 5391 "parse.c"
break;
case 48:
#line 801 "parse.y"
	{
			if (keep_state_defaults != NULL) {
				yyerror("cannot redefine state-defaults");
				YYERROR;
			}
			keep_state_defaults = yystack.l_mark[0].v.state_opt;
		}
#line 5402 "parse.c"
break;
case 49:
#line 808 "parse.y"
	{
			pf->keep_counters = true;
		}
#line 5409 "parse.c"
break;
case 50:
#line 811 "parse.y"
	{
			if (pfctl_cfg_syncookies(pf, yystack.l_mark[-1].v.i, yystack.l_mark[0].v.watermarks)) {
				yyerror("error setting syncookies");
				YYERROR;
			}
		}
#line 5419 "parse.c"
break;
case 51:
#line 819 "parse.y"
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
#line 5435 "parse.c"
break;
case 52:
#line 832 "parse.y"
	{ yyval.v.watermarks = NULL; }
#line 5440 "parse.c"
break;
case 53:
#line 833 "parse.y"
	{
			memset(&syncookie_opts, 0, sizeof(syncookie_opts));
		  }
#line 5447 "parse.c"
break;
case 54:
#line 835 "parse.y"
	{ yyval.v.watermarks = &syncookie_opts; }
#line 5452 "parse.c"
break;
case 57:
#line 842 "parse.y"
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
#line 5476 "parse.c"
break;
case 58:
#line 864 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 5481 "parse.c"
break;
case 59:
#line 865 "parse.y"
	{
			if ((yyval.v.string = strdup("all")) == NULL) {
				err(1, "stringall: strdup");
			}
		}
#line 5490 "parse.c"
break;
case 60:
#line 872 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
#line 5500 "parse.c"
break;
case 62:
#line 881 "parse.y"
	{
			if (asprintf(&yyval.v.string, "%s %s", yystack.l_mark[-1].v.string, yystack.l_mark[0].v.string) == -1)
				err(1, "string: asprintf");
			free(yystack.l_mark[-1].v.string);
			free(yystack.l_mark[0].v.string);
		}
#line 5510 "parse.c"
break;
case 64:
#line 890 "parse.y"
	{
			char	*s;
			if (asprintf(&s, "%lld", (long long)yystack.l_mark[0].v.number) == -1) {
				yyerror("string: asprintf");
				YYERROR;
			}
			yyval.v.string = s;
		}
#line 5522 "parse.c"
break;
case 66:
#line 901 "parse.y"
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
#line 5542 "parse.c"
break;
case 67:
#line 919 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 5547 "parse.c"
break;
case 68:
#line 920 "parse.y"
	{ yyval.v.string = NULL; }
#line 5552 "parse.c"
break;
case 73:
#line 930 "parse.y"
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
#line 5577 "parse.c"
break;
case 74:
#line 951 "parse.y"
	{
			pf->alast = pf->anchor;
			pf->asd--;
			pf->anchor = pf->astack[pf->asd];
		}
#line 5586 "parse.c"
break;
case 76:
#line 961 "parse.y"
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
#line 5731 "parse.c"
break;
case 77:
#line 1102 "parse.y"
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
#line 5756 "parse.c"
break;
case 78:
#line 1123 "parse.y"
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
#line 5802 "parse.c"
break;
case 79:
#line 1165 "parse.y"
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
#line 5841 "parse.c"
break;
case 80:
#line 1202 "parse.y"
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
#line 5875 "parse.c"
break;
case 81:
#line 1233 "parse.y"
	{
			yyval.v.b.b2 = yyval.v.b.w = 0;
			if (yystack.l_mark[-1].v.i)
				yyval.v.b.b1 = PF_NOSCRUB;
			else
				yyval.v.b.b1 = PF_SCRUB;
		}
#line 5886 "parse.c"
break;
case 82:
#line 1243 "parse.y"
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
#line 5924 "parse.c"
break;
case 87:
#line 1286 "parse.y"
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
#line 5944 "parse.c"
break;
case 88:
#line 1302 "parse.y"
	{
			pf->ealast = pf->eanchor;
			pf->asd--;
			pf->eanchor = pf->eastack[pf->asd];
		}
#line 5953 "parse.c"
break;
case 90:
#line 1311 "parse.y"
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
#line 6019 "parse.c"
break;
case 91:
#line 1375 "parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
			}
#line 6026 "parse.c"
break;
case 92:
#line 1379 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 6031 "parse.c"
break;
case 93:
#line 1380 "parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			yyval.v.filter_opts = filter_opts;
		}
#line 6039 "parse.c"
break;
case 96:
#line 1389 "parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
#line 6050 "parse.c"
break;
case 97:
#line 1396 "parse.y"
	{
			filter_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 6057 "parse.c"
break;
case 98:
#line 1399 "parse.y"
	{
			if (filter_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			filter_opts.label[filter_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 6068 "parse.c"
break;
case 99:
#line 1406 "parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
#line 6075 "parse.c"
break;
case 100:
#line 1409 "parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 6083 "parse.c"
break;
case 101:
#line 1413 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 6091 "parse.c"
break;
case 102:
#line 1417 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 6099 "parse.c"
break;
case 103:
#line 1423 "parse.y"
	{
			yyval.v.bridge_to = NULL;
		}
#line 6106 "parse.c"
break;
case 104:
#line 1426 "parse.y"
	{
			yyval.v.bridge_to = strdup(yystack.l_mark[0].v.string);
		}
#line 6113 "parse.c"
break;
case 105:
#line 1432 "parse.y"
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
#line 6172 "parse.c"
break;
case 106:
#line 1489 "parse.y"
	{
				bzero(&scrub_opts, sizeof scrub_opts);
				scrub_opts.rtableid = -1;
			}
#line 6180 "parse.c"
break;
case 107:
#line 1494 "parse.y"
	{ yyval.v.scrub_opts = scrub_opts; }
#line 6185 "parse.c"
break;
case 108:
#line 1495 "parse.y"
	{
			bzero(&scrub_opts, sizeof scrub_opts);
			scrub_opts.rtableid = -1;
			yyval.v.scrub_opts = scrub_opts;
		}
#line 6194 "parse.c"
break;
case 111:
#line 1506 "parse.y"
	{
			if (scrub_opts.nodf) {
				yyerror("no-df cannot be respecified");
				YYERROR;
			}
			scrub_opts.nodf = 1;
		}
#line 6205 "parse.c"
break;
case 112:
#line 1513 "parse.y"
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
#line 6221 "parse.c"
break;
case 113:
#line 1525 "parse.y"
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
#line 6237 "parse.c"
break;
case 114:
#line 1537 "parse.y"
	{
			if (scrub_opts.marker & FOM_SETTOS) {
				yyerror("set-tos cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= FOM_SETTOS;
			scrub_opts.settos = yystack.l_mark[0].v.number;
		}
#line 6249 "parse.c"
break;
case 115:
#line 1545 "parse.y"
	{
			if (scrub_opts.marker & FOM_FRAGCACHE) {
				yyerror("fragcache cannot be respecified");
				YYERROR;
			}
			scrub_opts.marker |= FOM_FRAGCACHE;
			scrub_opts.fragcache = yystack.l_mark[0].v.i;
		}
#line 6261 "parse.c"
break;
case 116:
#line 1553 "parse.y"
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
#line 6279 "parse.c"
break;
case 117:
#line 1567 "parse.y"
	{
			if (scrub_opts.randomid) {
				yyerror("random-id cannot be respecified");
				YYERROR;
			}
			scrub_opts.randomid = 1;
		}
#line 6290 "parse.c"
break;
case 118:
#line 1574 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			scrub_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 6301 "parse.c"
break;
case 119:
#line 1581 "parse.y"
	{
			scrub_opts.match_tag = yystack.l_mark[0].v.string;
			scrub_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 6309 "parse.c"
break;
case 120:
#line 1587 "parse.y"
	{ yyval.v.i = 0; /* default */ }
#line 6314 "parse.c"
break;
case 121:
#line 1588 "parse.y"
	{ yyval.v.i = PFRULE_FRAGMENT_NOREASS; }
#line 6319 "parse.c"
break;
case 122:
#line 1591 "parse.y"
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
#line 6411 "parse.c"
break;
case 123:
#line 1681 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 6416 "parse.c"
break;
case 124:
#line 1682 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 6421 "parse.c"
break;
case 125:
#line 1685 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 6426 "parse.c"
break;
case 126:
#line 1686 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
#line 6435 "parse.c"
break;
case 127:
#line 1693 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 6440 "parse.c"
break;
case 128:
#line 1694 "parse.y"
	{
			yystack.l_mark[-1].v.interface->dynamic = 1;
			yyval.v.interface = yystack.l_mark[-1].v.interface;
		}
#line 6448 "parse.c"
break;
case 129:
#line 1700 "parse.y"
	{
				bzero(&antispoof_opts, sizeof antispoof_opts);
				antispoof_opts.rtableid = -1;
			}
#line 6456 "parse.c"
break;
case 130:
#line 1705 "parse.y"
	{ yyval.v.antispoof_opts = antispoof_opts; }
#line 6461 "parse.c"
break;
case 131:
#line 1706 "parse.y"
	{
			bzero(&antispoof_opts, sizeof antispoof_opts);
			antispoof_opts.rtableid = -1;
			yyval.v.antispoof_opts = antispoof_opts;
		}
#line 6470 "parse.c"
break;
case 134:
#line 1717 "parse.y"
	{
			if (antispoof_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			antispoof_opts.label[antispoof_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 6481 "parse.c"
break;
case 135:
#line 1724 "parse.y"
	{
			antispoof_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 6488 "parse.c"
break;
case 136:
#line 1727 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			antispoof_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 6499 "parse.c"
break;
case 137:
#line 1736 "parse.y"
	{ yyval.v.number = 1; }
#line 6504 "parse.c"
break;
case 138:
#line 1737 "parse.y"
	{ yyval.v.number = 0; }
#line 6509 "parse.c"
break;
case 139:
#line 1740 "parse.y"
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
#line 6541 "parse.c"
break;
case 140:
#line 1770 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
		}
#line 6549 "parse.c"
break;
case 141:
#line 1775 "parse.y"
	{ yyval.v.table_opts = table_opts; }
#line 6554 "parse.c"
break;
case 142:
#line 1777 "parse.y"
	{
			bzero(&table_opts, sizeof table_opts);
			SIMPLEQ_INIT(&table_opts.init_nodes);
			yyval.v.table_opts = table_opts;
		}
#line 6563 "parse.c"
break;
case 145:
#line 1788 "parse.y"
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
#line 6581 "parse.c"
break;
case 146:
#line 1802 "parse.y"
	{ table_opts.init_addr = 1; }
#line 6586 "parse.c"
break;
case 147:
#line 1803 "parse.y"
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
#line 6630 "parse.c"
break;
case 148:
#line 1843 "parse.y"
	{
			struct node_tinit	*ti;

			if (!(ti = calloc(1, sizeof(*ti))))
				err(1, "table_opt: calloc");
			ti->file = yystack.l_mark[0].v.string;
			SIMPLEQ_INSERT_TAIL(&table_opts.init_nodes, ti,
			    entries);
			table_opts.init_addr = 1;
		}
#line 6644 "parse.c"
break;
case 149:
#line 1855 "parse.y"
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
#line 6670 "parse.c"
break;
case 150:
#line 1879 "parse.y"
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
#line 6709 "parse.c"
break;
case 151:
#line 1916 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
		}
#line 6720 "parse.c"
break;
case 152:
#line 1924 "parse.y"
	{ yyval.v.queue_opts = queue_opts; }
#line 6725 "parse.c"
break;
case 153:
#line 1925 "parse.y"
	{
			bzero(&queue_opts, sizeof queue_opts);
			queue_opts.priority = DEFAULT_PRIORITY;
			queue_opts.qlimit = DEFAULT_QLIMIT;
			queue_opts.scheduler.qtype = ALTQT_NONE;
			queue_opts.queue_bwspec.bw_percent = 100;
			yyval.v.queue_opts = queue_opts;
		}
#line 6737 "parse.c"
break;
case 156:
#line 1939 "parse.y"
	{
			if (queue_opts.marker & QOM_BWSPEC) {
				yyerror("bandwidth cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_BWSPEC;
			queue_opts.queue_bwspec = yystack.l_mark[0].v.queue_bwspec;
		}
#line 6749 "parse.c"
break;
case 157:
#line 1947 "parse.y"
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
#line 6765 "parse.c"
break;
case 158:
#line 1959 "parse.y"
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
#line 6781 "parse.c"
break;
case 159:
#line 1971 "parse.y"
	{
			if (queue_opts.marker & QOM_SCHEDULER) {
				yyerror("scheduler cannot be respecified");
				YYERROR;
			}
			queue_opts.marker |= QOM_SCHEDULER;
			queue_opts.scheduler = yystack.l_mark[0].v.queue_options;
		}
#line 6793 "parse.c"
break;
case 160:
#line 1979 "parse.y"
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
#line 6809 "parse.c"
break;
case 161:
#line 1993 "parse.y"
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
#line 6856 "parse.c"
break;
case 162:
#line 2036 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= LLONG_MAX) {
				yyerror("bandwidth number too big");
				YYERROR;
			}
			yyval.v.queue_bwspec.bw_percent = 0;
			yyval.v.queue_bwspec.bw_absolute = yystack.l_mark[0].v.number;
		}
#line 6868 "parse.c"
break;
case 163:
#line 2046 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = 0;
		}
#line 6876 "parse.c"
break;
case 164:
#line 2050 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CBQ;
			yyval.v.queue_options.data.cbq_opts.flags = yystack.l_mark[-1].v.number;
		}
#line 6884 "parse.c"
break;
case 165:
#line 2054 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = 0;
		}
#line 6892 "parse.c"
break;
case 166:
#line 2058 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_PRIQ;
			yyval.v.queue_options.data.priq_opts.flags = yystack.l_mark[-1].v.number;
		}
#line 6900 "parse.c"
break;
case 167:
#line 2062 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			bzero(&yyval.v.queue_options.data.hfsc_opts,
			    sizeof(struct node_hfsc_opts));
		}
#line 6909 "parse.c"
break;
case 168:
#line 2067 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_HFSC;
			yyval.v.queue_options.data.hfsc_opts = yystack.l_mark[-1].v.hfsc_opts;
		}
#line 6917 "parse.c"
break;
case 169:
#line 2071 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			bzero(&yyval.v.queue_options.data.fairq_opts,
				sizeof(struct node_fairq_opts));
		}
#line 6926 "parse.c"
break;
case 170:
#line 2076 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_FAIRQ;
			yyval.v.queue_options.data.fairq_opts = yystack.l_mark[-1].v.fairq_opts;
		}
#line 6934 "parse.c"
break;
case 171:
#line 2080 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			bzero(&yyval.v.queue_options.data.codel_opts,
				sizeof(struct codel_opts));
		}
#line 6943 "parse.c"
break;
case 172:
#line 2085 "parse.y"
	{
			yyval.v.queue_options.qtype = ALTQT_CODEL;
			yyval.v.queue_options.data.codel_opts = yystack.l_mark[-1].v.codel_opts;
		}
#line 6951 "parse.c"
break;
case 173:
#line 2091 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6956 "parse.c"
break;
case 174:
#line 2092 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6961 "parse.c"
break;
case 175:
#line 2095 "parse.y"
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
#line 6985 "parse.c"
break;
case 176:
#line 2117 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6990 "parse.c"
break;
case 177:
#line 2118 "parse.y"
	{ yyval.v.number |= yystack.l_mark[0].v.number; }
#line 6995 "parse.c"
break;
case 178:
#line 2121 "parse.y"
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
#line 7017 "parse.c"
break;
case 179:
#line 2141 "parse.y"
	{
				bzero(&hfsc_opts,
				    sizeof(struct node_hfsc_opts));
			}
#line 7025 "parse.c"
break;
case 180:
#line 2145 "parse.y"
	{
			yyval.v.hfsc_opts = hfsc_opts;
		}
#line 7032 "parse.c"
break;
case 183:
#line 2154 "parse.y"
	{
			if (hfsc_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			hfsc_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.linkshare.used = 1;
		}
#line 7044 "parse.c"
break;
case 184:
#line 2163 "parse.y"
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
#line 7062 "parse.c"
break;
case 185:
#line 2177 "parse.y"
	{
			if (hfsc_opts.realtime.used) {
				yyerror("realtime already specified");
				YYERROR;
			}
			hfsc_opts.realtime.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.realtime.used = 1;
		}
#line 7074 "parse.c"
break;
case 186:
#line 2186 "parse.y"
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
#line 7092 "parse.c"
break;
case 187:
#line 2200 "parse.y"
	{
			if (hfsc_opts.upperlimit.used) {
				yyerror("upperlimit already specified");
				YYERROR;
			}
			hfsc_opts.upperlimit.m2 = yystack.l_mark[0].v.queue_bwspec;
			hfsc_opts.upperlimit.used = 1;
		}
#line 7104 "parse.c"
break;
case 188:
#line 2209 "parse.y"
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
#line 7122 "parse.c"
break;
case 189:
#line 2223 "parse.y"
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
#line 7144 "parse.c"
break;
case 190:
#line 2243 "parse.y"
	{
				bzero(&fairq_opts,
				    sizeof(struct node_fairq_opts));
			}
#line 7152 "parse.c"
break;
case 191:
#line 2247 "parse.y"
	{
			yyval.v.fairq_opts = fairq_opts;
		}
#line 7159 "parse.c"
break;
case 194:
#line 2256 "parse.y"
	{
			if (fairq_opts.linkshare.used) {
				yyerror("linkshare already specified");
				YYERROR;
			}
			fairq_opts.linkshare.m2 = yystack.l_mark[0].v.queue_bwspec;
			fairq_opts.linkshare.used = 1;
		}
#line 7171 "parse.c"
break;
case 195:
#line 2264 "parse.y"
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
#line 7185 "parse.c"
break;
case 196:
#line 2274 "parse.y"
	{
			fairq_opts.hogs_bw = yystack.l_mark[0].v.queue_bwspec;
		}
#line 7192 "parse.c"
break;
case 197:
#line 2277 "parse.y"
	{
			fairq_opts.nbuckets = yystack.l_mark[0].v.number;
		}
#line 7199 "parse.c"
break;
case 198:
#line 2280 "parse.y"
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
#line 7221 "parse.c"
break;
case 199:
#line 2300 "parse.y"
	{
				bzero(&codel_opts,
				    sizeof(struct codel_opts));
			}
#line 7229 "parse.c"
break;
case 200:
#line 2304 "parse.y"
	{
			yyval.v.codel_opts = codel_opts;
		}
#line 7236 "parse.c"
break;
case 203:
#line 2313 "parse.y"
	{
			if (codel_opts.interval) {
				yyerror("interval already specified");
				YYERROR;
			}
			codel_opts.interval = yystack.l_mark[0].v.number;
		}
#line 7247 "parse.c"
break;
case 204:
#line 2320 "parse.y"
	{
			if (codel_opts.target) {
				yyerror("target already specified");
				YYERROR;
			}
			codel_opts.target = yystack.l_mark[0].v.number;
		}
#line 7258 "parse.c"
break;
case 205:
#line 2327 "parse.y"
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
#line 7272 "parse.c"
break;
case 206:
#line 2339 "parse.y"
	{ yyval.v.queue = NULL; }
#line 7277 "parse.c"
break;
case 207:
#line 2340 "parse.y"
	{ yyval.v.queue = yystack.l_mark[0].v.queue; }
#line 7282 "parse.c"
break;
case 208:
#line 2341 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
#line 7287 "parse.c"
break;
case 209:
#line 2344 "parse.y"
	{ yyval.v.queue = yystack.l_mark[-1].v.queue; }
#line 7292 "parse.c"
break;
case 210:
#line 2345 "parse.y"
	{
			yystack.l_mark[-3].v.queue->tail->next = yystack.l_mark[-1].v.queue;
			yystack.l_mark[-3].v.queue->tail = yystack.l_mark[-1].v.queue;
			yyval.v.queue = yystack.l_mark[-3].v.queue;
		}
#line 7301 "parse.c"
break;
case 211:
#line 2352 "parse.y"
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
#line 7321 "parse.c"
break;
case 212:
#line 2372 "parse.y"
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
				case PF_STATE_OPT_ALLOW_RELATED:
					if (r.rule_flag & PFRULE_ALLOW_RELATED) {
						yyerror("state allow-related option: "
						    "multiple definitions");
						YYERROR;
					}
					r.rule_flag |= PFRULE_ALLOW_RELATED;
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
#line 7781 "parse.c"
break;
case 213:
#line 2830 "parse.y"
	{
				bzero(&filter_opts, sizeof filter_opts);
				filter_opts.rtableid = -1;
			}
#line 7789 "parse.c"
break;
case 214:
#line 2835 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 7794 "parse.c"
break;
case 215:
#line 2836 "parse.y"
	{
			bzero(&filter_opts, sizeof filter_opts);
			filter_opts.rtableid = -1;
			yyval.v.filter_opts = filter_opts;
		}
#line 7803 "parse.c"
break;
case 218:
#line 2847 "parse.y"
	{
			if (filter_opts.uid)
				yystack.l_mark[0].v.uid->tail->next = filter_opts.uid;
			filter_opts.uid = yystack.l_mark[0].v.uid;
		}
#line 7812 "parse.c"
break;
case 219:
#line 2852 "parse.y"
	{
			if (filter_opts.gid)
				yystack.l_mark[0].v.gid->tail->next = filter_opts.gid;
			filter_opts.gid = yystack.l_mark[0].v.gid;
		}
#line 7821 "parse.c"
break;
case 220:
#line 2857 "parse.y"
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
#line 7836 "parse.c"
break;
case 221:
#line 2868 "parse.y"
	{
			if (filter_opts.marker & FOM_ICMP) {
				yyerror("icmp-type cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_ICMP;
			filter_opts.icmpspec = yystack.l_mark[0].v.icmp;
		}
#line 7848 "parse.c"
break;
case 222:
#line 2876 "parse.y"
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
#line 7864 "parse.c"
break;
case 223:
#line 2888 "parse.y"
	{
			if (filter_opts.marker & FOM_TOS) {
				yyerror("tos cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_TOS;
			filter_opts.tos = yystack.l_mark[0].v.number;
		}
#line 7876 "parse.c"
break;
case 224:
#line 2896 "parse.y"
	{
			if (filter_opts.marker & FOM_KEEP) {
				yyerror("modulate or keep cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_KEEP;
			filter_opts.keep.action = yystack.l_mark[0].v.keep_state.action;
			filter_opts.keep.options = yystack.l_mark[0].v.keep_state.options;
		}
#line 7889 "parse.c"
break;
case 225:
#line 2905 "parse.y"
	{
			filter_opts.ridentifier = yystack.l_mark[0].v.number;
		}
#line 7896 "parse.c"
break;
case 226:
#line 2908 "parse.y"
	{
			filter_opts.fragment = 1;
		}
#line 7903 "parse.c"
break;
case 227:
#line 2911 "parse.y"
	{
			filter_opts.allowopts = 1;
		}
#line 7910 "parse.c"
break;
case 228:
#line 2914 "parse.y"
	{
			if (filter_opts.labelcount >= PF_RULE_MAX_LABEL_COUNT) {
				yyerror("label can only be used %d times", PF_RULE_MAX_LABEL_COUNT);
				YYERROR;
			}
			filter_opts.label[filter_opts.labelcount++] = yystack.l_mark[0].v.string;
		}
#line 7921 "parse.c"
break;
case 229:
#line 2921 "parse.y"
	{
			if (filter_opts.queues.qname) {
				yyerror("queue cannot be redefined");
				YYERROR;
			}
			filter_opts.queues = yystack.l_mark[0].v.qassign;
		}
#line 7932 "parse.c"
break;
case 230:
#line 2928 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7940 "parse.c"
break;
case 231:
#line 2932 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[-1].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7948 "parse.c"
break;
case 232:
#line 2936 "parse.y"
	{
			filter_opts.dnrpipe = yystack.l_mark[-1].v.number;
			filter_opts.dnpipe = yystack.l_mark[-3].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_PIPE;
		}
#line 7957 "parse.c"
break;
case 233:
#line 2941 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[0].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7965 "parse.c"
break;
case 234:
#line 2945 "parse.y"
	{
			filter_opts.dnrpipe = yystack.l_mark[-1].v.number;
			filter_opts.dnpipe = yystack.l_mark[-3].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7974 "parse.c"
break;
case 235:
#line 2950 "parse.y"
	{
			filter_opts.dnpipe = yystack.l_mark[-1].v.number;
			filter_opts.free_flags |= PFRULE_DN_IS_QUEUE;
		}
#line 7982 "parse.c"
break;
case 236:
#line 2954 "parse.y"
	{
			filter_opts.tag = yystack.l_mark[0].v.string;
		}
#line 7989 "parse.c"
break;
case 237:
#line 2957 "parse.y"
	{
			filter_opts.match_tag = yystack.l_mark[0].v.string;
			filter_opts.match_tag_not = yystack.l_mark[-2].v.number;
		}
#line 7997 "parse.c"
break;
case 238:
#line 2961 "parse.y"
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
#line 8013 "parse.c"
break;
case 239:
#line 2973 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			filter_opts.rtableid = yystack.l_mark[0].v.number;
		}
#line 8024 "parse.c"
break;
case 240:
#line 2980 "parse.y"
	{
#ifdef __FreeBSD__
			filter_opts.divert.port = yystack.l_mark[0].v.range.a;
			if (!filter_opts.divert.port) {
				yyerror("invalid divert port: %u", ntohs(yystack.l_mark[0].v.range.a));
				YYERROR;
			}
#endif
		}
#line 8037 "parse.c"
break;
case 241:
#line 2989 "parse.y"
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
#line 8059 "parse.c"
break;
case 242:
#line 3007 "parse.y"
	{
#ifdef __FreeBSD__
			yyerror("divert-reply has no meaning in FreeBSD pf(4)");
			YYERROR;
#else
			filter_opts.divert.port = 1;	/* some random value */
#endif
		}
#line 8071 "parse.c"
break;
case 243:
#line 3015 "parse.y"
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
#line 8093 "parse.c"
break;
case 245:
#line 3036 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 8098 "parse.c"
break;
case 246:
#line 3037 "parse.y"
	{ yyval.v.filter_opts = filter_opts; }
#line 8103 "parse.c"
break;
case 249:
#line 3044 "parse.y"
	{
			if (filter_opts.marker & FOM_SETPRIO) {
				yyerror("prio cannot be redefined");
				YYERROR;
			}
			filter_opts.marker |= FOM_SETPRIO;
			filter_opts.set_prio[0] = yystack.l_mark[0].v.b.b1;
			filter_opts.set_prio[1] = yystack.l_mark[0].v.b.b2;
		}
#line 8116 "parse.c"
break;
case 250:
#line 3053 "parse.y"
	{
			if (filter_opts.marker & FOM_SETTOS) {
				yyerror("tos cannot be respecified");
				YYERROR;
			}
			filter_opts.marker |= FOM_SETTOS;
			filter_opts.settos = yystack.l_mark[0].v.number;
		}
#line 8128 "parse.c"
break;
case 251:
#line 3061 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yyval.v.b.b2 = yystack.l_mark[0].v.number;
		}
#line 8139 "parse.c"
break;
case 252:
#line 3068 "parse.y"
	{
			if (yystack.l_mark[-3].v.number < 0 || yystack.l_mark[-3].v.number > PF_PRIO_MAX ||
			    yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > PF_PRIO_MAX) {
				yyerror("prio must be 0 - %u", PF_PRIO_MAX);
				YYERROR;
			}
			yyval.v.b.b1 = yystack.l_mark[-3].v.number;
			yyval.v.b.b2 = yystack.l_mark[-1].v.number;
		}
#line 8152 "parse.c"
break;
case 253:
#line 3079 "parse.y"
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
#line 8172 "parse.c"
break;
case 254:
#line 3095 "parse.y"
	{
			yyval.v.probability = (double)yystack.l_mark[0].v.number;
		}
#line 8179 "parse.c"
break;
case 255:
#line 3101 "parse.y"
	{
			yyval.v.b.b1 = PF_PASS;
			yyval.v.b.b2 = failpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8189 "parse.c"
break;
case 256:
#line 3107 "parse.y"
	{ yyval.v.b.b1 = PF_MATCH; yyval.v.b.b2 = yyval.v.b.w = 0; }
#line 8194 "parse.c"
break;
case 257:
#line 3108 "parse.y"
	{ yyval.v.b = yystack.l_mark[0].v.b; yyval.v.b.b1 = PF_DROP; }
#line 8199 "parse.c"
break;
case 258:
#line 3111 "parse.y"
	{
			yyval.v.b.b2 = blockpolicy;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8208 "parse.c"
break;
case 259:
#line 3116 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_DROP;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
#line 8217 "parse.c"
break;
case 260:
#line 3121 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = 0;
			yyval.v.b.w2 = 0;
		}
#line 8226 "parse.c"
break;
case 261:
#line 3126 "parse.y"
	{
			if (yystack.l_mark[-1].v.number < 0 || yystack.l_mark[-1].v.number > 255) {
				yyerror("illegal ttl value %d", yystack.l_mark[-1].v.number);
				YYERROR;
			}
			yyval.v.b.b2 = PFRULE_RETURNRST;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = 0;
		}
#line 8239 "parse.c"
break;
case 262:
#line 3135 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8248 "parse.c"
break;
case 263:
#line 3140 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8257 "parse.c"
break;
case 264:
#line 3145 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-1].v.number;
			yyval.v.b.w2 = returnicmpdefault;
		}
#line 8266 "parse.c"
break;
case 265:
#line 3150 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
#line 8275 "parse.c"
break;
case 266:
#line 3155 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURNICMP;
			yyval.v.b.w = yystack.l_mark[-3].v.number;
			yyval.v.b.w2 = yystack.l_mark[-1].v.number;
		}
#line 8284 "parse.c"
break;
case 267:
#line 3160 "parse.y"
	{
			yyval.v.b.b2 = PFRULE_RETURN;
			yyval.v.b.w = returnicmpdefault;
			yyval.v.b.w2 = returnicmp6default;
		}
#line 8293 "parse.c"
break;
case 268:
#line 3167 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 8304 "parse.c"
break;
case 269:
#line 3174 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmpdefault >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
#line 8318 "parse.c"
break;
case 270:
#line 3186 "parse.y"
	{
			if (!(yyval.v.number = parseicmpspec(yystack.l_mark[0].v.string, AF_INET6))) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 8329 "parse.c"
break;
case 271:
#line 3193 "parse.y"
	{
			u_int8_t		icmptype;

			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("invalid icmp code %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			icmptype = returnicmp6default >> 8;
			yyval.v.number = (icmptype << 8 | yystack.l_mark[0].v.number);
		}
#line 8343 "parse.c"
break;
case 272:
#line 3205 "parse.y"
	{ yyval.v.i = PF_INOUT; }
#line 8348 "parse.c"
break;
case 273:
#line 3206 "parse.y"
	{ yyval.v.i = PF_IN; }
#line 8353 "parse.c"
break;
case 274:
#line 3207 "parse.y"
	{ yyval.v.i = PF_OUT; }
#line 8358 "parse.c"
break;
case 275:
#line 3210 "parse.y"
	{ yyval.v.logquick.quick = 0; }
#line 8363 "parse.c"
break;
case 276:
#line 3211 "parse.y"
	{ yyval.v.logquick.quick = 1; }
#line 8368 "parse.c"
break;
case 277:
#line 3214 "parse.y"
	{ yyval.v.logquick.log = 0; yyval.v.logquick.quick = 0; yyval.v.logquick.logif = 0; }
#line 8373 "parse.c"
break;
case 278:
#line 3215 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 0; }
#line 8378 "parse.c"
break;
case 279:
#line 3216 "parse.y"
	{ yyval.v.logquick.quick = 1; yyval.v.logquick.log = 0; yyval.v.logquick.logif = 0; }
#line 8383 "parse.c"
break;
case 280:
#line 3217 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[-1].v.logquick; yyval.v.logquick.quick = 1; }
#line 8388 "parse.c"
break;
case 281:
#line 3218 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; yyval.v.logquick.quick = 1; }
#line 8393 "parse.c"
break;
case 282:
#line 3221 "parse.y"
	{ yyval.v.logquick.log = PF_LOG; yyval.v.logquick.logif = 0; }
#line 8398 "parse.c"
break;
case 283:
#line 3222 "parse.y"
	{
			yyval.v.logquick.log = PF_LOG | yystack.l_mark[-1].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[-1].v.logquick.logif;
		}
#line 8406 "parse.c"
break;
case 284:
#line 3228 "parse.y"
	{ yyval.v.logquick = yystack.l_mark[0].v.logquick; }
#line 8411 "parse.c"
break;
case 285:
#line 3229 "parse.y"
	{
			yyval.v.logquick.log = yystack.l_mark[-2].v.logquick.log | yystack.l_mark[0].v.logquick.log;
			yyval.v.logquick.logif = yystack.l_mark[0].v.logquick.logif;
			if (yyval.v.logquick.logif == 0)
				yyval.v.logquick.logif = yystack.l_mark[-2].v.logquick.logif;
		}
#line 8421 "parse.c"
break;
case 286:
#line 3237 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_ALL; yyval.v.logquick.logif = 0; }
#line 8426 "parse.c"
break;
case 287:
#line 3238 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
#line 8431 "parse.c"
break;
case 288:
#line 3239 "parse.y"
	{ yyval.v.logquick.log = PF_LOG_SOCKET_LOOKUP; yyval.v.logquick.logif = 0; }
#line 8436 "parse.c"
break;
case 289:
#line 3240 "parse.y"
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
#line 8459 "parse.c"
break;
case 290:
#line 3261 "parse.y"
	{ yyval.v.interface = NULL; }
#line 8464 "parse.c"
break;
case 291:
#line 3262 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; }
#line 8469 "parse.c"
break;
case 292:
#line 3263 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 8474 "parse.c"
break;
case 293:
#line 3266 "parse.y"
	{ yyval.v.interface = yystack.l_mark[-1].v.interface; }
#line 8479 "parse.c"
break;
case 294:
#line 3267 "parse.y"
	{
			yystack.l_mark[-3].v.interface->tail->next = yystack.l_mark[-1].v.interface;
			yystack.l_mark[-3].v.interface->tail = yystack.l_mark[-1].v.interface;
			yyval.v.interface = yystack.l_mark[-3].v.interface;
		}
#line 8488 "parse.c"
break;
case 295:
#line 3274 "parse.y"
	{ yyval.v.interface = yystack.l_mark[0].v.interface; yyval.v.interface->not = yystack.l_mark[-1].v.number; }
#line 8493 "parse.c"
break;
case 296:
#line 3277 "parse.y"
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
#line 8519 "parse.c"
break;
case 297:
#line 3301 "parse.y"
	{ yyval.v.i = 0; }
#line 8524 "parse.c"
break;
case 298:
#line 3302 "parse.y"
	{ yyval.v.i = AF_INET; }
#line 8529 "parse.c"
break;
case 299:
#line 3303 "parse.y"
	{ yyval.v.i = AF_INET6; }
#line 8534 "parse.c"
break;
case 300:
#line 3306 "parse.y"
	{ yyval.v.etherproto = NULL; }
#line 8539 "parse.c"
break;
case 301:
#line 3307 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[0].v.etherproto; }
#line 8544 "parse.c"
break;
case 302:
#line 3308 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[-1].v.etherproto; }
#line 8549 "parse.c"
break;
case 303:
#line 3311 "parse.y"
	{ yyval.v.etherproto = yystack.l_mark[-1].v.etherproto; }
#line 8554 "parse.c"
break;
case 304:
#line 3312 "parse.y"
	{
			yystack.l_mark[-3].v.etherproto->tail->next = yystack.l_mark[-1].v.etherproto;
			yystack.l_mark[-3].v.etherproto->tail = yystack.l_mark[-1].v.etherproto;
			yyval.v.etherproto = yystack.l_mark[-3].v.etherproto;
		}
#line 8563 "parse.c"
break;
case 305:
#line 3319 "parse.y"
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
#line 8582 "parse.c"
break;
case 306:
#line 3336 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 65565) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
#line 8592 "parse.c"
break;
case 307:
#line 3343 "parse.y"
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
#line 8607 "parse.c"
break;
case 308:
#line 3356 "parse.y"
	{ yyval.v.proto = NULL; }
#line 8612 "parse.c"
break;
case 309:
#line 3357 "parse.y"
	{ yyval.v.proto = yystack.l_mark[0].v.proto; }
#line 8617 "parse.c"
break;
case 310:
#line 3358 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
#line 8622 "parse.c"
break;
case 311:
#line 3361 "parse.y"
	{ yyval.v.proto = yystack.l_mark[-1].v.proto; }
#line 8627 "parse.c"
break;
case 312:
#line 3362 "parse.y"
	{
			yystack.l_mark[-3].v.proto->tail->next = yystack.l_mark[-1].v.proto;
			yystack.l_mark[-3].v.proto->tail = yystack.l_mark[-1].v.proto;
			yyval.v.proto = yystack.l_mark[-3].v.proto;
		}
#line 8636 "parse.c"
break;
case 313:
#line 3369 "parse.y"
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
#line 8655 "parse.c"
break;
case 314:
#line 3386 "parse.y"
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
#line 8671 "parse.c"
break;
case 315:
#line 3398 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("protocol outside range");
				YYERROR;
			}
		}
#line 8681 "parse.c"
break;
case 316:
#line 3406 "parse.y"
	{
			bzero(&yyval.v.fromto, sizeof(yyval.v.fromto));
		}
#line 8688 "parse.c"
break;
case 317:
#line 3409 "parse.y"
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
#line 8707 "parse.c"
break;
case 318:
#line 3425 "parse.y"
	{
			yyval.v.etherfromto.src = NULL;
			yyval.v.etherfromto.dst = NULL;
		}
#line 8715 "parse.c"
break;
case 319:
#line 3429 "parse.y"
	{
			yyval.v.etherfromto.src = yystack.l_mark[-1].v.etheraddr.mac;
			yyval.v.etherfromto.dst = yystack.l_mark[0].v.etheraddr.mac;
		}
#line 8723 "parse.c"
break;
case 320:
#line 3435 "parse.y"
	{
			bzero(&yyval.v.etheraddr, sizeof(yyval.v.etheraddr));
		}
#line 8730 "parse.c"
break;
case 321:
#line 3438 "parse.y"
	{
			yyval.v.etheraddr.mac = yystack.l_mark[0].v.mac;
		}
#line 8737 "parse.c"
break;
case 322:
#line 3443 "parse.y"
	{
			bzero(&yyval.v.etheraddr, sizeof(yyval.v.etheraddr));
		}
#line 8744 "parse.c"
break;
case 323:
#line 3446 "parse.y"
	{
			yyval.v.etheraddr.mac = yystack.l_mark[0].v.mac;
		}
#line 8751 "parse.c"
break;
case 324:
#line 3451 "parse.y"
	{
			yyval.v.mac = node_mac_from_string_masklen(yystack.l_mark[-2].v.string, yystack.l_mark[0].v.number);
			free(yystack.l_mark[-2].v.string);
			if (yyval.v.mac == NULL)
				YYERROR;
		}
#line 8761 "parse.c"
break;
case 325:
#line 3457 "parse.y"
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
#line 8779 "parse.c"
break;
case 326:
#line 3471 "parse.y"
	{
			struct node_mac	*n;

			for (n = yystack.l_mark[0].v.mac; n != NULL; n = n->next)
				n->neg = yystack.l_mark[-1].v.number;
			yyval.v.mac = yystack.l_mark[0].v.mac;
		}
#line 8790 "parse.c"
break;
case 327:
#line 3479 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[0].v.mac;
		}
#line 8797 "parse.c"
break;
case 328:
#line 3483 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[-1].v.mac;
		}
#line 8804 "parse.c"
break;
case 329:
#line 3487 "parse.y"
	{
			yyval.v.mac = yystack.l_mark[-1].v.mac;
		}
#line 8811 "parse.c"
break;
case 330:
#line 3490 "parse.y"
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
#line 8826 "parse.c"
break;
case 331:
#line 3502 "parse.y"
	{
			yyval.v.fromto.src.host = NULL;
			yyval.v.fromto.src.port = NULL;
			yyval.v.fromto.dst.host = NULL;
			yyval.v.fromto.dst.port = NULL;
			yyval.v.fromto.src_os = NULL;
		}
#line 8837 "parse.c"
break;
case 332:
#line 3509 "parse.y"
	{
			yyval.v.fromto.src = yystack.l_mark[-2].v.peer;
			yyval.v.fromto.src_os = yystack.l_mark[-1].v.os;
			yyval.v.fromto.dst = yystack.l_mark[0].v.peer;
		}
#line 8846 "parse.c"
break;
case 333:
#line 3516 "parse.y"
	{ yyval.v.os = NULL; }
#line 8851 "parse.c"
break;
case 334:
#line 3517 "parse.y"
	{ yyval.v.os = yystack.l_mark[0].v.os; }
#line 8856 "parse.c"
break;
case 335:
#line 3518 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
#line 8861 "parse.c"
break;
case 336:
#line 3521 "parse.y"
	{
			yyval.v.os = calloc(1, sizeof(struct node_os));
			if (yyval.v.os == NULL)
				err(1, "os: calloc");
			yyval.v.os->os = yystack.l_mark[0].v.string;
			yyval.v.os->tail = yyval.v.os;
		}
#line 8872 "parse.c"
break;
case 337:
#line 3530 "parse.y"
	{ yyval.v.os = yystack.l_mark[-1].v.os; }
#line 8877 "parse.c"
break;
case 338:
#line 3531 "parse.y"
	{
			yystack.l_mark[-3].v.os->tail->next = yystack.l_mark[-1].v.os;
			yystack.l_mark[-3].v.os->tail = yystack.l_mark[-1].v.os;
			yyval.v.os = yystack.l_mark[-3].v.os;
		}
#line 8886 "parse.c"
break;
case 339:
#line 3538 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
#line 8894 "parse.c"
break;
case 340:
#line 3542 "parse.y"
	{
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
#line 8901 "parse.c"
break;
case 341:
#line 3547 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = NULL;
		}
#line 8909 "parse.c"
break;
case 342:
#line 3551 "parse.y"
	{
			if (disallow_urpf_failed(yystack.l_mark[0].v.peer.host, "\"urpf-failed\" is "
			    "not permitted in a destination address"))
				YYERROR;
			yyval.v.peer = yystack.l_mark[0].v.peer;
		}
#line 8919 "parse.c"
break;
case 343:
#line 3559 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[0].v.host;
			yyval.v.peer.port = NULL;
		}
#line 8927 "parse.c"
break;
case 344:
#line 3563 "parse.y"
	{
			yyval.v.peer.host = yystack.l_mark[-2].v.host;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
#line 8935 "parse.c"
break;
case 345:
#line 3567 "parse.y"
	{
			yyval.v.peer.host = NULL;
			yyval.v.peer.port = yystack.l_mark[0].v.port;
		}
#line 8943 "parse.c"
break;
case 348:
#line 3577 "parse.y"
	{ yyval.v.host = NULL; }
#line 8948 "parse.c"
break;
case 349:
#line 3578 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 8953 "parse.c"
break;
case 350:
#line 3579 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 8958 "parse.c"
break;
case 351:
#line 3582 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 8963 "parse.c"
break;
case 352:
#line 3583 "parse.y"
	{ yyval.v.host = NULL; }
#line 8968 "parse.c"
break;
case 353:
#line 3586 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 8973 "parse.c"
break;
case 354:
#line 3587 "parse.y"
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
#line 8988 "parse.c"
break;
case 355:
#line 3600 "parse.y"
	{
			struct node_host	*n;

			for (n = yystack.l_mark[0].v.host; n != NULL; n = n->next)
				n->not = yystack.l_mark[-1].v.number;
			yyval.v.host = yystack.l_mark[0].v.host;
		}
#line 8999 "parse.c"
break;
case 356:
#line 3607 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_NOROUTE;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
#line 9012 "parse.c"
break;
case 357:
#line 3616 "parse.y"
	{
			yyval.v.host = calloc(1, sizeof(struct node_host));
			if (yyval.v.host == NULL)
				err(1, "xhost: calloc");
			yyval.v.host->addr.type = PF_ADDR_URPFFAILED;
			yyval.v.host->next = NULL;
			yyval.v.host->not = yystack.l_mark[-1].v.number;
			yyval.v.host->tail = yyval.v.host;
		}
#line 9025 "parse.c"
break;
case 358:
#line 3627 "parse.y"
	{
			if ((yyval.v.host = host(yystack.l_mark[0].v.string)) == NULL)	{
				/* error. "any" is handled elsewhere */
				free(yystack.l_mark[0].v.string);
				yyerror("could not parse host specification");
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);

		}
#line 9039 "parse.c"
break;
case 359:
#line 3637 "parse.y"
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
#line 9076 "parse.c"
break;
case 360:
#line 3670 "parse.y"
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
#line 9094 "parse.c"
break;
case 361:
#line 3684 "parse.y"
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
#line 9116 "parse.c"
break;
case 363:
#line 3703 "parse.y"
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
#line 9131 "parse.c"
break;
case 364:
#line 3714 "parse.y"
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
#line 9153 "parse.c"
break;
case 366:
#line 3735 "parse.y"
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
#line 9168 "parse.c"
break;
case 367:
#line 3748 "parse.y"
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
#line 9224 "parse.c"
break;
case 368:
#line 3802 "parse.y"
	{ yyval.v.port = yystack.l_mark[0].v.port; }
#line 9229 "parse.c"
break;
case 369:
#line 3803 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
#line 9234 "parse.c"
break;
case 370:
#line 3806 "parse.y"
	{ yyval.v.port = yystack.l_mark[-1].v.port; }
#line 9239 "parse.c"
break;
case 371:
#line 3807 "parse.y"
	{
			yystack.l_mark[-3].v.port->tail->next = yystack.l_mark[-1].v.port;
			yystack.l_mark[-3].v.port->tail = yystack.l_mark[-1].v.port;
			yyval.v.port = yystack.l_mark[-3].v.port;
		}
#line 9248 "parse.c"
break;
case 372:
#line 3814 "parse.y"
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
#line 9265 "parse.c"
break;
case 373:
#line 3827 "parse.y"
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
#line 9284 "parse.c"
break;
case 374:
#line 3842 "parse.y"
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
#line 9303 "parse.c"
break;
case 375:
#line 3859 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, 0) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 9314 "parse.c"
break;
case 376:
#line 3868 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 9325 "parse.c"
break;
case 377:
#line 3877 "parse.y"
	{ yyval.v.uid = yystack.l_mark[0].v.uid; }
#line 9330 "parse.c"
break;
case 378:
#line 3878 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
#line 9335 "parse.c"
break;
case 379:
#line 3881 "parse.y"
	{ yyval.v.uid = yystack.l_mark[-1].v.uid; }
#line 9340 "parse.c"
break;
case 380:
#line 3882 "parse.y"
	{
			yystack.l_mark[-3].v.uid->tail->next = yystack.l_mark[-1].v.uid;
			yystack.l_mark[-3].v.uid->tail = yystack.l_mark[-1].v.uid;
			yyval.v.uid = yystack.l_mark[-3].v.uid;
		}
#line 9349 "parse.c"
break;
case 381:
#line 3889 "parse.y"
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
#line 9363 "parse.c"
break;
case 382:
#line 3899 "parse.y"
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
#line 9382 "parse.c"
break;
case 383:
#line 3914 "parse.y"
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
#line 9401 "parse.c"
break;
case 384:
#line 3931 "parse.y"
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
#line 9420 "parse.c"
break;
case 385:
#line 3946 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= UID_MAX) {
				yyerror("illegal uid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
#line 9431 "parse.c"
break;
case 386:
#line 3955 "parse.y"
	{ yyval.v.gid = yystack.l_mark[0].v.gid; }
#line 9436 "parse.c"
break;
case 387:
#line 3956 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
#line 9441 "parse.c"
break;
case 388:
#line 3959 "parse.y"
	{ yyval.v.gid = yystack.l_mark[-1].v.gid; }
#line 9446 "parse.c"
break;
case 389:
#line 3960 "parse.y"
	{
			yystack.l_mark[-3].v.gid->tail->next = yystack.l_mark[-1].v.gid;
			yystack.l_mark[-3].v.gid->tail = yystack.l_mark[-1].v.gid;
			yyval.v.gid = yystack.l_mark[-3].v.gid;
		}
#line 9455 "parse.c"
break;
case 390:
#line 3967 "parse.y"
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
#line 9469 "parse.c"
break;
case 391:
#line 3977 "parse.y"
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
#line 9488 "parse.c"
break;
case 392:
#line 3992 "parse.y"
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
#line 9507 "parse.c"
break;
case 393:
#line 4009 "parse.y"
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
#line 9526 "parse.c"
break;
case 394:
#line 4024 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number >= GID_MAX) {
				yyerror("illegal gid value %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number;
		}
#line 9537 "parse.c"
break;
case 395:
#line 4033 "parse.y"
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
#line 9552 "parse.c"
break;
case 396:
#line 4046 "parse.y"
	{ yyval.v.b.b1 = yystack.l_mark[-2].v.b.b1; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
#line 9557 "parse.c"
break;
case 397:
#line 4047 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.b.b1; }
#line 9562 "parse.c"
break;
case 398:
#line 4048 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = 0; }
#line 9567 "parse.c"
break;
case 399:
#line 4051 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
#line 9572 "parse.c"
break;
case 400:
#line 4052 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9577 "parse.c"
break;
case 401:
#line 4053 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[0].v.icmp; }
#line 9582 "parse.c"
break;
case 402:
#line 4054 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9587 "parse.c"
break;
case 403:
#line 4057 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9592 "parse.c"
break;
case 404:
#line 4058 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
#line 9601 "parse.c"
break;
case 405:
#line 4065 "parse.y"
	{ yyval.v.icmp = yystack.l_mark[-1].v.icmp; }
#line 9606 "parse.c"
break;
case 406:
#line 4066 "parse.y"
	{
			yystack.l_mark[-3].v.icmp->tail->next = yystack.l_mark[-1].v.icmp;
			yystack.l_mark[-3].v.icmp->tail = yystack.l_mark[-1].v.icmp;
			yyval.v.icmp = yystack.l_mark[-3].v.icmp;
		}
#line 9615 "parse.c"
break;
case 407:
#line 4073 "parse.y"
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
#line 9629 "parse.c"
break;
case 408:
#line 4083 "parse.y"
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
#line 9652 "parse.c"
break;
case 409:
#line 4102 "parse.y"
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
#line 9670 "parse.c"
break;
case 410:
#line 4118 "parse.y"
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
#line 9684 "parse.c"
break;
case 411:
#line 4128 "parse.y"
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
#line 9707 "parse.c"
break;
case 412:
#line 4147 "parse.y"
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
#line 9725 "parse.c"
break;
case 413:
#line 4163 "parse.y"
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
#line 9740 "parse.c"
break;
case 414:
#line 4174 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
#line 9751 "parse.c"
break;
case 415:
#line 4183 "parse.y"
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
#line 9767 "parse.c"
break;
case 416:
#line 4195 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > 255) {
				yyerror("illegal icmp6-type %lu", yystack.l_mark[0].v.number);
				YYERROR;
			}
			yyval.v.number = yystack.l_mark[0].v.number + 1;
		}
#line 9778 "parse.c"
break;
case 417:
#line 4204 "parse.y"
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
#line 9802 "parse.c"
break;
case 418:
#line 4224 "parse.y"
	{
			yyval.v.number = yystack.l_mark[0].v.number;
			if (yyval.v.number < 0 || yyval.v.number > 255) {
				yyerror("illegal tos value %s", yystack.l_mark[0].v.number);
				YYERROR;
			}
		}
#line 9813 "parse.c"
break;
case 419:
#line 4233 "parse.y"
	{ yyval.v.i = PF_SRCTRACK; }
#line 9818 "parse.c"
break;
case 420:
#line 4234 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_GLOBAL; }
#line 9823 "parse.c"
break;
case 421:
#line 4235 "parse.y"
	{ yyval.v.i = PF_SRCTRACK_RULE; }
#line 9828 "parse.c"
break;
case 422:
#line 4238 "parse.y"
	{
			yyval.v.i = PFRULE_IFBOUND;
		}
#line 9835 "parse.c"
break;
case 423:
#line 4241 "parse.y"
	{
			yyval.v.i = 0;
		}
#line 9842 "parse.c"
break;
case 424:
#line 4246 "parse.y"
	{
			yyval.v.keep_state.action = 0;
			yyval.v.keep_state.options = NULL;
		}
#line 9850 "parse.c"
break;
case 425:
#line 4250 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_NORMAL;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9858 "parse.c"
break;
case 426:
#line 4254 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_MODULATE;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9866 "parse.c"
break;
case 427:
#line 4258 "parse.y"
	{
			yyval.v.keep_state.action = PF_STATE_SYNPROXY;
			yyval.v.keep_state.options = yystack.l_mark[0].v.state_opt;
		}
#line 9874 "parse.c"
break;
case 428:
#line 4264 "parse.y"
	{ yyval.v.i = 0; }
#line 9879 "parse.c"
break;
case 429:
#line 4265 "parse.y"
	{ yyval.v.i = PF_FLUSH; }
#line 9884 "parse.c"
break;
case 430:
#line 4266 "parse.y"
	{
			yyval.v.i = PF_FLUSH | PF_FLUSH_GLOBAL;
		}
#line 9891 "parse.c"
break;
case 431:
#line 4271 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[-1].v.state_opt; }
#line 9896 "parse.c"
break;
case 432:
#line 4272 "parse.y"
	{ yyval.v.state_opt = NULL; }
#line 9901 "parse.c"
break;
case 433:
#line 4275 "parse.y"
	{ yyval.v.state_opt = yystack.l_mark[0].v.state_opt; }
#line 9906 "parse.c"
break;
case 434:
#line 4276 "parse.y"
	{
			yystack.l_mark[-2].v.state_opt->tail->next = yystack.l_mark[0].v.state_opt;
			yystack.l_mark[-2].v.state_opt->tail = yystack.l_mark[0].v.state_opt;
			yyval.v.state_opt = yystack.l_mark[-2].v.state_opt;
		}
#line 9915 "parse.c"
break;
case 435:
#line 4283 "parse.y"
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
#line 9932 "parse.c"
break;
case 436:
#line 4296 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_NOSYNC;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 9944 "parse.c"
break;
case 437:
#line 4304 "parse.y"
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
#line 9961 "parse.c"
break;
case 438:
#line 4317 "parse.y"
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
#line 9978 "parse.c"
break;
case 439:
#line 4330 "parse.y"
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
#line 9997 "parse.c"
break;
case 440:
#line 4345 "parse.y"
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
#line 10019 "parse.c"
break;
case 441:
#line 4363 "parse.y"
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
#line 10036 "parse.c"
break;
case 442:
#line 4376 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SRCTRACK;
			yyval.v.state_opt->data.src_track = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10049 "parse.c"
break;
case 443:
#line 4385 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_STATELOCK;
			yyval.v.state_opt->data.statelock = yystack.l_mark[0].v.i;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10062 "parse.c"
break;
case 444:
#line 4394 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_SLOPPY;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10074 "parse.c"
break;
case 445:
#line 4402 "parse.y"
	{
			yyval.v.state_opt = calloc(1, sizeof(struct node_state_opt));
			if (yyval.v.state_opt == NULL)
				err(1, "state_opt_item: calloc");
			yyval.v.state_opt->type = PF_STATE_OPT_ALLOW_RELATED;
			yyval.v.state_opt->next = NULL;
			yyval.v.state_opt->tail = yyval.v.state_opt;
		}
#line 10086 "parse.c"
break;
case 446:
#line 4410 "parse.y"
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
#line 10120 "parse.c"
break;
case 447:
#line 4442 "parse.y"
	{
			yyval.v.string = yystack.l_mark[0].v.string;
		}
#line 10127 "parse.c"
break;
case 448:
#line 4447 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
		}
#line 10134 "parse.c"
break;
case 449:
#line 4450 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
		}
#line 10141 "parse.c"
break;
case 450:
#line 4455 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[0].v.string;
			yyval.v.qassign.pqname = NULL;
		}
#line 10149 "parse.c"
break;
case 451:
#line 4459 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-1].v.string;
			yyval.v.qassign.pqname = NULL;
		}
#line 10157 "parse.c"
break;
case 452:
#line 4463 "parse.y"
	{
			yyval.v.qassign.qname = yystack.l_mark[-3].v.string;
			yyval.v.qassign.pqname = yystack.l_mark[-1].v.string;
		}
#line 10165 "parse.c"
break;
case 453:
#line 4469 "parse.y"
	{ yyval.v.i = 0; }
#line 10170 "parse.c"
break;
case 454:
#line 4470 "parse.y"
	{ yyval.v.i = 1; }
#line 10175 "parse.c"
break;
case 455:
#line 4473 "parse.y"
	{
			if (parseport(yystack.l_mark[0].v.string, &yyval.v.range, PPORT_RANGE|PPORT_STAR) == -1) {
				free(yystack.l_mark[0].v.string);
				YYERROR;
			}
			free(yystack.l_mark[0].v.string);
		}
#line 10186 "parse.c"
break;
case 456:
#line 4482 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 10191 "parse.c"
break;
case 457:
#line 4483 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10196 "parse.c"
break;
case 458:
#line 4486 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10201 "parse.c"
break;
case 459:
#line 4487 "parse.y"
	{
			yystack.l_mark[-3].v.host->tail->next = yystack.l_mark[-1].v.host;
			yystack.l_mark[-3].v.host->tail = yystack.l_mark[-1].v.host->tail;
			yyval.v.host = yystack.l_mark[-3].v.host;
		}
#line 10210 "parse.c"
break;
case 460:
#line 4494 "parse.y"
	{ yyval.v.redirection = NULL; }
#line 10215 "parse.c"
break;
case 461:
#line 4495 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
#line 10226 "parse.c"
break;
case 462:
#line 4502 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
#line 10237 "parse.c"
break;
case 463:
#line 4512 "parse.y"
	{
			yyval.v.hashkey = calloc(1, sizeof(struct pf_poolhashkey));
			if (yyval.v.hashkey == NULL)
				err(1, "hashkey: calloc");
			yyval.v.hashkey->key32[0] = arc4random();
			yyval.v.hashkey->key32[1] = arc4random();
			yyval.v.hashkey->key32[2] = arc4random();
			yyval.v.hashkey->key32[3] = arc4random();
		}
#line 10250 "parse.c"
break;
case 464:
#line 4522 "parse.y"
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
#line 10291 "parse.c"
break;
case 465:
#line 4561 "parse.y"
	{ bzero(&pool_opts, sizeof pool_opts); }
#line 10296 "parse.c"
break;
case 466:
#line 4563 "parse.y"
	{ yyval.v.pool_opts = pool_opts; }
#line 10301 "parse.c"
break;
case 467:
#line 4564 "parse.y"
	{
			bzero(&pool_opts, sizeof pool_opts);
			yyval.v.pool_opts = pool_opts;
		}
#line 10309 "parse.c"
break;
case 470:
#line 4574 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type =  PF_POOL_BITMASK;
		}
#line 10320 "parse.c"
break;
case 471:
#line 4581 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_RANDOM;
		}
#line 10331 "parse.c"
break;
case 472:
#line 4588 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_SRCHASH;
			pool_opts.key = yystack.l_mark[0].v.hashkey;
		}
#line 10343 "parse.c"
break;
case 473:
#line 4596 "parse.y"
	{
			if (pool_opts.type) {
				yyerror("pool type cannot be redefined");
				YYERROR;
			}
			pool_opts.type = PF_POOL_ROUNDROBIN;
		}
#line 10354 "parse.c"
break;
case 474:
#line 4603 "parse.y"
	{
			if (pool_opts.staticport) {
				yyerror("static-port cannot be redefined");
				YYERROR;
			}
			pool_opts.staticport = 1;
		}
#line 10365 "parse.c"
break;
case 475:
#line 4610 "parse.y"
	{
			if (pool_opts.marker & POM_STICKYADDRESS) {
				yyerror("sticky-address cannot be redefined");
				YYERROR;
			}
			pool_opts.marker |= POM_STICKYADDRESS;
			pool_opts.opts |= PF_POOL_STICKYADDR;
		}
#line 10377 "parse.c"
break;
case 476:
#line 4618 "parse.y"
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
#line 10411 "parse.c"
break;
case 477:
#line 4650 "parse.y"
	{ yyval.v.redirection = NULL; }
#line 10416 "parse.c"
break;
case 478:
#line 4651 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[0].v.host;
			yyval.v.redirection->rport.a = yyval.v.redirection->rport.b = yyval.v.redirection->rport.t = 0;
		}
#line 10427 "parse.c"
break;
case 479:
#line 4658 "parse.y"
	{
			yyval.v.redirection = calloc(1, sizeof(struct redirection));
			if (yyval.v.redirection == NULL)
				err(1, "redirection: calloc");
			yyval.v.redirection->host = yystack.l_mark[-2].v.host;
			yyval.v.redirection->rport = yystack.l_mark[0].v.range;
		}
#line 10438 "parse.c"
break;
case 480:
#line 4667 "parse.y"
	{ yyval.v.b.b1 = yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
#line 10443 "parse.c"
break;
case 481:
#line 4668 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = 0; yyval.v.b.w2 = 0; }
#line 10448 "parse.c"
break;
case 482:
#line 4669 "parse.y"
	{ yyval.v.b.b1 = 1; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
#line 10453 "parse.c"
break;
case 483:
#line 4670 "parse.y"
	{ yyval.v.b.b1 = 0; yyval.v.b.b2 = yystack.l_mark[0].v.logquick.log; yyval.v.b.w2 = yystack.l_mark[0].v.logquick.logif; }
#line 10458 "parse.c"
break;
case 484:
#line 4673 "parse.y"
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
#line 10475 "parse.c"
break;
case 485:
#line 4686 "parse.y"
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
#line 10492 "parse.c"
break;
case 486:
#line 4703 "parse.y"
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
#line 10678 "parse.c"
break;
case 487:
#line 4889 "parse.y"
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
#line 10857 "parse.c"
break;
case 488:
#line 5066 "parse.y"
	{ yyval.v.string = NULL; }
#line 10862 "parse.c"
break;
case 489:
#line 5067 "parse.y"
	{ yyval.v.string = yystack.l_mark[0].v.string; }
#line 10867 "parse.c"
break;
case 490:
#line 5070 "parse.y"
	{ yyval.v.tagged.neg = 0; yyval.v.tagged.name = NULL; }
#line 10872 "parse.c"
break;
case 491:
#line 5071 "parse.y"
	{ yyval.v.tagged.neg = yystack.l_mark[-2].v.number; yyval.v.tagged.name = yystack.l_mark[0].v.string; }
#line 10877 "parse.c"
break;
case 492:
#line 5074 "parse.y"
	{ yyval.v.rtableid = -1; }
#line 10882 "parse.c"
break;
case 493:
#line 5075 "parse.y"
	{
			if (yystack.l_mark[0].v.number < 0 || yystack.l_mark[0].v.number > rt_tableid_max()) {
				yyerror("invalid rtable id");
				YYERROR;
			}
			yyval.v.rtableid = yystack.l_mark[0].v.number;
		}
#line 10893 "parse.c"
break;
case 494:
#line 5084 "parse.y"
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
#line 10910 "parse.c"
break;
case 495:
#line 5097 "parse.y"
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
#line 10926 "parse.c"
break;
case 496:
#line 5111 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10931 "parse.c"
break;
case 497:
#line 5112 "parse.y"
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
#line 10947 "parse.c"
break;
case 498:
#line 5126 "parse.y"
	{ yyval.v.host = yystack.l_mark[0].v.host; }
#line 10952 "parse.c"
break;
case 499:
#line 5127 "parse.y"
	{ yyval.v.host = yystack.l_mark[-1].v.host; }
#line 10957 "parse.c"
break;
case 500:
#line 5130 "parse.y"
	{
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
#line 10966 "parse.c"
break;
case 501:
#line 5135 "parse.y"
	{
			/* backwards-compat */
			yyval.v.route.host = NULL;
			yyval.v.route.rt = 0;
			yyval.v.route.pool_opts = 0;
		}
#line 10976 "parse.c"
break;
case 502:
#line 5141 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_ROUTETO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 10987 "parse.c"
break;
case 503:
#line 5148 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_REPLYTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 10998 "parse.c"
break;
case 504:
#line 5155 "parse.y"
	{
			yyval.v.route.host = yystack.l_mark[-1].v.host;
			yyval.v.route.rt = PF_DUPTO;
			yyval.v.route.pool_opts = yystack.l_mark[0].v.pool_opts.type | yystack.l_mark[0].v.pool_opts.opts;
			if (yystack.l_mark[0].v.pool_opts.key != NULL)
				yyval.v.route.key = yystack.l_mark[0].v.pool_opts.key;
		}
#line 11009 "parse.c"
break;
case 505:
#line 5165 "parse.y"
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
#line 11029 "parse.c"
break;
case 506:
#line 5181 "parse.y"
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
#line 11043 "parse.c"
break;
case 509:
#line 5198 "parse.y"
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
#line 11063 "parse.c"
break;
case 514:
#line 5224 "parse.y"
	{ yyval.v.number = 0; }
#line 11068 "parse.c"
break;
case 515:
#line 5225 "parse.y"
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
#line 11083 "parse.c"
break;
case 516:
#line 5238 "parse.y"
	{ yyval.v.i = PF_OP_EQ; }
#line 11088 "parse.c"
break;
case 517:
#line 5239 "parse.y"
	{ yyval.v.i = PF_OP_NE; }
#line 11093 "parse.c"
break;
case 518:
#line 5240 "parse.y"
	{ yyval.v.i = PF_OP_LE; }
#line 11098 "parse.c"
break;
case 519:
#line 5241 "parse.y"
	{ yyval.v.i = PF_OP_LT; }
#line 11103 "parse.c"
break;
case 520:
#line 5242 "parse.y"
	{ yyval.v.i = PF_OP_GE; }
#line 11108 "parse.c"
break;
case 521:
#line 5243 "parse.y"
	{ yyval.v.i = PF_OP_GT; }
#line 11113 "parse.c"
break;
#line 11115 "parse.c"
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
