/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_PFCTLY_PFCTLY_TAB_H_INCLUDED
# define YY_PFCTLY_PFCTLY_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int pfctlydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PASS = 258,
     BLOCK = 259,
     SCRUB = 260,
     RETURN = 261,
     IN = 262,
     OS = 263,
     OUT = 264,
     LOG = 265,
     QUICK = 266,
     ON = 267,
     FROM = 268,
     TO = 269,
     FLAGS = 270,
     RETURNRST = 271,
     RETURNICMP = 272,
     RETURNICMP6 = 273,
     PROTO = 274,
     INET = 275,
     INET6 = 276,
     ALL = 277,
     ANY = 278,
     ICMPTYPE = 279,
     ICMP6TYPE = 280,
     CODE = 281,
     KEEP = 282,
     MODULATE = 283,
     STATE = 284,
     PORT = 285,
     RDR = 286,
     NAT = 287,
     BINAT = 288,
     ARROW = 289,
     NODF = 290,
     MINTTL = 291,
     ERROR = 292,
     ALLOWOPTS = 293,
     FASTROUTE = 294,
     FILENAME = 295,
     ROUTETO = 296,
     DUPTO = 297,
     REPLYTO = 298,
     NO = 299,
     LABEL = 300,
     NOROUTE = 301,
     URPFFAILED = 302,
     FRAGMENT = 303,
     USER = 304,
     GROUP = 305,
     MAXMSS = 306,
     MAXIMUM = 307,
     TTL = 308,
     TOS = 309,
     DROP = 310,
     TABLE = 311,
     REASSEMBLE = 312,
     FRAGDROP = 313,
     FRAGCROP = 314,
     ANCHOR = 315,
     NATANCHOR = 316,
     RDRANCHOR = 317,
     BINATANCHOR = 318,
     SET = 319,
     OPTIMIZATION = 320,
     TIMEOUT = 321,
     LIMIT = 322,
     LOGINTERFACE = 323,
     BLOCKPOLICY = 324,
     RANDOMID = 325,
     REQUIREORDER = 326,
     SYNPROXY = 327,
     FINGERPRINTS = 328,
     NOSYNC = 329,
     DEBUG = 330,
     SKIP = 331,
     HOSTID = 332,
     ANTISPOOF = 333,
     FOR = 334,
     INCLUDE = 335,
     BITMASK = 336,
     RANDOM = 337,
     SOURCEHASH = 338,
     ROUNDROBIN = 339,
     STATICPORT = 340,
     PROBABILITY = 341,
     ALTQ = 342,
     CBQ = 343,
     CODEL = 344,
     PRIQ = 345,
     HFSC = 346,
     FAIRQ = 347,
     BANDWIDTH = 348,
     TBRSIZE = 349,
     LINKSHARE = 350,
     REALTIME = 351,
     UPPERLIMIT = 352,
     QUEUE = 353,
     PRIORITY = 354,
     QLIMIT = 355,
     HOGS = 356,
     BUCKETS = 357,
     RTABLE = 358,
     TARGET = 359,
     INTERVAL = 360,
     LOAD = 361,
     RULESET_OPTIMIZATION = 362,
     PRIO = 363,
     STICKYADDRESS = 364,
     MAXSRCSTATES = 365,
     MAXSRCNODES = 366,
     SOURCETRACK = 367,
     GLOBAL = 368,
     RULE = 369,
     MAXSRCCONN = 370,
     MAXSRCCONNRATE = 371,
     OVERLOAD = 372,
     FLUSH = 373,
     SLOPPY = 374,
     TAGGED = 375,
     TAG = 376,
     IFBOUND = 377,
     FLOATING = 378,
     STATEPOLICY = 379,
     STATEDEFAULTS = 380,
     ROUTE = 381,
     SETTOS = 382,
     DIVERTTO = 383,
     DIVERTREPLY = 384,
     STRING = 385,
     NUMBER = 386,
     PORTBINARY = 387
   };
#endif
/* Tokens.  */
#define PASS 258
#define BLOCK 259
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
#define FRAGDROP 313
#define FRAGCROP 314
#define ANCHOR 315
#define NATANCHOR 316
#define RDRANCHOR 317
#define BINATANCHOR 318
#define SET 319
#define OPTIMIZATION 320
#define TIMEOUT 321
#define LIMIT 322
#define LOGINTERFACE 323
#define BLOCKPOLICY 324
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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE pfctlylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int pfctlyparse (void *YYPARSE_PARAM);
#else
int pfctlyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int pfctlyparse (void);
#else
int pfctlyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_PFCTLY_PFCTLY_TAB_H_INCLUDED  */
