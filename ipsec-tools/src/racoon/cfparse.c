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
#define yyparse    racoonyyparse
#endif /* yyparse */

#ifndef yylex
#define yylex      racoonyylex
#endif /* yylex */

#ifndef yyerror
#define yyerror    racoonyyerror
#endif /* yyerror */

#ifndef yychar
#define yychar     racoonyychar
#endif /* yychar */

#ifndef yyval
#define yyval      racoonyyval
#endif /* yyval */

#ifndef yylval
#define yylval     racoonyylval
#endif /* yylval */

#ifndef yydebug
#define yydebug    racoonyydebug
#endif /* yydebug */

#ifndef yynerrs
#define yynerrs    racoonyynerrs
#endif /* yynerrs */

#ifndef yyerrflag
#define yyerrflag  racoonyyerrflag
#endif /* yyerrflag */

#ifndef yylhs
#define yylhs      racoonyylhs
#endif /* yylhs */

#ifndef yylen
#define yylen      racoonyylen
#endif /* yylen */

#ifndef yydefred
#define yydefred   racoonyydefred
#endif /* yydefred */

#ifndef yydgoto
#define yydgoto    racoonyydgoto
#endif /* yydgoto */

#ifndef yysindex
#define yysindex   racoonyysindex
#endif /* yysindex */

#ifndef yyrindex
#define yyrindex   racoonyyrindex
#endif /* yyrindex */

#ifndef yygindex
#define yygindex   racoonyygindex
#endif /* yygindex */

#ifndef yytable
#define yytable    racoonyytable
#endif /* yytable */

#ifndef yycheck
#define yycheck    racoonyycheck
#endif /* yycheck */

#ifndef yyname
#define yyname     racoonyyname
#endif /* yyname */

#ifndef yyrule
#define yyrule     racoonyyrule
#endif /* yyrule */
#define YYPREFIX "racoonyy"

#define YYPURE 0

#line 6 "../../ipsec-tools/src/racoon/cfparse.y"
/*
 * Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 and 2003 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include PATH_IPSEC_H

#ifdef ENABLE_HYBRID
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <grp.h>

#include "var.h"
#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "sockmisc.h"
#include "str2val.h"
#include "genlist.h"
#include "debug.h"

#include "admin.h"
#include "privsep.h"
#include "cfparse_proto.h"
#include "cftoken_proto.h"
#include "algorithm.h"
#include "localconf.h"
#include "policy.h"
#include "sainfo.h"
#include "oakley.h"
#include "pfkey.h"
#include "remoteconf.h"
#include "grabmyaddr.h"
#include "isakmp_var.h"
#include "handler.h"
#include "isakmp.h"
#include "nattraversal.h"
#include "isakmp_frag.h"
#ifdef ENABLE_HYBRID
#include "resolv.h"
#include "isakmp_unity.h"
#include "isakmp_xauth.h"
#include "isakmp_cfg.h"
#endif
#include "ipsec_doi.h"
#include "strnames.h"
#include "gcmalloc.h"
#ifdef HAVE_GSSAPI
#include "gssapi.h"
#endif
#include "vendorid.h"
#include "rsalist.h"
#include "crypto_openssl.h"

struct secprotospec {
	int prop_no;
	int trns_no;
	int strength;		/* for isakmp/ipsec */
	int encklen;		/* for isakmp/ipsec */
	time_t lifetime;	/* for isakmp */
	int lifebyte;		/* for isakmp */
	int proto_id;		/* for ipsec (isakmp?) */
	int ipsec_level;	/* for ipsec */
	int encmode;		/* for ipsec */
	int vendorid;		/* for isakmp */
	char *gssid;
	struct sockaddr *remote;
	int algclass[MAXALGCLASS];

	struct secprotospec *next;	/* the tail is the most prefiered. */
	struct secprotospec *prev;
};

static int num2dhgroup[] = {
	0,
	OAKLEY_ATTR_GRP_DESC_MODP768,
	OAKLEY_ATTR_GRP_DESC_MODP1024,
	OAKLEY_ATTR_GRP_DESC_EC2N155,
	OAKLEY_ATTR_GRP_DESC_EC2N185,
	OAKLEY_ATTR_GRP_DESC_MODP1536,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	OAKLEY_ATTR_GRP_DESC_MODP2048,
	OAKLEY_ATTR_GRP_DESC_MODP3072,
	OAKLEY_ATTR_GRP_DESC_MODP4096,
	OAKLEY_ATTR_GRP_DESC_MODP6144,
	OAKLEY_ATTR_GRP_DESC_MODP8192
};

static struct remoteconf *cur_rmconf;
static int tmpalgtype[MAXALGCLASS];
static struct sainfo *cur_sainfo;
static int cur_algclass;
static int oldloglevel = LLV_BASE;

static struct secprotospec *newspspec __P((void));
static void insspspec __P((struct remoteconf *, struct secprotospec *));
void dupspspec_list __P((struct remoteconf *dst, struct remoteconf *src));
void flushspspec __P((struct remoteconf *));
static void adminsock_conf __P((vchar_t *, vchar_t *, vchar_t *, int));

static int set_isakmp_proposal __P((struct remoteconf *));
static void clean_tmpalgtype __P((void));
static int expand_isakmpspec __P((int, int, int *,
	int, int, time_t, int, int, int, char *, struct remoteconf *));

void freeetypes (struct etypes **etypes);

static int load_x509(const char *file, char **filenameptr,
		     vchar_t **certptr)
{
	char path[PATH_MAX];

	getpathname(path, sizeof(path), LC_PATHTYPE_CERT, file);
	*certptr = eay_get_x509cert(path);
	if (*certptr == NULL)
		return -1;

	*filenameptr = racoon_strdup(file);
	STRDUP_FATAL(*filenameptr);

	return 0;
}

static int process_rmconf()
{

	/* check a exchange mode */
	if (cur_rmconf->etypes == NULL) {
		yyerror("no exchange mode specified.\n");
		return -1;
	}

	if (cur_rmconf->idvtype == IDTYPE_UNDEFINED)
		cur_rmconf->idvtype = IDTYPE_ADDRESS;

	if (cur_rmconf->idvtype == IDTYPE_ASN1DN) {
		if (cur_rmconf->mycertfile) {
			if (cur_rmconf->idv)
				yywarn("Both CERT and ASN1 ID "
				       "are set. Hope this is OK.\n");
			/* TODO: Preparse the DN here */
		} else if (cur_rmconf->idv) {
			/* OK, using asn1dn without X.509. */
		} else {
			yyerror("ASN1 ID not specified "
				"and no CERT defined!\n");
			return -1;
		}
	}

	if (duprmconf_finish(cur_rmconf))
		return -1;

	if (set_isakmp_proposal(cur_rmconf) != 0)
		return -1;

	/* DH group settting if aggressive mode is there. */
	if (check_etypeok(cur_rmconf, (void*) ISAKMP_ETYPE_AGG)) {
		struct isakmpsa *p;
		int b = 0;

		/* DH group */
		for (p = cur_rmconf->proposal; p; p = p->next) {
			if (b == 0 || (b && b == p->dh_group)) {
				b = p->dh_group;
				continue;
			}
			yyerror("DH group must be equal "
				"in all proposals "
				"when aggressive mode is "
				"used.\n");
			return -1;
		}
		cur_rmconf->dh_group = b;

		if (cur_rmconf->dh_group == 0) {
			yyerror("DH group must be set in the proposal.\n");
			return -1;
		}

		/* DH group settting if PFS is required. */
		if (oakley_setdhgroup(cur_rmconf->dh_group,
				&cur_rmconf->dhgrp) < 0) {
			yyerror("failed to set DH value.\n");
			return -1;
		}
	}

	insrmconf(cur_rmconf);

	return 0;
}

