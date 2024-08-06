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
#include <openssl/pkcs7err.h>
#include "crypto/pkcs7err.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA PKCS7_str_reasons[] = {
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_CERTIFICATE_VERIFY_ERROR),
    "certificate verify error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER),
    "cipher has no object identifier"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_CIPHER_NOT_INITIALIZED),
    "cipher not initialized"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_CONTENT_AND_DATA_PRESENT),
    "content and data present"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_CTRL_ERROR), "ctrl error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_DECRYPT_ERROR), "decrypt error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_DIGEST_FAILURE), "digest failure"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_ENCRYPTION_CTRL_FAILURE),
    "encryption ctrl failure"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_ENCRYPTION_NOT_SUPPORTED_FOR_THIS_KEY_TYPE),
    "encryption not supported for this key type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_ERROR_ADDING_RECIPIENT),
    "error adding recipient"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_ERROR_SETTING_CIPHER),
    "error setting cipher"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_INVALID_NULL_POINTER),
    "invalid null pointer"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_INVALID_SIGNED_DATA_TYPE),
    "invalid signed data type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_CONTENT), "no content"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_DEFAULT_DIGEST),
    "no default digest"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_MATCHING_DIGEST_TYPE_FOUND),
    "no matching digest type found"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_RECIPIENT_MATCHES_CERTIFICATE),
    "no recipient matches certificate"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_SIGNATURES_ON_DATA),
    "no signatures on data"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_NO_SIGNERS), "no signers"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_OPERATION_NOT_SUPPORTED_ON_THIS_TYPE),
    "operation not supported on this type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_PKCS7_ADD_SIGNATURE_ERROR),
    "pkcs7 add signature error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_PKCS7_ADD_SIGNER_ERROR),
    "pkcs7 add signer error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_PKCS7_DATASIGN), "pkcs7 datasign"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_PRIVATE_KEY_DOES_NOT_MATCH_CERTIFICATE),
    "private key does not match certificate"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_SIGNATURE_FAILURE),
    "signature failure"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_SIGNER_CERTIFICATE_NOT_FOUND),
    "signer certificate not found"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_SIGNING_CTRL_FAILURE),
    "signing ctrl failure"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_SIGNING_NOT_SUPPORTED_FOR_THIS_KEY_TYPE),
    "signing not supported for this key type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_SMIME_TEXT_ERROR), "smime text error"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNABLE_TO_FIND_CERTIFICATE),
    "unable to find certificate"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNABLE_TO_FIND_MEM_BIO),
    "unable to find mem bio"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNABLE_TO_FIND_MESSAGE_DIGEST),
    "unable to find message digest"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNKNOWN_DIGEST_TYPE),
    "unknown digest type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNKNOWN_OPERATION),
    "unknown operation"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNSUPPORTED_CIPHER_TYPE),
    "unsupported cipher type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_UNSUPPORTED_CONTENT_TYPE),
    "unsupported content type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_WRONG_CONTENT_TYPE),
    "wrong content type"},
    {ERR_PACK(ERR_LIB_PKCS7, 0, PKCS7_R_WRONG_PKCS7_TYPE), "wrong pkcs7 type"},
    {0, NULL}
};

#endif

int ossl_err_load_PKCS7_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(PKCS7_str_reasons[0].error) == NULL)
        ERR_load_strings_const(PKCS7_str_reasons);
#endif
    return 1;
}
