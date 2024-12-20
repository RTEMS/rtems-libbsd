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
#include "e_capi_err.h"

#ifndef OPENSSL_NO_ERR

static ERR_STRING_DATA CAPI_str_reasons[] = {
    {ERR_PACK(0, 0, CAPI_R_CANT_CREATE_HASH_OBJECT), "cant create hash object"},
    {ERR_PACK(0, 0, CAPI_R_CANT_FIND_CAPI_CONTEXT), "cant find capi context"},
    {ERR_PACK(0, 0, CAPI_R_CANT_GET_KEY), "cant get key"},
    {ERR_PACK(0, 0, CAPI_R_CANT_SET_HASH_VALUE), "cant set hash value"},
    {ERR_PACK(0, 0, CAPI_R_CRYPTACQUIRECONTEXT_ERROR),
    "cryptacquirecontext error"},
    {ERR_PACK(0, 0, CAPI_R_CRYPTENUMPROVIDERS_ERROR),
    "cryptenumproviders error"},
    {ERR_PACK(0, 0, CAPI_R_DECRYPT_ERROR), "decrypt error"},
    {ERR_PACK(0, 0, CAPI_R_ENGINE_NOT_INITIALIZED), "engine not initialized"},
    {ERR_PACK(0, 0, CAPI_R_ENUMCONTAINERS_ERROR), "enumcontainers error"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_ADDING_CERT), "error adding cert"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_CREATING_STORE), "error creating store"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_GETTING_FRIENDLY_NAME),
    "error getting friendly name"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_GETTING_KEY_PROVIDER_INFO),
    "error getting key provider info"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_OPENING_STORE), "error opening store"},
    {ERR_PACK(0, 0, CAPI_R_ERROR_SIGNING_HASH), "error signing hash"},
    {ERR_PACK(0, 0, CAPI_R_FILE_OPEN_ERROR), "file open error"},
    {ERR_PACK(0, 0, CAPI_R_FUNCTION_NOT_SUPPORTED), "function not supported"},
    {ERR_PACK(0, 0, CAPI_R_GETUSERKEY_ERROR), "getuserkey error"},
    {ERR_PACK(0, 0, CAPI_R_INVALID_DIGEST_LENGTH), "invalid digest length"},
    {ERR_PACK(0, 0, CAPI_R_INVALID_DSA_PUBLIC_KEY_BLOB_MAGIC_NUMBER),
    "invalid dsa public key blob magic number"},
    {ERR_PACK(0, 0, CAPI_R_INVALID_LOOKUP_METHOD), "invalid lookup method"},
    {ERR_PACK(0, 0, CAPI_R_INVALID_PUBLIC_KEY_BLOB), "invalid public key blob"},
    {ERR_PACK(0, 0, CAPI_R_INVALID_RSA_PUBLIC_KEY_BLOB_MAGIC_NUMBER),
    "invalid rsa public key blob magic number"},
    {ERR_PACK(0, 0, CAPI_R_PUBKEY_EXPORT_ERROR), "pubkey export error"},
    {ERR_PACK(0, 0, CAPI_R_PUBKEY_EXPORT_LENGTH_ERROR),
    "pubkey export length error"},
    {ERR_PACK(0, 0, CAPI_R_UNKNOWN_COMMAND), "unknown command"},
    {ERR_PACK(0, 0, CAPI_R_UNSUPPORTED_ALGORITHM_NID),
    "unsupported algorithm nid"},
    {ERR_PACK(0, 0, CAPI_R_UNSUPPORTED_PADDING), "unsupported padding"},
    {ERR_PACK(0, 0, CAPI_R_UNSUPPORTED_PUBLIC_KEY_ALGORITHM),
    "unsupported public key algorithm"},
    {ERR_PACK(0, 0, CAPI_R_WIN32_ERROR), "win32 error"},
    {0, NULL}
};

#endif

static int lib_code = 0;
static int error_loaded = 0;

static int ERR_load_CAPI_strings(void)
{
    if (lib_code == 0)
        lib_code = ERR_get_next_error_library();

    if (!error_loaded) {
#ifndef OPENSSL_NO_ERR
        ERR_load_strings(lib_code, CAPI_str_reasons);
#endif
        error_loaded = 1;
    }
    return 1;
}

static void ERR_unload_CAPI_strings(void)
{
    if (error_loaded) {
#ifndef OPENSSL_NO_ERR
        ERR_unload_strings(lib_code, CAPI_str_reasons);
#endif
        error_loaded = 0;
    }
}

static void ERR_CAPI_error(int function, int reason, const char *file, int line)
{
    if (lib_code == 0)
        lib_code = ERR_get_next_error_library();
    ERR_raise(lib_code, reason);
    ERR_set_debug(file, line, NULL);
}
