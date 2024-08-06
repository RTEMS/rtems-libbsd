#include <machine/rtems-bsd-user-space.h>

#ifdef __rtems__
#define RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
#include <machine/rtems-bsd-program.h>
#include "../rtems-bsd-openssl-namespace.h"
#include "function.h"
#include "apps.h"
#endif /* __rtems__ */
/*
 * Copyright 2018-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "fmt.h"

int FMT_istext(int format)
{
    return (format & B_FORMAT_TEXT) == B_FORMAT_TEXT;
}
#ifdef __rtems__
#include "rtems-bsd-openssl-fmt-data.h"
#endif /* __rtems__ */
