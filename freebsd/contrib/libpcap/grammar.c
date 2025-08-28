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
#define yyparse    pcap_parse
#endif /* yyparse */

#ifndef yylex
#define yylex      pcap_lex
#endif /* yylex */

#ifndef yyerror
#define yyerror    pcap_error
#endif /* yyerror */

#ifndef yychar
#define yychar     pcap_char
#endif /* yychar */

#ifndef yyval
#define yyval      pcap_val
#endif /* yyval */

#ifndef yylval
#define yylval     pcap_lval
#endif /* yylval */

#ifndef yydebug
#define yydebug    pcap_debug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    pcap_nerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  pcap_errflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      pcap_lhs
#endif /* yylhs */

#ifndef yylen
#define yylen      pcap_len
#endif /* yylen */

#ifndef yydefred
#define yydefred   pcap_defred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    pcap_dgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   pcap_sindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   pcap_rindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   pcap_gindex
#endif /* yygindex */

#ifndef yytable
#define yytable    pcap_table
#endif /* yytable */

#ifndef yycheck
#define yycheck    pcap_check
#endif /* yycheck */

#ifndef yyname
#define yyname     pcap_name
#endif /* yyname */

#ifndef yyrule
#define yyrule     pcap_rule
#endif /* yyrule */
#define YYPREFIX "pcap_"

#define YYPURE 1

#line 48 "grammar.y"
/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <config.h>

/*
 * grammar.h requires gencode.h and sometimes breaks in a polluted namespace
 * (see ftmacros.h), so include it early.
 */
#include "gencode.h"
#include "grammar.h"

#include <stdlib.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>

#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* _WIN32 */

#include <stdio.h>

#include "diag-control.h"

#include "pcap-int.h"

#include "scanner.h"

#include "llc.h"
#include "ieee80211.h"
#include "pflog.h"
#include <pcap/namedb.h>

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

/*
 * Work around some bugs in Berkeley YACC prior to the 2017-07-09
 * release.
 *
 * The 2005-05-05 release was the first one to define YYPATCH, so
 * we treat any release that either 1) doesn't define YYPATCH or
 * 2) defines it to a value < 20170709 as being buggy.
 */
#if defined(YYBYACC) && (!defined(YYPATCH) || YYPATCH < 20170709)
/*
 * Both Berkeley YACC and Bison define yydebug (under whatever name
 * it has) as a global, but Bison does so only if YYDEBUG is defined.
 * Berkeley YACC, prior to the 2017-07-09 release, defines it even if
 * YYDEBUG isn't defined; declare it here to suppress a warning.  The
 * 2017-07-09 release fixes that.
 */
#if !defined(YYDEBUG)
extern int yydebug;
#endif

/*
 * In Berkeley YACC, prior to the 2017-07-09 release, yynerrs (under
 * whatever name it has) is global, even if it's building a reentrant
 * parser.  In Bison, and in the Berkeley YACC 2017-07-09 release and
 * later, it's local in reentrant parsers.
 *
 * Declare it to squelch a warning.
 */
extern int yynerrs;
#endif

#define QSET(q, p, d, a) (q).proto = (unsigned char)(p),\
			 (q).dir = (unsigned char)(d),\
			 (q).addr = (unsigned char)(a)

struct tok {
	int v;			/* value */
	const char *s;		/* string */
};

static const struct tok ieee80211_types[] = {
	{ IEEE80211_FC0_TYPE_DATA, "data" },
	{ IEEE80211_FC0_TYPE_MGT, "mgt" },
	{ IEEE80211_FC0_TYPE_MGT, "management" },
	{ IEEE80211_FC0_TYPE_CTL, "ctl" },
	{ IEEE80211_FC0_TYPE_CTL, "control" },
	{ 0, NULL }
};
static const struct tok ieee80211_mgt_subtypes[] = {
	{ IEEE80211_FC0_SUBTYPE_ASSOC_REQ, "assocreq" },
	{ IEEE80211_FC0_SUBTYPE_ASSOC_REQ, "assoc-req" },
	{ IEEE80211_FC0_SUBTYPE_ASSOC_RESP, "assocresp" },
	{ IEEE80211_FC0_SUBTYPE_ASSOC_RESP, "assoc-resp" },
	{ IEEE80211_FC0_SUBTYPE_REASSOC_REQ, "reassocreq" },
	{ IEEE80211_FC0_SUBTYPE_REASSOC_REQ, "reassoc-req" },
	{ IEEE80211_FC0_SUBTYPE_REASSOC_RESP, "reassocresp" },
	{ IEEE80211_FC0_SUBTYPE_REASSOC_RESP, "reassoc-resp" },
	{ IEEE80211_FC0_SUBTYPE_PROBE_REQ, "probereq" },
	{ IEEE80211_FC0_SUBTYPE_PROBE_REQ, "probe-req" },
	{ IEEE80211_FC0_SUBTYPE_PROBE_RESP, "proberesp" },
	{ IEEE80211_FC0_SUBTYPE_PROBE_RESP, "probe-resp" },
	{ IEEE80211_FC0_SUBTYPE_BEACON, "beacon" },
	{ IEEE80211_FC0_SUBTYPE_ATIM, "atim" },
	{ IEEE80211_FC0_SUBTYPE_DISASSOC, "disassoc" },
	{ IEEE80211_FC0_SUBTYPE_DISASSOC, "disassociation" },
	{ IEEE80211_FC0_SUBTYPE_AUTH, "auth" },
	{ IEEE80211_FC0_SUBTYPE_AUTH, "authentication" },
	{ IEEE80211_FC0_SUBTYPE_DEAUTH, "deauth" },
	{ IEEE80211_FC0_SUBTYPE_DEAUTH, "deauthentication" },
	{ 0, NULL }
};
static const struct tok ieee80211_ctl_subtypes[] = {
	{ IEEE80211_FC0_SUBTYPE_PS_POLL, "ps-poll" },
	{ IEEE80211_FC0_SUBTYPE_RTS, "rts" },
	{ IEEE80211_FC0_SUBTYPE_CTS, "cts" },
	{ IEEE80211_FC0_SUBTYPE_ACK, "ack" },
	{ IEEE80211_FC0_SUBTYPE_CF_END, "cf-end" },
	{ IEEE80211_FC0_SUBTYPE_CF_END_ACK, "cf-end-ack" },
	{ 0, NULL }
};
static const struct tok ieee80211_data_subtypes[] = {
	{ IEEE80211_FC0_SUBTYPE_DATA, "data" },
	{ IEEE80211_FC0_SUBTYPE_CF_ACK, "data-cf-ack" },
	{ IEEE80211_FC0_SUBTYPE_CF_POLL, "data-cf-poll" },
	{ IEEE80211_FC0_SUBTYPE_CF_ACPL, "data-cf-ack-poll" },
	{ IEEE80211_FC0_SUBTYPE_NODATA, "null" },
	{ IEEE80211_FC0_SUBTYPE_NODATA_CF_ACK, "cf-ack" },
	{ IEEE80211_FC0_SUBTYPE_NODATA_CF_POLL, "cf-poll"  },
	{ IEEE80211_FC0_SUBTYPE_NODATA_CF_ACPL, "cf-ack-poll" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_DATA, "qos-data" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_CF_ACK, "qos-data-cf-ack" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_CF_POLL, "qos-data-cf-poll" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_CF_ACPL, "qos-data-cf-ack-poll" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_NODATA, "qos" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_NODATA_CF_POLL, "qos-cf-poll" },
	{ IEEE80211_FC0_SUBTYPE_QOS|IEEE80211_FC0_SUBTYPE_NODATA_CF_ACPL, "qos-cf-ack-poll" },
	{ 0, NULL }
};
static const struct tok llc_s_subtypes[] = {
	{ LLC_RR, "rr" },
	{ LLC_RNR, "rnr" },
	{ LLC_REJ, "rej" },
	{ 0, NULL }
};
static const struct tok llc_u_subtypes[] = {
	{ LLC_UI, "ui" },
	{ LLC_UA, "ua" },
	{ LLC_DISC, "disc" },
	{ LLC_DM, "dm" },
	{ LLC_SABME, "sabme" },
	{ LLC_TEST, "test" },
	{ LLC_XID, "xid" },
	{ LLC_FRMR, "frmr" },
	{ 0, NULL }
};
struct type2tok {
	int type;
	const struct tok *tok;
};
static const struct type2tok ieee80211_type_subtypes[] = {
	{ IEEE80211_FC0_TYPE_MGT, ieee80211_mgt_subtypes },
	{ IEEE80211_FC0_TYPE_CTL, ieee80211_ctl_subtypes },
	{ IEEE80211_FC0_TYPE_DATA, ieee80211_data_subtypes },
	{ 0, NULL }
};

static int
str2tok(const char *str, const struct tok *toks)
{
	int i;

	for (i = 0; toks[i].s != NULL; i++) {
		if (pcapint_strcasecmp(toks[i].s, str) == 0) {
			/*
			 * Just in case somebody is using this to
			 * generate values of -1/0xFFFFFFFF.
			 * That won't work, as it's indistinguishable
			 * from an error.
			 */
			if (toks[i].v == -1)
				abort();
			return (toks[i].v);
		}
	}
	return (-1);
}

static const struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF, Q_UNDEF };

static void
yyerror(void *yyscanner _U_, compiler_state_t *cstate, const char *msg)
{
	bpf_set_error(cstate, "can't parse filter expression: %s", msg);
}

static const struct tok pflog_reasons[] = {
	{ PFRES_MATCH,		"match" },
	{ PFRES_BADOFF,		"bad-offset" },
	{ PFRES_FRAG,		"fragment" },
	{ PFRES_SHORT,		"short" },
	{ PFRES_NORM,		"normalize" },
	{ PFRES_MEMORY,		"memory" },
	{ PFRES_TS,		"bad-timestamp" },
	{ PFRES_CONGEST,	"congestion" },
	{ PFRES_IPOPTIONS,	"ip-option" },
	{ PFRES_PROTCKSUM,	"proto-cksum" },
	{ PFRES_BADSTATE,	"state-mismatch" },
	{ PFRES_STATEINS,	"state-insert" },
	{ PFRES_MAXSTATES,	"state-limit" },
	{ PFRES_SRCLIMIT,	"src-limit" },
	{ PFRES_SYNPROXY,	"synproxy" },
#if defined(__FreeBSD__)
	{ PFRES_MAPFAILED,	"map-failed" },
#elif defined(__NetBSD__)
	{ PFRES_STATELOCKED,	"state-locked" },
#elif defined(__OpenBSD__)
	{ PFRES_TRANSLATE,	"translate" },
	{ PFRES_NOROUTE,	"no-route" },
#elif defined(__APPLE__)
	{ PFRES_DUMMYNET,	"dummynet" },
#endif
	{ 0, NULL }
};

static int
pfreason_to_num(compiler_state_t *cstate, const char *reason)
{
	int i;

	i = str2tok(reason, pflog_reasons);
	if (i == -1)
		bpf_set_error(cstate, "unknown PF reason \"%s\"", reason);
	return (i);
}

