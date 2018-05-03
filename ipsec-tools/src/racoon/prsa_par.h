#define COLON 257
#define HEX 258
#define OBRACE 259
#define EBRACE 260
#define TAG_RSA 261
#define TAG_PUB 262
#define TAG_PSK 263
#define MODULUS 264
#define PUBLIC_EXPONENT 265
#define PRIVATE_EXPONENT 266
#define PRIME1 267
#define PRIME2 268
#define EXPONENT1 269
#define EXPONENT2 270
#define COEFFICIENT 271
#define ADDR4 272
#define ADDR6 273
#define ADDRANY 274
#define SLASH 275
#define NUMBER 276
#define BASE64 277
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	BIGNUM *bn;
	RSA *rsa;
	char *chr;
	long num;
	struct netaddr *naddr;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE racoonprsalval;
