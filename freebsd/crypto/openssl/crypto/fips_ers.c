#include <machine/rtems-bsd-user-space.h>

#include <openssl/opensslconf.h>

#ifdef OPENSSL_FIPS
# include "fips_err.h"
#else
static void *dummy = &dummy;
#endif
