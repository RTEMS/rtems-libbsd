#include <machine/rtems-bsd-user-space.h>

/*
 * Copyright 2002-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/opensslv.h>
#include <openssl/aes.h>
#include "aes_local.h"

#ifndef OPENSSL_NO_DEPRECATED_3_0
const char *AES_options(void)
{
# ifdef FULL_UNROLL
    return "aes(full)";
# else
    return "aes(partial)";
# endif
}
#endif