#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 247 "../../ipsec-tools/src/racoon/cfparse.y"
typedef union {
	unsigned long num;
	vchar_t *val;
	struct remoteconf *rmconf;
	struct sockaddr *saddr;
	struct sainfoalg *alg;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 356 "racoonyy.tab.c"

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

#define PRIVSEP 257
#define USER 258
#define GROUP 259
#define CHROOT 260
#define PATH 261
#define PATHTYPE 262
#define INCLUDE 263
#define PFKEY_BUFFER 264
#define LOGGING 265
#define LOGLEV 266
#define PADDING 267
#define PAD_RANDOMIZE 268
#define PAD_RANDOMIZELEN 269
#define PAD_MAXLEN 270
#define PAD_STRICT 271
#define PAD_EXCLTAIL 272
#define LISTEN 273
#define X_ISAKMP 274
#define X_ISAKMP_NATT 275
#define X_ADMIN 276
#define STRICT_ADDRESS 277
#define ADMINSOCK 278
#define DISABLED 279
#define LDAPCFG 280
#define LDAP_HOST 281
#define LDAP_PORT 282
#define LDAP_PVER 283
#define LDAP_BASE 284
#define LDAP_BIND_DN 285
#define LDAP_BIND_PW 286
#define LDAP_SUBTREE 287
#define LDAP_ATTR_USER 288
#define LDAP_ATTR_ADDR 289
#define LDAP_ATTR_MASK 290
#define LDAP_ATTR_GROUP 291
#define LDAP_ATTR_MEMBER 292
#define RADCFG 293
#define RAD_AUTH 294
#define RAD_ACCT 295
#define RAD_TIMEOUT 296
#define RAD_RETRIES 297
#define MODECFG 298
#define CFG_NET4 299
#define CFG_MASK4 300
#define CFG_DNS4 301
#define CFG_NBNS4 302
#define CFG_DEFAULT_DOMAIN 303
#define CFG_AUTH_SOURCE 304
#define CFG_AUTH_GROUPS 305
#define CFG_SYSTEM 306
#define CFG_RADIUS 307
#define CFG_PAM 308
#define CFG_LDAP 309
#define CFG_LOCAL 310
#define CFG_NONE 311
#define CFG_GROUP_SOURCE 312
#define CFG_ACCOUNTING 313
#define CFG_CONF_SOURCE 314
#define CFG_MOTD 315
#define CFG_POOL_SIZE 316
#define CFG_AUTH_THROTTLE 317
#define CFG_SPLIT_NETWORK 318
#define CFG_SPLIT_LOCAL 319
#define CFG_SPLIT_INCLUDE 320
#define CFG_SPLIT_DNS 321
#define CFG_PFS_GROUP 322
#define CFG_SAVE_PASSWD 323
#define RETRY 324
#define RETRY_COUNTER 325
#define RETRY_INTERVAL 326
#define RETRY_PERSEND 327
#define RETRY_PHASE1 328
#define RETRY_PHASE2 329
#define NATT_KA 330
#define ALGORITHM_CLASS 331
#define ALGORITHMTYPE 332
#define STRENGTHTYPE 333
#define SAINFO 334
#define FROM 335
#define REMOTE 336
#define ANONYMOUS 337
#define CLIENTADDR 338
#define INHERIT 339
#define REMOTE_ADDRESS 340
#define EXCHANGE_MODE 341
#define EXCHANGETYPE 342
#define DOI 343
#define DOITYPE 344
#define SITUATION 345
#define SITUATIONTYPE 346
#define CERTIFICATE_TYPE 347
#define CERTTYPE 348
#define PEERS_CERTFILE 349
#define CA_TYPE 350
#define VERIFY_CERT 351
#define SEND_CERT 352
#define SEND_CR 353
#define MATCH_EMPTY_CR 354
#define IDENTIFIERTYPE 355
#define IDENTIFIERQUAL 356
#define MY_IDENTIFIER 357
#define PEERS_IDENTIFIER 358
#define VERIFY_IDENTIFIER 359
#define DNSSEC 360
#define CERT_X509 361
#define CERT_PLAINRSA 362
#define NONCE_SIZE 363
#define DH_GROUP 364
#define KEEPALIVE 365
#define PASSIVE 366
#define INITIAL_CONTACT 367
#define NAT_TRAVERSAL 368
#define REMOTE_FORCE_LEVEL 369
#define PROPOSAL_CHECK 370
#define PROPOSAL_CHECK_LEVEL 371
#define GENERATE_POLICY 372
#define GENERATE_LEVEL 373
#define SUPPORT_PROXY 374
#define PROPOSAL 375
#define EXEC_PATH 376
#define EXEC_COMMAND 377
#define EXEC_SUCCESS 378
#define EXEC_FAILURE 379
#define GSS_ID 380
#define GSS_ID_ENC 381
#define GSS_ID_ENCTYPE 382
#define COMPLEX_BUNDLE 383
#define DPD 384
#define DPD_DELAY 385
#define DPD_RETRY 386
#define DPD_MAXFAIL 387
#define PH1ID 388
#define XAUTH_LOGIN 389
#define WEAK_PHASE1_CHECK 390
#define REKEY 391
#define PREFIX 392
#define PORT 393
#define PORTANY 394
#define UL_PROTO 395
#define ANY 396
#define IKE_FRAG 397
#define ESP_FRAG 398
#define MODE_CFG 399
#define PFS_GROUP 400
#define LIFETIME 401
#define LIFETYPE_TIME 402
#define LIFETYPE_BYTE 403
#define STRENGTH 404
#define REMOTEID 405
#define SCRIPT 406
#define PHASE1_UP 407
#define PHASE1_DOWN 408
#define PHASE1_DEAD 409
#define NUMBER 410
#define SWITCH 411
#define BOOLEAN 412
#define HEXSTRING 413
#define QUOTEDSTRING 414
#define ADDRSTRING 415
#define ADDRRANGE 416
#define UNITTYPE_BYTE 417
#define UNITTYPE_KBYTES 418
#define UNITTYPE_MBYTES 419
#define UNITTYPE_TBYTES 420
#define UNITTYPE_SEC 421
#define UNITTYPE_MIN 422
#define UNITTYPE_HOUR 423
#define EOS 424
#define BOC 425
#define EOC 426
#define COMMA 427
#define YYERRCODE 256
typedef int YYINT;
static const YYINT racoonyylhs[] = {                     -1,
    0,    0,   14,   14,   14,   14,   14,   14,   14,   14,
   14,   14,   14,   14,   14,   14,   14,   15,   30,   30,
   32,   31,   33,   31,   34,   31,   35,   31,   36,   31,
   37,   16,   38,   29,   17,   18,   19,   20,   39,   21,
   40,   40,   42,   41,   43,   41,   44,   41,   45,   41,
   46,   41,   22,   47,   47,   49,   48,   50,   48,   51,
   48,   52,   48,   53,   48,   54,   48,   12,    5,    5,
   55,   24,   56,   56,   58,   57,   59,   57,   60,   57,
   61,   57,   62,   57,   63,   57,   64,   23,   65,   65,
   67,   66,   68,   66,   69,   66,   70,   66,   71,   66,
   72,   66,   73,   66,   74,   66,   75,   66,   76,   66,
   77,   66,   78,   66,   25,   79,   79,   81,   80,   82,
   80,   80,   80,   86,   80,   87,   80,   89,   80,   90,
   80,   91,   80,   92,   80,   93,   80,   94,   80,   96,
   80,   97,   80,   98,   80,   99,   80,  100,   80,  101,
   80,  102,   80,  103,   80,  104,   80,  105,   80,  106,
   80,  107,   80,  108,   80,  109,   80,  110,   80,   83,
   83,  111,   84,   84,  112,   85,   85,  113,   95,   95,
  114,   88,   88,  115,   26,  116,  116,  118,  117,  119,
  117,  120,  117,  121,  117,  122,  117,  123,  117,  125,
  128,   27,  124,  124,  124,  124,  124,  124,    9,    9,
    9,  126,  126,  126,  127,  127,  130,  129,  131,  129,
  132,  129,  133,  129,  135,  129,  134,  136,  134,   13,
    3,    3,    4,    4,    4,    6,    6,    6,    1,    1,
  138,   28,  140,   28,  141,   28,  142,   28,  137,  137,
  139,   11,   11,  143,  143,  145,  144,  147,  144,  148,
  144,  149,  144,  144,  151,  144,  152,  144,  153,  144,
  154,  144,  155,  144,  156,  144,  157,  144,  158,  144,
  159,  144,  160,  144,  161,  144,  162,  144,  163,  144,
  164,  144,  165,  144,  166,  144,  167,  144,  168,  144,
  169,  144,  170,  144,  171,  144,  172,  144,  173,  144,
  174,  144,  175,  144,  176,  144,  177,  144,  178,  144,
  179,  144,  180,  144,  181,  144,  182,  144,  183,  144,
  184,  144,  185,  144,  186,  144,  187,  144,  188,  144,
  189,  144,  190,  144,  191,  144,  192,  144,  193,  144,
  146,  146,  195,  150,  196,  150,    2,    2,   10,   10,
   10,  194,  194,  198,  197,  199,  197,  200,  197,  201,
  197,  202,  197,    7,    7,    7,    8,    8,    8,    8,
};
static const YYINT racoonyylen[] = {                      2,
    0,    2,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    4,    0,    2,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    5,    0,    4,    3,    3,    3,    3,    1,    4,
    0,    2,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    4,    0,    2,    0,    4,    0,    4,    0,
    7,    0,    4,    0,    4,    0,    3,    2,    0,    1,
    0,    5,    0,    2,    0,    5,    0,    6,    0,    5,
    0,    6,    0,    4,    0,    4,    0,    5,    0,    2,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    4,    0,    2,    0,    4,    0,
    4,    3,    3,    0,    5,    0,    5,    0,    4,    0,
    4,    0,    4,    0,    4,    0,    4,    0,    4,    0,
    4,    0,    4,    0,    4,    0,    4,    0,    4,    0,
    4,    0,    4,    0,    4,    0,    4,    0,    4,    0,
    4,    0,    4,    0,    4,    0,    4,    0,    4,    1,
    3,    1,    1,    3,    1,    1,    3,    2,    1,    3,
    1,    1,    3,    1,    4,    0,    2,    0,    4,    0,
    5,    0,    4,    0,    5,    0,    5,    0,    5,    0,
    0,    8,    1,    2,    2,    2,    2,    2,    5,    6,
    2,    0,    3,    2,    0,    2,    0,    4,    0,    4,
    0,    6,    0,    6,    0,    4,    1,    0,    4,    2,
    0,    1,    0,    1,    1,    1,    1,    1,    0,    1,
    0,    6,    0,    4,    0,    6,    0,    4,    1,    1,
    3,    2,    1,    0,    2,    0,    4,    0,    4,    0,
    4,    0,    4,    2,    0,    4,    0,    5,    0,    5,
    0,    4,    0,    5,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    5,    0,    6,    0,    4,    0,    5,
    0,    6,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    0,    4,    0,    5,    0,    5,
    0,    5,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    6,    0,    4,    0,    6,    0,    5,
    0,    2,    0,    5,    0,    4,    1,    1,    0,    1,
    1,    0,    2,    0,    6,    0,    6,    0,    4,    0,
    4,    0,    5,    1,    1,    1,    1,    1,    1,    1,
};
static const YYINT racoonyydefred[] = {                   1,
    0,    0,    0,    0,    0,    0,    0,    0,   87,   71,
    0,    0,  200,    0,    0,    0,    2,    3,    4,    5,
    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
   16,   17,   19,    0,    0,    0,   39,    0,   41,   54,
    0,    0,  116,  186,    0,    0,    0,    0,    0,  253,
    0,   33,    0,   31,   35,   36,   38,    0,    0,   89,
   73,    0,    0,    0,    0,    0,    0,   70,  252,    0,
    0,   68,    0,    0,   37,    0,    0,    0,    0,   18,
   20,    0,    0,    0,    0,    0,    0,   40,   42,    0,
    0,   66,    0,   53,   55,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  115,  117,    0,    0,    0,    0,
    0,    0,  185,  187,  204,  205,  211,    0,  206,  207,
  208,    0,    0,    0,  241,  254,  244,  245,  248,   34,
   23,   21,   27,   25,   29,   32,   43,   45,   47,   49,
   51,   56,   58,    0,   64,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   88,   90,
    0,    0,    0,    0,   72,   74,  118,  120,  172,    0,
    0,  175,    0,    0,  130,  132,  134,  136,  138,  181,
  140,    0,  142,  144,  148,  150,  152,  146,  164,  166,
  162,  168,  154,  160,    0,    0,  184,  128,    0,  156,
  158,  188,    0,  192,    0,    0,    0,  232,    0,    0,
  214,    0,  215,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   67,    0,
    0,    0,   93,   95,   91,   97,  101,  103,   99,  105,
  107,  109,  111,  113,    0,    0,   83,   85,    0,    0,
  122,    0,  123,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  176,    0,    0,    0,    0,
    0,    0,  374,  375,  376,  190,    0,  194,  196,  198,
    0,  234,  235,    0,  361,  360,  213,    0,  250,  242,
  249,    0,  258,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  297,    0,    0,    0,
    0,    0,    0,  349,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  251,  255,  246,
   24,   22,   28,   26,   30,   44,   46,   48,   50,   52,
   57,   59,   65,    0,   63,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   75,    0,
   79,    0,    0,  119,  121,  171,  174,  131,  133,  135,
  137,  139,  141,  180,  143,  145,  149,  151,  153,  147,
  165,  167,  163,  169,  155,  161,  178,    0,    0,    0,
  129,  183,  157,  159,  189,    0,  193,    0,    0,    0,
    0,  237,  238,  236,  209,  225,    0,    0,    0,    0,
  216,  256,  351,  260,  262,    0,    0,  264,  271,    0,
    0,  265,    0,  275,  277,  279,  281,    0,    0,  293,
  295,    0,  299,  323,  327,  325,  345,  319,  317,  321,
    0,  329,  331,  333,  335,  341,  287,  315,  339,  337,
  303,  301,  305,  313,    0,    0,    0,   60,   94,   96,
   92,   98,  102,  104,  100,  106,  108,  110,  112,  114,
   77,    0,   81,    0,   84,   86,  177,  125,  127,  191,
  195,  197,  199,  210,    0,  357,  358,  217,    0,    0,
  219,  202,    0,    0,    0,    0,    0,  355,    0,  267,
  269,    0,  273,    0,    0,    0,    0,    0,  283,    0,
  289,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  362,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  307,  309,
  311,    0,    0,   76,    0,   80,    0,    0,    0,    0,
    0,    0,    0,  257,  352,  259,  261,  263,  353,    0,
  272,    0,    0,  266,    0,  276,  278,  280,  282,  285,
    0,  291,    0,  294,  296,  298,  300,  324,  328,  326,
  346,  320,  318,  322,    0,  330,  332,  334,  336,  342,
  288,  316,  340,  338,  304,  302,  306,  314,  343,  377,
  378,  379,  380,  347,    0,    0,    0,   61,   78,   82,
  240,  230,    0,  226,  218,  221,  223,  220,    0,  356,
  268,  270,  274,    0,  284,    0,  290,    0,    0,    0,
    0,  350,  363,    0,    0,  308,  310,  312,    0,    0,
    0,  354,  286,  292,    0,  368,  370,    0,    0,  344,
  348,  229,  222,  224,  372,    0,    0,    0,    0,    0,
  369,  371,  364,  366,  373,    0,    0,  365,  367,
};
static const YYINT racoonyydgoto[] = {                    1,
  632,  508,  220,  304,   69,  425,  296,  624,   66,  307,
   49,   50,  568,   17,   18,   19,   20,   21,   22,   23,
   24,   25,   26,   27,   28,   29,   30,   31,   32,   53,
   81,  228,  227,  230,  229,  231,   82,   76,   38,   58,
   89,  232,  233,  234,  235,  236,   59,   95,  237,  238,
  562,  242,  240,  154,   42,   97,  176,  492,  563,  494,
  565,  382,  383,   41,   96,  170,  368,  366,  367,  369,
  372,  370,  371,  373,  374,  375,  376,  377,   62,  116,
  259,  260,  180,  183,  285,  409,  410,  208,  288,  265,
  266,  267,  268,  269,  191,  270,  272,  273,  277,  274,
  275,  276,  282,  290,  291,  283,  280,  278,  279,  281,
  181,  184,  286,  192,  209,   63,  124,  292,  416,  297,
  418,  419,  420,   67,   45,  134,  308,  430,  431,  570,
  573,  660,  661,  569,  505,  633,  310,  224,  311,   71,
  226,   74,  225,  349,  513,  514,  433,  515,  516,  438,
  522,  582,  583,  519,  585,  524,  525,  526,  527,  591,
  644,  549,  593,  646,  532,  533,  452,  535,  554,  553,
  555,  625,  626,  627,  556,  550,  541,  540,  542,  536,
  538,  537,  544,  545,  546,  547,  552,  551,  548,  654,
  539,  655,  461,  605,  639,  580,  653,  686,  687,  676,
  677,  680,
};
static const YYINT racoonyysindex[] = {                   0,
 -193, -348, -174, -292, -306, -130, -266, -246,    0,    0,
 -244, -234,    0, -268, -217, -216,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -264, -220, -191,    0, -168,    0,    0,
 -219, -196,    0,    0, -252, -185, -146, -185, -108,    0,
 -164,    0, -251,    0,    0,    0,    0, -257, -258,    0,
    0, -256, -275, -231, -321, -242, -218,    0,    0, -171,
 -163,    0, -255, -163,    0, -159, -284, -282, -143,    0,
    0, -152, -138, -137, -134, -132, -131,    0,    0, -128,
 -128,    0, -261,    0,    0, -260, -259, -127, -126, -125,
 -124, -129, -122, -121, -200, -209, -136, -120, -118, -115,
 -165, -117, -114, -113,    0,    0, -111, -110, -109, -106,
 -105, -104,    0,    0,    0,    0,    0, -308,    0,    0,
    0, -112,  -80, -148,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -142,    0, -103, -102, -101, -100,  -99,
  -98,  -97,  -93,  -95,  -94,  -92,  -91,  -90,    0,    0,
  -89,  -88,  -96,  -83,    0,    0,    0,    0,    0, -141,
 -149,    0, -116,  -86,    0,    0,    0,    0,    0,    0,
    0,  -84,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -85,  -85,    0,    0,  -82,    0,
    0,    0, -201,    0, -201, -201, -201,    0,  -79, -195,
    0, -199,    0, -172, -140, -172,  -78,  -77,  -76,  -75,
  -74,  -73,  -72,  -71,  -70,  -69,  -67,  -66,    0,  -65,
  -81,  -64,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -265, -262,    0,    0,  -63,  -62,
    0, -125,    0, -124,  -61,  -60,  -59,  -58,  -57,  -56,
 -121,  -55,  -54,  -53,  -52,  -51,  -50,  -49,  -48,  -47,
  -46,  -45,  -44,  -11,  -43,    0,  -43,  -42, -117,  -41,
  -39,  -38,    0,    0,    0,    0,  -37,    0,    0,    0,
 -195,    0,    0, -238,    0,    0,    0, -289,    0,    0,
    0, -128,    0,  -23,  -18, -107, -270,  -32,  -22,  -21,
  -20,  -19,  -24,  -17,  -16,  -14,    0,  -13,  -12, -293,
  -68, -310,  -10,    0,   -9,   -7,   -6,   -5,   -4, -199,
   -3, -288, -286,   -1,    1, -139,  -26,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    3,    0,  -31,  -30,  -27,   -8,   -2,
    2,    4,    5,    6,    7,    8,    9,   10,    0,   11,
    0,   12,   13,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -85,   14,   15,
    0,    0,    0,    0,    0,   16,    0,   17,   18,   19,
 -238,    0,    0,    0,    0,    0, -254, -135,   24, -145,
    0,    0,    0,    0,    0,   21,   30,    0,    0,   31,
   32,    0,   33,    0,    0,    0,    0, -281, -277,    0,
    0, -254,    0,    0,    0,    0,    0,    0,    0,    0,
  -25,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   38,   39, -167,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   26,    0,   27,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   75,    0,    0,    0,   42,   43,
    0,    0,   34, -303,   35,   36,   40,    0,   37,    0,
    0,   41,    0,   44,   45,   46,   47, -199,    0, -199,
    0,   48,   49,   50,   51,   52,   53,   54,   55,   56,
   57,   58,    0,   59,   60,   61,   62,   64,   65,   66,
   67,   68,   69,   70,   71,   72, -201, -181,    0,    0,
    0,   73,   74,    0,   76,    0,   77,    0,   78,   79,
 -201, -181,   80,    0,    0,    0,    0,    0,    0,   81,
    0,   82,   83,    0,   84,    0,    0,    0,    0,    0,
   85,    0,   86,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -291,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   87,   88,   89,    0,    0,    0,
    0,    0,   28,    0,    0,    0,    0,    0,   90,    0,
    0,    0,    0,   91,    0,   92,    0,   95, -254,  103,
 -133,    0,    0,   94,   96,    0,    0,    0,   75,   97,
   98,    0,    0,    0,   77,    0,    0,  109,  113,    0,
    0,    0,    0,    0,    0,  100,  101, -201, -181,  102,
    0,    0,    0,    0,    0,  104,  105,    0,    0,
};
static const YYINT racoonyyrindex[] = {                   0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -305,  -15, -305,  106,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -249,    0,    0,  107,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -232,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  110,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  111,    0,    0,  112,    0,    0,    0,    0,    0,    0,
    0,  114,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  115,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -232, -213,
    0,  108,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  116,    0,  117,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -213,    0,    0,    0,    0,    0,    0, -119,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -70,
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
    0,    0,    0,    0,    0,    0,    0,  -70,  -70,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -70,    0,  -70,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -285, -247,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -142,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT racoonyygindex[] = {                   0,
 -381, -447,  120,  119,  284,  121, -215, -539,   63, -334,
  261,  -87,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  149,  150,  129,    0,    0,  126,    0,    0,
    0,    0,    0,    0,  146,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   93,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -323,    0,    0,  192,    0,  118,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
#define YYTABLESIZE 542
static const YYINT racoonyytable[] = {                  298,
  299,  300,  152,  153,  534,  467,   77,   78,   79,  203,
   83,   84,   85,   86,   87,   90,   91,  155,   92,   93,
  157,  158,  159,  160,  161,  162,  163,  164,  165,  166,
  167,  168,  637,   69,  171,  172,  173,  174,  575,  648,
  132,  426,   98,   99,  100,  101,  102,  103,  104,  117,
  118,  119,  120,  121,  122,  105,  106,  107,  108,  109,
  110,  111,  458,    2,  112,  113,  114,    3,   46,    4,
    5,    6,  649,    7,  528,  455,   33,  506,  530,    8,
  469,   46,  471,  218,   64,  203,    9,   34,  650,  439,
  440,  441,  127,  128,  129,  130,  195,  196,  197,   10,
  459,  198,   65,   36,   11,  193,  125,  219,  194,  651,
  427,  428,   65,  529,  531,  429,  133,  456,   69,   69,
  576,   35,  470,   65,  472,  141,  126,  143,  131,  142,
   12,  144,  305,  306,  652,   37,  305,  306,  239,  684,
   13,  239,   14,  442,  378,   47,   48,  380,  379,   54,
  123,  381,  156,  205,  206,  507,  422,  423,   39,   48,
  231,  231,  231,  231,   51,  169,  175,   94,   88,  115,
  199,  424,  200,  201,   80,  203,  227,  231,   40,  228,
   43,  233,  233,  186,  187,  188,  189,   15,  137,   16,
   44,  139,   70,  590,   52,  592,  233,  302,  303,  312,
  313,  666,  314,   55,  315,   60,  316,   68,  317,  318,
  319,  320,  321,  322,  305,  306,  323,  324,  325,  293,
  294,  295,  326,  327,  432,  328,  329,  330,   61,  331,
   73,  332,   56,  333,  334,  620,  621,  622,  623,  559,
  560,  561,  135,  335,  336,  337,  338,  339,  340,  341,
  342,  309,  136,  436,  437,   57,  343,  344,  345,   75,
  346,  136,  475,  476,  140,  347,  509,  510,  668,  669,
  145,  146,  147,  148,  222,  149,  223,  262,  150,  151,
  512,  239,  261,  675,  185,  348,   48,  177,  178,  179,
  182,  203,  190,  202,  204,  210,  207,  211,  212,  213,
  214,  221,  457,  215,  216,  217,  201,  263,  244,  245,
  241,  243,  218,  257,  246,  247,  248,  249,  250,  251,
  434,  252,  253,  254,  255,  256,  258,  435,  443,  284,
  448,   72,  364,  138,  287,  672,    0,  449,  301,    0,
  264,  619,  271,    0,  289,  351,  352,  353,  354,  355,
  356,  357,  358,  359,  360,  636,  361,  362,  363,  365,
  384,  385,  388,  389,  390,  391,  392,  393,  395,  396,
  397,  398,  399,  400,  401,  402,  403,  404,  405,  406,
  407,  411,  413,  408,  414,  415,  417,  477,  444,  445,
  446,  447,  479,  480,  450,  451,  481,  453,  454,  543,
  460,  462,  463,  464,  465,  466,  567,  468,  473,  243,
  386,  474,  478,  387,  412,  482,  394,  350,    0,  421,
    0,  483,    0,  491,  493,  484,  665,  485,  486,  487,
  488,  489,  490,  511,  517,  495,  496,  498,  499,  500,
  501,  502,  503,  518,  520,  521,  523,  557,  558,  564,
  566,  571,  572,  579,  659,    0,    0,  574,  577,  578,
  581,    0,  683,    0,  584,    0,    0,  586,  587,  588,
  589,  594,  595,  596,  597,  598,  599,  600,  601,  602,
  603,  604,  606,  607,  608,  609,  631,  610,  611,  612,
  613,  614,  615,  616,  617,  618,  628,  629,    0,  630,
  497,  634,  635,  638,  640,  641,  642,  643,  645,  647,
  656,  657,  658,  662,  663,  664,  667,  670,  678,  671,
  673,  674,  679,  681,  682,  685,    0,  688,  689,    0,
  247,  212,  359,   62,  170,  173,    0,  179,  182,  124,
  126,  504,
};
static const YYINT racoonyycheck[] = {                  215,
  216,  217,   90,   91,  452,  340,  258,  259,  260,  259,
  268,  269,  270,  271,  272,  274,  275,  279,  277,  278,
  281,  282,  283,  284,  285,  286,  287,  288,  289,  290,
  291,  292,  572,  339,  294,  295,  296,  297,  342,  331,
  259,  331,  299,  300,  301,  302,  303,  304,  305,  325,
  326,  327,  328,  329,  330,  312,  313,  314,  315,  316,
  317,  318,  373,  257,  321,  322,  323,  261,  337,  263,
  264,  265,  364,  267,  356,  369,  425,  332,  356,  273,
  369,  337,  369,  392,  337,  335,  280,  262,  380,  360,
  361,  362,  414,  415,  337,  338,  306,  307,  308,  293,
  411,  311,  355,  410,  298,  306,  338,  416,  309,  401,
  400,  401,  355,  448,  449,  405,  335,  411,  424,  425,
  424,  414,  411,  355,  411,  410,   64,  410,   66,  414,
  324,  414,  414,  415,  426,  266,  414,  415,  424,  679,
  334,  427,  336,  414,  410,  414,  415,  410,  414,  414,
  426,  414,  414,  319,  320,  410,  395,  396,  425,  415,
  393,  394,  395,  396,  382,  426,  426,  426,  426,  426,
  307,  410,  309,  310,  426,  425,  424,  410,  425,  427,
  425,  395,  396,  306,  307,  308,  309,  381,   71,  383,
  425,   74,  339,  528,  411,  530,  410,  393,  394,  340,
  341,  649,  343,  424,  345,  425,  347,  393,  349,  350,
  351,  352,  353,  354,  414,  415,  357,  358,  359,  421,
  422,  423,  363,  364,  312,  366,  367,  368,  425,  370,
  339,  372,  424,  374,  375,  417,  418,  419,  420,  407,
  408,  409,  414,  384,  385,  386,  387,  388,  389,  390,
  391,  424,  425,  361,  362,  424,  397,  398,  399,  424,
  401,  425,  402,  403,  424,  406,  402,  403,  402,  403,
  414,  424,  411,  411,  355,  410,  425,  427,  411,  411,
  426,  424,  424,  665,  414,  426,  415,  415,  415,  415,
  415,  410,  414,  414,  410,  410,  414,  411,  410,  410,
  410,  414,  371,  410,  410,  410,  426,  424,  410,  410,
  414,  414,  392,  410,  414,  414,  414,  411,  414,  414,
  344,  414,  414,  414,  414,  414,  410,  346,  361,  415,
  355,   48,  414,   73,  206,  659,   -1,  355,  219,   -1,
  427,  557,  427,   -1,  427,  424,  424,  424,  424,  424,
  424,  424,  424,  424,  424,  571,  424,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
  392,  424,  424,  427,  424,  424,  424,  414,  411,  411,
  411,  411,  424,  424,  411,  410,  424,  411,  411,  425,
  411,  411,  410,  410,  410,  410,  332,  411,  410,  425,
  262,  411,  410,  264,  289,  424,  271,  226,   -1,  301,
   -1,  424,   -1,  414,  414,  424,  332,  424,  424,  424,
  424,  424,  424,  410,  414,  424,  424,  424,  424,  424,
  424,  424,  424,  414,  414,  414,  414,  410,  410,  424,
  424,  410,  410,  414,  427,   -1,   -1,  424,  424,  424,
  424,   -1,  678,   -1,  424,   -1,   -1,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  424,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  410,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  424,  424,   -1,  424,
  408,  424,  424,  424,  424,  424,  424,  424,  424,  424,
  424,  424,  424,  424,  424,  424,  414,  424,  410,  424,
  424,  424,  410,  424,  424,  424,   -1,  424,  424,   -1,
  425,  425,  425,  424,  424,  424,   -1,  424,  424,  424,
  424,  421,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 427
#define YYUNDFTOKEN 632
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const racoonyyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"PRIVSEP","USER","GROUP","CHROOT",
"PATH","PATHTYPE","INCLUDE","PFKEY_BUFFER","LOGGING","LOGLEV","PADDING",
"PAD_RANDOMIZE","PAD_RANDOMIZELEN","PAD_MAXLEN","PAD_STRICT","PAD_EXCLTAIL",
"LISTEN","X_ISAKMP","X_ISAKMP_NATT","X_ADMIN","STRICT_ADDRESS","ADMINSOCK",
"DISABLED","LDAPCFG","LDAP_HOST","LDAP_PORT","LDAP_PVER","LDAP_BASE",
"LDAP_BIND_DN","LDAP_BIND_PW","LDAP_SUBTREE","LDAP_ATTR_USER","LDAP_ATTR_ADDR",
"LDAP_ATTR_MASK","LDAP_ATTR_GROUP","LDAP_ATTR_MEMBER","RADCFG","RAD_AUTH",
"RAD_ACCT","RAD_TIMEOUT","RAD_RETRIES","MODECFG","CFG_NET4","CFG_MASK4",
"CFG_DNS4","CFG_NBNS4","CFG_DEFAULT_DOMAIN","CFG_AUTH_SOURCE","CFG_AUTH_GROUPS",
"CFG_SYSTEM","CFG_RADIUS","CFG_PAM","CFG_LDAP","CFG_LOCAL","CFG_NONE",
"CFG_GROUP_SOURCE","CFG_ACCOUNTING","CFG_CONF_SOURCE","CFG_MOTD",
"CFG_POOL_SIZE","CFG_AUTH_THROTTLE","CFG_SPLIT_NETWORK","CFG_SPLIT_LOCAL",
"CFG_SPLIT_INCLUDE","CFG_SPLIT_DNS","CFG_PFS_GROUP","CFG_SAVE_PASSWD","RETRY",
"RETRY_COUNTER","RETRY_INTERVAL","RETRY_PERSEND","RETRY_PHASE1","RETRY_PHASE2",
"NATT_KA","ALGORITHM_CLASS","ALGORITHMTYPE","STRENGTHTYPE","SAINFO","FROM",
"REMOTE","ANONYMOUS","CLIENTADDR","INHERIT","REMOTE_ADDRESS","EXCHANGE_MODE",
"EXCHANGETYPE","DOI","DOITYPE","SITUATION","SITUATIONTYPE","CERTIFICATE_TYPE",
"CERTTYPE","PEERS_CERTFILE","CA_TYPE","VERIFY_CERT","SEND_CERT","SEND_CR",
"MATCH_EMPTY_CR","IDENTIFIERTYPE","IDENTIFIERQUAL","MY_IDENTIFIER",
"PEERS_IDENTIFIER","VERIFY_IDENTIFIER","DNSSEC","CERT_X509","CERT_PLAINRSA",
"NONCE_SIZE","DH_GROUP","KEEPALIVE","PASSIVE","INITIAL_CONTACT","NAT_TRAVERSAL",
"REMOTE_FORCE_LEVEL","PROPOSAL_CHECK","PROPOSAL_CHECK_LEVEL","GENERATE_POLICY",
"GENERATE_LEVEL","SUPPORT_PROXY","PROPOSAL","EXEC_PATH","EXEC_COMMAND",
"EXEC_SUCCESS","EXEC_FAILURE","GSS_ID","GSS_ID_ENC","GSS_ID_ENCTYPE",
"COMPLEX_BUNDLE","DPD","DPD_DELAY","DPD_RETRY","DPD_MAXFAIL","PH1ID",
"XAUTH_LOGIN","WEAK_PHASE1_CHECK","REKEY","PREFIX","PORT","PORTANY","UL_PROTO",
"ANY","IKE_FRAG","ESP_FRAG","MODE_CFG","PFS_GROUP","LIFETIME","LIFETYPE_TIME",
"LIFETYPE_BYTE","STRENGTH","REMOTEID","SCRIPT","PHASE1_UP","PHASE1_DOWN",
"PHASE1_DEAD","NUMBER","SWITCH","BOOLEAN","HEXSTRING","QUOTEDSTRING",
"ADDRSTRING","ADDRRANGE","UNITTYPE_BYTE","UNITTYPE_KBYTES","UNITTYPE_MBYTES",
"UNITTYPE_TBYTES","UNITTYPE_SEC","UNITTYPE_MIN","UNITTYPE_HOUR","EOS","BOC",
"EOC","COMMA",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,"illegal-symbol",
};
static const char *const racoonyyrule[] = {
"$accept : statements",
"statements :",
"statements : statements statement",
"statement : privsep_statement",
"statement : path_statement",
"statement : include_statement",
"statement : pfkey_statement",
"statement : gssenc_statement",
"statement : logging_statement",
"statement : padding_statement",
"statement : listen_statement",
"statement : ldapcfg_statement",
"statement : radcfg_statement",
"statement : modecfg_statement",
"statement : timer_statement",
"statement : sainfo_statement",
"statement : remote_statement",
"statement : special_statement",
"privsep_statement : PRIVSEP BOC privsep_stmts EOC",
"privsep_stmts :",
"privsep_stmts : privsep_stmts privsep_stmt",
"$$1 :",
"privsep_stmt : USER QUOTEDSTRING $$1 EOS",
"$$2 :",
"privsep_stmt : USER NUMBER $$2 EOS",
"$$3 :",
"privsep_stmt : GROUP QUOTEDSTRING $$3 EOS",
"$$4 :",
"privsep_stmt : GROUP NUMBER $$4 EOS",
"$$5 :",
"privsep_stmt : CHROOT QUOTEDSTRING $$5 EOS",
"$$6 :",
"path_statement : PATH PATHTYPE QUOTEDSTRING $$6 EOS",
"$$7 :",
"special_statement : COMPLEX_BUNDLE SWITCH $$7 EOS",
"include_statement : INCLUDE QUOTEDSTRING EOS",
"pfkey_statement : PFKEY_BUFFER NUMBER EOS",
"gssenc_statement : GSS_ID_ENC GSS_ID_ENCTYPE EOS",
"logging_statement : LOGGING log_level EOS",
"log_level : LOGLEV",
"padding_statement : PADDING BOC padding_stmts EOC",
"padding_stmts :",
"padding_stmts : padding_stmts padding_stmt",
"$$8 :",
"padding_stmt : PAD_RANDOMIZE SWITCH $$8 EOS",
"$$9 :",
"padding_stmt : PAD_RANDOMIZELEN SWITCH $$9 EOS",
"$$10 :",
"padding_stmt : PAD_MAXLEN NUMBER $$10 EOS",
"$$11 :",
"padding_stmt : PAD_STRICT SWITCH $$11 EOS",
"$$12 :",
"padding_stmt : PAD_EXCLTAIL SWITCH $$12 EOS",
"listen_statement : LISTEN BOC listen_stmts EOC",
"listen_stmts :",
"listen_stmts : listen_stmts listen_stmt",
"$$13 :",
"listen_stmt : X_ISAKMP ike_addrinfo_port $$13 EOS",
"$$14 :",
"listen_stmt : X_ISAKMP_NATT ike_addrinfo_port $$14 EOS",
"$$15 :",
"listen_stmt : ADMINSOCK QUOTEDSTRING QUOTEDSTRING QUOTEDSTRING NUMBER $$15 EOS",
"$$16 :",
"listen_stmt : ADMINSOCK QUOTEDSTRING $$16 EOS",
"$$17 :",
"listen_stmt : ADMINSOCK DISABLED $$17 EOS",
"$$18 :",
"listen_stmt : STRICT_ADDRESS $$18 EOS",
"ike_addrinfo_port : ADDRSTRING ike_port",
"ike_port :",
"ike_port : PORT",
"$$19 :",
"radcfg_statement : RADCFG $$19 BOC radcfg_stmts EOC",
"radcfg_stmts :",
"radcfg_stmts : radcfg_stmts radcfg_stmt",
"$$20 :",
"radcfg_stmt : RAD_AUTH QUOTEDSTRING QUOTEDSTRING $$20 EOS",
"$$21 :",
"radcfg_stmt : RAD_AUTH QUOTEDSTRING NUMBER QUOTEDSTRING $$21 EOS",
"$$22 :",
"radcfg_stmt : RAD_ACCT QUOTEDSTRING QUOTEDSTRING $$22 EOS",
"$$23 :",
"radcfg_stmt : RAD_ACCT QUOTEDSTRING NUMBER QUOTEDSTRING $$23 EOS",
"$$24 :",
"radcfg_stmt : RAD_TIMEOUT NUMBER $$24 EOS",
"$$25 :",
"radcfg_stmt : RAD_RETRIES NUMBER $$25 EOS",
"$$26 :",
"ldapcfg_statement : LDAPCFG $$26 BOC ldapcfg_stmts EOC",
"ldapcfg_stmts :",
"ldapcfg_stmts : ldapcfg_stmts ldapcfg_stmt",
"$$27 :",
"ldapcfg_stmt : LDAP_PVER NUMBER $$27 EOS",
"$$28 :",
"ldapcfg_stmt : LDAP_HOST QUOTEDSTRING $$28 EOS",
"$$29 :",
"ldapcfg_stmt : LDAP_PORT NUMBER $$29 EOS",
"$$30 :",
"ldapcfg_stmt : LDAP_BASE QUOTEDSTRING $$30 EOS",
"$$31 :",
"ldapcfg_stmt : LDAP_SUBTREE SWITCH $$31 EOS",
"$$32 :",
"ldapcfg_stmt : LDAP_BIND_DN QUOTEDSTRING $$32 EOS",
"$$33 :",
"ldapcfg_stmt : LDAP_BIND_PW QUOTEDSTRING $$33 EOS",
"$$34 :",
"ldapcfg_stmt : LDAP_ATTR_USER QUOTEDSTRING $$34 EOS",
"$$35 :",
"ldapcfg_stmt : LDAP_ATTR_ADDR QUOTEDSTRING $$35 EOS",
"$$36 :",
"ldapcfg_stmt : LDAP_ATTR_MASK QUOTEDSTRING $$36 EOS",
"$$37 :",
"ldapcfg_stmt : LDAP_ATTR_GROUP QUOTEDSTRING $$37 EOS",
"$$38 :",
"ldapcfg_stmt : LDAP_ATTR_MEMBER QUOTEDSTRING $$38 EOS",
"modecfg_statement : MODECFG BOC modecfg_stmts EOC",
"modecfg_stmts :",
"modecfg_stmts : modecfg_stmts modecfg_stmt",
"$$39 :",
"modecfg_stmt : CFG_NET4 ADDRSTRING $$39 EOS",
"$$40 :",
"modecfg_stmt : CFG_MASK4 ADDRSTRING $$40 EOS",
"modecfg_stmt : CFG_DNS4 addrdnslist EOS",
"modecfg_stmt : CFG_NBNS4 addrwinslist EOS",
"$$41 :",
"modecfg_stmt : CFG_SPLIT_NETWORK CFG_SPLIT_LOCAL splitnetlist $$41 EOS",
"$$42 :",
"modecfg_stmt : CFG_SPLIT_NETWORK CFG_SPLIT_INCLUDE splitnetlist $$42 EOS",
"$$43 :",
"modecfg_stmt : CFG_SPLIT_DNS splitdnslist $$43 EOS",
"$$44 :",
"modecfg_stmt : CFG_DEFAULT_DOMAIN QUOTEDSTRING $$44 EOS",
"$$45 :",
"modecfg_stmt : CFG_AUTH_SOURCE CFG_SYSTEM $$45 EOS",
"$$46 :",
"modecfg_stmt : CFG_AUTH_SOURCE CFG_RADIUS $$46 EOS",
"$$47 :",
"modecfg_stmt : CFG_AUTH_SOURCE CFG_PAM $$47 EOS",
"$$48 :",
"modecfg_stmt : CFG_AUTH_SOURCE CFG_LDAP $$48 EOS",
"$$49 :",
"modecfg_stmt : CFG_AUTH_GROUPS authgrouplist $$49 EOS",
"$$50 :",
"modecfg_stmt : CFG_GROUP_SOURCE CFG_SYSTEM $$50 EOS",
"$$51 :",
"modecfg_stmt : CFG_GROUP_SOURCE CFG_LDAP $$51 EOS",
"$$52 :",
"modecfg_stmt : CFG_ACCOUNTING CFG_NONE $$52 EOS",
"$$53 :",
"modecfg_stmt : CFG_ACCOUNTING CFG_SYSTEM $$53 EOS",
"$$54 :",
"modecfg_stmt : CFG_ACCOUNTING CFG_RADIUS $$54 EOS",
"$$55 :",
"modecfg_stmt : CFG_ACCOUNTING CFG_PAM $$55 EOS",
"$$56 :",
"modecfg_stmt : CFG_POOL_SIZE NUMBER $$56 EOS",
"$$57 :",
"modecfg_stmt : CFG_PFS_GROUP NUMBER $$57 EOS",
"$$58 :",
"modecfg_stmt : CFG_SAVE_PASSWD SWITCH $$58 EOS",
"$$59 :",
"modecfg_stmt : CFG_AUTH_THROTTLE NUMBER $$59 EOS",
"$$60 :",
"modecfg_stmt : CFG_CONF_SOURCE CFG_LOCAL $$60 EOS",
"$$61 :",
"modecfg_stmt : CFG_CONF_SOURCE CFG_RADIUS $$61 EOS",
"$$62 :",
"modecfg_stmt : CFG_CONF_SOURCE CFG_LDAP $$62 EOS",
"$$63 :",
"modecfg_stmt : CFG_MOTD QUOTEDSTRING $$63 EOS",
"addrdnslist : addrdns",
"addrdnslist : addrdns COMMA addrdnslist",
"addrdns : ADDRSTRING",
"addrwinslist : addrwins",
"addrwinslist : addrwins COMMA addrwinslist",
"addrwins : ADDRSTRING",
"splitnetlist : splitnet",
"splitnetlist : splitnetlist COMMA splitnet",
"splitnet : ADDRSTRING PREFIX",
"authgrouplist : authgroup",
"authgrouplist : authgroup COMMA authgrouplist",
"authgroup : QUOTEDSTRING",
"splitdnslist : splitdns",
"splitdnslist : splitdns COMMA splitdnslist",
"splitdns : QUOTEDSTRING",
"timer_statement : RETRY BOC timer_stmts EOC",
"timer_stmts :",
"timer_stmts : timer_stmts timer_stmt",
"$$64 :",
"timer_stmt : RETRY_COUNTER NUMBER $$64 EOS",
"$$65 :",
"timer_stmt : RETRY_INTERVAL NUMBER unittype_time $$65 EOS",
"$$66 :",
"timer_stmt : RETRY_PERSEND NUMBER $$66 EOS",
"$$67 :",
"timer_stmt : RETRY_PHASE1 NUMBER unittype_time $$67 EOS",
"$$68 :",
"timer_stmt : RETRY_PHASE2 NUMBER unittype_time $$68 EOS",
"$$69 :",
"timer_stmt : NATT_KA NUMBER unittype_time $$69 EOS",
"$$70 :",
"$$71 :",
"sainfo_statement : SAINFO $$70 sainfo_name sainfo_param BOC sainfo_specs $$71 EOC",
"sainfo_name : ANONYMOUS",
"sainfo_name : ANONYMOUS CLIENTADDR",
"sainfo_name : ANONYMOUS sainfo_id",
"sainfo_name : sainfo_id ANONYMOUS",
"sainfo_name : sainfo_id CLIENTADDR",
"sainfo_name : sainfo_id sainfo_id",
"sainfo_id : IDENTIFIERTYPE ADDRSTRING prefix port ul_proto",
"sainfo_id : IDENTIFIERTYPE ADDRSTRING ADDRRANGE prefix port ul_proto",
"sainfo_id : IDENTIFIERTYPE QUOTEDSTRING",
"sainfo_param :",
"sainfo_param : FROM IDENTIFIERTYPE identifierstring",
"sainfo_param : GROUP QUOTEDSTRING",
"sainfo_specs :",
"sainfo_specs : sainfo_specs sainfo_spec",
"$$72 :",
"sainfo_spec : PFS_GROUP dh_group_num $$72 EOS",
"$$73 :",
"sainfo_spec : REMOTEID NUMBER $$73 EOS",
"$$74 :",
"sainfo_spec : LIFETIME LIFETYPE_TIME NUMBER unittype_time $$74 EOS",
"$$75 :",
"sainfo_spec : LIFETIME LIFETYPE_BYTE NUMBER unittype_byte $$75 EOS",
"$$76 :",
"sainfo_spec : ALGORITHM_CLASS $$76 algorithms EOS",
"algorithms : algorithm",
"$$77 :",
"algorithms : algorithm $$77 COMMA algorithms",
"algorithm : ALGORITHMTYPE keylength",
"prefix :",
"prefix : PREFIX",
"port :",
"port : PORT",
"port : PORTANY",
"ul_proto : NUMBER",
"ul_proto : UL_PROTO",
"ul_proto : ANY",
"keylength :",
"keylength : NUMBER",
"$$78 :",
"remote_statement : REMOTE QUOTEDSTRING INHERIT QUOTEDSTRING $$78 remote_specs_inherit_block",
"$$79 :",
"remote_statement : REMOTE QUOTEDSTRING $$79 remote_specs_block",
"$$80 :",
"remote_statement : REMOTE remote_index INHERIT remote_index $$80 remote_specs_inherit_block",
"$$81 :",
"remote_statement : REMOTE remote_index $$81 remote_specs_block",
"remote_specs_inherit_block : remote_specs_block",
"remote_specs_inherit_block : EOS",
"remote_specs_block : BOC remote_specs EOC",
"remote_index : ANONYMOUS ike_port",
"remote_index : ike_addrinfo_port",
"remote_specs :",
"remote_specs : remote_specs remote_spec",
"$$82 :",
"remote_spec : REMOTE_ADDRESS ike_addrinfo_port $$82 EOS",
"$$83 :",
"remote_spec : EXCHANGE_MODE $$83 exchange_types EOS",
"$$84 :",
"remote_spec : DOI DOITYPE $$84 EOS",
"$$85 :",
"remote_spec : SITUATION SITUATIONTYPE $$85 EOS",
"remote_spec : CERTIFICATE_TYPE cert_spec",
"$$86 :",
"remote_spec : PEERS_CERTFILE QUOTEDSTRING $$86 EOS",
"$$87 :",
"remote_spec : PEERS_CERTFILE CERT_X509 QUOTEDSTRING $$87 EOS",
"$$88 :",
"remote_spec : PEERS_CERTFILE CERT_PLAINRSA QUOTEDSTRING $$88 EOS",
"$$89 :",
"remote_spec : PEERS_CERTFILE DNSSEC $$89 EOS",
"$$90 :",
"remote_spec : CA_TYPE CERT_X509 QUOTEDSTRING $$90 EOS",
"$$91 :",
"remote_spec : VERIFY_CERT SWITCH $$91 EOS",
"$$92 :",
"remote_spec : SEND_CERT SWITCH $$92 EOS",
"$$93 :",
"remote_spec : SEND_CR SWITCH $$93 EOS",
"$$94 :",
"remote_spec : MATCH_EMPTY_CR SWITCH $$94 EOS",
"$$95 :",
"remote_spec : MY_IDENTIFIER IDENTIFIERTYPE identifierstring $$95 EOS",
"$$96 :",
"remote_spec : MY_IDENTIFIER IDENTIFIERTYPE IDENTIFIERQUAL identifierstring $$96 EOS",
"$$97 :",
"remote_spec : XAUTH_LOGIN identifierstring $$97 EOS",
"$$98 :",
"remote_spec : PEERS_IDENTIFIER IDENTIFIERTYPE identifierstring $$98 EOS",
"$$99 :",
"remote_spec : PEERS_IDENTIFIER IDENTIFIERTYPE IDENTIFIERQUAL identifierstring $$99 EOS",
"$$100 :",
"remote_spec : VERIFY_IDENTIFIER SWITCH $$100 EOS",
"$$101 :",
"remote_spec : NONCE_SIZE NUMBER $$101 EOS",
"$$102 :",
"remote_spec : DH_GROUP $$102 dh_group_num EOS",
"$$103 :",
"remote_spec : PASSIVE SWITCH $$103 EOS",
"$$104 :",
"remote_spec : IKE_FRAG SWITCH $$104 EOS",
"$$105 :",
"remote_spec : IKE_FRAG REMOTE_FORCE_LEVEL $$105 EOS",
"$$106 :",
"remote_spec : ESP_FRAG NUMBER $$106 EOS",
"$$107 :",
"remote_spec : SCRIPT QUOTEDSTRING PHASE1_UP $$107 EOS",
"$$108 :",
"remote_spec : SCRIPT QUOTEDSTRING PHASE1_DOWN $$108 EOS",
"$$109 :",
"remote_spec : SCRIPT QUOTEDSTRING PHASE1_DEAD $$109 EOS",
"$$110 :",
"remote_spec : MODE_CFG SWITCH $$110 EOS",
"$$111 :",
"remote_spec : WEAK_PHASE1_CHECK SWITCH $$111 EOS",
"$$112 :",
"remote_spec : GENERATE_POLICY SWITCH $$112 EOS",
"$$113 :",
"remote_spec : GENERATE_POLICY GENERATE_LEVEL $$113 EOS",
"$$114 :",
"remote_spec : SUPPORT_PROXY SWITCH $$114 EOS",
"$$115 :",
"remote_spec : INITIAL_CONTACT SWITCH $$115 EOS",
"$$116 :",
"remote_spec : NAT_TRAVERSAL SWITCH $$116 EOS",
"$$117 :",
"remote_spec : NAT_TRAVERSAL REMOTE_FORCE_LEVEL $$117 EOS",
"$$118 :",
"remote_spec : DPD SWITCH $$118 EOS",
"$$119 :",
"remote_spec : DPD_DELAY NUMBER $$119 EOS",
"$$120 :",
"remote_spec : DPD_RETRY NUMBER $$120 EOS",
"$$121 :",
"remote_spec : DPD_MAXFAIL NUMBER $$121 EOS",
"$$122 :",
"remote_spec : REKEY SWITCH $$122 EOS",
"$$123 :",
"remote_spec : REKEY REMOTE_FORCE_LEVEL $$123 EOS",
"$$124 :",
"remote_spec : PH1ID NUMBER $$124 EOS",
"$$125 :",
"remote_spec : LIFETIME LIFETYPE_TIME NUMBER unittype_time $$125 EOS",
"$$126 :",
"remote_spec : PROPOSAL_CHECK PROPOSAL_CHECK_LEVEL $$126 EOS",
"$$127 :",
"remote_spec : LIFETIME LIFETYPE_BYTE NUMBER unittype_byte $$127 EOS",
"$$128 :",
"remote_spec : PROPOSAL $$128 BOC isakmpproposal_specs EOC",
"exchange_types :",
"exchange_types : exchange_types EXCHANGETYPE",
"$$129 :",
"cert_spec : CERT_X509 QUOTEDSTRING QUOTEDSTRING $$129 EOS",
"$$130 :",
"cert_spec : CERT_PLAINRSA QUOTEDSTRING $$130 EOS",
"dh_group_num : ALGORITHMTYPE",
"dh_group_num : NUMBER",
"identifierstring :",
"identifierstring : ADDRSTRING",
"identifierstring : QUOTEDSTRING",
"isakmpproposal_specs :",
"isakmpproposal_specs : isakmpproposal_specs isakmpproposal_spec",
"$$131 :",
"isakmpproposal_spec : LIFETIME LIFETYPE_TIME NUMBER unittype_time $$131 EOS",
"$$132 :",
"isakmpproposal_spec : LIFETIME LIFETYPE_BYTE NUMBER unittype_byte $$132 EOS",
"$$133 :",
"isakmpproposal_spec : DH_GROUP dh_group_num $$133 EOS",
"$$134 :",
"isakmpproposal_spec : GSS_ID QUOTEDSTRING $$134 EOS",
"$$135 :",
"isakmpproposal_spec : ALGORITHM_CLASS ALGORITHMTYPE keylength $$135 EOS",
"unittype_time : UNITTYPE_SEC",
"unittype_time : UNITTYPE_MIN",
"unittype_time : UNITTYPE_HOUR",
"unittype_byte : UNITTYPE_BYTE",
"unittype_byte : UNITTYPE_KBYTES",
"unittype_byte : UNITTYPE_MBYTES",
"unittype_byte : UNITTYPE_TBYTES",

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
#line 2399 "../../ipsec-tools/src/racoon/cfparse.y"

static struct secprotospec *
newspspec()
{
	struct secprotospec *new;

	new = racoon_calloc(1, sizeof(*new));
	if (new == NULL) {
		yyerror("failed to allocate spproto");
		return NULL;
	}

	new->encklen = 0;	/*XXX*/

	/*
	 * Default to "uknown" vendor -- we will override this
	 * as necessary.  When we send a Vendor ID payload, an
	 * "unknown" will be translated to a KAME/racoon ID.
	 */
	new->vendorid = VENDORID_UNKNOWN;

	return new;
}

/*
 * insert into head of list.
 */
static void
insspspec(rmconf, spspec)
	struct remoteconf *rmconf;
	struct secprotospec *spspec;
{
	if (rmconf->spspec != NULL)
		rmconf->spspec->prev = spspec;
	spspec->next = rmconf->spspec;
	rmconf->spspec = spspec;
}

static struct secprotospec *
dupspspec(spspec)
	struct secprotospec *spspec;
{
	struct secprotospec *new;

	new = newspspec();
	if (new == NULL) {
		plog(LLV_ERROR, LOCATION, NULL, 
		    "dupspspec: malloc failed\n");
		return NULL;
	}
	memcpy(new, spspec, sizeof(*new));

	if (spspec->gssid) {
		new->gssid = racoon_strdup(spspec->gssid);
		STRDUP_FATAL(new->gssid);
	}
	if (spspec->remote) {
		new->remote = racoon_malloc(sizeof(*new->remote));
		if (new->remote == NULL) {
			plog(LLV_ERROR, LOCATION, NULL, 
			    "dupspspec: malloc failed (remote)\n");
			return NULL;
		}
		memcpy(new->remote, spspec->remote, sizeof(*new->remote));
	}

	return new;
}

/*
 * copy the whole list
 */
void
dupspspec_list(dst, src)
	struct remoteconf *dst, *src;
{
	struct secprotospec *p, *new, *last;

	for(p = src->spspec, last = NULL; p; p = p->next, last = new) {
		new = dupspspec(p);
		if (new == NULL)
			exit(1);

		new->prev = last;
		new->next = NULL; /* not necessary but clean */

		if (last)
			last->next = new;
		else /* first element */
			dst->spspec = new;

	}
}

/*
 * delete the whole list
 */
void
flushspspec(rmconf)
	struct remoteconf *rmconf;
{
	struct secprotospec *p;

	while(rmconf->spspec != NULL) {
		p = rmconf->spspec;
		rmconf->spspec = p->next;
		if (p->next != NULL)
			p->next->prev = NULL; /* not necessary but clean */

		if (p->gssid)
			racoon_free(p->gssid);
		if (p->remote)
			racoon_free(p->remote);
		racoon_free(p);
	}
	rmconf->spspec = NULL;
}

/* set final acceptable proposal */
static int
set_isakmp_proposal(rmconf)
	struct remoteconf *rmconf;
{
	struct secprotospec *s;
	int prop_no = 1; 
	int trns_no = 1;
	int32_t types[MAXALGCLASS];

	/* mandatory check */
	if (rmconf->spspec == NULL) {
		yyerror("no remote specification found: %s.\n",
			saddr2str(rmconf->remote));
		return -1;
	}
	for (s = rmconf->spspec; s != NULL; s = s->next) {
		/* XXX need more to check */
		if (s->algclass[algclass_isakmp_enc] == 0) {
			yyerror("encryption algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_hash] == 0) {
			yyerror("hash algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_dh] == 0) {
			yyerror("DH group required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_ameth] == 0) {
			yyerror("authentication method required.");
			return -1;
		}
	}

	/* skip to last part */
	for (s = rmconf->spspec; s->next != NULL; s = s->next)
		;

	while (s != NULL) {
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifetime = %ld\n", (long)
			(s->lifetime ? s->lifetime : rmconf->lifetime));
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifebyte = %d\n",
			s->lifebyte ? s->lifebyte : rmconf->lifebyte);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"encklen=%d\n", s->encklen);

		memset(types, 0, ARRAYLEN(types));
		types[algclass_isakmp_enc] = s->algclass[algclass_isakmp_enc];
		types[algclass_isakmp_hash] = s->algclass[algclass_isakmp_hash];
		types[algclass_isakmp_dh] = s->algclass[algclass_isakmp_dh];
		types[algclass_isakmp_ameth] =
		    s->algclass[algclass_isakmp_ameth];

		/* expanding spspec */
		clean_tmpalgtype();
		trns_no = expand_isakmpspec(prop_no, trns_no, types,
				algclass_isakmp_enc, algclass_isakmp_ameth + 1,
				s->lifetime ? s->lifetime : rmconf->lifetime,
				s->lifebyte ? s->lifebyte : rmconf->lifebyte,
				s->encklen, s->vendorid, s->gssid,
				rmconf);
		if (trns_no == -1) {
			plog(LLV_ERROR, LOCATION, NULL,
				"failed to expand isakmp proposal.\n");
			return -1;
		}

		s = s->prev;
	}

	if (rmconf->proposal == NULL) {
		plog(LLV_ERROR, LOCATION, NULL,
			"no proposal found.\n");
		return -1;
	}

	return 0;
}

static void
clean_tmpalgtype()
{
	int i;
	for (i = 0; i < MAXALGCLASS; i++)
		tmpalgtype[i] = 0;	/* means algorithm undefined. */
}

static int
expand_isakmpspec(prop_no, trns_no, types,
		class, last, lifetime, lifebyte, encklen, vendorid, gssid,
		rmconf)
	int prop_no, trns_no;
	int *types, class, last;
	time_t lifetime;
	int lifebyte;
	int encklen;
	int vendorid;
	char *gssid;
	struct remoteconf *rmconf;
{
	struct isakmpsa *new;

	/* debugging */
    {
	int j;
	char tb[10];
	plog(LLV_DEBUG2, LOCATION, NULL,
		"p:%d t:%d\n", prop_no, trns_no);
	for (j = class; j < MAXALGCLASS; j++) {
		snprintf(tb, sizeof(tb), "%d", types[j]);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"%s%s%s%s\n",
			s_algtype(j, types[j]),
			types[j] ? "(" : "",
			tb[0] == '0' ? "" : tb,
			types[j] ? ")" : "");
	}
	plog(LLV_DEBUG2, LOCATION, NULL, "\n");
    }

#define TMPALGTYPE2STR(n) \
	s_algtype(algclass_isakmp_##n, types[algclass_isakmp_##n])
		/* check mandatory values */
		if (types[algclass_isakmp_enc] == 0
		 || types[algclass_isakmp_ameth] == 0
		 || types[algclass_isakmp_hash] == 0
		 || types[algclass_isakmp_dh] == 0) {
			yyerror("few definition of algorithm "
				"enc=%s ameth=%s hash=%s dhgroup=%s.\n",
				TMPALGTYPE2STR(enc),
				TMPALGTYPE2STR(ameth),
				TMPALGTYPE2STR(hash),
				TMPALGTYPE2STR(dh));
			return -1;
		}
#undef TMPALGTYPE2STR

	/* set new sa */
	new = newisakmpsa();
	if (new == NULL) {
		yyerror("failed to allocate isakmp sa");
		return -1;
	}
	new->prop_no = prop_no;
	new->trns_no = trns_no++;
	new->lifetime = lifetime;
	new->lifebyte = lifebyte;
	new->enctype = types[algclass_isakmp_enc];
	new->encklen = encklen;
	new->authmethod = types[algclass_isakmp_ameth];
	new->hashtype = types[algclass_isakmp_hash];
	new->dh_group = types[algclass_isakmp_dh];
	new->vendorid = vendorid;
#ifdef HAVE_GSSAPI
	if (new->authmethod == OAKLEY_ATTR_AUTH_METHOD_GSSAPI_KRB) {
		if (gssid != NULL) {
			if ((new->gssid = vmalloc(strlen(gssid))) == NULL) {
				racoon_free(new);
				yyerror("failed to allocate gssid");
				return -1;
			}
			memcpy(new->gssid->v, gssid, new->gssid->l);
			racoon_free(gssid);
		} else {
			/*
			 * Allocate the default ID so that it gets put
			 * into a GSS ID attribute during the Phase 1
			 * exchange.
			 */
			new->gssid = gssapi_get_default_gss_id();
		}
	}
#endif
	insisakmpsa(new, rmconf);

	return trns_no;
}

#if 0
/*
 * fix lifebyte.
 * Must be more than 1024B because its unit is kilobytes.
 * That is defined RFC2407.
 */
static int
fix_lifebyte(t)
	unsigned long t;
{
	if (t < 1024) {
		yyerror("byte size should be more than 1024B.");
		return 0;
	}

	return(t / 1024);
}
#endif

int
cfparse()
{
	int error;

	yyerrorcount = 0;
	yycf_init_buffer();

	if (yycf_switch_buffer(lcconf->racoon_conf) != 0) {
		plog(LLV_ERROR, LOCATION, NULL, 
		    "could not read configuration file \"%s\"\n", 
		    lcconf->racoon_conf);
		return -1;
	}

	error = yyparse();
	if (error != 0) {
		if (yyerrorcount) {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure (%d errors)\n",
				yyerrorcount);
		} else {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure.\n");
		}
		return -1;
	}

	if (error == 0 && yyerrorcount) {
		plog(LLV_ERROR, LOCATION, NULL,
			"parse error is nothing, but yyerrorcount is %d.\n",
				yyerrorcount);
		exit(1);
	}

	yycf_clean_buffer();

	plog(LLV_DEBUG2, LOCATION, NULL, "parse successed.\n");

	return 0;
}

int
cfreparse()
{
	flushph2();
	flushph1();
	flushrmconf();
	flushsainfo();
	clean_tmpalgtype();
	return(cfparse());
}

#ifdef ENABLE_ADMINPORT
static void
adminsock_conf(path, owner, group, mode_dec)
	vchar_t *path;
	vchar_t *owner;
	vchar_t *group;
	int mode_dec;
{
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	mode_t mode = 0;
	uid_t uid;
	gid_t gid;
	int isnum;

	adminsock_path = path->v;

	if (owner == NULL)
		return;

	errno = 0;
	uid = atoi(owner->v);
	isnum = !errno;
	if (((pw = getpwnam(owner->v)) == NULL) && !isnum)
		yyerror("User \"%s\" does not exist", owner->v);

	if (pw)
		adminsock_owner = pw->pw_uid;
	else
		adminsock_owner = uid;

	if (group == NULL)
		return;

	errno = 0;
	gid = atoi(group->v);
	isnum = !errno;
	if (((gr = getgrnam(group->v)) == NULL) && !isnum)
		yyerror("Group \"%s\" does not exist", group->v);

	if (gr)
		adminsock_group = gr->gr_gid;
	else
		adminsock_group = gid;

	if (mode_dec == -1)
		return;

	if (mode_dec > 777)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 400) { mode += 0400; mode_dec -= 400; }
	if (mode_dec >= 200) { mode += 0200; mode_dec -= 200; }
	if (mode_dec >= 100) { mode += 0200; mode_dec -= 100; }

	if (mode_dec > 77)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 40) { mode += 040; mode_dec -= 40; }
	if (mode_dec >= 20) { mode += 020; mode_dec -= 20; }
	if (mode_dec >= 10) { mode += 020; mode_dec -= 10; }

	if (mode_dec > 7)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 4) { mode += 04; mode_dec -= 4; }
	if (mode_dec >= 2) { mode += 02; mode_dec -= 2; }
	if (mode_dec >= 1) { mode += 02; mode_dec -= 1; }
	
	adminsock_mode = mode;

	return;
}
#endif
#line 1933 "racoonyy.tab.c"

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
case 21:
#line 368 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct passwd *pw;

			if ((pw = getpwnam(yystack.l_mark[0].val->v)) == NULL) {
				yyerror("unknown user \"%s\"", yystack.l_mark[0].val->v);
				return -1;
			}
			lcconf->uid = pw->pw_uid;
		}