static const struct tok pflog_actions[] = {
	{ PF_PASS,		"pass" },
	{ PF_PASS,		"accept" },	/* alias for "pass" */
	{ PF_DROP,		"drop" },
	{ PF_DROP,		"block" },	/* alias for "drop" */
	{ PF_SCRUB,		"scrub" },
	{ PF_NOSCRUB,		"noscrub" },
	{ PF_NAT,		"nat" },
	{ PF_NONAT,		"nonat" },
	{ PF_BINAT,		"binat" },
	{ PF_NOBINAT,		"nobinat" },
	{ PF_RDR,		"rdr" },
	{ PF_NORDR,		"nordr" },
	{ PF_SYNPROXY_DROP,	"synproxy-drop" },
#if defined(__FreeBSD__)
	{ PF_DEFER,		"defer" },
#elif defined(__OpenBSD__)
	{ PF_DEFER,		"defer" },
	{ PF_MATCH,		"match" },
	{ PF_DIVERT,		"divert" },
	{ PF_RT,		"rt" },
	{ PF_AFRT,		"afrt" },
#elif defined(__APPLE__)
	{ PF_DUMMYNET,		"dummynet" },
	{ PF_NODUMMYNET,	"nodummynet" },
	{ PF_NAT64,		"nat64" },
	{ PF_NONAT64,		"nonat64" },
#endif
	{ 0, NULL },
};

static int
pfaction_to_num(compiler_state_t *cstate, const char *action)
{
	int i;

	i = str2tok(action, pflog_actions);
	if (i == -1)
		bpf_set_error(cstate, "unknown PF action \"%s\"", action);
	return (i);
}

/*
 * For calls that might return an "an error occurred" value.
 */
#define CHECK_INT_VAL(val)	if (val == -1) YYABORT
#define CHECK_PTR_VAL(val)	if (val == NULL) YYABORT

