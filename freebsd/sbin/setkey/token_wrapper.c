#include <machine/rtems-bsd-user-space.h>
#include <machine/rtems-bsd-program.h>
#include "rtems-bsd-setkey-namespace.h"
#define yylval setkeyyylval
#define yyparse setkeyyyparse
#include "token.c"
#include "rtems-bsd-setkey-token_wrapper-data.h"