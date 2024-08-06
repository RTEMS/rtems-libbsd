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
#include <openssl/rsaerr.h>
#include "crypto/rsaerr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA RSA_str_reasons[] = {
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_ALGORITHM_MISMATCH), "algorithm mismatch"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BAD_E_VALUE), "bad e value"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BAD_FIXED_HEADER_DECRYPT),
    "bad fixed header decrypt"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BAD_PAD_BYTE_COUNT), "bad pad byte count"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BAD_SIGNATURE), "bad signature"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BLOCK_TYPE_IS_NOT_01),
    "block type is not 01"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_BLOCK_TYPE_IS_NOT_02),
    "block type is not 02"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_GREATER_THAN_MOD_LEN),
    "data greater than mod len"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_TOO_LARGE), "data too large"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_TOO_LARGE_FOR_KEY_SIZE),
    "data too large for key size"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_TOO_LARGE_FOR_MODULUS),
    "data too large for modulus"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_TOO_SMALL), "data too small"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DATA_TOO_SMALL_FOR_KEY_SIZE),
    "data too small for key size"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DIGEST_DOES_NOT_MATCH),
    "digest does not match"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DIGEST_NOT_ALLOWED), "digest not allowed"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DIGEST_TOO_BIG_FOR_RSA_KEY),
    "digest too big for rsa key"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DMP1_NOT_CONGRUENT_TO_D),
    "dmp1 not congruent to d"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_DMQ1_NOT_CONGRUENT_TO_D),
    "dmq1 not congruent to d"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_D_E_NOT_CONGRUENT_TO_1),
    "d e not congruent to 1"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_FIRST_OCTET_INVALID),
    "first octet invalid"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_ILLEGAL_OR_UNSUPPORTED_PADDING_MODE),
    "illegal or unsupported padding mode"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_DIGEST), "invalid digest"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_DIGEST_LENGTH),
    "invalid digest length"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_HEADER), "invalid header"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_KEYPAIR), "invalid keypair"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_KEY_LENGTH), "invalid key length"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_LABEL), "invalid label"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_LENGTH), "invalid length"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_MESSAGE_LENGTH),
    "invalid message length"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_MGF1_MD), "invalid mgf1 md"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_MODULUS), "invalid modulus"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_MULTI_PRIME_KEY),
    "invalid multi prime key"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_OAEP_PARAMETERS),
    "invalid oaep parameters"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_PADDING), "invalid padding"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_PADDING_MODE),
    "invalid padding mode"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_PSS_PARAMETERS),
    "invalid pss parameters"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_PSS_SALTLEN),
    "invalid pss saltlen"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_REQUEST), "invalid request"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_SALT_LENGTH),
    "invalid salt length"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_STRENGTH), "invalid strength"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_TRAILER), "invalid trailer"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_INVALID_X931_DIGEST),
    "invalid x931 digest"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_IQMP_NOT_INVERSE_OF_Q),
    "iqmp not inverse of q"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_KEY_PRIME_NUM_INVALID),
    "key prime num invalid"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_KEY_SIZE_TOO_SMALL), "key size too small"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_LAST_OCTET_INVALID), "last octet invalid"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MGF1_DIGEST_NOT_ALLOWED),
    "mgf1 digest not allowed"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MISSING_PRIVATE_KEY),
    "missing private key"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MODULUS_TOO_LARGE), "modulus too large"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MP_COEFFICIENT_NOT_INVERSE_OF_R),
    "mp coefficient not inverse of r"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MP_EXPONENT_NOT_CONGRUENT_TO_D),
    "mp exponent not congruent to d"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_MP_R_NOT_PRIME), "mp r not prime"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_NO_PUBLIC_EXPONENT), "no public exponent"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_NULL_BEFORE_BLOCK_MISSING),
    "null before block missing"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_N_DOES_NOT_EQUAL_PRODUCT_OF_PRIMES),
    "n does not equal product of primes"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_N_DOES_NOT_EQUAL_P_Q),
    "n does not equal p q"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_OAEP_DECODING_ERROR),
    "oaep decoding error"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_OPERATION_NOT_SUPPORTED_FOR_THIS_KEYTYPE),
    "operation not supported for this keytype"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_PADDING_CHECK_FAILED),
    "padding check failed"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_PAIRWISE_TEST_FAILURE),
    "pairwise test failure"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_PKCS_DECODING_ERROR),
    "pkcs decoding error"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_PSS_SALTLEN_TOO_SMALL),
    "pss saltlen too small"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_PUB_EXPONENT_OUT_OF_RANGE),
    "pub exponent out of range"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_P_NOT_PRIME), "p not prime"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_Q_NOT_PRIME), "q not prime"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_RANDOMNESS_SOURCE_STRENGTH_INSUFFICIENT),
    "randomness source strength insufficient"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_RSA_OPERATIONS_NOT_SUPPORTED),
    "rsa operations not supported"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_SLEN_CHECK_FAILED),
    "salt length check failed"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_SLEN_RECOVERY_FAILED),
    "salt length recovery failed"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_SSLV3_ROLLBACK_ATTACK),
    "sslv3 rollback attack"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_THE_ASN1_OBJECT_IDENTIFIER_IS_NOT_KNOWN_FOR_THIS_MD),
    "the asn1 object identifier is not known for this md"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_ALGORITHM_TYPE),
    "unknown algorithm type"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_DIGEST), "unknown digest"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_MASK_DIGEST),
    "unknown mask digest"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_PADDING_TYPE),
    "unknown padding type"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNSUPPORTED_ENCRYPTION_TYPE),
    "unsupported encryption type"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNSUPPORTED_LABEL_SOURCE),
    "unsupported label source"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNSUPPORTED_MASK_ALGORITHM),
    "unsupported mask algorithm"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNSUPPORTED_MASK_PARAMETER),
    "unsupported mask parameter"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_UNSUPPORTED_SIGNATURE_TYPE),
    "unsupported signature type"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_VALUE_MISSING), "value missing"},
    {ERR_PACK(ERR_LIB_RSA, 0, RSA_R_WRONG_SIGNATURE_LENGTH),
    "wrong signature length"},
    {0, NULL}
};

#endif

int ossl_err_load_RSA_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(RSA_str_reasons[0].error) == NULL)
        ERR_load_strings_const(RSA_str_reasons);
#endif
    return 1;
}
