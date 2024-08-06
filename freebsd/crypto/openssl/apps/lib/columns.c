#include <machine/rtems-bsd-user-space.h>

#ifdef __rtems__
#define RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
#include <machine/rtems-bsd-program.h>
#include "../rtems-bsd-openssl-namespace.h"
#include "function.h"
#include "apps.h"
#endif /* __rtems__ */

/*
 * Copyright 2017-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>
#include "apps.h"
#include "function.h"

void calculate_columns(FUNCTION *functions, DISPLAY_COLUMNS *dc)
{
    FUNCTION *f;
    int len, maxlen = 0;

    for (f = functions; f->name != NULL; ++f)
        if (f->type == FT_general || f->type == FT_md || f->type == FT_cipher)
            if ((len = strlen(f->name)) > maxlen)
                maxlen = len;

    dc->width = maxlen + 2;
    dc->columns = (80 - 1) / dc->width;
}

#ifdef __rtems__
#include "rtems-bsd-openssl-columns-data.h"
#endif /* __rtems__ */