DIAG_OFF_BISON_BYACC
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 357 "grammar.y"
typedef union {
	int i;
	bpf_u_int32 h;
	char *s;
	struct stmt *stmt;
	struct arth *a;
	struct {
		struct qual q;
		int atmfieldtype;
		int mtp3fieldtype;
		struct block *b;
	} blk;
	struct block *rblk;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 431 "grammar.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void *yyscanner, compiler_state_t *cstate)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# ifdef YYLEX_PARAM_TYPE
#  define YYLEX_DECL() yylex(YYSTYPE *yylval, YYLEX_PARAM_TYPE YYLEX_PARAM)
# else
#  define YYLEX_DECL() yylex(YYSTYPE *yylval, void * YYLEX_PARAM)
# endif
# define YYLEX yylex(&yylval, YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(YYSTYPE *yylval, void *yyscanner)
# define YYLEX yylex(&yylval, yyscanner)
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(void *yyscanner, compiler_state_t *cstate, const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(yyscanner, cstate, msg)
#endif

extern int YYPARSE_DECL();

#define DST 257
#define SRC 258
#define HOST 259
#define GATEWAY 260
#define NET 261
#define NETMASK 262
#define PORT 263
#define PORTRANGE 264
#define LESS 265
#define GREATER 266
#define PROTO 267
#define PROTOCHAIN 268
#define CBYTE 269
#define ARP 270
#define RARP 271
#define IP 272
#define SCTP 273
#define TCP 274
#define UDP 275
#define ICMP 276
#define IGMP 277
#define IGRP 278
#define PIM 279
#define VRRP 280
#define CARP 281
#define ATALK 282
#define AARP 283
#define DECNET 284
#define LAT 285
#define SCA 286
#define MOPRC 287
#define MOPDL 288
#define TK_BROADCAST 289
#define TK_MULTICAST 290
#define NUM 291
#define INBOUND 292
#define OUTBOUND 293
#define IFINDEX 294
#define PF_IFNAME 295
#define PF_RSET 296
#define PF_RNR 297
#define PF_SRNR 298
#define PF_REASON 299
#define PF_ACTION 300
#define TYPE 301
#define SUBTYPE 302
#define DIR 303
#define ADDR1 304
#define ADDR2 305
#define ADDR3 306
#define ADDR4 307
#define RA 308
#define TA 309
#define LINK 310
#define GEQ 311
#define LEQ 312
#define NEQ 313
#define ID 314
#define EID 315
#define HID 316
#define HID6 317
#define AID 318
#define LSH 319
#define RSH 320
#define LEN 321
#define IPV6 322
#define ICMPV6 323
#define AH 324
#define ESP 325
#define VLAN 326
#define MPLS 327
#define PPPOED 328
#define PPPOES 329
#define GENEVE 330
#define ISO 331
#define ESIS 332
#define CLNP 333
#define ISIS 334
#define L1 335
#define L2 336
#define IIH 337
#define LSP 338
#define SNP 339
#define CSNP 340
#define PSNP 341
#define STP 342
#define IPX 343
#define NETBEUI 344
#define LANE 345
#define LLC 346
#define METAC 347
#define BCC 348
#define SC 349
#define ILMIC 350
#define OAMF4EC 351
#define OAMF4SC 352
#define OAM 353
#define OAMF4 354
#define CONNECTMSG 355
#define METACONNECT 356
#define VPI 357
#define VCI 358
#define RADIO 359
#define FISU 360
#define LSSU 361
#define MSU 362
#define HFISU 363
#define HLSSU 364
#define HMSU 365
#define SIO 366
#define OPC 367
#define DPC 368
#define SLS 369
#define HSIO 370
#define HOPC 371
#define HDPC 372
#define HSLS 373
#define LEX_ERROR 374
#define OR 375
#define AND 376
#define UMINUS 377
#define YYERRCODE 256
typedef int YYINT;
static const YYINT pcap_lhs[] = {                        -1,
    0,    0,   24,    1,    1,    1,    1,    1,   20,   21,
    2,    2,    2,    3,    3,    3,    3,    3,    3,    3,
    3,    3,   23,   22,    4,    4,    4,    7,    7,    5,
    5,    8,    8,    8,    8,    8,    8,    6,    6,    6,
    6,    6,    6,    6,    6,    6,    6,    6,    9,    9,
   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,
   10,   10,   11,   11,   11,   11,   12,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
   16,   16,   16,   16,   16,   16,   25,   25,   25,   25,
   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,
   25,   25,   25,   25,   25,   25,   26,   26,   26,   26,
   26,   26,   27,   27,   27,   27,   42,   42,   43,   43,
   44,   28,   28,   28,   45,   45,   41,   41,   40,   17,
   17,   17,   18,   18,   18,   13,   13,   14,   14,   14,
   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,
   14,   14,   15,   15,   15,   15,   15,   19,   19,   29,
   29,   29,   29,   29,   29,   29,   30,   30,   30,   30,
   31,   31,   33,   33,   33,   33,   32,   34,   34,   35,
   35,   35,   35,   35,   35,   36,   36,   36,   36,   36,
   36,   36,   36,   38,   38,   38,   38,   37,   39,   39,
};
static const YYINT pcap_len[] = {                         2,
    2,    1,    0,    1,    3,    3,    3,    3,    1,    1,
    1,    1,    3,    1,    3,    3,    1,    3,    1,    1,
    1,    2,    1,    1,    1,    3,    3,    1,    1,    1,
    2,    3,    2,    2,    2,    2,    2,    2,    3,    1,
    3,    3,    1,    1,    1,    2,    1,    2,    1,    0,
    1,    1,    3,    3,    3,    3,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    2,    2,    2,    2,
    4,    1,    1,    2,    2,    1,    2,    1,    1,    2,
    1,    2,    1,    1,    2,    1,    2,    2,    2,    2,
    2,    2,    4,    2,    2,    2,    1,    1,    1,    1,
    1,    1,    2,    2,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    4,    6,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    2,
    3,    1,    1,    1,    1,    1,    1,    1,    3,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    2,    2,    3,    1,    1,    3,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    2,    2,    3,    1,    1,    3,
};
static const YYINT pcap_defred[] = {                      3,
    0,    0,    0,    0,    0,   70,   71,   69,   72,   73,
   74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
   84,   85,   87,   86,  178,  112,  113,    0,    0,    0,
    0,    0,    0,    0,   68,  172,   88,   89,   90,   91,
    0,    0,  119,    0,    0,   92,   93,  102,   94,   95,
   96,   97,   98,   99,  101,  100,  103,  104,  105,  180,
    0,  181,  182,  185,  186,  183,  184,  187,  188,  189,
  190,  191,  192,  106,  200,  201,  202,  203,  204,  205,
  206,  207,  208,  209,  210,  211,  212,  213,   23,    0,
   24,    0,    4,   30,    0,    0,    0,  157,    0,  156,
    0,    0,   43,  124,  126,   44,   45,    0,   47,    0,
  109,  110,    0,  114,  127,  128,  129,  130,  147,  148,
  131,  149,  132,  115,    0,  117,  120,  122,  144,  143,
    0,    0,    0,   10,    9,    0,    0,   14,   20,    0,
    0,   21,   38,   11,   12,    0,    0,    0,    0,   63,
   67,   64,   65,   66,   35,   36,  107,  108,    0,    0,
    0,   57,   58,   59,   60,   61,   62,    0,   34,   37,
  125,  151,  153,  155,    0,    0,    0,    0,    0,    0,
    0,    0,  150,  152,  154,    0,    0,    0,    0,    0,
    0,    0,    0,   31,  197,    0,    0,    0,  193,   46,
  218,    0,    0,    0,  214,   48,  174,  173,  176,  177,
  175,    0,    0,    0,    6,    5,    0,    0,    0,    8,
    7,    0,    0,    0,   25,    0,    0,    0,   22,    0,
    0,    0,    0,  137,  138,    0,  141,  135,  145,  146,
  136,   32,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   39,  171,  179,  194,  195,
  198,    0,  215,  216,  219,    0,  111,    0,   16,   15,
   18,   13,    0,    0,   54,   56,   53,   55,    0,  158,
    0,  196,    0,  217,    0,   26,   27,  139,  140,  133,
    0,  199,  220,  159,
};
static const YYINT pcap_dgoto[] = {                       1,
  191,  229,  144,  226,   93,   94,  227,   95,   96,  168,
  169,  170,   97,   98,  212,  132,  188,  189,  100,  136,
  137,  133,  147,    2,  103,  104,  171,  105,  106,  107,
  108,  199,  200,  262,  109,  110,  205,  206,  266,  123,
  121,  236,  290,  238,  241,
};
static const YYINT pcap_sindex[] = {                      0,
    0,  433, -269, -265, -254,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -252, -263, -234,
 -184, -177, -194, -230,    0,    0,    0,    0,    0,    0,
  -24,  -24,    0,  -24,  -24,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -290,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  655,
    0, -358,    0,    0,  -21,  961,  850,    0,   31,    0,
  433,  433,    0,    0,    0,    0,    0,  140,    0,  239,
    0,    0,  251,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -24,    0,    0,    0,    0,    0,
  -27,   31,  655,    0,    0,  321,  321,    0,    0,  -47,
   57,    0,    0,    0,    0,  -21,  -21, -342, -340,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -185, -176,
 -183,    0,    0,    0,    0,    0,    0,  -86,    0,    0,
    0,    0,    0,    0,  655,  655,  655,  655,  655,  655,
  655,  655,    0,    0,    0,  655,  655,  655,  655,  655,
  -39,  104,  109,    0,    0, -148, -139, -135,    0,    0,
    0, -128, -125, -122,    0,    0,    0,    0,    0,    0,
    0, -117,  109,  914,    0,    0,    0,  321,  321,    0,
    0, -140, -112, -110,    0,  150, -358,  109,    0,  -65,
  -63,  -61,  -60,    0,    0,  -97,    0,    0,    0,    0,
    0,    0,  445,  445,  184,  530,  238,  238,  -27,  -27,
  914,  914,  914,  914, 1006,    0,    0,    0,    0,    0,
    0,  -37,    0,    0,    0,  -36,    0,  109,    0,    0,
    0,    0,  -21,  -21,    0,    0,    0,    0, -173,    0,
  -85,    0, -135,    0, -122,    0,    0,    0,    0,    0,
  115,    0,    0,    0,
};
static const YYINT pcap_rindex[] = {                      0,
    0,  576,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    6,    8,    0,   13,   25,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   28,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  212,    0,    0,    0,    0,    0,    0,    1,    0,
 1038, 1038,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    3,    0,    0,    0,    0, 1038, 1038,    0,    0,   32,
   38,    0,    0,    0,    0,    0,    0,  709,  778,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   53,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  861,  973,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   15, 1038, 1038,    0,
    0,    0,    0,    0,    0, -250,    0, -187,    0,    0,
    0,    0,    0,    0,    0,   70,    0,    0,    0,    0,
    0,    0,   30,  124,  158,  149,   99,  110,   40,   74,
  183,  194,   72,   89,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  770,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
static const YYINT pcap_gindex[] = {                      0,
  211,   10, -115,    0,  102,    0,    0,    0,    0,    0,
   46,    0, 1026,  -90,    0,  261,  -87,  -81, 1063,   -2,
 -207, 1030,  407,    0,    0,    0,    0,    0,    0,    0,
    0, -189,    0,    0,    0,    0, -190,    0,    0,    0,
    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 1347
static const YYINT pcap_table[] = {                     223,
   40,  256,  170,  282,  284,  116,  129,  118,  261,  186,
  192,   89,  121,  265,   12,   91,  134,  135,   91,  274,
  196,  111,  202,  130,  123,  112,  197,  142,  203,  168,
  225,   17,  230,  231,  232,  233,  113,   19,  114,  162,
  170,   40,  192,  170,  170,  170,  116,  170,  118,  170,
  115,  156,  156,  121,  283,   12,  156,  156,  285,  156,
  170,  156,  170,  170,  170,  123,  187,  168,  142,  134,
  168,   41,   17,  163,  156,  156,  156,  162,   19,  116,
  162,  162,  162,  122,  162,   33,  162,  168,   42,  168,
  168,  168,   33,  292,  293,  170,  119,  162,  160,  162,
  162,  162,  225,  224,  143,  234,  117,  239,  156,  161,
  134,  163,   41,  118,  163,  163,  163,  288,  163,  120,
  163,  190,  168,  169,   29,   29,  170,  192,  235,   42,
  240,  163,  162,  163,  163,  163,  160,  237,  156,  160,
  289,  160,  259,  160,  257,  215,  220,  161,  165,  258,
  161,  260,  161,  168,  161,  195,  160,  166,  160,  160,
  160,  169,  263,  162,  169,  264,  163,  161,  201,  161,
  161,  161,  150,  267,  152,  269,  153,  154,  270,   91,
  271,  169,  164,  169,  169,  169,  165,   28,   28,  165,
  272,  160,  275,  167,  276,  277,  278,  163,  166,  185,
  184,  183,  161,  194,  279,  291,  165,  294,  165,  165,
  165,    1,   92,  242,  222,  166,  169,  166,  166,  166,
  186,  178,  160,  164,  273,  181,  179,    0,  180,    0,
  182,    0,    0,  161,  167,    0,    0,  216,  221,    0,
  164,  165,  164,  164,  164,    0,    0,  169,    0,    0,
  166,  167,    0,  167,  167,  167,    0,   49,   49,   49,
   49,   49,   99,   49,   49,    0,   25,   49,   49,   25,
    0,    0,  165,    0,  186,  164,    0,  187,   91,  181,
    0,  166,  286,  287,  182,    0,  167,    0,  208,   49,
   49,    0,  138,  139,  140,  141,  142,    0,  185,  184,
  183,   49,   49,   49,   49,   49,   49,   49,   49,   49,
  211,  210,  209,  170,  170,  170,    0,    0,    0,    0,
  194,  170,  170,    0,    0,  156,  156,  156,    0,    0,
    0,  187,    0,  156,  156,  134,  135,  134,  134,    0,
  168,  168,  168,   33,    0,    0,    0,    0,  168,  168,
  162,  162,  162,   89,    0,    0,    0,    0,  162,  162,
   91,   99,   99,    0,    0,   90,   33,   33,   33,   33,
   33,    0,    0,    0,  207,   40,   40,  170,  170,    0,
  116,  116,  118,  118,  163,  163,  163,  121,  121,   12,
   12,    0,  163,  163,    0,    0,   99,   99,    0,  123,
  123,    0,  142,  142,  168,  168,   17,   17,  102,  160,
  160,  160,   19,   19,  162,  162,    0,  160,  160,    0,
  161,  161,  161,    0,    0,    0,    0,    0,  161,  161,
  195,    0,    0,    0,  169,  169,  169,    0,    0,    0,
    0,    0,  169,  169,  134,  134,   41,   41,  163,  163,
  172,  173,  174,    0,    0,    0,    0,    0,    0,  165,
  165,  165,    0,   42,   42,   89,    0,    0,  166,  166,
  166,    0,   91,  160,  160,    0,    0,   90,   99,   99,
    0,  186,    0,    0,  161,  161,  181,  179,    0,  180,
    0,  182,    0,  164,  164,  164,    0,    0,  169,  169,
    0,    0,  175,  176,  167,  167,  167,  102,  102,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  165,  165,    0,    0,    0,    0,  201,
    0,    0,  166,  166,    0,    0,    0,    0,  187,    0,
    0,    0,  219,  219,    0,    0,    0,    0,    0,  172,
  173,  174,    0,    0,    0,    0,    0,  164,  164,    0,
    0,    0,    0,    0,    0,    0,  186,    0,  167,  167,
    0,  181,  179,    0,  180,    2,  182,    0,    0,    0,
    0,    0,    0,    0,    0,    3,    4,    0,    0,    5,
    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,    0,
    0,   25,   26,   27,   28,   29,   30,   31,   32,   33,
   34,    0,    0,  187,  219,  219,    0,    0,    0,    0,
   35,    0,    0,    0,  138,  139,  140,  141,  142,    0,
    0,   36,   37,   38,   39,   40,   41,   42,   43,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
   55,   56,   57,   58,   59,   60,   61,   62,   63,   64,
   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,
   75,   76,   77,   78,   79,   80,   81,   82,   83,   84,
   85,   86,   87,   88,   91,    0,    0,    3,    4,   90,
    0,    5,    6,    7,    8,    9,   10,   11,   12,   13,
   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
   24,    0,    0,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,    0,    0,    0,    0,    0,    0,    0,
    0,   52,   35,    0,    0,    0,    0,    0,   52,    0,
    0,    0,    0,   36,   37,   38,   39,   40,   41,   42,
   43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,
   63,   64,   65,   66,   67,   68,   69,   70,   71,   72,
   73,   74,   75,   76,   77,   78,   79,   80,   81,   82,
   83,   84,   85,   86,   87,   88,  156,  156,    0,    0,
   51,  156,  156,    0,  156,    0,  156,   51,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  156,
  156,  156,   50,   50,   50,   50,   50,    0,   50,   50,
    0,    0,   50,   50,    0,    0,    0,    0,  175,  176,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  156,   50,   50,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   50,   50,   50,   50,
   50,   50,   50,   50,   50,    0,  186,  178,    0,    0,
    0,  181,  179,  156,  180,    0,  182,  157,  157,    0,
    0,    0,  157,  157,    0,  157,    0,  157,    0,  185,
  184,  183,    0,    0,    0,    0,    0,    0,    0,    0,
  157,  157,  157,    0,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
   22,   23,   24,  187,    0,   25,    0,    0,    0,    0,
  186,  178,    0,    0,  157,  181,  179,    0,  180,    0,
  182,    0,    0,    0,   35,    0,    0,   52,    0,   52,
    0,   52,   52,  177,    0,   36,   37,   38,   39,   40,
    0,    0,    0,    0,  157,   46,   47,   48,   49,   50,
   51,   52,   53,   54,   55,   56,   57,   58,   59,   52,
    0,    0,    0,    0,    0,    0,    0,  187,    0,  156,
  156,    0,    0,   74,  156,  156,    0,  156,    0,  156,
    0,    0,   52,   52,   52,   52,   52,    0,    0,    0,
    0,  101,  156,  156,  156,    0,   51,  177,   51,    0,
   51,   51,  186,  178,    0,    0,    0,  181,  179,    0,
  180,    0,  182,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  281,    0,    0,  156,    0,   51,    0,
  125,  125,    0,  125,  125,    0,    0,    0,    0,    0,
  156,  156,  156,    0,    0,    0,    0,    0,  156,  156,
    0,   51,   51,   51,   51,   51,  156,    0,  280,  187,
    0,    0,    0,  124,  126,    0,  127,  128,    0,    0,
    0,    0,    0,    0,    0,  131,    0,    0,    0,    0,
    0,    0,    0,    0,  146,    0,    0,    0,    0,  177,
  101,  101,    0,    0,    0,    0,    0,  198,    0,  204,
    0,    0,    0,    0,   28,   28,    0,    0,    0,    0,
    0,    0,    0,    0,  125,    0,    0,  145,  214,    0,
  172,  173,  174,  193,    0,  218,  218,    0,  175,  176,
    0,  157,  157,  157,    0,  125,  146,    0,    0,  157,
  157,    0,    0,    0,    0,    0,    0,  213,    0,    0,
    0,    0,    0,    0,    0,  193,    0,    0,  217,  217,
  243,  244,  245,  246,  247,  248,  249,  250,  228,  145,
    0,  251,  252,  253,  254,  255,    0,  148,  149,  150,
  151,  152,    0,  153,  154,    0,    0,  155,  156,    0,
    0,    0,  175,  176,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  101,  218,  157,
  158,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  159,  160,  161,  162,  163,  164,  165,  166,  167,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  268,  217,    0,  156,  156,  156,    0,    0,    0,    0,
    0,  156,  156,    0,   50,   50,   50,   50,   50,    0,
   50,   50,  146,  146,   50,   50,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  175,  176,   50,   50,    0,    0,
    0,    0,    0,    0,    0,  145,  145,    0,   50,   50,
   50,   50,   50,   50,   50,   50,   50,
};
static const YYINT pcap_check[] = {                      47,
    0,   41,    0,   41,   41,    0,  297,    0,  198,   37,
  101,   33,    0,  204,    0,   40,  375,  376,   40,  227,
  108,  291,  110,  314,    0,  291,  108,    0,  110,    0,
  146,    0,  375,  376,  375,  376,  291,    0,  291,    0,
   38,   41,  133,   41,   42,   43,   41,   45,   41,   47,
  314,   37,   38,   41,  262,   41,   42,   43,  266,   45,
   58,   47,   60,   61,   62,   41,   94,   38,   41,    0,
   41,    0,   41,    0,   60,   61,   62,   38,   41,  314,
   41,   42,   43,  314,   45,   33,   47,   58,    0,   60,
   61,   62,   40,  283,  285,   93,  291,   58,    0,   60,
   61,   62,  218,   47,   95,  291,  291,  291,   94,    0,
   41,   38,   41,  291,   41,   42,   43,  291,   45,  314,
   47,   91,   93,    0,  375,  376,  124,  218,  314,   41,
  314,   58,   93,   60,   61,   62,   38,  314,  124,   41,
  314,   43,  291,   45,   41,  136,  137,   38,    0,   41,
   41,  291,   43,  124,   45,  291,   58,    0,   60,   61,
   62,   38,  291,  124,   41,  291,   93,   58,  291,   60,
   61,   62,  259,  291,  261,  316,  263,  264,  291,   40,
  291,   58,    0,   60,   61,   62,   38,  375,  376,   41,
   41,   93,  258,    0,  258,  257,  257,  124,   41,   60,
   61,   62,   93,  102,  302,  291,   58,   93,   60,   61,
   62,    0,    2,  168,  262,   58,   93,   60,   61,   62,
   37,   38,  124,   41,  227,   42,   43,   -1,   45,   -1,
   47,   -1,   -1,  124,   41,   -1,   -1,  136,  137,   -1,
   58,   93,   60,   61,   62,   -1,   -1,  124,   -1,   -1,
   93,   58,   -1,   60,   61,   62,   -1,  257,  258,  259,
  260,  261,    2,  263,  264,   -1,  291,  267,  268,  291,
   -1,   -1,  124,   -1,   37,   93,   -1,   94,   40,   42,
   -1,  124,  273,  274,   47,   -1,   93,   -1,   38,  289,
  290,   -1,  314,  315,  316,  317,  318,   -1,   60,   61,
   62,  301,  302,  303,  304,  305,  306,  307,  308,  309,
   60,   61,   62,  311,  312,  313,   -1,   -1,   -1,   -1,
  219,  319,  320,   -1,   -1,  311,  312,  313,   -1,   -1,
   -1,   94,   -1,  319,  320,  375,  376,  375,  375,   -1,
  311,  312,  313,  291,   -1,   -1,   -1,   -1,  319,  320,
  311,  312,  313,   33,   -1,   -1,   -1,   -1,  319,  320,
   40,  101,  102,   -1,   -1,   45,  314,  315,  316,  317,
  318,   -1,   -1,   -1,  124,  375,  376,  375,  376,   -1,
  375,  376,  375,  376,  311,  312,  313,  375,  376,  375,
  376,   -1,  319,  320,   -1,   -1,  136,  137,   -1,  375,
  376,   -1,  375,  376,  375,  376,  375,  376,    2,  311,
  312,  313,  375,  376,  375,  376,   -1,  319,  320,   -1,
  311,  312,  313,   -1,   -1,   -1,   -1,   -1,  319,  320,
  291,   -1,   -1,   -1,  311,  312,  313,   -1,   -1,   -1,
   -1,   -1,  319,  320,  375,  376,  375,  376,  375,  376,
  311,  312,  313,   -1,   -1,   -1,   -1,   -1,   -1,  311,
  312,  313,   -1,  375,  376,   33,   -1,   -1,  311,  312,
  313,   -1,   40,  375,  376,   -1,   -1,   45,  218,  219,
   -1,   37,   -1,   -1,  375,  376,   42,   43,   -1,   45,
   -1,   47,   -1,  311,  312,  313,   -1,   -1,  375,  376,
   -1,   -1,  319,  320,  311,  312,  313,  101,  102,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  375,  376,   -1,   -1,   -1,   -1,  291,
   -1,   -1,  375,  376,   -1,   -1,   -1,   -1,   94,   -1,
   -1,   -1,  136,  137,   -1,   -1,   -1,   -1,   -1,  311,
  312,  313,   -1,   -1,   -1,   -1,   -1,  375,  376,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   37,   -1,  375,  376,
   -1,   42,   43,   -1,   45,    0,   47,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  265,  266,   -1,   -1,  269,
  270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,  283,  284,  285,  286,  287,  288,   -1,
   -1,  291,  292,  293,  294,  295,  296,  297,  298,  299,
  300,   -1,   -1,   94,  218,  219,   -1,   -1,   -1,   -1,
  310,   -1,   -1,   -1,  314,  315,  316,  317,  318,   -1,
   -1,  321,  322,  323,  324,  325,  326,  327,  328,  329,
  330,  331,  332,  333,  334,  335,  336,  337,  338,  339,
  340,  341,  342,  343,  344,  345,  346,  347,  348,  349,
  350,  351,  352,  353,  354,  355,  356,  357,  358,  359,
  360,  361,  362,  363,  364,  365,  366,  367,  368,  369,
  370,  371,  372,  373,   40,   -1,   -1,  265,  266,   45,
   -1,  269,  270,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  283,  284,  285,  286,  287,
  288,   -1,   -1,  291,  292,  293,  294,  295,  296,  297,
  298,  299,  300,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   33,  310,   -1,   -1,   -1,   -1,   -1,   40,   -1,
   -1,   -1,   -1,  321,  322,  323,  324,  325,  326,  327,
  328,  329,  330,  331,  332,  333,  334,  335,  336,  337,
  338,  339,  340,  341,  342,  343,  344,  345,  346,  347,
  348,  349,  350,  351,  352,  353,  354,  355,  356,  357,
  358,  359,  360,  361,  362,  363,  364,  365,  366,  367,
  368,  369,  370,  371,  372,  373,   37,   38,   -1,   -1,
   33,   42,   43,   -1,   45,   -1,   47,   40,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   60,
   61,   62,  257,  258,  259,  260,  261,   -1,  263,  264,
   -1,   -1,  267,  268,   -1,   -1,   -1,   -1,  319,  320,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   94,  289,  290,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  301,  302,  303,  304,
  305,  306,  307,  308,  309,   -1,   37,   38,   -1,   -1,
   -1,   42,   43,  124,   45,   -1,   47,   37,   38,   -1,
   -1,   -1,   42,   43,   -1,   45,   -1,   47,   -1,   60,
   61,   62,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   60,   61,   62,   -1,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,  282,  283,  284,  285,
  286,  287,  288,   94,   -1,  291,   -1,   -1,   -1,   -1,
   37,   38,   -1,   -1,   94,   42,   43,   -1,   45,   -1,
   47,   -1,   -1,   -1,  310,   -1,   -1,  259,   -1,  261,
   -1,  263,  264,  124,   -1,  321,  322,  323,  324,  325,
   -1,   -1,   -1,   -1,  124,  331,  332,  333,  334,  335,
  336,  337,  338,  339,  340,  341,  342,  343,  344,  291,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   94,   -1,   37,
   38,   -1,   -1,  359,   42,   43,   -1,   45,   -1,   47,
   -1,   -1,  314,  315,  316,  317,  318,   -1,   -1,   -1,
   -1,    2,   60,   61,   62,   -1,  259,  124,  261,   -1,
  263,  264,   37,   38,   -1,   -1,   -1,   42,   43,   -1,
   45,   -1,   47,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   58,   -1,   -1,   94,   -1,  291,   -1,
   41,   42,   -1,   44,   45,   -1,   -1,   -1,   -1,   -1,
  311,  312,  313,   -1,   -1,   -1,   -1,   -1,  319,  320,
   -1,  314,  315,  316,  317,  318,  124,   -1,   93,   94,
   -1,   -1,   -1,   41,   42,   -1,   44,   45,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   90,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   95,   -1,   -1,   -1,   -1,  124,
  101,  102,   -1,   -1,   -1,   -1,   -1,  108,   -1,  110,
   -1,   -1,   -1,   -1,  375,  376,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  125,   -1,   -1,   95,  133,   -1,
  311,  312,  313,  101,   -1,  136,  137,   -1,  319,  320,
   -1,  311,  312,  313,   -1,  146,  147,   -1,   -1,  319,
  320,   -1,   -1,   -1,   -1,   -1,   -1,  125,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  133,   -1,   -1,  136,  137,
  175,  176,  177,  178,  179,  180,  181,  182,  146,  147,
   -1,  186,  187,  188,  189,  190,   -1,  257,  258,  259,
  260,  261,   -1,  263,  264,   -1,   -1,  267,  268,   -1,
   -1,   -1,  319,  320,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  218,  219,  289,
  290,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  301,  302,  303,  304,  305,  306,  307,  308,  309,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  218,  219,   -1,  311,  312,  313,   -1,   -1,   -1,   -1,
   -1,  319,  320,   -1,  257,  258,  259,  260,  261,   -1,
  263,  264,  273,  274,  267,  268,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  319,  320,  289,  290,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  273,  274,   -1,  301,  302,
  303,  304,  305,  306,  307,  308,  309,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 377
#define YYUNDFTOKEN 425
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const pcap_name[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,
0,0,0,"':'",0,"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'|'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DST","SRC","HOST","GATEWAY","NET",
"NETMASK","PORT","PORTRANGE","LESS","GREATER","PROTO","PROTOCHAIN","CBYTE",
"ARP","RARP","IP","SCTP","TCP","UDP","ICMP","IGMP","IGRP","PIM","VRRP","CARP",
"ATALK","AARP","DECNET","LAT","SCA","MOPRC","MOPDL","TK_BROADCAST",
"TK_MULTICAST","NUM","INBOUND","OUTBOUND","IFINDEX","PF_IFNAME","PF_RSET",
"PF_RNR","PF_SRNR","PF_REASON","PF_ACTION","TYPE","SUBTYPE","DIR","ADDR1",
"ADDR2","ADDR3","ADDR4","RA","TA","LINK","GEQ","LEQ","NEQ","ID","EID","HID",
"HID6","AID","LSH","RSH","LEN","IPV6","ICMPV6","AH","ESP","VLAN","MPLS",
"PPPOED","PPPOES","GENEVE","ISO","ESIS","CLNP","ISIS","L1","L2","IIH","LSP",
"SNP","CSNP","PSNP","STP","IPX","NETBEUI","LANE","LLC","METAC","BCC","SC",
"ILMIC","OAMF4EC","OAMF4SC","OAM","OAMF4","CONNECTMSG","METACONNECT","VPI",
"VCI","RADIO","FISU","LSSU","MSU","HFISU","HLSSU","HMSU","SIO","OPC","DPC",
"SLS","HSIO","HOPC","HDPC","HSLS","LEX_ERROR","OR","AND","UMINUS",0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"illegal-symbol",
};
static const char *const pcap_rule[] = {
"$accept : prog",
"prog : null expr",
"prog : null",
"null :",
"expr : term",
"expr : expr and term",
"expr : expr and id",
"expr : expr or term",
"expr : expr or id",
"and : AND",
"or : OR",
"id : nid",
"id : pnum",
"id : paren pid ')'",
"nid : ID",
"nid : HID '/' NUM",
"nid : HID NETMASK HID",
"nid : HID",
"nid : HID6 '/' NUM",
"nid : HID6",
"nid : EID",
"nid : AID",
"nid : not id",
"not : '!'",
"paren : '('",
"pid : nid",
"pid : qid and id",
"pid : qid or id",
"qid : pnum",
"qid : pid",
"term : rterm",
"term : not term",
"head : pqual dqual aqual",
"head : pqual dqual",
"head : pqual aqual",
"head : pqual PROTO",
"head : pqual PROTOCHAIN",
"head : pqual ndaqual",
"rterm : head id",
"rterm : paren expr ')'",
"rterm : pname",
"rterm : arth relop arth",
"rterm : arth irelop arth",
"rterm : other",
"rterm : atmtype",
"rterm : atmmultitype",
"rterm : atmfield atmvalue",
"rterm : mtp2type",
"rterm : mtp3field mtp3value",
"pqual : pname",
"pqual :",
"dqual : SRC",
"dqual : DST",
"dqual : SRC OR DST",
"dqual : DST OR SRC",
"dqual : SRC AND DST",
"dqual : DST AND SRC",
"dqual : ADDR1",
"dqual : ADDR2",
"dqual : ADDR3",
"dqual : ADDR4",
"dqual : RA",
"dqual : TA",
"aqual : HOST",
"aqual : NET",
"aqual : PORT",
"aqual : PORTRANGE",
"ndaqual : GATEWAY",
"pname : LINK",
"pname : IP",
"pname : ARP",
"pname : RARP",
"pname : SCTP",
"pname : TCP",
"pname : UDP",
"pname : ICMP",
"pname : IGMP",
"pname : IGRP",
"pname : PIM",
"pname : VRRP",
"pname : CARP",
"pname : ATALK",
"pname : AARP",
"pname : DECNET",
"pname : LAT",
"pname : SCA",
"pname : MOPDL",
"pname : MOPRC",
"pname : IPV6",
"pname : ICMPV6",
"pname : AH",
"pname : ESP",
"pname : ISO",
"pname : ESIS",
"pname : ISIS",
"pname : L1",
"pname : L2",
"pname : IIH",
"pname : LSP",
"pname : SNP",
"pname : PSNP",
"pname : CSNP",
"pname : CLNP",
"pname : STP",
"pname : IPX",
"pname : NETBEUI",
"pname : RADIO",
"other : pqual TK_BROADCAST",
"other : pqual TK_MULTICAST",
"other : LESS NUM",
"other : GREATER NUM",
"other : CBYTE NUM byteop NUM",
"other : INBOUND",
"other : OUTBOUND",
"other : IFINDEX NUM",
"other : VLAN pnum",
"other : VLAN",
"other : MPLS pnum",
"other : MPLS",
"other : PPPOED",
"other : PPPOES pnum",
"other : PPPOES",
"other : GENEVE pnum",
"other : GENEVE",
"other : pfvar",
"other : pqual p80211",
"other : pllc",
"pfvar : PF_IFNAME ID",
"pfvar : PF_RSET ID",
"pfvar : PF_RNR NUM",
"pfvar : PF_SRNR NUM",
"pfvar : PF_REASON reason",
"pfvar : PF_ACTION action",
"p80211 : TYPE type SUBTYPE subtype",
"p80211 : TYPE type",
"p80211 : SUBTYPE type_subtype",
"p80211 : DIR dir",
"type : NUM",
"type : ID",
"subtype : NUM",
"subtype : ID",
"type_subtype : ID",
"pllc : LLC",
"pllc : LLC ID",
"pllc : LLC PF_RNR",
"dir : NUM",
"dir : ID",
"reason : NUM",
"reason : ID",
"action : ID",
"relop : '>'",
"relop : GEQ",
"relop : '='",
"irelop : LEQ",
"irelop : '<'",
"irelop : NEQ",
"arth : pnum",
"arth : narth",
"narth : pname '[' arth ']'",
"narth : pname '[' arth ':' NUM ']'",
"narth : arth '+' arth",
"narth : arth '-' arth",
"narth : arth '*' arth",
"narth : arth '/' arth",
"narth : arth '%' arth",
"narth : arth '&' arth",
"narth : arth '|' arth",
"narth : arth '^' arth",
"narth : arth LSH arth",
"narth : arth RSH arth",
"narth : '-' arth",
"narth : paren narth ')'",
"narth : LEN",
"byteop : '&'",
"byteop : '|'",
"byteop : '<'",
"byteop : '>'",
"byteop : '='",
"pnum : NUM",
"pnum : paren pnum ')'",
"atmtype : LANE",
"atmtype : METAC",
"atmtype : BCC",
"atmtype : OAMF4EC",
"atmtype : OAMF4SC",
"atmtype : SC",
"atmtype : ILMIC",
"atmmultitype : OAM",
"atmmultitype : OAMF4",
"atmmultitype : CONNECTMSG",
"atmmultitype : METACONNECT",
"atmfield : VPI",
"atmfield : VCI",
"atmvalue : atmfieldvalue",
"atmvalue : relop NUM",
"atmvalue : irelop NUM",
"atmvalue : paren atmlistvalue ')'",
"atmfieldvalue : NUM",
"atmlistvalue : atmfieldvalue",
"atmlistvalue : atmlistvalue or atmfieldvalue",
"mtp2type : FISU",
"mtp2type : LSSU",
"mtp2type : MSU",
"mtp2type : HFISU",
"mtp2type : HLSSU",
"mtp2type : HMSU",
"mtp3field : SIO",
"mtp3field : OPC",
"mtp3field : DPC",
"mtp3field : SLS",
"mtp3field : HSIO",
"mtp3field : HOPC",
"mtp3field : HDPC",
"mtp3field : HSLS",
"mtp3value : mtp3fieldvalue",
"mtp3value : relop NUM",
"mtp3value : irelop NUM",
"mtp3value : paren mtp3listvalue ')'",
"mtp3fieldvalue : NUM",
"mtp3listvalue : mtp3fieldvalue",
"mtp3listvalue : mtp3listvalue or mtp3fieldvalue",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

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
    int      yyerrflag;
    int      yychar;
    YYSTYPE  yyval;
    YYSTYPE  yylval;
    int      yynerrs;

    /* variables for the parser stack */
    YYSTACKDATA yystack;
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

    memset(&yyval,  0, sizeof(yyval));
    memset(&yylval, 0, sizeof(yylval));

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
case 1:
#line 432 "grammar.y"
	{
	/*
	 * I'm not sure we have a reason to use yynerrs, but it's
	 * declared, and incremented, whether we need it or not,
	 * which means that Clang 15 will give a "used but not
	 * set" warning.  This should suppress the warning for
	 * yynerrs without suppressing it for other variables.
	 */
	(void) yynerrs;
	CHECK_INT_VAL(finish_parse(cstate, yystack.l_mark[0].blk.b));
}
#line 1532 "grammar.c"
break;
case 3:
#line 445 "grammar.y"
	{ yyval.blk.q = qerr; }
#line 1537 "grammar.c"
break;
case 5:
#line 448 "grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1542 "grammar.c"
break;
case 6:
#line 449 "grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1547 "grammar.c"
break;
case 7:
#line 450 "grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1552 "grammar.c"
break;
case 8:
#line 451 "grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1557 "grammar.c"
break;
case 9:
#line 453 "grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
#line 1562 "grammar.c"
break;
case 10:
#line 455 "grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
#line 1567 "grammar.c"
break;
case 12:
#line 458 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_ncode(cstate, NULL, yystack.l_mark[0].h,
						   yyval.blk.q = yystack.l_mark[-1].blk.q))); }
#line 1573 "grammar.c"
break;
case 13:
#line 460 "grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
#line 1578 "grammar.c"
break;
case 14:
#line 462 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_PTR_VAL((yyval.blk.b = gen_scode(cstate, yystack.l_mark[0].s, yyval.blk.q = yystack.l_mark[-1].blk.q))); }
#line 1583 "grammar.c"
break;
case 15:
#line 463 "grammar.y"
	{
				  CHECK_PTR_VAL(yystack.l_mark[-2].s);
				  /* Check whether HID/NUM is being used when appropriate */
				  yyval.blk.q = yystack.l_mark[-3].blk.q;
				  if (yyval.blk.q.addr == Q_PORT) {
					bpf_set_error(cstate, "'port' modifier applied to IP address and prefix length");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PORTRANGE) {
					bpf_set_error(cstate, "'portrange' modifier applied to IP address and prefix length");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTO) {
					bpf_set_error(cstate, "'proto' modifier applied to IP address and prefix length");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTOCHAIN) {
					bpf_set_error(cstate, "'protochain' modifier applied to IP address and prefix length");
					YYABORT;
				  }
				  CHECK_PTR_VAL((yyval.blk.b = gen_mcode(cstate, yystack.l_mark[-2].s, NULL, yystack.l_mark[0].h, yyval.blk.q)));
				}
#line 1606 "grammar.c"
break;
case 16:
#line 482 "grammar.y"
	{
				  CHECK_PTR_VAL(yystack.l_mark[-2].s);
				  /* Check whether HID mask HID is being used when appropriate */
				  yyval.blk.q = yystack.l_mark[-3].blk.q;
				  if (yyval.blk.q.addr == Q_PORT) {
					bpf_set_error(cstate, "'port' modifier applied to IP address and netmask");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PORTRANGE) {
					bpf_set_error(cstate, "'portrange' modifier applied to IP address and netmask");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTO) {
					bpf_set_error(cstate, "'proto' modifier applied to IP address and netmask");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTOCHAIN) {
					bpf_set_error(cstate, "'protochain' modifier applied to IP address and netmask");
					YYABORT;
				  }
				  CHECK_PTR_VAL((yyval.blk.b = gen_mcode(cstate, yystack.l_mark[-2].s, yystack.l_mark[0].s, 0, yyval.blk.q)));
				}
#line 1629 "grammar.c"
break;
case 17:
#line 501 "grammar.y"
	{
				  CHECK_PTR_VAL(yystack.l_mark[0].s);
				  /* Check whether HID is being used when appropriate */
				  yyval.blk.q = yystack.l_mark[-1].blk.q;
				  if (yyval.blk.q.addr == Q_PORT) {
					bpf_set_error(cstate, "'port' modifier applied to IP address");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PORTRANGE) {
					bpf_set_error(cstate, "'portrange' modifier applied to IP address");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTO) {
					bpf_set_error(cstate, "'proto' modifier applied to IP address");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTOCHAIN) {
					bpf_set_error(cstate, "'protochain' modifier applied to IP address");
					YYABORT;
				  }
				  CHECK_PTR_VAL((yyval.blk.b = gen_ncode(cstate, yystack.l_mark[0].s, 0, yyval.blk.q)));
				}
#line 1652 "grammar.c"
break;
case 18:
#line 520 "grammar.y"
	{
				  CHECK_PTR_VAL(yystack.l_mark[-2].s);
#ifdef INET6
				  /* Check whether HID6/NUM is being used when appropriate */
				  yyval.blk.q = yystack.l_mark[-3].blk.q;
				  if (yyval.blk.q.addr == Q_PORT) {
					bpf_set_error(cstate, "'port' modifier applied to IP address and prefix length");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PORTRANGE) {
					bpf_set_error(cstate, "'portrange' modifier applied to IP address and prefix length");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTO) {
					bpf_set_error(cstate, "'proto' modifier applied to IP address and prefix length ");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTOCHAIN) {
					bpf_set_error(cstate, "'protochain' modifier applied to IP address and prefix length");
					YYABORT;
				  }
				  CHECK_PTR_VAL((yyval.blk.b = gen_mcode6(cstate, yystack.l_mark[-2].s, yystack.l_mark[0].h, yyval.blk.q)));
#else
				  bpf_set_error(cstate, "IPv6 addresses not supported "
					"in this configuration");
				  YYABORT;
#endif /*INET6*/
				}
#line 1681 "grammar.c"
break;
case 19:
#line 545 "grammar.y"
	{
				  CHECK_PTR_VAL(yystack.l_mark[0].s);
#ifdef INET6
				  /* Check whether HID6 is being used when appropriate */
				  yyval.blk.q = yystack.l_mark[-1].blk.q;
				  if (yyval.blk.q.addr == Q_PORT) {
					bpf_set_error(cstate, "'port' modifier applied to IP address");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PORTRANGE) {
					bpf_set_error(cstate, "'portrange' modifier applied to IP address");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTO) {
					bpf_set_error(cstate, "'proto' modifier applied to 'ip6addr/prefixlen");
					YYABORT;
				  } else if (yyval.blk.q.addr == Q_PROTOCHAIN) {
					bpf_set_error(cstate, "'protochain' modifier applied to IP address");
					YYABORT;
				  }
				  CHECK_PTR_VAL((yyval.blk.b = gen_mcode6(cstate, yystack.l_mark[0].s, 128, yyval.blk.q)));
#else
				  bpf_set_error(cstate, "IPv6 addresses not supported "
					"in this configuration");
				  YYABORT;
#endif /*INET6*/
				}
#line 1710 "grammar.c"
break;
case 20:
#line 570 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_PTR_VAL((yyval.blk.b = gen_ecode(cstate, yystack.l_mark[0].s, yyval.blk.q = yystack.l_mark[-1].blk.q))); }
#line 1715 "grammar.c"
break;
case 21:
#line 571 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_PTR_VAL((yyval.blk.b = gen_acode(cstate, yystack.l_mark[0].s, yyval.blk.q = yystack.l_mark[-1].blk.q))); }
#line 1720 "grammar.c"
break;
case 22:
#line 572 "grammar.y"
	{ gen_not(yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1725 "grammar.c"
break;
case 23:
#line 574 "grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
#line 1730 "grammar.c"
break;
case 24:
#line 576 "grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
#line 1735 "grammar.c"
break;
case 26:
#line 579 "grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1740 "grammar.c"
break;
case 27:
#line 580 "grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1745 "grammar.c"
break;
case 28:
#line 582 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_ncode(cstate, NULL, yystack.l_mark[0].h,
						   yyval.blk.q = yystack.l_mark[-1].blk.q))); }
