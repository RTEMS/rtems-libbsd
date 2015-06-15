/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0

#ifndef yyparse
#define yyparse    pcapparse
#endif /* yyparse */

#ifndef yylex
#define yylex      pcaplex
#endif /* yylex */

#ifndef yyerror
#define yyerror    pcaperror
#endif /* yyerror */

#ifndef yychar
#define yychar     pcapchar
#endif /* yychar */

#ifndef yyval
#define yyval      pcapval
#endif /* yyval */

#ifndef yylval
#define yylval     pcaplval
#endif /* yylval */

#ifndef yydebug
#define yydebug    pcapdebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    pcapnerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  pcaperrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      pcaplhs
#endif /* yylhs */

#ifndef yylen
#define yylen      pcaplen
#endif /* yylen */

#ifndef yydefred
#define yydefred   pcapdefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    pcapdgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   pcapsindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   pcaprindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   pcapgindex
#endif /* yygindex */

#ifndef yytable
#define yytable    pcaptable
#endif /* yytable */

#ifndef yycheck
#define yycheck    pcapcheck
#endif /* yycheck */

#ifndef yyname
#define yyname     pcapname
#endif /* yyname */

#ifndef yyrule
#define yyrule     pcaprule
#endif /* yyrule */
#define YYPREFIX "pcap"

#define YYPURE 0

#line 2 "../../freebsd/contrib/libpcap/grammar.y"
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
 * $FreeBSD$
 */
#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /tcpdump/master/libpcap/grammar.y,v 1.101 2007-11-18 02:03:52 guy Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <pcap-stdinc.h>
#else /* WIN32 */
#include <sys/types.h>
#include <sys/socket.h>
#endif /* WIN32 */

#include <stdlib.h>

#ifndef WIN32
#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* WIN32 */

#include <stdio.h>

#include "pcap-int.h"

#include "gencode.h"
#ifdef HAVE_NET_PFVAR_H
#include <net/if.h>
#include <net/pfvar.h>
#include <net/if_pflog.h>
#endif
#include "ieee80211.h"
#include <pcap/namedb.h>

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define QSET(q, p, d, a) (q).proto = (p),\
			 (q).dir = (d),\
			 (q).addr = (a)

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
		if (pcap_strcasecmp(toks[i].s, str) == 0)
			return (toks[i].v);
	}
	return (-1);
}

int n_errors = 0;

static struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF, Q_UNDEF };

static void
yyerror(const char *msg)
{
	++n_errors;
	bpf_error("%s", msg);
	/* NOTREACHED */
}

#ifdef NEED_YYPARSE_WRAPPER
int yyparse(void);

int
pcap_parse()
{
	return (yyparse());
}
#endif

#ifdef HAVE_NET_PFVAR_H
static int
pfreason_to_num(const char *reason)
{
	const char *reasons[] = PFRES_NAMES;
	int i;

	for (i = 0; reasons[i]; i++) {
		if (pcap_strcasecmp(reason, reasons[i]) == 0)
			return (i);
	}
	bpf_error("unknown PF reason");
	/*NOTREACHED*/
}

static int
pfaction_to_num(const char *action)
{
	if (pcap_strcasecmp(action, "pass") == 0 ||
	    pcap_strcasecmp(action, "accept") == 0)
		return (PF_PASS);
	else if (pcap_strcasecmp(action, "drop") == 0 ||
		pcap_strcasecmp(action, "block") == 0)
		return (PF_DROP);
#if HAVE_PF_NAT_THROUGH_PF_NORDR
	else if (pcap_strcasecmp(action, "rdr") == 0)
		return (PF_RDR);
	else if (pcap_strcasecmp(action, "nat") == 0)
		return (PF_NAT);
	else if (pcap_strcasecmp(action, "binat") == 0)
		return (PF_BINAT);
	else if (pcap_strcasecmp(action, "nordr") == 0)
		return (PF_NORDR);
#endif
	else {
		bpf_error("unknown PF action");
		/*NOTREACHED*/
	}
}
#else /* !HAVE_NET_PFVAR_H */
static int
pfreason_to_num(const char *reason)
{
	bpf_error("libpcap was compiled on a machine without pf support");
	/*NOTREACHED*/

	/* this is to make the VC compiler happy */
	return -1;
}