break;
case 23:
#line 378 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->uid = yystack.l_mark[0].num; }
break;
case 25:
#line 380 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct group *gr;

			if ((gr = getgrnam(yystack.l_mark[0].val->v)) == NULL) {
				yyerror("unknown group \"%s\"", yystack.l_mark[0].val->v);
				return -1;
			}
			lcconf->gid = gr->gr_gid;
		}
break;
case 27:
#line 390 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->gid = yystack.l_mark[0].num; }
break;
case 29:
#line 391 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->chroot = yystack.l_mark[0].val->v; }
break;
case 31:
#line 397 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (yystack.l_mark[-1].num >= LC_PATHTYPE_MAX) {
				yyerror("invalid path type %d", yystack.l_mark[-1].num);
				return -1;
			}

			/* free old pathinfo */
			if (lcconf->pathinfo[yystack.l_mark[-1].num])
				racoon_free(lcconf->pathinfo[yystack.l_mark[-1].num]);

			/* set new pathinfo */
			lcconf->pathinfo[yystack.l_mark[-1].num] = racoon_strdup(yystack.l_mark[0].val->v);
			STRDUP_FATAL(lcconf->pathinfo[yystack.l_mark[-1].num]);
			vfree(yystack.l_mark[0].val);
		}
break;
case 33:
#line 417 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->complex_bundle = yystack.l_mark[0].num; }
break;
case 35:
#line 423 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char path[MAXPATHLEN];

			getpathname(path, sizeof(path),
				LC_PATHTYPE_INCLUDE, yystack.l_mark[-1].val->v);
			vfree(yystack.l_mark[-1].val);
			if (yycf_switch_buffer(path) != 0)
				return -1;
		}
