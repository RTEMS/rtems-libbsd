/*
 * Copyright (c) 2013-2019 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/libcsupport.h>
#include <rtems/bsd/modules.h>

#include <sys/stat.h>
#include <fcntl.h>

#define TEST_NAME "LIBBSD OPENSSL 2"

#define ARGC(x) RTEMS_BSD_ARGC(x)

#ifdef RTEMS_BSD_MODULE_USR_BIN_OPENSSL
static void
test_cmd_gencert(void)
{
	int fd;
	int rv;
	struct stat sb;
	char *openssl_gencert[] = {
		"openssl",
		"req",
		"-config",
		"/openssl.cfg",
		"-nodes",
		"-newkey",
		"rsa:2048",
		"-keyout",
		"/example.key",
		"-out",
		"example.csr",
		"-subj",
		"/C=GB/ST=London/L=London/O=Global Security/OU=IT Department/CN=example.com",
		NULL
	};

	static const char config[] =
		"[ req ]\n"
		"distinguished_name = req_distinguished_name\n"
		"[ req_distinguished_name ]\n"
		"countryName = Country Name (2 letter code)\n"
		"stateOrProvinceName = State or Province Name (full name)\n"
		"localityName = Locality Name (eg, city)\n"
		"0.organizationName = Organization Name (eg, company)\n"
		"0.organizationName_default = Internet Widgits Pty Ltd\n"
		"organizationalUnitName = Organizational Unit Name (eg, section)\n"
		"organizationalUnitName_default =\n"
		"commonName = Common Name (e.g. server FQDN or YOUR name)\n"
		"commonName_max = 64\n"
		"emailAddress = Email Address\n"
		"emailAddress_max = 64\n";

	/* Create config file. */
	fd = open("/openssl.cfg", O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	assert(fd != -1);
	rv = write(fd, config, sizeof(config));
	assert(rv == sizeof(config));
	rv = close(fd);
	assert(rv == 0);

	rtems_bsd_command_openssl(ARGC(openssl_gencert), openssl_gencert);

	assert(stat("/example.key", &sb) == 0);
	assert(stat("/example.csr", &sb) == 0);
}

static void
test_cmd_speed(void)
{
	int fd;
	int rv;
	struct stat sb;
	char *openssl_speed[] = {
		"openssl",
		"speed",
		"sha1",
		NULL
	};

	rtems_bsd_command_openssl(ARGC(openssl_speed), openssl_speed);
}
#endif /* RTEMS_BSD_MODULE_USR_BIN_OPENSSL */

static void
test_main(void)
{
#ifdef RTEMS_BSD_MODULE_USR_BIN_OPENSSL
	test_cmd_gencert();
	test_cmd_speed();
#else /* ! RTEMS_BSD_MODULE_USR_BIN_OPENSSL */
	puts("openssl command not enabled in the current build set");
#endif /* RTEMS_BSD_MODULE_USR_BIN_OPENSSL */
	exit(0);
}

#include <rtems/bsd/test/default-init.h>