static int
pfaction_to_num(const char *action)
{
	bpf_error("libpcap was compiled on a machine without pf support");
	/*NOTREACHED*/

	/* this is to make the VC compiler happy */
	return -1;
}
#endif /* HAVE_NET_PFVAR_H */
#line 243 "../../freebsd/contrib/libpcap/grammar.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	int i;
	bpf_u_int32 h;
	u_char *e;
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
#line 363 "pcap.tab.c"

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
#define PF_IFNAME 294
#define PF_RSET 295
#define PF_RNR 296
#define PF_SRNR 297
#define PF_REASON 298
#define PF_ACTION 299
#define TYPE 300
#define SUBTYPE 301
#define DIR 302
#define ADDR1 303
#define ADDR2 304
#define ADDR3 305
#define ADDR4 306
#define RA 307
#define TA 308
#define LINK 309
#define GEQ 310
#define LEQ 311
#define NEQ 312
#define ID 313
#define EID 314
#define HID 315
#define HID6 316
#define AID 317
#define LSH 318
#define RSH 319
#define LEN 320
#define IPV6 321
#define ICMPV6 322
#define AH 323
#define ESP 324
#define VLAN 325
#define MPLS 326
#define PPPOED 327
#define PPPOES 328
#define ISO 329
#define ESIS 330
#define CLNP 331
#define ISIS 332
#define L1 333
#define L2 334
#define IIH 335
#define LSP 336
#define SNP 337
#define CSNP 338
#define PSNP 339
#define STP 340
#define IPX 341
#define NETBEUI 342
#define LANE 343
#define LLC 344
#define METAC 345
#define BCC 346
#define SC 347
#define ILMIC 348
#define OAMF4EC 349
#define OAMF4SC 350
#define OAM 351
#define OAMF4 352
#define CONNECTMSG 353
#define METACONNECT 354
#define VPI 355
#define VCI 356
#define RADIO 357
#define FISU 358
#define LSSU 359
#define MSU 360
#define SIO 361
#define OPC 362
#define DPC 363
#define SLS 364
#define OR 365
#define AND 366
#define UMINUS 367
#define YYERRCODE 256
typedef short YYINT;
static const YYINT pcaplhs[] = {                         -1,
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
   25,   26,   26,   26,   26,   26,   26,   27,   27,   27,
   27,   41,   41,   42,   42,   43,   44,   44,   40,   40,
   39,   18,   18,   18,   19,   19,   19,   13,   13,   14,
   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,
   14,   14,   15,   15,   15,   15,   15,   17,   17,   28,
   28,   28,   28,   28,   28,   28,   28,   29,   29,   29,
   29,   30,   30,   32,   32,   32,   32,   31,   33,   33,
   34,   34,   34,   35,   35,   35,   35,   37,   37,   37,
   37,   36,   38,   38,
};
static const YYINT pcaplen[] = {                          2,
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
    4,    1,    1,    2,    1,    2,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    2,    2,    4,    2,    2,
    2,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    4,
    6,    3,    3,    3,    3,    3,    3,    3,    3,    2,
    3,    1,    1,    1,    1,    1,    1,    1,    3,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    2,    2,    3,    1,    1,    3,
    1,    1,    1,    1,    1,    1,    1,    1,    2,    2,
    3,    1,    1,    3,
};
static const YYINT pcapdefred[] = {                       3,
    0,    0,    0,    0,    0,   70,   71,   69,   72,   73,
   74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
   84,   85,   87,   86,  168,  112,  113,    0,    0,    0,
    0,    0,    0,   68,  162,   88,   89,   90,   91,    0,
    0,  118,  119,   92,   93,  102,   94,   95,   96,   97,
   98,   99,  101,  100,  103,  104,  105,  170,  171,  172,
  173,  176,  177,  174,  175,  178,  179,  180,  181,  182,
  183,  106,  191,  192,  193,  194,  195,  196,  197,   23,
    0,   24,    0,    4,   30,    0,    0,    0,  149,    0,
  148,    0,    0,   43,  120,   44,   45,    0,   47,    0,
  109,  110,    0,  122,  123,  124,  125,  139,  140,  126,
  141,  127,  114,    0,  116,  160,    0,    0,   10,    9,
    0,    0,   14,   20,    0,    0,   21,   38,   11,   12,
    0,    0,    0,    0,   63,   67,   64,   65,   66,   35,
   36,  107,  108,    0,    0,    0,   57,   58,   59,   60,
   61,   62,    0,   34,   37,  121,  143,  145,  147,    0,
    0,    0,    0,    0,    0,    0,    0,  142,  144,  146,
    0,    0,    0,    0,    0,    0,   31,  188,    0,    0,
    0,  184,   46,  202,    0,    0,    0,  198,   48,  164,
  163,  166,  167,  165,    0,    0,    0,    6,    5,    0,
    0,    0,    8,    7,    0,    0,    0,   25,    0,    0,
    0,   22,    0,    0,    0,    0,  132,  133,    0,  136,
  130,  137,  138,  131,   32,    0,    0,    0,    0,    0,
    0,  154,  155,    0,    0,    0,   39,  161,  169,  185,
  186,  189,    0,  199,  200,  203,    0,  111,    0,   16,
   15,   18,   13,    0,    0,   54,   56,   53,   55,    0,
  150,    0,  187,    0,  201,    0,   26,   27,  134,  135,
  128,    0,  190,  204,  151,
};
static const YYINT pcapdgoto[] = {                        1,
  174,  212,  129,  209,   84,   85,  210,   86,   87,  153,
  154,  155,   88,   89,  195,  117,   91,  171,  172,  121,
  122,  118,  132,    2,   94,   95,  156,   96,   97,   98,
  182,  183,  243,   99,  100,  188,  189,  247,  112,  110,
  219,  271,  221,  224,
};
static const YYINT pcapsindex[] = {                       0,
    0,  296, -268, -231, -223,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -216, -204, -174,
 -169, -284, -193,    0,    0,    0,    0,    0,    0,  -40,
  -40,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  396,    0, -334,    0,    0,  -19,  611,  647,    0,   34,
    0,  296,  296,    0,    0,    0,    0,   42,    0,  651,
    0,    0,   96,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -40,    0,    0,   34,  396,    0,    0,
  185,  185,    0,    0,  -38,   88,    0,    0,    0,    0,
  -19,  -19, -236, -233,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -258, -172, -251,    0,    0,    0,    0,
    0,    0,  -85,    0,    0,    0,    0,    0,    0,  396,
  396,  396,  396,  396,  396,  396,  396,    0,    0,    0,
  396,  396,  396,  -39,  113,  126,    0,    0, -122, -121,
 -118,    0,    0,    0,  -99,  -98,  -91,    0,    0,    0,
    0,    0,    0,    0,  -86,  126,  236,    0,    0,    0,
  185,  185,    0,    0, -140,  -84,  -81,    0,  165, -334,
  126,    0,  -42,  -36,  -34,  -31,    0,    0,  -82,    0,
    0,    0,    0,    0,    0,  170,  170,   -8,  108,  -23,
  -23,    0,    0,  236,  236,  156,    0,    0,    0,    0,
    0,    0,  -37,    0,    0,    0,  -35,    0,  126,    0,
    0,    0,    0,  -19,  -19,    0,    0,    0,    0, -221,
    0,  -70,    0, -118,    0,  -91,    0,    0,    0,    0,
    0,  134,    0,    0,    0,
};
static const YYINT pcaprindex[] = {                       0,
    0,  488,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    8,
   11,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  228,    0,    0,    0,    0,    0,    0,    1,
    0,  686,  686,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  686,  686,    0,    0,   16,   18,    0,    0,    0,    0,
    0,    0,  -28,  520,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  132,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  725,  759,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
  686,  686,    0,    0,    0,    0,    0,    0, -222,    0,
 -205,    0,    0,    0,    0,    0,    0,    0,   20,    0,
    0,    0,    0,    0,    0,   28,   53,   87,   78,   13,
   38,    0,    0,   26,   36,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  121,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static const YYINT pcapgindex[] = {                       0,
  227,  -14, -116,    0,    2,    0,    0,    0,    0,    0,
   79,    0,   24,  -75,    0,  116,  724,  -73,   12,   29,
 -163,  767,  722,    0,    0,    0,    0,    0,    0,    0,
 -171,    0,    0,    0,    0, -165,    0,    0,    0,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 1078
static const YYINT pcaptable[] = {                       82,
   40,  237,   12,  263,   52,  265,  108,  115,  206,  242,
  117,   52,  152,   80,  208,   17,  175,   19,  166,  129,
   82,  246,  101,  167,  179,   41,  185,  158,  109,  163,
  119,  120,  217,  166,  164,   42,  165,  153,  167,  222,
  148,   40,  175,   12,  148,  148,  255,  148,  115,  148,
  152,  117,  159,  152,  218,  152,   17,  152,   19,  102,
  129,  223,  148,  148,  148,  158,   41,  103,  158,  269,
  152,  128,  152,  152,  152,  153,   42,  156,  153,  264,
  153,   82,  153,  266,  208,  158,  157,  158,  158,  158,
  159,  270,  273,  159,  177,  153,  104,  153,  153,  153,
  274,  170,  169,  168,  116,  152,  198,  203,  105,  180,
  159,  186,  159,  159,  159,  156,  106,   90,  156,  111,
  158,  107,  199,  204,  173,  175,  148,  157,  213,  214,
  153,  215,  216,  191,  207,  156,  152,  156,  156,  156,
  220,  197,   29,   29,  157,  159,  157,  157,  157,  166,
  164,  158,  165,  238,  167,  194,  193,  192,  148,   28,
   28,  153,  148,  148,   33,  148,  239,  148,  240,  241,
  156,   33,  178,  135,  250,  137,  159,  138,  139,  157,
  148,  148,  148,  226,  227,  228,  229,  230,  231,  232,
  233,  244,  245,  163,  234,  235,  236,  166,  164,  184,
  165,  156,  167,  177,  248,  253,  251,   90,   90,  252,
  157,  166,  164,  262,  165,  256,  167,   80,  260,  190,
  272,  257,  258,  205,   82,  259,  275,    1,   83,   81,
   52,  225,   52,    0,   52,   52,   90,   90,  254,  267,
  268,    0,    0,    0,  148,    0,    0,    0,  261,    0,
   25,    0,    0,    0,    0,    0,    0,   49,   49,   49,
   49,   49,   52,   49,   49,    0,    0,   49,   49,    0,
    0,   25,    0,  163,    0,    0,    0,  166,  164,  162,
  165,    0,  167,    0,   52,   52,   52,   52,   52,   49,
   49,    0,    0,  123,  124,  125,  126,  127,    0,    0,
   49,   49,   49,   49,   49,   49,   49,   49,   49,  160,
  161,    0,  148,  148,  148,    0,   90,   90,    0,    0,
  148,  148,  152,  152,  152,  119,  120,  119,   80,  119,
  152,  152,  178,    0,    0,   82,    0,  158,  158,  158,
   81,    0,    0,    0,    0,  158,  158,  153,  153,  153,
    0,  157,  158,  159,    0,  153,  153,    0,    0,  162,
    0,    0,  159,  159,  159,   40,   40,   12,   12,    0,
  159,  159,  115,  115,    0,  117,  117,  152,  152,    0,
   17,   17,   19,   19,  129,  129,    0,  156,  156,  156,
   41,   41,  158,  158,    0,    0,  157,  157,  157,    0,
   42,   42,  153,  153,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  159,  159,    0,
    0,    0,   33,    0,    0,  160,  161,    0,    0,    0,
  148,  148,  148,    0,    0,   82,    0,    0,  148,  148,
   81,    0,  156,  156,   33,   33,   33,   33,   33,    3,
    4,  157,  157,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
   22,   23,   24,  160,  161,   25,   26,   27,   28,   29,
   30,   31,   32,   33,    0,   28,   28,    2,    0,    0,
    0,    0,    0,   34,    0,    0,    0,  123,  124,  125,
  126,  127,    0,    0,   35,   36,   37,   38,   39,   40,
   41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,
   61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
   71,   72,   73,   74,   75,   76,   77,   78,   79,    0,
    0,    0,   51,  160,  161,    0,    0,    0,    0,   51,
    3,    4,    0,    0,    5,    6,    7,    8,    9,   10,
   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
   21,   22,   23,   24,    0,    0,   25,   26,   27,   28,
   29,   30,   31,   32,   33,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   34,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   35,   36,   37,   38,   39,
   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,
   60,   61,   62,   63,   64,   65,   66,   67,   68,   69,
   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
    0,    0,    0,    0,    0,    6,    7,    8,    9,   10,
   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
   21,   22,   23,   24,  163,    0,   25,    0,  166,  164,
   82,  165,    0,  167,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   34,    0,  170,  169,  168,    0,
  170,  169,  168,    0,    0,   35,   36,   37,   38,   39,
    0,    0,    0,   93,   44,   45,   46,   47,   48,   49,
   50,   51,   52,   53,   54,   55,   56,   57,    0,    0,
    0,    0,    0,    0,   50,   50,   50,   50,   50,    0,
   50,   50,   72,    0,   50,   50,    0,    0,    0,    0,
    0,    0,  149,  113,  115,    0,  149,  149,   92,  149,
  162,  149,    0,    0,    0,    0,   50,   50,   51,    0,
   51,    0,   51,   51,  149,  149,  149,   50,   50,   50,
   50,   50,   50,   50,   50,   50,  148,    0,    0,    0,
  148,  148,    0,  148,    0,  148,  114,  114,    0,  130,
   51,    0,    0,   93,   93,  176,    0,    0,  148,  148,
  148,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   51,   51,   51,   51,   51,  196,    0,    0,
    0,  176,  202,  202,  200,  200,    0,    0,  149,    0,
    0,    0,  131,    0,  211,  130,    0,    0,   92,   92,
    0,    0,    0,    0,  181,    0,  187,  133,  134,  135,
  136,  137,    0,  138,  139,    0,    0,  140,  141,    0,
  114,    0,  148,    0,    0,    0,    0,  201,  201,    0,
    0,    0,    0,    0,    0,    0,    0,  114,  131,  142,
  143,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  144,  145,  146,  147,  148,  149,  150,  151,  152,    0,
    0,    0,  202,  202,  249,  200,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  184,   50,   50,   50,   50,   50,    0,   50,   50,
    0,    0,   50,   50,    0,    0,  157,  158,  159,    0,
  157,  158,  159,    0,  160,  161,    0,   92,  201,    0,
    0,    0,    0,    0,   50,   50,    0,  130,  130,    0,
    0,    0,    0,    0,    0,   50,   50,   50,   50,   50,
   50,   50,   50,   50,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  131,  131,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  149,  149,  149,    0,    0,    0,
    0,    0,  149,  149,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  148,  148,
  148,    0,    0,    0,    0,    0,  148,  148,
};
static const YYINT pcapcheck[] = {                       40,
    0,   41,    0,   41,   33,   41,  291,    0,   47,  181,
    0,   40,    0,   33,  131,    0,   92,    0,   42,    0,
   40,  187,  291,   47,   98,    0,  100,    0,  313,   38,
  365,  366,  291,   42,   43,    0,   45,    0,   47,  291,
   38,   41,  118,   41,   42,   43,  210,   45,   41,   47,
   38,   41,    0,   41,  313,   43,   41,   45,   41,  291,
   41,  313,   60,   61,   62,   38,   41,  291,   41,  291,
   58,   86,   60,   61,   62,   38,   41,    0,   41,  243,
   43,   40,   45,  247,  201,   58,    0,   60,   61,   62,
   38,  313,  264,   41,   93,   58,  313,   60,   61,   62,
  266,   60,   61,   62,   81,   93,  121,  122,  313,   98,
   58,  100,   60,   61,   62,   38,  291,    2,   41,  313,
   93,  291,  121,  122,   91,  201,  124,   41,  365,  366,
   93,  365,  366,   38,   47,   58,  124,   60,   61,   62,
  313,  118,  365,  366,   58,   93,   60,   61,   62,   42,
   43,  124,   45,   41,   47,   60,   61,   62,   38,  365,
  366,  124,   42,   43,   33,   45,   41,   47,  291,  291,
   93,   40,  291,  259,  315,  261,  124,  263,  264,   93,
   60,   61,   62,  160,  161,  162,  163,  164,  165,  166,
  167,  291,  291,   38,  171,  172,  173,   42,   43,  291,
   45,  124,   47,  202,  291,   41,  291,   92,   93,  291,
  124,   42,   43,   58,   45,  258,   47,   33,  301,  124,
  291,  258,  257,  262,   40,  257,   93,    0,    2,   45,
  259,  153,  261,   -1,  263,  264,  121,  122,  210,  254,
  255,   -1,   -1,   -1,  124,   -1,   -1,   -1,   93,   -1,
  291,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
  260,  261,  291,  263,  264,   -1,   -1,  267,  268,   -1,
   -1,  291,   -1,   38,   -1,   -1,   -1,   42,   43,  124,
   45,   -1,   47,   -1,  313,  314,  315,  316,  317,  289,
  290,   -1,   -1,  313,  314,  315,  316,  317,   -1,   -1,
  300,  301,  302,  303,  304,  305,  306,  307,  308,  318,
  319,   -1,  310,  311,  312,   -1,  201,  202,   -1,   -1,
  318,  319,  310,  311,  312,  365,  366,  365,   33,  365,
  318,  319,  291,   -1,   -1,   40,   -1,  310,  311,  312,
   45,   -1,   -1,   -1,   -1,  318,  319,  310,  311,  312,
   -1,  310,  311,  312,   -1,  318,  319,   -1,   -1,  124,
   -1,   -1,  310,  311,  312,  365,  366,  365,  366,   -1,
  318,  319,  365,  366,   -1,  365,  366,  365,  366,   -1,
  365,  366,  365,  366,  365,  366,   -1,  310,  311,  312,
  365,  366,  365,  366,   -1,   -1,  310,  311,  312,   -1,
  365,  366,  365,  366,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  365,  366,   -1,
   -1,   -1,  291,   -1,   -1,  318,  319,   -1,   -1,   -1,
  310,  311,  312,   -1,   -1,   40,   -1,   -1,  318,  319,
   45,   -1,  365,  366,  313,  314,  315,  316,  317,  265,
  266,  365,  366,  269,  270,  271,  272,  273,  274,  275,
  276,  277,  278,  279,  280,  281,  282,  283,  284,  285,
  286,  287,  288,  318,  319,  291,  292,  293,  294,  295,
  296,  297,  298,  299,   -1,  365,  366,    0,   -1,   -1,
   -1,   -1,   -1,  309,   -1,   -1,   -1,  313,  314,  315,
  316,  317,   -1,   -1,  320,  321,  322,  323,  324,  325,
  326,  327,  328,  329,  330,  331,  332,  333,  334,  335,
  336,  337,  338,  339,  340,  341,  342,  343,  344,  345,
  346,  347,  348,  349,  350,  351,  352,  353,  354,  355,
  356,  357,  358,  359,  360,  361,  362,  363,  364,   -1,
   -1,   -1,   33,  318,  319,   -1,   -1,   -1,   -1,   40,
  265,  266,   -1,   -1,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  287,  288,   -1,   -1,  291,  292,  293,  294,
  295,  296,  297,  298,  299,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  309,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  320,  321,  322,  323,  324,
  325,  326,  327,  328,  329,  330,  331,  332,  333,  334,
  335,  336,  337,  338,  339,  340,  341,  342,  343,  344,
  345,  346,  347,  348,  349,  350,  351,  352,  353,  354,
  355,  356,  357,  358,  359,  360,  361,  362,  363,  364,
   -1,   -1,   -1,   -1,   -1,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  287,  288,   38,   -1,  291,   -1,   42,   43,
   40,   45,   -1,   47,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  309,   -1,   60,   61,   62,   -1,
   60,   61,   62,   -1,   -1,  320,  321,  322,  323,  324,
   -1,   -1,   -1,    2,  329,  330,  331,  332,  333,  334,
  335,  336,  337,  338,  339,  340,  341,  342,   -1,   -1,
   -1,   -1,   -1,   -1,  257,  258,  259,  260,  261,   -1,
  263,  264,  357,   -1,  267,  268,   -1,   -1,   -1,   -1,
   -1,   -1,   38,   40,   41,   -1,   42,   43,    2,   45,
  124,   47,   -1,   -1,   -1,   -1,  289,  290,  259,   -1,
  261,   -1,  263,  264,   60,   61,   62,  300,  301,  302,
  303,  304,  305,  306,  307,  308,   38,   -1,   -1,   -1,
   42,   43,   -1,   45,   -1,   47,   40,   41,   -1,   86,
  291,   -1,   -1,   92,   93,   92,   -1,   -1,   60,   61,
   62,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  313,  314,  315,  316,  317,  114,   -1,   -1,
   -1,  118,  121,  122,  121,  122,   -1,   -1,  124,   -1,
   -1,   -1,   86,   -1,  131,  132,   -1,   -1,   92,   93,
   -1,   -1,   -1,   -1,   98,   -1,  100,  257,  258,  259,
  260,  261,   -1,  263,  264,   -1,   -1,  267,  268,   -1,
  114,   -1,  124,   -1,   -1,   -1,   -1,  121,  122,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  131,  132,  289,
  290,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  300,  301,  302,  303,  304,  305,  306,  307,  308,   -1,
   -1,   -1,  201,  202,  201,  202,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  291,  257,  258,  259,  260,  261,   -1,  263,  264,
   -1,   -1,  267,  268,   -1,   -1,  310,  311,  312,   -1,
  310,  311,  312,   -1,  318,  319,   -1,  201,  202,   -1,
   -1,   -1,   -1,   -1,  289,  290,   -1,  254,  255,   -1,
   -1,   -1,   -1,   -1,   -1,  300,  301,  302,  303,  304,
  305,  306,  307,  308,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  254,  255,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  310,  311,  312,   -1,   -1,   -1,
   -1,   -1,  318,  319,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  310,  311,
  312,   -1,   -1,   -1,   -1,   -1,  318,  319,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 367
#define YYUNDFTOKEN 414
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const pcapname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,0,"'&'",0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,
0,"':'",0,"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'|'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"DST","SRC","HOST","GATEWAY","NET","NETMASK",
"PORT","PORTRANGE","LESS","GREATER","PROTO","PROTOCHAIN","CBYTE","ARP","RARP",
"IP","SCTP","TCP","UDP","ICMP","IGMP","IGRP","PIM","VRRP","CARP","ATALK","AARP",
"DECNET","LAT","SCA","MOPRC","MOPDL","TK_BROADCAST","TK_MULTICAST","NUM",
"INBOUND","OUTBOUND","PF_IFNAME","PF_RSET","PF_RNR","PF_SRNR","PF_REASON",
"PF_ACTION","TYPE","SUBTYPE","DIR","ADDR1","ADDR2","ADDR3","ADDR4","RA","TA",
"LINK","GEQ","LEQ","NEQ","ID","EID","HID","HID6","AID","LSH","RSH","LEN","IPV6",
"ICMPV6","AH","ESP","VLAN","MPLS","PPPOED","PPPOES","ISO","ESIS","CLNP","ISIS",
"L1","L2","IIH","LSP","SNP","CSNP","PSNP","STP","IPX","NETBEUI","LANE","LLC",
"METAC","BCC","SC","ILMIC","OAMF4EC","OAMF4SC","OAM","OAMF4","CONNECTMSG",
"METACONNECT","VPI","VCI","RADIO","FISU","LSSU","MSU","SIO","OPC","DPC","SLS",
"OR","AND","UMINUS",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const pcaprule[] = {
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
"other : VLAN pnum",
"other : VLAN",
"other : MPLS pnum",
"other : MPLS",
"other : PPPOED",
"other : PPPOES",
"other : pfvar",
"other : pqual p80211",
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
"narth : arth '&' arth",
"narth : arth '|' arth",
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
"atmtype : LLC",
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
"mtp3field : SIO",
"mtp3field : OPC",
"mtp3field : DPC",
"mtp3field : SLS",
"mtp3value : mtp3fieldvalue",
"mtp3value : relop NUM",
"mtp3value : irelop NUM",
"mtp3value : paren mtp3listvalue ')'",
"mtp3fieldvalue : NUM",
"mtp3listvalue : mtp3fieldvalue",
"mtp3listvalue : mtp3listvalue or mtp3fieldvalue",

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
#line 317 "../../freebsd/contrib/libpcap/grammar.y"
	{
	finish_parse(yystack.l_mark[0].blk.b);
}
break;
case 3:
#line 322 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.q = qerr; }
break;
case 5:
#line 325 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 6:
#line 326 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 7:
#line 327 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 8:
#line 328 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 9:
#line 330 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
break;
case 10:
#line 332 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
break;
case 12:
#line 335 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yystack.l_mark[0].i,
						   yyval.blk.q = yystack.l_mark[-1].blk.q); }
break;
case 13:
#line 337 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
break;
case 14:
#line 339 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_scode(yystack.l_mark[0].s, yyval.blk.q = yystack.l_mark[-1].blk.q); }
break;
case 15:
#line 340 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_mcode(yystack.l_mark[-2].s, NULL, yystack.l_mark[0].i,
				    yyval.blk.q = yystack.l_mark[-3].blk.q); }
