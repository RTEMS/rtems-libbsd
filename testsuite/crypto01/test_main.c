/**
 * @file
 *
 * @brief It tests /dev/crypto device.
 */

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

#include <sys/cdefs.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <crypto/cryptodev.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	TEST_NAME "LIBBSD CRYPTO 1"

#define	KEY_LENGTH 16

typedef struct {
	int dev_fd;
	int session_fd;
	struct session2_op session;
} test_context;

static test_context test_instance;

/* Test data obtained from http://cryptodev-linux.org/ */

static const char iv[AES_BLOCK_LEN];

static const char key_0[KEY_LENGTH] = { 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const char plaintext_0[AES_BLOCK_LEN];

static const char ciphertext_0[AES_BLOCK_LEN] = { 0xdf, 0x55, 0x6a, 0x33, 0x43,
    0x8d, 0xb8, 0x7b, 0xc4, 0x1b, 0x17, 0x52, 0xc5, 0x5e, 0x5e, 0x49 };

static const char key_1[KEY_LENGTH];

static const char plaintext_1[AES_BLOCK_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00 };

static const char ciphertext_1[AES_BLOCK_LEN] = { 0xb7, 0x97, 0x2b, 0x39, 0x41,
    0xc4, 0x4b, 0x90, 0xaf, 0xa7, 0xb2, 0x64, 0xbf, 0xba, 0x73, 0x87 };

static void
aes_session_create(test_context *ctx, const void *key, size_t keylen)
{
	int rv;

	rv = ioctl(ctx->dev_fd, CRIOGET, &ctx->session_fd);
	assert(rv == 0);

	memset(&ctx->session, 0, sizeof(ctx->session));
	ctx->session.cipher = CRYPTO_AES_CBC;
	ctx->session.key = (caddr_t)key;
	ctx->session.keylen = (u_int32_t)keylen;
	ctx->session.crid = CRYPTO_FLAG_HARDWARE | CRYPTO_FLAG_SOFTWARE;

	rv = ioctl(ctx->session_fd, CIOCGSESSION2, &ctx->session);
	assert(rv == 0);
}

static void
aes_session_destroy(test_context *ctx)
{
	int rv;

	rv = ioctl(ctx->session_fd, CIOCFSESSION, &ctx->session.ses);
	assert(rv == 0);

	rv = close(ctx->session_fd);
	assert(rv == 0);
}

static void
aes_encrypt(const test_context *ctx, const void *iv, const void *plaintext,
    void *ciphertext, size_t len)
{
	struct crypt_op op;
	int rv;

	memset(&op, 0, sizeof(op));
	op.op = COP_ENCRYPT;
	op.ses = ctx->session.ses;
	op.len = (u_int)len;
	op.src = __DECONST(void *, plaintext);
	op.dst = ciphertext;
	op.iv = __DECONST(void *, iv);
	rv = ioctl(ctx->session_fd, CIOCCRYPT, &op);
	assert(rv == 0);
}

static void
aes_decrypt(const test_context *ctx, const void *iv, const void *ciphertext,
    void *plaintext, size_t len)
{
	struct crypt_op op;
	int rv;

	memset(&op, 0, sizeof(op));
	op.op = COP_DECRYPT;
	op.ses = ctx->session.ses;
	op.len = (u_int)len;
	op.src = __DECONST(void *, ciphertext);
	op.dst = plaintext;
	op.iv = __DECONST(void *, iv);
	rv = ioctl(ctx->session_fd, CIOCCRYPT, &op);
	assert(rv == 0);
}

static void
aes_test(test_context *ctx, const char *key, const char *plaintext,
    const char *expected_ciphertext)
{
	char ciphertext[AES_BLOCK_LEN];
	char decrypted_ciphertext[AES_BLOCK_LEN];

	aes_session_create(ctx, key, KEY_LENGTH);

	memset(ciphertext, 0xff, AES_BLOCK_LEN);
	aes_encrypt(ctx, iv, plaintext, ciphertext, AES_BLOCK_LEN);
	assert(memcmp(ciphertext, expected_ciphertext, AES_BLOCK_LEN) == 0);

	memset(decrypted_ciphertext, 0xff, AES_BLOCK_LEN);
	aes_decrypt(ctx, iv, ciphertext, decrypted_ciphertext, AES_BLOCK_LEN);
	assert(memcmp(decrypted_ciphertext, plaintext, AES_BLOCK_LEN) == 0);

	aes_session_destroy(ctx);
}

static void
test_main(void)
{
	test_context *ctx;
	int allow;
	int rv;

	ctx = &test_instance;

	allow = 1;
	rv = sysctlbyname("kern.cryptodevallowsoft", NULL, NULL, &allow,
	    sizeof(allow));
	assert(rv == 0);

	ctx->dev_fd = open("/dev/crypto", O_RDWR);
	assert(ctx->dev_fd >= 0);

	aes_test(ctx, key_0, plaintext_0, ciphertext_0);
	aes_test(ctx, key_1, plaintext_1, ciphertext_1);

	rv = close(ctx->dev_fd);
	assert(rv == 0);

	exit(0);
}

#include <rtems/bsd/bsd.h>

#include <machine/rtems-bsd-nexus-bus.h>

SYSINIT_MODULE_REFERENCE(cryptodev);

RTEMS_BSD_DEFINE_NEXUS_DEVICE(cryptosoft, 0, 0, NULL);

#include <rtems/bsd/test/default-init.h>
