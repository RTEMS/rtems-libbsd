#include <freebsd/bsd.h>

#include <freebsd/sys/_types.h>

/*********************************************************************
 *  These are also defined in the FreeBSD version of this file.
 *********************************************************************/

#define _LIBC      1
/*
 * This is defined in the FreeBSD source but we have no code yet which
 * relies upon it.
 */
/* #define DO_PTHREADS     1 */
#define USE_KQUEUE 1

#define ISC_SOCKLEN_T   socklen_t
#define ISC_FORMAT_PRINTF(fmt, args) \
        __attribute__((__format__(__printf__, fmt, args)))
#define DE_CONST(konst, var) \
        do { \
                union { const void *k; void *v; } _u; \
                _u.k = konst; \
                var = _u.v; \
        } while (0)

#define UNUSED(x) (x) = (x)


/*********************************************************************
 *  FROM HERE DOWN, THESE ARE NOT IN THE FreeBSD VERSION!!!
 *********************************************************************/
#ifndef __ssize_t
#define __ssize_t ssize_t
#endif

#include <freebsd/machine/_align.h>