#line 1751 "grammar.c"
break;
case 31:
#line 587 "grammar.y"
	{ gen_not(yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 1756 "grammar.c"
break;
case 32:
#line 589 "grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-2].i, yystack.l_mark[-1].i, yystack.l_mark[0].i); }
#line 1761 "grammar.c"
break;
case 33:
#line 590 "grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, yystack.l_mark[0].i, Q_DEFAULT); }
#line 1766 "grammar.c"
break;
case 34:
#line 591 "grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, yystack.l_mark[0].i); }
#line 1771 "grammar.c"
break;
case 35:
#line 592 "grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, Q_PROTO); }
#line 1776 "grammar.c"
break;
case 36:
#line 593 "grammar.y"
	{
#ifdef NO_PROTOCHAIN
				  bpf_set_error(cstate, "protochain not supported");
				  YYABORT;
#else
				  QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, Q_PROTOCHAIN);
#endif
				}
#line 1788 "grammar.c"
break;
case 37:
#line 601 "grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, yystack.l_mark[0].i); }
#line 1793 "grammar.c"
break;
case 38:
#line 603 "grammar.y"
	{ yyval.blk = yystack.l_mark[0].blk; }
#line 1798 "grammar.c"
break;
case 39:
#line 604 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = yystack.l_mark[-2].blk.q; }
#line 1803 "grammar.c"
break;
case 40:
#line 605 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_proto_abbrev(cstate, yystack.l_mark[0].i))); yyval.blk.q = qerr; }
#line 1808 "grammar.c"
break;
case 41:
#line 606 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_relation(cstate, yystack.l_mark[-1].i, yystack.l_mark[-2].a, yystack.l_mark[0].a, 0)));
				  yyval.blk.q = qerr; }
