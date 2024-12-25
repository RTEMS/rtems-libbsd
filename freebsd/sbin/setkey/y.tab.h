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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
        int num;
        unsigned long ulnum;
        vchar_t val;
        struct addrinfo *res;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE setkeyyylval;