break;
case 36:
#line 437 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->pfkey_buffer_size = yystack.l_mark[-1].num;
        }
break;
case 37:
#line 444 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (yystack.l_mark[-1].num >= LC_GSSENC_MAX) {
				yyerror("invalid GSS ID encoding %d", yystack.l_mark[-1].num);
				return -1;
			}
			lcconf->gss_id_enc = yystack.l_mark[-1].num;
		}
break;
case 39:
#line 459 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			/*
			 * set the loglevel to the value specified
			 * in the configuration file plus the number
			 * of -d options specified on the command line
			 */
			loglevel += yystack.l_mark[0].num - oldloglevel;
			oldloglevel = yystack.l_mark[0].num;
		}
break;
case 43:
#line 479 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->pad_random = yystack.l_mark[0].num; }
break;
case 45:
#line 480 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->pad_randomlen = yystack.l_mark[0].num; }
break;
case 47:
#line 481 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->pad_maxsize = yystack.l_mark[0].num; }
break;
case 49:
#line 482 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->pad_strict = yystack.l_mark[0].num; }
break;
case 51:
#line 483 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->pad_excltail = yystack.l_mark[0].num; }
break;
case 56:
#line 496 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			myaddr_listen(yystack.l_mark[0].saddr, FALSE);
			racoon_free(yystack.l_mark[0].saddr);
		}