#line 1814 "grammar.c"
break;
case 42:
#line 608 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_relation(cstate, yystack.l_mark[-1].i, yystack.l_mark[-2].a, yystack.l_mark[0].a, 1)));
				  yyval.blk.q = qerr; }
#line 1820 "grammar.c"
break;
case 43:
#line 610 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].rblk; yyval.blk.q = qerr; }
#line 1825 "grammar.c"
break;
case 44:
#line 611 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_atmtype_abbrev(cstate, yystack.l_mark[0].i))); yyval.blk.q = qerr; }
#line 1830 "grammar.c"
break;
case 45:
#line 612 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_atmmulti_abbrev(cstate, yystack.l_mark[0].i))); yyval.blk.q = qerr; }
#line 1835 "grammar.c"
break;
case 46:
#line 613 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].blk.b; yyval.blk.q = qerr; }
#line 1840 "grammar.c"
break;
case 47:
#line 614 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_mtp2type_abbrev(cstate, yystack.l_mark[0].i))); yyval.blk.q = qerr; }
#line 1845 "grammar.c"
break;
case 48:
#line 615 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].blk.b; yyval.blk.q = qerr; }
#line 1850 "grammar.c"
break;
case 50:
#line 619 "grammar.y"
	{ yyval.i = Q_DEFAULT; }
