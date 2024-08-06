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
#include <openssl/storeerr.h>
#include "crypto/storeerr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA OSSL_STORE_str_reasons[] = {
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_AMBIGUOUS_CONTENT_TYPE),
    "ambiguous content type"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_BAD_PASSWORD_READ),
    "bad password read"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_ERROR_VERIFYING_PKCS12_MAC),
    "error verifying pkcs12 mac"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_FINGERPRINT_SIZE_DOES_NOT_MATCH_DIGEST),
    "fingerprint size does not match digest"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_INVALID_SCHEME),
    "invalid scheme"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_IS_NOT_A), "is not a"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_LOADER_INCOMPLETE),
    "loader incomplete"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_LOADING_STARTED),
    "loading started"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_A_CERTIFICATE),
    "not a certificate"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_A_CRL), "not a crl"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_A_NAME), "not a name"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_A_PRIVATE_KEY),
    "not a private key"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_A_PUBLIC_KEY),
    "not a public key"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NOT_PARAMETERS),
    "not parameters"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_NO_LOADERS_FOUND),
    "no loaders found"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_PASSPHRASE_CALLBACK_ERROR),
    "passphrase callback error"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_PATH_MUST_BE_ABSOLUTE),
    "path must be absolute"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_SEARCH_ONLY_SUPPORTED_FOR_DIRECTORIES),
    "search only supported for directories"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_UI_PROCESS_INTERRUPTED_OR_CANCELLED),
    "ui process interrupted or cancelled"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_UNREGISTERED_SCHEME),
    "unregistered scheme"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_UNSUPPORTED_CONTENT_TYPE),
    "unsupported content type"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_UNSUPPORTED_OPERATION),
    "unsupported operation"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_UNSUPPORTED_SEARCH_TYPE),
    "unsupported search type"},
    {ERR_PACK(ERR_LIB_OSSL_STORE, 0, OSSL_STORE_R_URI_AUTHORITY_UNSUPPORTED),
    "uri authority unsupported"},
    {0, NULL}
};

#endif

int ossl_err_load_OSSL_STORE_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(OSSL_STORE_str_reasons[0].error) == NULL)
        ERR_load_strings_const(OSSL_STORE_str_reasons);
#endif
    return 1;
}