break;
case 16:
#line 342 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_mcode(yystack.l_mark[-2].s, yystack.l_mark[0].s, 0,
				    yyval.blk.q = yystack.l_mark[-3].blk.q); }
break;
case 17:
#line 344 "../../freebsd/contrib/libpcap/grammar.y"
	{
				  /* Decide how to parse HID based on proto */
				  yyval.blk.q = yystack.l_mark[-1].blk.q;
				  if (yyval.blk.q.addr == Q_PORT)
				  	bpf_error("'port' modifier applied to ip host");
				  else if (yyval.blk.q.addr == Q_PORTRANGE)
				  	bpf_error("'portrange' modifier applied to ip host");
				  else if (yyval.blk.q.addr == Q_PROTO)
				  	bpf_error("'proto' modifier applied to ip host");
				  else if (yyval.blk.q.addr == Q_PROTOCHAIN)
				  	bpf_error("'protochain' modifier applied to ip host");
				  yyval.blk.b = gen_ncode(yystack.l_mark[0].s, 0, yyval.blk.q);
				}
break;
case 18:
#line 357 "../../freebsd/contrib/libpcap/grammar.y"
	{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yystack.l_mark[-2].s, NULL, yystack.l_mark[0].i,
				    yyval.blk.q = yystack.l_mark[-3].blk.q);