break;
case 58:
#line 502 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_NATT
			myaddr_listen(yystack.l_mark[0].saddr, TRUE);
			racoon_free(yystack.l_mark[0].saddr);
#else
			racoon_free(yystack.l_mark[0].saddr);
			yyerror("NAT-T support not compiled in.");
#endif
		}
break;
case 60:
#line 513 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_ADMINPORT
			adminsock_conf(yystack.l_mark[-3].val, yystack.l_mark[-2].val, yystack.l_mark[-1].val, yystack.l_mark[0].num);
#else
			yywarn("admin port support not compiled in");
#endif
		}
break;
case 62:
#line 522 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_ADMINPORT
			adminsock_conf(yystack.l_mark[0].val, NULL, NULL, -1);
#else
			yywarn("admin port support not compiled in");
#endif
		}
break;
case 64:
#line 531 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_ADMINPORT
			adminsock_path = NULL;
#else
			yywarn("admin port support not compiled in");
#endif
		}
break;
case 66:
#line 539 "../../ipsec-tools/src/racoon/cfparse.y"
	{ lcconf->strict_address = TRUE; }
break;
case 68:
#line 543 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char portbuf[10];

			snprintf(portbuf, sizeof(portbuf), "%ld", yystack.l_mark[0].num);
			yyval.saddr = str2saddr(yystack.l_mark[-1].val->v, portbuf);
			vfree(yystack.l_mark[-1].val);
			if (!yyval.saddr)
				return -1;
		}