#line 1855 "grammar.c"
break;
case 51:
#line 622 "grammar.y"
	{ yyval.i = Q_SRC; }
#line 1860 "grammar.c"
break;
case 52:
#line 623 "grammar.y"
	{ yyval.i = Q_DST; }
#line 1865 "grammar.c"
break;
case 53:
#line 624 "grammar.y"
	{ yyval.i = Q_OR; }
#line 1870 "grammar.c"
break;
case 54:
#line 625 "grammar.y"
	{ yyval.i = Q_OR; }
#line 1875 "grammar.c"
break;
case 55:
#line 626 "grammar.y"
	{ yyval.i = Q_AND; }
#line 1880 "grammar.c"
break;
case 56:
#line 627 "grammar.y"
	{ yyval.i = Q_AND; }
#line 1885 "grammar.c"
break;
case 57:
#line 628 "grammar.y"
	{ yyval.i = Q_ADDR1; }
#line 1890 "grammar.c"
break;
case 58:
#line 629 "grammar.y"
	{ yyval.i = Q_ADDR2; }
#line 1895 "grammar.c"
break;
case 59:
#line 630 "grammar.y"
	{ yyval.i = Q_ADDR3; }
#line 1900 "grammar.c"
break;
case 60:
#line 631 "grammar.y"
	{ yyval.i = Q_ADDR4; }
#line 1905 "grammar.c"
break;
case 61:
#line 632 "grammar.y"
	{ yyval.i = Q_RA; }
#line 1910 "grammar.c"
break;
case 62:
#line 633 "grammar.y"
	{ yyval.i = Q_TA; }
#line 1915 "grammar.c"
break;
case 63:
#line 636 "grammar.y"
	{ yyval.i = Q_HOST; }
#line 1920 "grammar.c"
break;
case 64:
#line 637 "grammar.y"
	{ yyval.i = Q_NET; }
#line 1925 "grammar.c"
break;
case 65:
#line 638 "grammar.y"
	{ yyval.i = Q_PORT; }
#line 1930 "grammar.c"
break;
case 66:
#line 639 "grammar.y"
	{ yyval.i = Q_PORTRANGE; }
#line 1935 "grammar.c"
break;
case 67:
#line 642 "grammar.y"
	{ yyval.i = Q_GATEWAY; }
#line 1940 "grammar.c"
break;
case 68:
#line 644 "grammar.y"
	{ yyval.i = Q_LINK; }
#line 1945 "grammar.c"
break;
case 69:
#line 645 "grammar.y"
	{ yyval.i = Q_IP; }
#line 1950 "grammar.c"
break;
case 70:
#line 646 "grammar.y"
	{ yyval.i = Q_ARP; }
#line 1955 "grammar.c"
break;
case 71:
#line 647 "grammar.y"
	{ yyval.i = Q_RARP; }
#line 1960 "grammar.c"
break;
case 72:
#line 648 "grammar.y"
	{ yyval.i = Q_SCTP; }
#line 1965 "grammar.c"
break;
case 73:
#line 649 "grammar.y"
	{ yyval.i = Q_TCP; }
#line 1970 "grammar.c"
break;
case 74:
#line 650 "grammar.y"
	{ yyval.i = Q_UDP; }
#line 1975 "grammar.c"
break;
case 75:
#line 651 "grammar.y"
	{ yyval.i = Q_ICMP; }
#line 1980 "grammar.c"
break;
case 76:
#line 652 "grammar.y"
	{ yyval.i = Q_IGMP; }
#line 1985 "grammar.c"
break;
case 77:
#line 653 "grammar.y"
	{ yyval.i = Q_IGRP; }
#line 1990 "grammar.c"
break;
case 78:
#line 654 "grammar.y"
	{ yyval.i = Q_PIM; }
#line 1995 "grammar.c"
break;
case 79:
#line 655 "grammar.y"
	{ yyval.i = Q_VRRP; }
#line 2000 "grammar.c"
break;
case 80:
#line 656 "grammar.y"
	{ yyval.i = Q_CARP; }
#line 2005 "grammar.c"
break;
case 81:
#line 657 "grammar.y"
	{ yyval.i = Q_ATALK; }
#line 2010 "grammar.c"
break;
case 82:
#line 658 "grammar.y"
	{ yyval.i = Q_AARP; }
#line 2015 "grammar.c"
break;
case 83:
#line 659 "grammar.y"
	{ yyval.i = Q_DECNET; }
#line 2020 "grammar.c"
break;
case 84:
#line 660 "grammar.y"
	{ yyval.i = Q_LAT; }
#line 2025 "grammar.c"
break;
case 85:
#line 661 "grammar.y"
	{ yyval.i = Q_SCA; }
#line 2030 "grammar.c"
break;
case 86:
#line 662 "grammar.y"
	{ yyval.i = Q_MOPDL; }
#line 2035 "grammar.c"
break;
case 87:
#line 663 "grammar.y"
	{ yyval.i = Q_MOPRC; }
#line 2040 "grammar.c"
break;
case 88:
#line 664 "grammar.y"
	{ yyval.i = Q_IPV6; }
#line 2045 "grammar.c"
break;
case 89:
#line 665 "grammar.y"
	{ yyval.i = Q_ICMPV6; }
#line 2050 "grammar.c"
break;
case 90:
#line 666 "grammar.y"
	{ yyval.i = Q_AH; }