#else
				  bpf_error("'ip6addr/prefixlen' not supported "
					"in this configuration");
#endif /*INET6*/
				}
break;
case 19:
#line 366 "../../freebsd/contrib/libpcap/grammar.y"
	{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yystack.l_mark[0].s, 0, 128,
				    yyval.blk.q = yystack.l_mark[-1].blk.q);
#else
				  bpf_error("'ip6addr' not supported "
					"in this configuration");
#endif /*INET6*/
				}
break;
case 20:
#line 375 "../../freebsd/contrib/libpcap/grammar.y"
	{ 
				  yyval.blk.b = gen_ecode(yystack.l_mark[0].e, yyval.blk.q = yystack.l_mark[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yystack.l_mark[0].e);
				}
break;
case 21:
#line 384 "../../freebsd/contrib/libpcap/grammar.y"
	{
				  yyval.blk.b = gen_acode(yystack.l_mark[0].e, yyval.blk.q = yystack.l_mark[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yystack.l_mark[0].e);
				}
break;
case 22:
#line 393 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_not(yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 23:
#line 395 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
break;
case 24:
#line 397 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[-1].blk; }
break;
case 26:
#line 400 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_and(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 27:
#line 401 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 28:
#line 403 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yystack.l_mark[0].i,
						   yyval.blk.q = yystack.l_mark[-1].blk.q); }