break;
case 69:
#line 554 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = PORT_ISAKMP; }
break;
case 70:
#line 555 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 71:
#line 560 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifndef ENABLE_HYBRID
			yyerror("racoon not configured with --enable-hybrid");
			return -1;
#endif
#ifndef HAVE_LIBRADIUS
			yyerror("racoon not configured with --with-libradius");
			return -1;
#endif
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			xauth_rad_config.timeout = 3;
			xauth_rad_config.retries = 3;
#endif
#endif
		}
break;
case 75:
#line 583 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			int i = xauth_rad_config.auth_server_count;
			if (i == RADIUS_MAX_SERVERS) {
				yyerror("maximum radius auth servers exceeded");
				return -1;
			}

			xauth_rad_config.auth_server_list[i].host = vdup(yystack.l_mark[-1].val);
			xauth_rad_config.auth_server_list[i].secret = vdup(yystack.l_mark[0].val);
			xauth_rad_config.auth_server_list[i].port = 0; /* default port*/
			xauth_rad_config.auth_server_count++;
#endif
#endif
		}
break;
case 77:
#line 601 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			int i = xauth_rad_config.auth_server_count;
			if (i == RADIUS_MAX_SERVERS) {
				yyerror("maximum radius auth servers exceeded");
				return -1;
			}

			xauth_rad_config.auth_server_list[i].host = vdup(yystack.l_mark[-2].val);
			xauth_rad_config.auth_server_list[i].secret = vdup(yystack.l_mark[0].val);
			xauth_rad_config.auth_server_list[i].port = yystack.l_mark[-1].num;
			xauth_rad_config.auth_server_count++;
#endif
#endif
		}
break;
case 79:
#line 619 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			int i = xauth_rad_config.acct_server_count;
			if (i == RADIUS_MAX_SERVERS) {
				yyerror("maximum radius account servers exceeded");
				return -1;
			}

			xauth_rad_config.acct_server_list[i].host = vdup(yystack.l_mark[-1].val);
			xauth_rad_config.acct_server_list[i].secret = vdup(yystack.l_mark[0].val);
			xauth_rad_config.acct_server_list[i].port = 0; /* default port*/
			xauth_rad_config.acct_server_count++;
#endif
#endif
		}
break;
case 81:
#line 637 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			int i = xauth_rad_config.acct_server_count;
			if (i == RADIUS_MAX_SERVERS) {
				yyerror("maximum radius account servers exceeded");
				return -1;
			}

			xauth_rad_config.acct_server_list[i].host = vdup(yystack.l_mark[-2].val);
			xauth_rad_config.acct_server_list[i].secret = vdup(yystack.l_mark[0].val);
			xauth_rad_config.acct_server_list[i].port = yystack.l_mark[-1].num;
			xauth_rad_config.acct_server_count++;
#endif
#endif
		}
break;
case 83:
#line 655 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			xauth_rad_config.timeout = yystack.l_mark[0].num;
#endif
#endif
		}
break;
case 85:
#line 664 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			xauth_rad_config.retries = yystack.l_mark[0].num;
#endif
#endif
		}
break;
case 87:
#line 676 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifndef ENABLE_HYBRID
			yyerror("racoon not configured with --enable-hybrid");
			return -1;
#endif
#ifndef HAVE_LIBLDAP
			yyerror("racoon not configured with --with-libldap");
			return -1;
#endif
		}
break;
case 91:
#line 693 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if ((yystack.l_mark[0].num<2)||(yystack.l_mark[0].num>3))
				yyerror("invalid ldap protocol version (2|3)");
			xauth_ldap_config.pver = yystack.l_mark[0].num;
#endif
#endif
		}
break;
case 93:
#line 704 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.host != NULL)
				vfree(xauth_ldap_config.host);
			xauth_ldap_config.host = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 95:
#line 715 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			xauth_ldap_config.port = yystack.l_mark[0].num;
#endif
#endif
		}
break;
case 97:
#line 724 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.base != NULL)
				vfree(xauth_ldap_config.base);
			xauth_ldap_config.base = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 99:
#line 735 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			xauth_ldap_config.subtree = yystack.l_mark[0].num;
#endif
#endif
		}
break;
case 101:
#line 744 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.bind_dn != NULL)
				vfree(xauth_ldap_config.bind_dn);
			xauth_ldap_config.bind_dn = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 103:
#line 755 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.bind_pw != NULL)
				vfree(xauth_ldap_config.bind_pw);
			xauth_ldap_config.bind_pw = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 105:
#line 766 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.attr_user != NULL)
				vfree(xauth_ldap_config.attr_user);
			xauth_ldap_config.attr_user = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 107:
#line 777 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.attr_addr != NULL)
				vfree(xauth_ldap_config.attr_addr);
			xauth_ldap_config.attr_addr = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 109:
#line 788 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.attr_mask != NULL)
				vfree(xauth_ldap_config.attr_mask);
			xauth_ldap_config.attr_mask = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 111:
#line 799 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.attr_group != NULL)
				vfree(xauth_ldap_config.attr_group);
			xauth_ldap_config.attr_group = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 113:
#line 810 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			if (xauth_ldap_config.attr_member != NULL)
				vfree(xauth_ldap_config.attr_member);
			xauth_ldap_config.attr_member = vdup(yystack.l_mark[0].val);
#endif
#endif
		}
break;
case 118:
#line 832 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, yystack.l_mark[0].val->v,
			     &isakmp_cfg_config.network4) != 1)
				yyerror("bad IPv4 network address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 120:
#line 843 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, yystack.l_mark[0].val->v,
			    &isakmp_cfg_config.netmask4) != 1)
				yyerror("bad IPv4 netmask address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 124:
#line 858 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.splitnet_type = UNITY_LOCAL_LAN;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 126:
#line 867 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.splitnet_type = UNITY_SPLIT_INCLUDE;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 128:
#line 876 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifndef ENABLE_HYBRID
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 130:
#line 883 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			strncpy(&isakmp_cfg_config.default_domain[0], 
			    yystack.l_mark[0].val->v, MAXPATHLEN);
			isakmp_cfg_config.default_domain[MAXPATHLEN] = '\0';
			vfree(yystack.l_mark[0].val);
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 132:
#line 895 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_SYSTEM;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 134:
#line 904 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 136:
#line 917 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBPAM
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_PAM;
#else /* HAVE_LIBPAM */
			yyerror("racoon not configured with --with-libpam");
#endif /* HAVE_LIBPAM */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 138:
#line 930 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_LDAP;
#else /* HAVE_LIBLDAP */
			yyerror("racoon not configured with --with-libldap");
#endif /* HAVE_LIBLDAP */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 140:
#line 943 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifndef ENABLE_HYBRID
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 142:
#line 950 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.groupsource = ISAKMP_CFG_GROUP_SYSTEM;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 144:
#line 959 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			isakmp_cfg_config.groupsource = ISAKMP_CFG_GROUP_LDAP;
#else /* HAVE_LIBLDAP */
			yyerror("racoon not configured with --with-libldap");
#endif /* HAVE_LIBLDAP */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 146:
#line 972 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_NONE;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 148:
#line 981 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_SYSTEM;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 150:
#line 990 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 152:
#line 1003 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBPAM
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_PAM;
#else /* HAVE_LIBPAM */
			yyerror("racoon not configured with --with-libpam");
#endif /* HAVE_LIBPAM */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 154:
#line 1016 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			if (isakmp_cfg_resize_pool(yystack.l_mark[0].num) != 0)
				yyerror("cannot allocate memory for pool");
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 156:
#line 1026 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.pfs_group = yystack.l_mark[0].num;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 158:
#line 1035 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.save_passwd = yystack.l_mark[0].num;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 160:
#line 1044 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.auth_throttle = yystack.l_mark[0].num;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 162:
#line 1053 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_LOCAL;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 164:
#line 1062 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 166:
#line 1075 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBLDAP
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_LDAP;
#else /* HAVE_LIBLDAP */
			yyerror("racoon not configured with --with-libldap");
#endif /* HAVE_LIBLDAP */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
break;
case 168:
#line 1088 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			strncpy(&isakmp_cfg_config.motd[0], yystack.l_mark[0].val->v, MAXPATHLEN);
			isakmp_cfg_config.motd[MAXPATHLEN] = '\0';
			vfree(yystack.l_mark[0].val);
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 172:
#line 1106 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			struct isakmp_cfg_config *icc = &isakmp_cfg_config;

			if (icc->dns4_index > MAXNS)
				yyerror("No more than %d DNS", MAXNS);
			if (inet_pton(AF_INET, yystack.l_mark[0].val->v,
			    &icc->dns4[icc->dns4_index++]) != 1)
				yyerror("bad IPv4 DNS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 175:
#line 1127 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			struct isakmp_cfg_config *icc = &isakmp_cfg_config;

			if (icc->nbns4_index > MAXWINS)
				yyerror("No more than %d WINS", MAXWINS);
			if (inet_pton(AF_INET, yystack.l_mark[0].val->v,
			    &icc->nbns4[icc->nbns4_index++]) != 1)
				yyerror("bad IPv4 WINS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 178:
#line 1148 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			struct isakmp_cfg_config *icc = &isakmp_cfg_config;
			struct unity_network network;
			memset(&network,0,sizeof(network));

			if (inet_pton(AF_INET, yystack.l_mark[-1].val->v, &network.addr4) != 1)
				yyerror("bad IPv4 SPLIT address.");

			/* Turn $2 (the prefix) into a subnet mask */
			network.mask4.s_addr = (yystack.l_mark[0].num) ? htonl(~((1 << (32 - yystack.l_mark[0].num)) - 1)) : 0;

			/* add the network to our list */ 
			if (splitnet_list_add(&icc->splitnet_list, &network,&icc->splitnet_count))
				yyerror("Unable to allocate split network");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 181:
#line 1175 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			char * groupname = NULL;
			char ** grouplist = NULL;
			struct isakmp_cfg_config *icc = &isakmp_cfg_config;

			grouplist = racoon_realloc(icc->grouplist,
					sizeof(char**)*(icc->groupcount+1));
			if (grouplist == NULL) {
				yyerror("unable to allocate auth group list");
				return -1;
			}

			groupname = racoon_malloc(yystack.l_mark[0].val->l+1);
			if (groupname == NULL) {
				yyerror("unable to allocate auth group name");
				return -1;
			}

			memcpy(groupname,yystack.l_mark[0].val->v,yystack.l_mark[0].val->l);
			groupname[yystack.l_mark[0].val->l]=0;
			grouplist[icc->groupcount]=groupname;
			icc->grouplist = grouplist;
			icc->groupcount++;

			vfree(yystack.l_mark[0].val);
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 184:
#line 1213 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			struct isakmp_cfg_config *icc = &isakmp_cfg_config;

			if (!icc->splitdns_len)
			{
				icc->splitdns_list = racoon_malloc(yystack.l_mark[0].val->l);
				if(icc->splitdns_list == NULL) {
					yyerror("error allocating splitdns list buffer");
					return -1;
				}
				memcpy(icc->splitdns_list,yystack.l_mark[0].val->v,yystack.l_mark[0].val->l);
				icc->splitdns_len = yystack.l_mark[0].val->l;
			}
			else
			{
				int len = icc->splitdns_len + yystack.l_mark[0].val->l + 1;
				icc->splitdns_list = racoon_realloc(icc->splitdns_list,len);
				if(icc->splitdns_list == NULL) {
					yyerror("error allocating splitdns list buffer");
					return -1;
				}
				icc->splitdns_list[icc->splitdns_len] = ',';
				memcpy(icc->splitdns_list + icc->splitdns_len + 1, yystack.l_mark[0].val->v, yystack.l_mark[0].val->l);
				icc->splitdns_len = len;
			}
			vfree(yystack.l_mark[0].val);
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 188:
#line 1257 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->retry_counter = yystack.l_mark[0].num;
		}
break;
case 190:
#line 1262 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->retry_interval = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 192:
#line 1267 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->count_persend = yystack.l_mark[0].num;
		}
break;
case 194:
#line 1272 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->retry_checkph1 = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 196:
#line 1277 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			lcconf->wait_ph2complete = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 198:
#line 1282 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_NATT
        		if (libipsec_opt & LIBIPSEC_OPT_NATT)
				lcconf->natt_ka_interval = yystack.l_mark[-1].num * yystack.l_mark[0].num;
			else
                		yyerror("libipsec lacks NAT-T support");
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
break;
case 200:
#line 1298 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo = newsainfo();
			if (cur_sainfo == NULL) {
				yyerror("failed to allocate sainfo");
				return -1;
			}
		}
break;
case 201:
#line 1306 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct sainfo *check;

			/* default */
			if (cur_sainfo->algs[algclass_ipsec_enc] == 0) {
				yyerror("no encryption algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_auth] == 0) {
				yyerror("no authentication algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_comp] == 0) {
				yyerror("no compression algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}

			/* duplicate check */
			check = getsainfo(cur_sainfo->idsrc,
					  cur_sainfo->iddst,
					  cur_sainfo->id_i,
					  NULL,
					  cur_sainfo->remoteid);

			if (check && ((check->idsrc != SAINFO_ANONYMOUS) &&
				      (cur_sainfo->idsrc != SAINFO_ANONYMOUS))) {
				yyerror("duplicated sainfo: %s",
					sainfo2str(cur_sainfo));
				return -1;
			}

			inssainfo(cur_sainfo);
		}
break;
case 203:
#line 1346 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = SAINFO_ANONYMOUS;
			cur_sainfo->iddst = SAINFO_ANONYMOUS;
		}
break;
case 204:
#line 1351 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = SAINFO_ANONYMOUS;
			cur_sainfo->iddst = SAINFO_CLIENTADDR;
		}
break;
case 205:
#line 1356 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = SAINFO_ANONYMOUS;
			cur_sainfo->iddst = yystack.l_mark[0].val;
		}