#line 2055 "grammar.c"
break;
case 91:
#line 667 "grammar.y"
	{ yyval.i = Q_ESP; }
#line 2060 "grammar.c"
break;
case 92:
#line 668 "grammar.y"
	{ yyval.i = Q_ISO; }
#line 2065 "grammar.c"
break;
case 93:
#line 669 "grammar.y"
	{ yyval.i = Q_ESIS; }
#line 2070 "grammar.c"
break;
case 94:
#line 670 "grammar.y"
	{ yyval.i = Q_ISIS; }
#line 2075 "grammar.c"
break;
case 95:
#line 671 "grammar.y"
	{ yyval.i = Q_ISIS_L1; }
#line 2080 "grammar.c"
break;
case 96:
#line 672 "grammar.y"
	{ yyval.i = Q_ISIS_L2; }
#line 2085 "grammar.c"
break;
case 97:
#line 673 "grammar.y"
	{ yyval.i = Q_ISIS_IIH; }
#line 2090 "grammar.c"
break;
case 98:
#line 674 "grammar.y"
	{ yyval.i = Q_ISIS_LSP; }
#line 2095 "grammar.c"
break;
case 99:
#line 675 "grammar.y"
	{ yyval.i = Q_ISIS_SNP; }
#line 2100 "grammar.c"
break;
case 100:
#line 676 "grammar.y"
	{ yyval.i = Q_ISIS_PSNP; }
#line 2105 "grammar.c"
break;
case 101:
#line 677 "grammar.y"
	{ yyval.i = Q_ISIS_CSNP; }
#line 2110 "grammar.c"
break;
case 102:
#line 678 "grammar.y"
	{ yyval.i = Q_CLNP; }
#line 2115 "grammar.c"
break;
case 103:
#line 679 "grammar.y"
	{ yyval.i = Q_STP; }
#line 2120 "grammar.c"
break;
case 104:
#line 680 "grammar.y"
	{ yyval.i = Q_IPX; }
#line 2125 "grammar.c"
break;
case 105:
#line 681 "grammar.y"
	{ yyval.i = Q_NETBEUI; }
#line 2130 "grammar.c"
break;
case 106:
#line 682 "grammar.y"
	{ yyval.i = Q_RADIO; }
#line 2135 "grammar.c"
break;
case 107:
#line 684 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_broadcast(cstate, yystack.l_mark[-1].i))); }
#line 2140 "grammar.c"
break;
case 108:
#line 685 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_multicast(cstate, yystack.l_mark[-1].i))); }
#line 2145 "grammar.c"
break;
case 109:
#line 686 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_less(cstate, yystack.l_mark[0].h))); }
#line 2150 "grammar.c"
break;
case 110:
#line 687 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_greater(cstate, yystack.l_mark[0].h))); }
#line 2155 "grammar.c"
break;
case 111:
#line 688 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_byteop(cstate, yystack.l_mark[-1].i, yystack.l_mark[-2].h, yystack.l_mark[0].h))); }
#line 2160 "grammar.c"
break;
case 112:
#line 689 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_inbound(cstate, 0))); }
#line 2165 "grammar.c"
break;
case 113:
#line 690 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_inbound(cstate, 1))); }
#line 2170 "grammar.c"
break;
case 114:
#line 691 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_ifindex(cstate, yystack.l_mark[0].h))); }
#line 2175 "grammar.c"
break;
case 115:
#line 692 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_vlan(cstate, yystack.l_mark[0].h, 1))); }
#line 2180 "grammar.c"
break;
case 116:
#line 693 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_vlan(cstate, 0, 0))); }
#line 2185 "grammar.c"
break;
case 117:
#line 694 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_mpls(cstate, yystack.l_mark[0].h, 1))); }
#line 2190 "grammar.c"
break;
case 118:
#line 695 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_mpls(cstate, 0, 0))); }
#line 2195 "grammar.c"
break;
case 119:
#line 696 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pppoed(cstate))); }
#line 2200 "grammar.c"
break;
case 120:
#line 697 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pppoes(cstate, yystack.l_mark[0].h, 1))); }
#line 2205 "grammar.c"
break;
case 121:
#line 698 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pppoes(cstate, 0, 0))); }
#line 2210 "grammar.c"
break;
case 122:
#line 699 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_geneve(cstate, yystack.l_mark[0].h, 1))); }
#line 2215 "grammar.c"
break;
case 123:
#line 700 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_geneve(cstate, 0, 0))); }
#line 2220 "grammar.c"
break;
case 124:
#line 701 "grammar.y"
	{ yyval.rblk = yystack.l_mark[0].rblk; }
#line 2225 "grammar.c"
break;
case 125:
#line 702 "grammar.y"
	{ yyval.rblk = yystack.l_mark[0].rblk; }
#line 2230 "grammar.c"
break;
case 126:
#line 703 "grammar.y"
	{ yyval.rblk = yystack.l_mark[0].rblk; }
#line 2235 "grammar.c"
break;
case 127:
#line 706 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_PTR_VAL((yyval.rblk = gen_pf_ifname(cstate, yystack.l_mark[0].s))); }
#line 2240 "grammar.c"
break;
case 128:
#line 707 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_PTR_VAL((yyval.rblk = gen_pf_ruleset(cstate, yystack.l_mark[0].s))); }
#line 2245 "grammar.c"
break;
case 129:
#line 708 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pf_rnr(cstate, yystack.l_mark[0].h))); }
#line 2250 "grammar.c"
break;
case 130:
#line 709 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pf_srnr(cstate, yystack.l_mark[0].h))); }
#line 2255 "grammar.c"
break;
case 131:
#line 710 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pf_reason(cstate, yystack.l_mark[0].i))); }
#line 2260 "grammar.c"
break;
case 132:
#line 711 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_pf_action(cstate, yystack.l_mark[0].i))); }
#line 2265 "grammar.c"
break;
case 133:
#line 715 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_p80211_type(cstate, yystack.l_mark[-2].i | yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK |
					IEEE80211_FC0_SUBTYPE_MASK)));
				}
#line 2273 "grammar.c"
break;
case 134:
#line 719 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_p80211_type(cstate, yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK)));
				}
#line 2280 "grammar.c"
break;
case 135:
#line 722 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_p80211_type(cstate, yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK |
					IEEE80211_FC0_SUBTYPE_MASK)));
				}
#line 2288 "grammar.c"
break;
case 136:
#line 726 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_p80211_fcdir(cstate, yystack.l_mark[0].i))); }
#line 2293 "grammar.c"
break;
case 137:
#line 729 "grammar.y"
	{ if ((yystack.l_mark[0].h & (~IEEE80211_FC0_TYPE_MASK)) != 0) {
					bpf_set_error(cstate, "invalid 802.11 type value 0x%02x", yystack.l_mark[0].h);
					YYABORT;
				  }
				  yyval.i = (int)yystack.l_mark[0].h;
				}
#line 2303 "grammar.c"
break;
case 138:
#line 735 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s);
				  yyval.i = str2tok(yystack.l_mark[0].s, ieee80211_types);
				  if (yyval.i == -1) {
					bpf_set_error(cstate, "unknown 802.11 type name \"%s\"", yystack.l_mark[0].s);
					YYABORT;
				  }
				}
#line 2314 "grammar.c"
break;
case 139:
#line 744 "grammar.y"
	{ if ((yystack.l_mark[0].h & (~IEEE80211_FC0_SUBTYPE_MASK)) != 0) {
					bpf_set_error(cstate, "invalid 802.11 subtype value 0x%02x", yystack.l_mark[0].h);
					YYABORT;
				  }
				  yyval.i = (int)yystack.l_mark[0].h;
				}
#line 2324 "grammar.c"
break;
case 140:
#line 750 "grammar.y"
	{ const struct tok *types = NULL;
				  int i;
				  CHECK_PTR_VAL(yystack.l_mark[0].s);
				  for (i = 0;; i++) {
					if (ieee80211_type_subtypes[i].tok == NULL) {
						/* Ran out of types */
						bpf_set_error(cstate, "unknown 802.11 type");
						YYABORT;
					}
					if (yystack.l_mark[-2].i == ieee80211_type_subtypes[i].type) {
						types = ieee80211_type_subtypes[i].tok;
						break;
					}
				  }

				  yyval.i = str2tok(yystack.l_mark[0].s, types);
				  if (yyval.i == -1) {
					bpf_set_error(cstate, "unknown 802.11 subtype name \"%s\"", yystack.l_mark[0].s);
					YYABORT;
				  }
				}
#line 2349 "grammar.c"
break;
case 141:
#line 773 "grammar.y"
	{ int i;
				  CHECK_PTR_VAL(yystack.l_mark[0].s);
				  for (i = 0;; i++) {
					if (ieee80211_type_subtypes[i].tok == NULL) {
						/* Ran out of types */
						bpf_set_error(cstate, "unknown 802.11 type name");
						YYABORT;
					}
					yyval.i = str2tok(yystack.l_mark[0].s, ieee80211_type_subtypes[i].tok);
					if (yyval.i != -1) {
						yyval.i |= ieee80211_type_subtypes[i].type;
						break;
					}
				  }
				}
#line 2368 "grammar.c"
break;
case 142:
#line 790 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_llc(cstate))); }
#line 2373 "grammar.c"
break;
case 143:
#line 791 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s);
				  if (pcapint_strcasecmp(yystack.l_mark[0].s, "i") == 0) {
					CHECK_PTR_VAL((yyval.rblk = gen_llc_i(cstate)));
				  } else if (pcapint_strcasecmp(yystack.l_mark[0].s, "s") == 0) {
					CHECK_PTR_VAL((yyval.rblk = gen_llc_s(cstate)));
				  } else if (pcapint_strcasecmp(yystack.l_mark[0].s, "u") == 0) {
					CHECK_PTR_VAL((yyval.rblk = gen_llc_u(cstate)));
				  } else {
					int subtype;

					subtype = str2tok(yystack.l_mark[0].s, llc_s_subtypes);
					if (subtype != -1) {
						CHECK_PTR_VAL((yyval.rblk = gen_llc_s_subtype(cstate, subtype)));
					} else {
						subtype = str2tok(yystack.l_mark[0].s, llc_u_subtypes);
						if (subtype == -1) {
							bpf_set_error(cstate, "unknown LLC type name \"%s\"", yystack.l_mark[0].s);
							YYABORT;
						}
						CHECK_PTR_VAL((yyval.rblk = gen_llc_u_subtype(cstate, subtype)));
					}
				  }
				}
#line 2400 "grammar.c"
break;
case 144:
#line 815 "grammar.y"
	{ CHECK_PTR_VAL((yyval.rblk = gen_llc_s_subtype(cstate, LLC_RNR))); }
#line 2405 "grammar.c"
break;
case 145:
#line 818 "grammar.y"
	{ yyval.i = (int)yystack.l_mark[0].h; }