break;
case 31:
#line 408 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_not(yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 32:
#line 410 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-2].i, yystack.l_mark[-1].i, yystack.l_mark[0].i); }
break;
case 33:
#line 411 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, yystack.l_mark[0].i, Q_DEFAULT); }
break;
case 34:
#line 412 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, yystack.l_mark[0].i); }
break;
case 35:
#line 413 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, Q_PROTO); }
break;
case 36:
#line 414 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, Q_PROTOCHAIN); }
break;
case 37:
#line 415 "../../freebsd/contrib/libpcap/grammar.y"
	{ QSET(yyval.blk.q, yystack.l_mark[-1].i, Q_DEFAULT, yystack.l_mark[0].i); }
break;
case 38:
#line 417 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk = yystack.l_mark[0].blk; }
break;
case 39:
#line 418 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = yystack.l_mark[-2].blk.q; }
break;
case 40:
#line 419 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_proto_abbrev(yystack.l_mark[0].i); yyval.blk.q = qerr; }
break;
case 41:
#line 420 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_relation(yystack.l_mark[-1].i, yystack.l_mark[-2].a, yystack.l_mark[0].a, 0);
				  yyval.blk.q = qerr; }
break;
case 42:
#line 422 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_relation(yystack.l_mark[-1].i, yystack.l_mark[-2].a, yystack.l_mark[0].a, 1);
				  yyval.blk.q = qerr; }
