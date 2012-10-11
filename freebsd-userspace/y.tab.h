#define DIR 257
#define ACTION 258
#define PROTOCOL 259
#define MODE 260
#define LEVEL 261
#define LEVEL_SPECIFY 262
#define IPADDRESS 263
#define ME 264
#define ANY 265
#define SLASH 266
#define HYPHEN 267
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	u_int num;
	struct _val {
		int len;
		char *buf;
	} val;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE __libipsecyylval;