break;
case 206:
#line 1361 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = yystack.l_mark[-1].val;
			cur_sainfo->iddst = SAINFO_ANONYMOUS;
		}
break;
case 207:
#line 1366 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = yystack.l_mark[-1].val;
			cur_sainfo->iddst = SAINFO_CLIENTADDR;
		}
break;
case 208:
#line 1371 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->idsrc = yystack.l_mark[-1].val;
			cur_sainfo->iddst = yystack.l_mark[0].val;
		}
break;
case 209:
#line 1378 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char portbuf[10];
			struct sockaddr *saddr;

			if ((yystack.l_mark[0].num == IPPROTO_ICMP || yystack.l_mark[0].num == IPPROTO_ICMPV6)
			 && (yystack.l_mark[-1].num != IPSEC_PORT_ANY || yystack.l_mark[-1].num != IPSEC_PORT_ANY)) {
				yyerror("port number must be \"any\".");
				return -1;
			}

			snprintf(portbuf, sizeof(portbuf), "%lu", yystack.l_mark[-1].num);
			saddr = str2saddr(yystack.l_mark[-3].val->v, portbuf);
			vfree(yystack.l_mark[-3].val);
			if (saddr == NULL)
				return -1;

			switch (saddr->sa_family) {
			case AF_INET:
				if (yystack.l_mark[0].num == IPPROTO_ICMPV6) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				yyval.val = ipsecdoi_sockaddr2id(saddr,
										  yystack.l_mark[-2].num == ~0 ? (sizeof(struct in_addr) << 3): yystack.l_mark[-2].num,
										  yystack.l_mark[0].num);
				break;
#ifdef INET6
			case AF_INET6:
				if (yystack.l_mark[0].num == IPPROTO_ICMP) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				yyval.val = ipsecdoi_sockaddr2id(saddr, 
										  yystack.l_mark[-2].num == ~0 ? (sizeof(struct in6_addr) << 3): yystack.l_mark[-2].num,
										  yystack.l_mark[0].num);
				break;
#endif
			default:
				yyerror("invalid family: %d", saddr->sa_family);
				yyval.val = NULL;
				break;
			}
			racoon_free(saddr);
			if (yyval.val == NULL)
				return -1;
		}
break;
case 210:
#line 1427 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char portbuf[10];
			struct sockaddr *laddr = NULL, *haddr = NULL;
			char *cur = NULL;

			if ((yystack.l_mark[0].num == IPPROTO_ICMP || yystack.l_mark[0].num == IPPROTO_ICMPV6)
			 && (yystack.l_mark[-1].num != IPSEC_PORT_ANY || yystack.l_mark[-1].num != IPSEC_PORT_ANY)) {
				yyerror("port number must be \"any\".");
				return -1;
			}

			snprintf(portbuf, sizeof(portbuf), "%lu", yystack.l_mark[-1].num);
			
			laddr = str2saddr(yystack.l_mark[-4].val->v, portbuf);
			if (laddr == NULL) {
			    return -1;
			}
			vfree(yystack.l_mark[-4].val);
			haddr = str2saddr(yystack.l_mark[-3].val->v, portbuf);
			if (haddr == NULL) {
			    racoon_free(laddr);
			    return -1;
			}
			vfree(yystack.l_mark[-3].val);

			switch (laddr->sa_family) {
			case AF_INET:
				if (yystack.l_mark[0].num == IPPROTO_ICMPV6) {
				    yyerror("upper layer protocol mismatched.\n");
				    if (laddr)
					racoon_free(laddr);
				    if (haddr)
					racoon_free(haddr);
				    return -1;
				}
                                yyval.val = ipsecdoi_sockrange2id(laddr, haddr, 
							   yystack.l_mark[0].num);
				break;
#ifdef INET6
			case AF_INET6:
				if (yystack.l_mark[0].num == IPPROTO_ICMP) {
					yyerror("upper layer protocol mismatched.\n");
					if (laddr)
					    racoon_free(laddr);
					if (haddr)
					    racoon_free(haddr);
					return -1;
				}
				yyval.val = ipsecdoi_sockrange2id(laddr, haddr, 
							       yystack.l_mark[0].num);
				break;
#endif
			default:
				yyerror("invalid family: %d", laddr->sa_family);
				yyval.val = NULL;
				break;
			}
			if (laddr)
			    racoon_free(laddr);
			if (haddr)
			    racoon_free(haddr);
			if (yyval.val == NULL)
				return -1;
		}
break;
case 211:
#line 1492 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct ipsecdoi_id_b *id_b;

			if (yystack.l_mark[-1].num == IDTYPE_ASN1DN) {
				yyerror("id type forbidden: %d", yystack.l_mark[-1].num);
				yyval.val = NULL;
				return -1;
			}

			yystack.l_mark[0].val->l--;

			yyval.val = vmalloc(sizeof(*id_b) + yystack.l_mark[0].val->l);
			if (yyval.val == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)yyval.val->v;
			id_b->type = idtype2doi(yystack.l_mark[-1].num);

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy(yyval.val->v + sizeof(*id_b), yystack.l_mark[0].val->v, yystack.l_mark[0].val->l);
		}
break;
case 212:
#line 1520 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->id_i = NULL;
		}
break;
case 213:
#line 1524 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct ipsecdoi_id_b *id_b;
			vchar_t *idv;

			if (set_identifier(&idv, yystack.l_mark[-1].num, yystack.l_mark[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_sainfo->id_i = vmalloc(sizeof(*id_b) + idv->l);
			if (cur_sainfo->id_i == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)cur_sainfo->id_i->v;
			id_b->type = idtype2doi(yystack.l_mark[-1].num);

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy(cur_sainfo->id_i->v + sizeof(*id_b),
			       idv->v, idv->l);
			vfree(idv);
		}
break;
case 214:
#line 1549 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			if ((cur_sainfo->group = vdup(yystack.l_mark[0].val)) == NULL) {
				yyerror("failed to set sainfo xauth group.\n");
				return -1;
			}
#else
			yyerror("racoon not configured with --enable-hybrid");
			return -1;
#endif
 		}
break;
case 217:
#line 1567 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->pfs_group = yystack.l_mark[0].num;
		}
break;
case 219:
#line 1572 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->remoteid = yystack.l_mark[0].num;
		}
break;
case 221:
#line 1577 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_sainfo->lifetime = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 223:
#line 1582 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_sainfo->lifebyte = fix_lifebyte(yystack.l_mark[-1].num * yystack.l_mark[0].num);
			if (cur_sainfo->lifebyte == 0)
				return -1;
#endif
		}
break;
case 225:
#line 1593 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_algclass = yystack.l_mark[0].num;
		}
break;
case 227:
#line 1601 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			inssainfoalg(&cur_sainfo->algs[cur_algclass], yystack.l_mark[0].alg);
		}
break;
case 228:
#line 1605 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			inssainfoalg(&cur_sainfo->algs[cur_algclass], yystack.l_mark[0].alg);
		}
break;
case 230:
#line 1612 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			int defklen;

			yyval.alg = newsainfoalg();
			if (yyval.alg == NULL) {
				yyerror("failed to get algorithm allocation");
				return -1;
			}

			yyval.alg->alg = algtype2doi(cur_algclass, yystack.l_mark[-1].num);
			if (yyval.alg->alg == -1) {
				yyerror("algorithm mismatched");
				racoon_free(yyval.alg);
				yyval.alg = NULL;
				return -1;
			}

			defklen = default_keylen(cur_algclass, yystack.l_mark[-1].num);
			if (defklen == 0) {
				if (yystack.l_mark[0].num) {
					yyerror("keylen not allowed");
					racoon_free(yyval.alg);
					yyval.alg = NULL;
					return -1;
				}
			} else {
				if (yystack.l_mark[0].num && check_keylen(cur_algclass, yystack.l_mark[-1].num, yystack.l_mark[0].num) < 0) {
					yyerror("invalid keylen %d", yystack.l_mark[0].num);
					racoon_free(yyval.alg);
					yyval.alg = NULL;
					return -1;
				}
			}

			if (yystack.l_mark[0].num)
				yyval.alg->encklen = yystack.l_mark[0].num;
			else
				yyval.alg->encklen = defklen;

			/* check if it's supported algorithm by kernel */
			if (!(cur_algclass == algclass_ipsec_auth && yystack.l_mark[-1].num == algtype_non_auth)
			 && pk_checkalg(cur_algclass, yystack.l_mark[-1].num, yyval.alg->encklen)) {
				int a = algclass2doi(cur_algclass);
				int b = algtype2doi(cur_algclass, yystack.l_mark[-1].num);
				if (a == IPSECDOI_ATTR_AUTH)
					a = IPSECDOI_PROTO_IPSEC_AH;
				yyerror("algorithm %s not supported by the kernel (missing module?)",
					s_ipsecdoi_trns(a, b));
				racoon_free(yyval.alg);
				yyval.alg = NULL;
				return -1;
			}
		}
break;
case 231:
#line 1667 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = ~0; }
break;
case 232:
#line 1668 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 233:
#line 1671 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = IPSEC_PORT_ANY; }
break;
case 234:
#line 1672 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 235:
#line 1673 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = IPSEC_PORT_ANY; }
break;
case 236:
#line 1676 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 237:
#line 1677 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 238:
#line 1678 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = IPSEC_ULPROTO_ANY; }
break;
case 239:
#line 1681 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = 0; }
break;
case 240:
#line 1682 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 241:
#line 1688 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct remoteconf *from, *new;

			if (getrmconf_by_name(yystack.l_mark[-2].val->v) != NULL) {
				yyerror("named remoteconf \"%s\" already exists.");
				return -1;
			}

			from = getrmconf_by_name(yystack.l_mark[0].val->v);
			if (from == NULL) {
				yyerror("named parent remoteconf \"%s\" does not exist.",
					yystack.l_mark[0].val->v);
				return -1;
			}

			new = duprmconf_shallow(from);
			if (new == NULL) {
				yyerror("failed to duplicate remoteconf from \"%s\".",
					yystack.l_mark[0].val->v);
				return -1;
			}

			new->name = racoon_strdup(yystack.l_mark[-2].val->v);
			cur_rmconf = new;

			vfree(yystack.l_mark[-2].val);
			vfree(yystack.l_mark[0].val);
		}
break;
case 243:
#line 1718 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct remoteconf *new;

			if (getrmconf_by_name(yystack.l_mark[0].val->v) != NULL) {
				yyerror("Named remoteconf \"%s\" already exists.");
				return -1;
			}

			new = newrmconf();
			if (new == NULL) {
				yyerror("failed to get new remoteconf.");
				return -1;
			}
			new->name = racoon_strdup(yystack.l_mark[0].val->v);
			cur_rmconf = new;

			vfree(yystack.l_mark[0].val);
		}
break;
case 245:
#line 1738 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct remoteconf *from, *new;

			from = getrmconf(yystack.l_mark[0].saddr, GETRMCONF_F_NO_ANONYMOUS);
			if (from == NULL) {
				yyerror("failed to get remoteconf for %s.",
					saddr2str(yystack.l_mark[0].saddr));
				return -1;
			}

			new = duprmconf_shallow(from);
			if (new == NULL) {
				yyerror("failed to duplicate remoteconf from %s.",
					saddr2str(yystack.l_mark[0].saddr));
				return -1;
			}

			racoon_free(yystack.l_mark[0].saddr);
			new->remote = yystack.l_mark[-2].saddr;
			cur_rmconf = new;
		}
break;
case 247:
#line 1761 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct remoteconf *new;

			new = newrmconf();
			if (new == NULL) {
				yyerror("failed to get new remoteconf.");
				return -1;
			}

			new->remote = yystack.l_mark[0].saddr;
			cur_rmconf = new;
		}
break;
case 250:
#line 1779 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (process_rmconf() != 0)
				return -1;
		}
break;
case 251:
#line 1787 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (process_rmconf() != 0)
				return -1;
		}
break;
case 252:
#line 1794 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			yyval.saddr = newsaddr(sizeof(struct sockaddr));
			yyval.saddr->sa_family = AF_UNSPEC;
			((struct sockaddr_in *)yyval.saddr)->sin_port = htons(yystack.l_mark[0].num);
		}
break;
case 253:
#line 1800 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			yyval.saddr = yystack.l_mark[0].saddr;
			if (yyval.saddr == NULL) {
				yyerror("failed to allocate sockaddr");
				return -1;
			}
		}
break;
case 256:
#line 1814 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->remote != NULL) {
				yyerror("remote_address already specified");
				return -1;
			}
			cur_rmconf->remote = yystack.l_mark[0].saddr;
		}
break;
case 258:
#line 1823 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->etypes = NULL;
		}
break;
case 260:
#line 1827 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->doitype = yystack.l_mark[0].num; }
break;
case 262:
#line 1828 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->sittype = yystack.l_mark[0].num; }
break;
case 265:
#line 1831 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			yywarn("This directive without certtype will be removed!\n");
			yywarn("Please use 'peers_certfile x509 \"%s\";' instead\n", yystack.l_mark[0].val->v);

			if (cur_rmconf->peerscert != NULL) {
				yyerror("peers_certfile already defined\n");
				return -1;
			}

			if (load_x509(yystack.l_mark[0].val->v, &cur_rmconf->peerscertfile,
				      &cur_rmconf->peerscert)) {
				yyerror("failed to load certificate \"%s\"\n",
					yystack.l_mark[0].val->v);
				return -1;
			}

			vfree(yystack.l_mark[0].val);
		}
break;
case 267:
#line 1851 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->peerscert != NULL) {
				yyerror("peers_certfile already defined\n");
				return -1;
			}

			if (load_x509(yystack.l_mark[0].val->v, &cur_rmconf->peerscertfile,
				      &cur_rmconf->peerscert)) {
				yyerror("failed to load certificate \"%s\"\n",
					yystack.l_mark[0].val->v);
				return -1;
			}

			vfree(yystack.l_mark[0].val);
		}
break;
case 269:
#line 1868 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char path[MAXPATHLEN];
			int ret = 0;

			if (cur_rmconf->peerscert != NULL) {
				yyerror("peers_certfile already defined\n");
				return -1;
			}

			cur_rmconf->peerscert = vmalloc(1);
			if (cur_rmconf->peerscert == NULL) {
				yyerror("failed to allocate peerscert");
				return -1;
			}
			cur_rmconf->peerscert->v[0] = ISAKMP_CERT_PLAINRSA;

			getpathname(path, sizeof(path),
				    LC_PATHTYPE_CERT, yystack.l_mark[0].val->v);
			if (rsa_parse_file(cur_rmconf->rsa_public, path,
					   RSA_TYPE_PUBLIC)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL,
			     "Public PlainRSA keyfile parsed: %s\n", path);

			vfree(yystack.l_mark[0].val);
		}
break;
case 271:
#line 1898 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->peerscert != NULL) {
				yyerror("peers_certfile already defined\n");
				return -1;
			}
			cur_rmconf->peerscert = vmalloc(1);
			if (cur_rmconf->peerscert == NULL) {
				yyerror("failed to allocate peerscert");
				return -1;
			}
			cur_rmconf->peerscert->v[0] = ISAKMP_CERT_DNS;
		}