break;
case 43:
#line 424 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].rblk; yyval.blk.q = qerr; }
break;
case 44:
#line 425 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_atmtype_abbrev(yystack.l_mark[0].i); yyval.blk.q = qerr; }
break;
case 45:
#line 426 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_atmmulti_abbrev(yystack.l_mark[0].i); yyval.blk.q = qerr; }
break;
case 46:
#line 427 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].blk.b; yyval.blk.q = qerr; }
break;
case 47:
#line 428 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_mtp2type_abbrev(yystack.l_mark[0].i); yyval.blk.q = qerr; }
break;
case 48:
#line 429 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[0].blk.b; yyval.blk.q = qerr; }
break;
case 50:
#line 433 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_DEFAULT; }
break;
case 51:
#line 436 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_SRC; }
break;
case 52:
#line 437 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_DST; }
break;
case 53:
#line 438 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_OR; }
break;
case 54:
#line 439 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_OR; }
break;
case 55:
#line 440 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_AND; }
break;
case 56:
#line 441 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_AND; }
break;
case 57:
#line 442 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ADDR1; }
break;
case 58:
#line 443 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ADDR2; }
break;
case 59:
#line 444 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ADDR3; }
break;
case 60:
#line 445 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ADDR4; }
break;
case 61:
#line 446 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_RA; }
break;
case 62:
#line 447 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_TA; }
break;
case 63:
#line 450 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_HOST; }
break;
case 64:
#line 451 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_NET; }
break;
case 65:
#line 452 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_PORT; }
break;
case 66:
#line 453 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_PORTRANGE; }
break;
case 67:
#line 456 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_GATEWAY; }
break;
case 68:
#line 458 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_LINK; }
break;
case 69:
#line 459 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_IP; }
break;
case 70:
#line 460 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ARP; }
break;
case 71:
#line 461 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_RARP; }
break;
case 72:
#line 462 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_SCTP; }
break;
case 73:
#line 463 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_TCP; }
break;
case 74:
#line 464 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_UDP; }
break;
case 75:
#line 465 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ICMP; }
break;
case 76:
#line 466 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_IGMP; }
break;
case 77:
#line 467 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_IGRP; }
break;
case 78:
#line 468 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_PIM; }
break;
case 79:
#line 469 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_VRRP; }
break;
case 80:
#line 470 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_CARP; }
break;
case 81:
#line 471 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ATALK; }
break;
case 82:
#line 472 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_AARP; }
break;
case 83:
#line 473 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_DECNET; }
break;
case 84:
#line 474 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_LAT; }
break;
case 85:
#line 475 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_SCA; }
break;
case 86:
#line 476 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_MOPDL; }
break;
case 87:
#line 477 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_MOPRC; }
break;
case 88:
#line 478 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_IPV6; }
break;
case 89:
#line 479 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ICMPV6; }
break;
case 90:
#line 480 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_AH; }
break;
case 91:
#line 481 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ESP; }
break;
case 92:
#line 482 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISO; }
break;
case 93:
#line 483 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ESIS; }
break;
case 94:
#line 484 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS; }
break;
case 95:
#line 485 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_L1; }
break;
case 96:
#line 486 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_L2; }
break;
case 97:
#line 487 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_IIH; }
break;
case 98:
#line 488 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_LSP; }
break;
case 99:
#line 489 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_SNP; }
break;
case 100:
#line 490 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_PSNP; }
break;
case 101:
#line 491 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_ISIS_CSNP; }
break;
case 102:
#line 492 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_CLNP; }
break;
case 103:
#line 493 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_STP; }
break;
case 104:
#line 494 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_IPX; }
break;
case 105:
#line 495 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_NETBEUI; }
break;
case 106:
#line 496 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = Q_RADIO; }
break;
case 107:
#line 498 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_broadcast(yystack.l_mark[-1].i); }
break;
case 108:
#line 499 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_multicast(yystack.l_mark[-1].i); }
break;
case 109:
#line 500 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_less(yystack.l_mark[0].i); }
break;
case 110:
#line 501 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_greater(yystack.l_mark[0].i); }
break;
case 111:
#line 502 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_byteop(yystack.l_mark[-1].i, yystack.l_mark[-2].i, yystack.l_mark[0].i); }
break;
case 112:
#line 503 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_inbound(0); }
break;
case 113:
#line 504 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_inbound(1); }
break;
case 114:
#line 505 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_vlan(yystack.l_mark[0].i); }
break;
case 115:
#line 506 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_vlan(-1); }
break;
case 116:
#line 507 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_mpls(yystack.l_mark[0].i); }
break;
case 117:
#line 508 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_mpls(-1); }
break;
case 118:
#line 509 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pppoed(); }
break;
case 119:
#line 510 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pppoes(); }
break;
case 120:
#line 511 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = yystack.l_mark[0].rblk; }
break;
case 121:
#line 512 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = yystack.l_mark[0].rblk; }
break;
case 122:
#line 515 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_ifname(yystack.l_mark[0].s); }
break;
case 123:
#line 516 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_ruleset(yystack.l_mark[0].s); }
break;
case 124:
#line 517 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_rnr(yystack.l_mark[0].i); }
break;
case 125:
#line 518 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_srnr(yystack.l_mark[0].i); }
break;
case 126:
#line 519 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_reason(yystack.l_mark[0].i); }
break;
case 127:
#line 520 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_pf_action(yystack.l_mark[0].i); }
break;
case 128:
#line 524 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_p80211_type(yystack.l_mark[-2].i | yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK |
					IEEE80211_FC0_SUBTYPE_MASK);
				}