#line 2410 "grammar.c"
break;
case 146:
#line 819 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s);
				  if (pcapint_strcasecmp(yystack.l_mark[0].s, "nods") == 0)
					yyval.i = IEEE80211_FC1_DIR_NODS;
				  else if (pcapint_strcasecmp(yystack.l_mark[0].s, "tods") == 0)
					yyval.i = IEEE80211_FC1_DIR_TODS;
				  else if (pcapint_strcasecmp(yystack.l_mark[0].s, "fromds") == 0)
					yyval.i = IEEE80211_FC1_DIR_FROMDS;
				  else if (pcapint_strcasecmp(yystack.l_mark[0].s, "dstods") == 0)
					yyval.i = IEEE80211_FC1_DIR_DSTODS;
				  else {
					bpf_set_error(cstate, "unknown 802.11 direction");
					YYABORT;
				  }
				}
#line 2428 "grammar.c"
break;
case 147:
#line 835 "grammar.y"
	{ yyval.i = yystack.l_mark[0].h; }
#line 2433 "grammar.c"
break;
case 148:
#line 836 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_INT_VAL((yyval.i = pfreason_to_num(cstate, yystack.l_mark[0].s))); }
#line 2438 "grammar.c"
break;
case 149:
#line 839 "grammar.y"
	{ CHECK_PTR_VAL(yystack.l_mark[0].s); CHECK_INT_VAL((yyval.i = pfaction_to_num(cstate, yystack.l_mark[0].s))); }
#line 2443 "grammar.c"
break;
case 150:
#line 842 "grammar.y"
	{ yyval.i = BPF_JGT; }
#line 2448 "grammar.c"
break;
case 151:
#line 843 "grammar.y"
	{ yyval.i = BPF_JGE; }
#line 2453 "grammar.c"
break;
case 152:
#line 844 "grammar.y"
	{ yyval.i = BPF_JEQ; }
#line 2458 "grammar.c"
break;
case 153:
#line 846 "grammar.y"
	{ yyval.i = BPF_JGT; }
#line 2463 "grammar.c"
break;
case 154:
#line 847 "grammar.y"
	{ yyval.i = BPF_JGE; }
#line 2468 "grammar.c"
break;
case 155:
#line 848 "grammar.y"
	{ yyval.i = BPF_JEQ; }
#line 2473 "grammar.c"
break;
case 156:
#line 850 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_loadi(cstate, yystack.l_mark[0].h))); }
#line 2478 "grammar.c"
break;
case 158:
#line 853 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_load(cstate, yystack.l_mark[-3].i, yystack.l_mark[-1].a, 1))); }
#line 2483 "grammar.c"
break;
case 159:
#line 854 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_load(cstate, yystack.l_mark[-5].i, yystack.l_mark[-3].a, yystack.l_mark[-1].h))); }
#line 2488 "grammar.c"
break;
case 160:
#line 855 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_ADD, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2493 "grammar.c"
break;
case 161:
#line 856 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_SUB, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2498 "grammar.c"
break;
case 162:
#line 857 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_MUL, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2503 "grammar.c"
break;
case 163:
#line 858 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_DIV, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2508 "grammar.c"
break;
case 164:
#line 859 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_MOD, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2513 "grammar.c"
break;
case 165:
#line 860 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_AND, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2518 "grammar.c"
break;
case 166:
#line 861 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_OR, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2523 "grammar.c"
break;
case 167:
#line 862 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_XOR, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2528 "grammar.c"
break;
case 168:
#line 863 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_LSH, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2533 "grammar.c"
break;
case 169:
#line 864 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_arth(cstate, BPF_RSH, yystack.l_mark[-2].a, yystack.l_mark[0].a))); }
#line 2538 "grammar.c"
break;
case 170:
#line 865 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_neg(cstate, yystack.l_mark[0].a))); }
#line 2543 "grammar.c"
break;
case 171:
#line 866 "grammar.y"
	{ yyval.a = yystack.l_mark[-1].a; }
#line 2548 "grammar.c"
break;
case 172:
#line 867 "grammar.y"
	{ CHECK_PTR_VAL((yyval.a = gen_loadlen(cstate))); }
#line 2553 "grammar.c"
break;
case 173:
#line 869 "grammar.y"
	{ yyval.i = '&'; }
#line 2558 "grammar.c"
break;
case 174:
#line 870 "grammar.y"
	{ yyval.i = '|'; }
#line 2563 "grammar.c"
break;
case 175:
#line 871 "grammar.y"
	{ yyval.i = '<'; }
#line 2568 "grammar.c"
break;
case 176:
#line 872 "grammar.y"
	{ yyval.i = '>'; }
#line 2573 "grammar.c"
break;
case 177:
#line 873 "grammar.y"
	{ yyval.i = '='; }
#line 2578 "grammar.c"
break;
case 179:
#line 876 "grammar.y"
	{ yyval.h = yystack.l_mark[-1].h; }
#line 2583 "grammar.c"
break;
case 180:
#line 878 "grammar.y"
	{ yyval.i = A_LANE; }
#line 2588 "grammar.c"
break;
case 181:
#line 879 "grammar.y"
	{ yyval.i = A_METAC;	}
#line 2593 "grammar.c"
break;
case 182:
#line 880 "grammar.y"
	{ yyval.i = A_BCC; }
#line 2598 "grammar.c"
break;
case 183:
#line 881 "grammar.y"
	{ yyval.i = A_OAMF4EC; }
#line 2603 "grammar.c"
break;
case 184:
#line 882 "grammar.y"
	{ yyval.i = A_OAMF4SC; }
#line 2608 "grammar.c"
break;
case 185:
#line 883 "grammar.y"
	{ yyval.i = A_SC; }
#line 2613 "grammar.c"
break;
case 186:
#line 884 "grammar.y"
	{ yyval.i = A_ILMIC; }
#line 2618 "grammar.c"
break;
case 187:
#line 886 "grammar.y"
	{ yyval.i = A_OAM; }
#line 2623 "grammar.c"
break;
case 188:
#line 887 "grammar.y"
	{ yyval.i = A_OAMF4; }
#line 2628 "grammar.c"
break;
case 189:
#line 888 "grammar.y"
	{ yyval.i = A_CONNECTMSG; }
#line 2633 "grammar.c"
break;
case 190:
#line 889 "grammar.y"
	{ yyval.i = A_METACONNECT; }
#line 2638 "grammar.c"
break;
case 191:
#line 892 "grammar.y"
	{ yyval.blk.atmfieldtype = A_VPI; }
#line 2643 "grammar.c"
break;
case 192:
#line 893 "grammar.y"
	{ yyval.blk.atmfieldtype = A_VCI; }
#line 2648 "grammar.c"
break;
case 194:
#line 896 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_atmfield_code(cstate, yystack.l_mark[-2].blk.atmfieldtype, yystack.l_mark[0].h, yystack.l_mark[-1].i, 0))); }
#line 2653 "grammar.c"
break;
case 195:
#line 897 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_atmfield_code(cstate, yystack.l_mark[-2].blk.atmfieldtype, yystack.l_mark[0].h, yystack.l_mark[-1].i, 1))); }
#line 2658 "grammar.c"
break;
case 196:
#line 898 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = qerr; }
#line 2663 "grammar.c"
break;
case 197:
#line 900 "grammar.y"
	{
	yyval.blk.atmfieldtype = yystack.l_mark[-1].blk.atmfieldtype;
	if (yyval.blk.atmfieldtype == A_VPI ||
	    yyval.blk.atmfieldtype == A_VCI)
		CHECK_PTR_VAL((yyval.blk.b = gen_atmfield_code(cstate, yyval.blk.atmfieldtype, yystack.l_mark[0].h, BPF_JEQ, 0)));
	}
#line 2673 "grammar.c"
break;
case 199:
#line 908 "grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 2678 "grammar.c"
break;
case 200:
#line 911 "grammar.y"
	{ yyval.i = M_FISU; }
#line 2683 "grammar.c"
break;
case 201:
#line 912 "grammar.y"
	{ yyval.i = M_LSSU; }
#line 2688 "grammar.c"
break;
case 202:
#line 913 "grammar.y"
	{ yyval.i = M_MSU; }
#line 2693 "grammar.c"
break;
case 203:
#line 914 "grammar.y"
	{ yyval.i = MH_FISU; }
#line 2698 "grammar.c"
break;
case 204:
#line 915 "grammar.y"
	{ yyval.i = MH_LSSU; }
#line 2703 "grammar.c"
break;
case 205:
#line 916 "grammar.y"
	{ yyval.i = MH_MSU; }
#line 2708 "grammar.c"
break;
case 206:
#line 919 "grammar.y"
	{ yyval.blk.mtp3fieldtype = M_SIO; }
#line 2713 "grammar.c"
break;
case 207:
#line 920 "grammar.y"
	{ yyval.blk.mtp3fieldtype = M_OPC; }
#line 2718 "grammar.c"
break;
case 208:
#line 921 "grammar.y"
	{ yyval.blk.mtp3fieldtype = M_DPC; }
#line 2723 "grammar.c"
break;
case 209:
#line 922 "grammar.y"
	{ yyval.blk.mtp3fieldtype = M_SLS; }
#line 2728 "grammar.c"
break;
case 210:
#line 923 "grammar.y"
	{ yyval.blk.mtp3fieldtype = MH_SIO; }
#line 2733 "grammar.c"
break;
case 211:
#line 924 "grammar.y"
	{ yyval.blk.mtp3fieldtype = MH_OPC; }
#line 2738 "grammar.c"
break;
case 212:
#line 925 "grammar.y"
	{ yyval.blk.mtp3fieldtype = MH_DPC; }
#line 2743 "grammar.c"
break;
case 213:
#line 926 "grammar.y"
	{ yyval.blk.mtp3fieldtype = MH_SLS; }
#line 2748 "grammar.c"
break;
case 215:
#line 929 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_mtp3field_code(cstate, yystack.l_mark[-2].blk.mtp3fieldtype, yystack.l_mark[0].h, yystack.l_mark[-1].i, 0))); }
#line 2753 "grammar.c"
break;
case 216:
#line 930 "grammar.y"
	{ CHECK_PTR_VAL((yyval.blk.b = gen_mtp3field_code(cstate, yystack.l_mark[-2].blk.mtp3fieldtype, yystack.l_mark[0].h, yystack.l_mark[-1].i, 1))); }
#line 2758 "grammar.c"
break;
case 217:
#line 931 "grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = qerr; }
#line 2763 "grammar.c"
break;
case 218:
#line 933 "grammar.y"
	{
	yyval.blk.mtp3fieldtype = yystack.l_mark[-1].blk.mtp3fieldtype;
	if (yyval.blk.mtp3fieldtype == M_SIO ||
	    yyval.blk.mtp3fieldtype == M_OPC ||
	    yyval.blk.mtp3fieldtype == M_DPC ||
	    yyval.blk.mtp3fieldtype == M_SLS ||
	    yyval.blk.mtp3fieldtype == MH_SIO ||
	    yyval.blk.mtp3fieldtype == MH_OPC ||
	    yyval.blk.mtp3fieldtype == MH_DPC ||
	    yyval.blk.mtp3fieldtype == MH_SLS)
		CHECK_PTR_VAL((yyval.blk.b = gen_mtp3field_code(cstate, yyval.blk.mtp3fieldtype, yystack.l_mark[0].h, BPF_JEQ, 0)));
	}
#line 2779 "grammar.c"
break;
case 220:
#line 947 "grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
#line 2784 "grammar.c"
break;
#line 2786 "grammar.c"
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