break;
case 273:
#line 1912 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->cacert != NULL) {
				yyerror("ca_type already defined\n");
				return -1;
			}

			if (load_x509(yystack.l_mark[0].val->v, &cur_rmconf->cacertfile,
				      &cur_rmconf->cacert)) {
				yyerror("failed to load certificate \"%s\"\n",
					yystack.l_mark[0].val->v);
				return -1;
			}

			vfree(yystack.l_mark[0].val);
		}
break;
case 275:
#line 1928 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->verify_cert = yystack.l_mark[0].num; }
break;
case 277:
#line 1929 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->send_cert = yystack.l_mark[0].num; }
break;
case 279:
#line 1930 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->send_cr = yystack.l_mark[0].num; }
break;
case 281:
#line 1931 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->match_empty_cr = yystack.l_mark[0].num; }
break;
case 283:
#line 1933 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (set_identifier(&cur_rmconf->idv, yystack.l_mark[-1].num, yystack.l_mark[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_rmconf->idvtype = yystack.l_mark[-1].num;
		}
break;
case 285:
#line 1942 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (set_identifier_qual(&cur_rmconf->idv, yystack.l_mark[-2].num, yystack.l_mark[0].val, yystack.l_mark[-1].num) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_rmconf->idvtype = yystack.l_mark[-2].num;
		}
break;
case 287:
#line 1951 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_HYBRID
			/* formerly identifier type login */
			if (xauth_rmconf_used(&cur_rmconf->xauth) == -1) {
				yyerror("failed to allocate xauth state\n");
				return -1;
			}
			if ((cur_rmconf->xauth->login = vdup(yystack.l_mark[0].val)) == NULL) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
break;
case 289:
#line 1968 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct idspec  *id;
			id = newidspec();
			if (id == NULL) {
				yyerror("failed to allocate idspec");
				return -1;
			}
			if (set_identifier(&id->id, yystack.l_mark[-1].num, yystack.l_mark[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				racoon_free(id);
				return -1;
			}
			id->idtype = yystack.l_mark[-1].num;
			genlist_append (cur_rmconf->idvl_p, id);
		}
break;
case 291:
#line 1985 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct idspec  *id;
			id = newidspec();
			if (id == NULL) {
				yyerror("failed to allocate idspec");
				return -1;
			}
			if (set_identifier_qual(&id->id, yystack.l_mark[-2].num, yystack.l_mark[0].val, yystack.l_mark[-1].num) != 0) {
				yyerror("failed to set identifer.\n");
				racoon_free(id);
				return -1;
			}
			id->idtype = yystack.l_mark[-2].num;
			genlist_append (cur_rmconf->idvl_p, id);
		}
break;
case 293:
#line 2001 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->verify_identifier = yystack.l_mark[0].num; }
break;
case 295:
#line 2002 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->nonce_size = yystack.l_mark[0].num; }
break;
case 297:
#line 2004 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			yyerror("dh_group cannot be defined here.");
			return -1;
		}
break;
case 299:
#line 2009 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->passive = yystack.l_mark[0].num; }
break;
case 301:
#line 2010 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->ike_frag = yystack.l_mark[0].num; }
break;
case 303:
#line 2011 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->ike_frag = ISAKMP_FRAG_FORCE; }
break;
case 305:
#line 2012 "../../ipsec-tools/src/racoon/cfparse.y"
	{ 
#ifdef SADB_X_EXT_NAT_T_FRAG
        		if (libipsec_opt & LIBIPSEC_OPT_FRAG)
				cur_rmconf->esp_frag = yystack.l_mark[0].num; 
			else
                		yywarn("libipsec lacks IKE frag support");
#else
			yywarn("Your kernel does not support esp_frag");
#endif
		}
break;
case 307:
#line 2022 "../../ipsec-tools/src/racoon/cfparse.y"
	{ 
			if (cur_rmconf->script[SCRIPT_PHASE1_UP] != NULL)
				vfree(cur_rmconf->script[SCRIPT_PHASE1_UP]);

			cur_rmconf->script[SCRIPT_PHASE1_UP] = 
			    script_path_add(vdup(yystack.l_mark[-1].val));
		}
break;
case 309:
#line 2029 "../../ipsec-tools/src/racoon/cfparse.y"
	{ 
			if (cur_rmconf->script[SCRIPT_PHASE1_DOWN] != NULL)
				vfree(cur_rmconf->script[SCRIPT_PHASE1_DOWN]);

			cur_rmconf->script[SCRIPT_PHASE1_DOWN] = 
			    script_path_add(vdup(yystack.l_mark[-1].val));
		}
break;
case 311:
#line 2036 "../../ipsec-tools/src/racoon/cfparse.y"
	{ 
			if (cur_rmconf->script[SCRIPT_PHASE1_DEAD] != NULL)
				vfree(cur_rmconf->script[SCRIPT_PHASE1_DEAD]);

			cur_rmconf->script[SCRIPT_PHASE1_DEAD] = 
			    script_path_add(vdup(yystack.l_mark[-1].val));
		}
break;
case 313:
#line 2043 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->mode_cfg = yystack.l_mark[0].num; }
break;
case 315:
#line 2044 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->weak_phase1_check = yystack.l_mark[0].num;
		}
break;
case 317:
#line 2047 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->gen_policy = yystack.l_mark[0].num; }
break;
case 319:
#line 2048 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->gen_policy = yystack.l_mark[0].num; }
break;
case 321:
#line 2049 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->support_proxy = yystack.l_mark[0].num; }
break;
case 323:
#line 2050 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->ini_contact = yystack.l_mark[0].num; }
break;
case 325:
#line 2052 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_NATT
        		if (libipsec_opt & LIBIPSEC_OPT_NATT)
				cur_rmconf->nat_traversal = yystack.l_mark[0].num;
			else
                		yyerror("libipsec lacks NAT-T support");
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
break;
case 327:
#line 2063 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_NATT
			if (libipsec_opt & LIBIPSEC_OPT_NATT)
				cur_rmconf->nat_traversal = NATT_FORCE;
			else
                		yyerror("libipsec lacks NAT-T support");
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
break;
case 329:
#line 2074 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_DPD
			cur_rmconf->dpd = yystack.l_mark[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
break;
case 331:
#line 2082 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_DPD
			cur_rmconf->dpd_interval = yystack.l_mark[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
break;
case 333:
#line 2091 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_DPD
			cur_rmconf->dpd_retry = yystack.l_mark[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
break;
case 335:
#line 2100 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#ifdef ENABLE_DPD
			cur_rmconf->dpd_maxfails = yystack.l_mark[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
break;
case 337:
#line 2108 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->rekey = yystack.l_mark[0].num; }
break;
case 339:
#line 2109 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->rekey = REKEY_FORCE; }
break;
case 341:
#line 2111 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->ph1id = yystack.l_mark[0].num;
		}
break;
case 343:
#line 2116 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->lifetime = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 345:
#line 2120 "../../ipsec-tools/src/racoon/cfparse.y"
	{ cur_rmconf->pcheck_level = yystack.l_mark[0].num; }
break;
case 347:
#line 2122 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#if 1
			yyerror("byte lifetime support is deprecated in Phase1");
			return -1;
#else
			yywarn("the lifetime of bytes in phase 1 "
				"will be ignored at the moment.");
			cur_rmconf->lifebyte = fix_lifebyte(yystack.l_mark[-1].num * yystack.l_mark[0].num);
			if (cur_rmconf->lifebyte == 0)
				return -1;
#endif
		}
break;
case 349:
#line 2136 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct secprotospec *spspec;

			spspec = newspspec();
			if (spspec == NULL)
				return -1;
			insspspec(cur_rmconf, spspec);
		}
break;
case 352:
#line 2149 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			struct etypes *new;
			new = racoon_malloc(sizeof(struct etypes));
			if (new == NULL) {
				yyerror("failed to allocate etypes");
				return -1;
			}
			new->type = yystack.l_mark[0].num;
			new->next = NULL;
			if (cur_rmconf->etypes == NULL)
				cur_rmconf->etypes = new;
			else {
				struct etypes *p;
				for (p = cur_rmconf->etypes;
				     p->next != NULL;
				     p = p->next)
					;
				p->next = new;
			}
		}
break;
case 353:
#line 2172 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->mycert != NULL) {
				yyerror("certificate_type already defined\n");
				return -1;
			}

			if (load_x509(yystack.l_mark[-1].val->v, &cur_rmconf->mycertfile,
				      &cur_rmconf->mycert)) {
				yyerror("failed to load certificate \"%s\"\n",
					yystack.l_mark[-1].val->v);
				return -1;
			}

			cur_rmconf->myprivfile = racoon_strdup(yystack.l_mark[0].val->v);
			STRDUP_FATAL(cur_rmconf->myprivfile);

			vfree(yystack.l_mark[-1].val);
			vfree(yystack.l_mark[0].val);
		}
break;
case 355:
#line 2193 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			char path[MAXPATHLEN];
			int ret = 0;

			if (cur_rmconf->mycert != NULL) {
				yyerror("certificate_type already defined\n");
				return -1;
			}

			cur_rmconf->mycert = vmalloc(1);
			if (cur_rmconf->mycert == NULL) {
				yyerror("failed to allocate mycert");
				return -1;
			}
			cur_rmconf->mycert->v[0] = ISAKMP_CERT_PLAINRSA;

			getpathname(path, sizeof(path),
				    LC_PATHTYPE_CERT, yystack.l_mark[0].val->v);
			cur_rmconf->send_cr = FALSE;
			cur_rmconf->send_cert = FALSE;
			cur_rmconf->verify_cert = FALSE;
			if (rsa_parse_file(cur_rmconf->rsa_private, path,
					   RSA_TYPE_PRIVATE)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL,
			     "Private PlainRSA keyfile parsed: %s\n", path);
			vfree(yystack.l_mark[0].val);
		}
break;
case 357:
#line 2227 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			yyval.num = algtype2doi(algclass_isakmp_dh, yystack.l_mark[0].num);
			if (yyval.num == -1) {
				yyerror("must be DH group");
				return -1;
			}
		}
break;
case 358:
#line 2235 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (ARRAYLEN(num2dhgroup) > yystack.l_mark[0].num && num2dhgroup[yystack.l_mark[0].num] != 0) {
				yyval.num = num2dhgroup[yystack.l_mark[0].num];
			} else {
				yyerror("must be DH group");
				yyval.num = 0;
				return -1;
			}
		}
break;
case 359:
#line 2246 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.val = NULL; }
break;
case 360:
#line 2247 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.val = yystack.l_mark[0].val; }
break;
case 361:
#line 2248 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.val = yystack.l_mark[0].val; }
break;
case 364:
#line 2256 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->spspec->lifetime = yystack.l_mark[-1].num * yystack.l_mark[0].num;
		}
break;
case 366:
#line 2261 "../../ipsec-tools/src/racoon/cfparse.y"
	{
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_rmconf->spspec->lifebyte = fix_lifebyte(yystack.l_mark[-1].num * yystack.l_mark[0].num);
			if (cur_rmconf->spspec->lifebyte == 0)
				return -1;
#endif
		}
break;
case 368:
#line 2273 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			cur_rmconf->spspec->algclass[algclass_isakmp_dh] = yystack.l_mark[0].num;
		}
break;
case 370:
#line 2278 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			if (cur_rmconf->spspec->vendorid != VENDORID_GSSAPI) {
				yyerror("wrong Vendor ID for gssapi_id");
				return -1;
			}
			if (cur_rmconf->spspec->gssid != NULL)
				racoon_free(cur_rmconf->spspec->gssid);
			cur_rmconf->spspec->gssid =
			    racoon_strdup(yystack.l_mark[0].val->v);
			STRDUP_FATAL(cur_rmconf->spspec->gssid);
		}
break;
case 372:
#line 2291 "../../ipsec-tools/src/racoon/cfparse.y"
	{
			int doi;
			int defklen;

			doi = algtype2doi(yystack.l_mark[-2].num, yystack.l_mark[-1].num);
			if (doi == -1) {
				yyerror("algorithm mismatched 1");
				return -1;
			}

			switch (yystack.l_mark[-2].num) {
			case algclass_isakmp_enc:
			/* reject suppressed algorithms */
#ifndef HAVE_OPENSSL_RC5_H
				if (yystack.l_mark[-1].num == algtype_rc5) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif
#ifndef HAVE_OPENSSL_IDEA_H
				if (yystack.l_mark[-1].num == algtype_idea) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif

				cur_rmconf->spspec->algclass[algclass_isakmp_enc] = doi;
				defklen = default_keylen(yystack.l_mark[-2].num, yystack.l_mark[-1].num);
				if (defklen == 0) {
					if (yystack.l_mark[0].num) {
						yyerror("keylen not allowed");
						return -1;
					}
				} else {
					if (yystack.l_mark[0].num && check_keylen(yystack.l_mark[-2].num, yystack.l_mark[-1].num, yystack.l_mark[0].num) < 0) {
						yyerror("invalid keylen %d", yystack.l_mark[0].num);
						return -1;
					}
				}
				if (yystack.l_mark[0].num)
					cur_rmconf->spspec->encklen = yystack.l_mark[0].num;
				else
					cur_rmconf->spspec->encklen = defklen;
				break;
			case algclass_isakmp_hash:
				cur_rmconf->spspec->algclass[algclass_isakmp_hash] = doi;
				break;
			case algclass_isakmp_ameth:
				cur_rmconf->spspec->algclass[algclass_isakmp_ameth] = doi;
				/*
				 * We may have to set the Vendor ID for the
				 * authentication method we're using.
				 */
				switch (yystack.l_mark[-1].num) {
				case algtype_gssapikrb:
					if (cur_rmconf->spspec->vendorid !=
					    VENDORID_UNKNOWN) {
						yyerror("Vendor ID mismatch "
						    "for auth method");
						return -1;
					}
					/*
					 * For interoperability with Win2k,
					 * we set the Vendor ID to "GSSAPI".
					 */
					cur_rmconf->spspec->vendorid =
					    VENDORID_GSSAPI;
					break;
				case algtype_rsasig:
					if (oakley_get_certtype(cur_rmconf->peerscert) == ISAKMP_CERT_PLAINRSA) {
						if (rsa_list_count(cur_rmconf->rsa_private) == 0) {
							yyerror ("Private PlainRSA key not set. "
								 "Use directive 'certificate_type plainrsa ...'\n");
							return -1;
						}
						if (rsa_list_count(cur_rmconf->rsa_public) == 0) {
							yyerror ("Public PlainRSA keys not set. "
								 "Use directive 'peers_certfile plainrsa ...'\n");
							return -1;
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				yyerror("algorithm mismatched 2");
				return -1;
			}
		}
break;
case 374:
#line 2388 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = 1; }
break;
case 375:
#line 2389 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = 60; }
break;
case 376:
#line 2390 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = (60 * 60); }
break;
case 377:
#line 2393 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = 1; }
break;
case 378:
#line 2394 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = 1024; }
break;
case 379:
#line 2395 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = (1024 * 1024); }
break;
case 380:
#line 2396 "../../ipsec-tools/src/racoon/cfparse.y"
	{ yyval.num = (1024 * 1024 * 1024); }
break;
#line 4299 "racoonyy.tab.c"
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