break;
case 129:
#line 528 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_p80211_type(yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK);
				}
break;
case 130:
#line 531 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_p80211_type(yystack.l_mark[0].i,
					IEEE80211_FC0_TYPE_MASK |
					IEEE80211_FC0_SUBTYPE_MASK);
				}
break;
case 131:
#line 535 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.rblk = gen_p80211_fcdir(yystack.l_mark[0].i); }
break;
case 133:
#line 539 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = str2tok(yystack.l_mark[0].s, ieee80211_types);
				  if (yyval.i == -1)
				  	bpf_error("unknown 802.11 type name");
				}
break;
case 135:
#line 546 "../../freebsd/contrib/libpcap/grammar.y"
	{ const struct tok *types = NULL;
				  int i;
				  for (i = 0;; i++) {
				  	if (ieee80211_type_subtypes[i].tok == NULL) {
				  		/* Ran out of types */
						bpf_error("unknown 802.11 type");
						break;
					}
					if (yystack.l_mark[-2].i == ieee80211_type_subtypes[i].type) {
						types = ieee80211_type_subtypes[i].tok;
						break;
					}
				  }

				  yyval.i = str2tok(yystack.l_mark[0].s, types);
				  if (yyval.i == -1)
					bpf_error("unknown 802.11 subtype name");
				}
break;
case 136:
#line 566 "../../freebsd/contrib/libpcap/grammar.y"
	{ int i;
				  for (i = 0;; i++) {
				  	if (ieee80211_type_subtypes[i].tok == NULL) {
				  		/* Ran out of types */
						bpf_error("unknown 802.11 type name");
						break;
					}
					yyval.i = str2tok(yystack.l_mark[0].s, ieee80211_type_subtypes[i].tok);
					if (yyval.i != -1) {
						yyval.i |= ieee80211_type_subtypes[i].type;
						break;
					}
				  }
				}
break;
case 138:
#line 583 "../../freebsd/contrib/libpcap/grammar.y"
	{ if (pcap_strcasecmp(yystack.l_mark[0].s, "nods") == 0)
					yyval.i = IEEE80211_FC1_DIR_NODS;
				  else if (pcap_strcasecmp(yystack.l_mark[0].s, "tods") == 0)
					yyval.i = IEEE80211_FC1_DIR_TODS;
				  else if (pcap_strcasecmp(yystack.l_mark[0].s, "fromds") == 0)
					yyval.i = IEEE80211_FC1_DIR_FROMDS;
				  else if (pcap_strcasecmp(yystack.l_mark[0].s, "dstods") == 0)
					yyval.i = IEEE80211_FC1_DIR_DSTODS;
				  else
					bpf_error("unknown 802.11 direction");
				}
