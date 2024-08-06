#include <machine/rtems-bsd-user-space.h>
/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include <openssl/decodererr.h>
#include "crypto/decodererr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA OSSL_DECODER_str_reasons[] = {
    {ERR_PACK(ERR_LIB_OSSL_DECODER, 0, OSSL_DECODER_R_COULD_NOT_DECODE_OBJECT),
    "could not decode object"},
    {ERR_PACK(ERR_LIB_OSSL_DECODER, 0, OSSL_DECODER_R_DECODER_NOT_FOUND),
    "decoder not found"},
    {ERR_PACK(ERR_LIB_OSSL_DECODER, 0, OSSL_DECODER_R_MISSING_GET_PARAMS),
    "missing get params"},
    {0, NULL}
};

#endif

int ossl_err_load_OSSL_DECODER_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(OSSL_DECODER_str_reasons[0].error) == NULL)
        ERR_load_strings_const(OSSL_DECODER_str_reasons);
#endif
    return 1;
}
