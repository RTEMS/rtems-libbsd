/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define TEST_NAME "LIBBSD OPENSSL 1"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/bsd/modules.h>

#ifdef RTEMS_BSD_MODULE_CRYPTO_OPENSSL

#include <openssl/bio.h>
#include <openssl/evp.h>

static const unsigned char key[16] = "0123456789abcde";

static const unsigned char iv[16] = "fghijklmnopqrst";

static const unsigned char plaintext[] = "The quick brown fox jumps over the lazy dog";

static unsigned char ciphertext[48];

static unsigned char deciphertext[sizeof(plaintext)];

static void
encrypt(void)
{
	EVP_CIPHER_CTX *ctx;
	int status;
	int len;

	ctx = EVP_CIPHER_CTX_new();
	assert(ctx != NULL);

	status = EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);
	assert(status == 1);

	status = EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, (int)sizeof(plaintext));
	assert(status == 1);
	assert(len == 32);

	status = EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
	assert(status == 1);
	assert(len == 16);

	EVP_CIPHER_CTX_free(ctx);
}

static void
decrypt(void)
{
	EVP_CIPHER_CTX *ctx;
	int status;
	int len;

	ctx = EVP_CIPHER_CTX_new();
	assert(ctx != NULL);

	status = EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);
	assert(status == 1);

	status = EVP_DecryptUpdate(ctx, deciphertext, &len, ciphertext, (int)sizeof(ciphertext));
	assert(status == 1);
	assert(len == 32);

	status = EVP_DecryptFinal_ex(ctx, deciphertext + len, &len);
	assert(status == 1);
	assert(len == 12);

	EVP_CIPHER_CTX_free(ctx);
}

static void
test(void)
{

	encrypt();

	puts("Ciphertext is:");
	BIO_dump_fp(stdout, ciphertext, sizeof(ciphertext));

	decrypt();
	assert(memcmp(plaintext, deciphertext, sizeof(plaintext)) == 0);

	printf("Deciphered text is:\n%s\n", deciphertext);
}

#else /* RTEMS_BSD_MODULE_OPENSSL */

static void
test(void)
{

	puts("OpenSSL not enabled in the current build set");
}

#endif /* RTEMS_BSD_MODULE_OPENSSL */

static void
test_main(void)
{

	test();
	exit(0);
}

#include <rtems/bsd/test/default-init.h>