break;
case 139:
#line 596 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = yystack.l_mark[0].i; }
break;
case 140:
#line 597 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = pfreason_to_num(yystack.l_mark[0].s); }
break;
case 141:
#line 600 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = pfaction_to_num(yystack.l_mark[0].s); }
break;
case 142:
#line 603 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JGT; }
break;
case 143:
#line 604 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JGE; }
break;
case 144:
#line 605 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JEQ; }
break;
case 145:
#line 607 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JGT; }
break;
case 146:
#line 608 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JGE; }
break;
case 147:
#line 609 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = BPF_JEQ; }
break;
case 148:
#line 611 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_loadi(yystack.l_mark[0].i); }
break;
case 150:
#line 614 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_load(yystack.l_mark[-3].i, yystack.l_mark[-1].a, 1); }
break;
case 151:
#line 615 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_load(yystack.l_mark[-5].i, yystack.l_mark[-3].a, yystack.l_mark[-1].i); }
break;
case 152:
#line 616 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_ADD, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 153:
#line 617 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_SUB, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 154:
#line 618 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_MUL, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 155:
#line 619 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_DIV, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 156:
#line 620 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_AND, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 157:
#line 621 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_OR, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 158:
#line 622 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_LSH, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 159:
#line 623 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_arth(BPF_RSH, yystack.l_mark[-2].a, yystack.l_mark[0].a); }
break;
case 160:
#line 624 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_neg(yystack.l_mark[0].a); }
break;
case 161:
#line 625 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = yystack.l_mark[-1].a; }
break;
case 162:
#line 626 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.a = gen_loadlen(); }
break;
case 163:
#line 628 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = '&'; }
break;
case 164:
#line 629 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = '|'; }
break;
case 165:
#line 630 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = '<'; }
break;
case 166:
#line 631 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = '>'; }
break;
case 167:
#line 632 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = '='; }
break;
case 169:
#line 635 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = yystack.l_mark[-1].i; }
break;
case 170:
#line 637 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_LANE; }
break;
case 171:
#line 638 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_LLC; }
break;
case 172:
#line 639 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_METAC;	}
break;
case 173:
#line 640 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_BCC; }
break;
case 174:
#line 641 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_OAMF4EC; }
break;
case 175:
#line 642 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_OAMF4SC; }
break;
case 176:
#line 643 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_SC; }
break;
case 177:
#line 644 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_ILMIC; }
break;
case 178:
#line 646 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_OAM; }
break;
case 179:
#line 647 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_OAMF4; }
break;
case 180:
#line 648 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_CONNECTMSG; }
break;
case 181:
#line 649 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = A_METACONNECT; }
break;
case 182:
#line 652 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.atmfieldtype = A_VPI; }
break;
case 183:
#line 653 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.atmfieldtype = A_VCI; }
break;
case 185:
#line 656 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_atmfield_code(yystack.l_mark[-2].blk.atmfieldtype, (bpf_int32)yystack.l_mark[0].i, (bpf_u_int32)yystack.l_mark[-1].i, 0); }
break;
case 186:
#line 657 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_atmfield_code(yystack.l_mark[-2].blk.atmfieldtype, (bpf_int32)yystack.l_mark[0].i, (bpf_u_int32)yystack.l_mark[-1].i, 1); }
break;
case 187:
#line 658 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = qerr; }
break;
case 188:
#line 660 "../../freebsd/contrib/libpcap/grammar.y"
	{
	yyval.blk.atmfieldtype = yystack.l_mark[-1].blk.atmfieldtype;
	if (yyval.blk.atmfieldtype == A_VPI ||
	    yyval.blk.atmfieldtype == A_VCI)
		yyval.blk.b = gen_atmfield_code(yyval.blk.atmfieldtype, (bpf_int32) yystack.l_mark[0].i, BPF_JEQ, 0);
	}
break;
case 190:
#line 668 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
case 191:
#line 671 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = M_FISU; }
break;
case 192:
#line 672 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = M_LSSU; }
break;
case 193:
#line 673 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.i = M_MSU; }
break;
case 194:
#line 676 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.mtp3fieldtype = M_SIO; }
break;
case 195:
#line 677 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.mtp3fieldtype = M_OPC; }
break;
case 196:
#line 678 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.mtp3fieldtype = M_DPC; }
break;
case 197:
#line 679 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.mtp3fieldtype = M_SLS; }
break;
case 199:
#line 682 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_mtp3field_code(yystack.l_mark[-2].blk.mtp3fieldtype, (u_int)yystack.l_mark[0].i, (u_int)yystack.l_mark[-1].i, 0); }
break;
case 200:
#line 683 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = gen_mtp3field_code(yystack.l_mark[-2].blk.mtp3fieldtype, (u_int)yystack.l_mark[0].i, (u_int)yystack.l_mark[-1].i, 1); }
break;
case 201:
#line 684 "../../freebsd/contrib/libpcap/grammar.y"
	{ yyval.blk.b = yystack.l_mark[-1].blk.b; yyval.blk.q = qerr; }
break;
case 202:
#line 686 "../../freebsd/contrib/libpcap/grammar.y"
	{
	yyval.blk.mtp3fieldtype = yystack.l_mark[-1].blk.mtp3fieldtype;
	if (yyval.blk.mtp3fieldtype == M_SIO ||
	    yyval.blk.mtp3fieldtype == M_OPC ||
	    yyval.blk.mtp3fieldtype == M_DPC ||
	    yyval.blk.mtp3fieldtype == M_SLS )
		yyval.blk.b = gen_mtp3field_code(yyval.blk.mtp3fieldtype, (u_int) yystack.l_mark[0].i, BPF_JEQ, 0);
	}
break;
case 204:
#line 696 "../../freebsd/contrib/libpcap/grammar.y"
	{ gen_or(yystack.l_mark[-2].blk.b, yystack.l_mark[0].blk.b); yyval.blk = yystack.l_mark[0].blk; }
break;
#line 2222 "pcap.tab.c"
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
