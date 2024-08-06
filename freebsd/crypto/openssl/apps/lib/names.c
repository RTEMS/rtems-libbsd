#include <machine/rtems-bsd-user-space.h>

#ifdef __rtems__
#define RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
#include <machine/rtems-bsd-program.h>
#include "../rtems-bsd-openssl-namespace.h"
#include "function.h"
#include "apps.h"
#endif /* __rtems__ */

/*
 * Copyright 2019-2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>
#include <openssl/bio.h>
#include <openssl/safestack.h>
#include "names.h"
#include "openssl/crypto.h"

int name_cmp(const char * const *a, const char * const *b)
{
    return OPENSSL_strcasecmp(*a, *b);
}

void collect_names(const char *name, void *vdata)
{
    STACK_OF(OPENSSL_CSTRING) *names = vdata;

    sk_OPENSSL_CSTRING_push(names, name);
}

void print_names(BIO *out, STACK_OF(OPENSSL_CSTRING) *names)
{
    int i = sk_OPENSSL_CSTRING_num(names);
    int j;

    sk_OPENSSL_CSTRING_sort(names);
    if (i > 1)
        BIO_printf(out, "{ ");
    for (j = 0; j < i; j++) {
        const char *name = sk_OPENSSL_CSTRING_value(names, j);

        if (j > 0)
            BIO_printf(out, ", ");
        BIO_printf(out, "%s", name);
    }
    if (i > 1)
        BIO_printf(out, " }");
}
#ifdef __rtems__
#include "rtems-bsd-openssl-names-data.h"
#endif /* __rtems__ */
