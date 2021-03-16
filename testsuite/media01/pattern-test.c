/**
 * @file
 *
 * @brief Command to fill a file with a pattern.
 *
 * The written pattern is a simple 32 bit counter which makes it easy to find
 * problems where buffers are not written to the correct location or where some
 * blocks are missing.
 *
 * This file can be either compiled as part of RTEMS or as a stand alone Linux
 * or (most likely) FreeBSD application. To compile it as stand alone, just use
 * "gcc pattern-test.c"
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 embedded brains GmbH.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __rtems__
#include "pattern-test.h"
#endif /* __rtems__ */

#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static const char * const usage =
	"Fill a file or a device file with a simple incrementing 32 bit pattern.\n"
	"Use with:\n"
	"    pattern [fill|check] <file> <size> <block_size> [<start_value> [<output>]]\n"
	"Arguments:\n"
	"    All numbers can be decimal (e.g. 256) or hex (e.g. 0x100)\n"
	"    <size> and <block_size> is in bytes\n"
	"    <start_value> start with this counter value\n"
	"    <output>: Only relevant for check. Can be: \n"
	"              <nr>: print the first <nr> errors\n"
	"              \"short\": Print all errors but short form\n";

static int
check_and_process_params(
	int argc,
	char *argv[],
	int open_flags,
	int *fd,
	size_t *size,
	size_t *block_size,
	uint8_t **block,
	uint8_t **read_block,
	int *max_errors,
	bool *short_output,
	uint32_t *start_value
)
{
	if (argc < 4) {
		printf(usage);
		return -1;
	}

	if (argc > 4) {
		*start_value = strtoul(argv[4], NULL, 0);
	} else {
		*start_value = 0;
	}

	if (short_output != NULL && max_errors != NULL) {
		*short_output = false;
		*max_errors = 1;
		if (argc > 5) {
			if (strcmp("short", argv[5]) == 0) {
				*max_errors = -1;
				*short_output = true;
			} else {
				*max_errors = strtol(argv[5], NULL, 0);
				if (*max_errors <= 0) {
					warn("Can't use <output> parameter.");
					return -1;
				}
			}
		}
	}

	*size = strtoul(argv[2], NULL, 0);
	if (size == 0) {
		warn("Couldn't convert size or size set to 0");
		return -1;
	}

	*block_size = strtoul(argv[3], NULL, 0);
	if (block_size == 0) {
		warn("Couldn't convert block_size or block_size set to 0");
		return -1;
	}

	if (block != NULL) {
		*block = malloc(*block_size);
		if (*block == NULL) {
			warn("Couldn't allocate block");
			return -1;
		}
	}

	if (read_block != NULL) {
		*read_block = malloc(*block_size);
		if (*read_block == NULL) {
			warn("Couldn't allocate read_block");
			if (block != NULL) {
				free(*block);
			}
			return -1;
		}
	}

	*fd = open(argv[1], open_flags, 0666);
	if (*fd < 0) {
		warn("Couldn't open file");
		if (block != NULL) {
			free(*block);
		}
		if (read_block != NULL) {
			free(*read_block);
		}
		return -1;
	}

	printf("File: %s\nSize: 0x%x\nBlock size: 0x%x\n",
	    argv[1], *size, *block_size);

	return 0;
}

/* Write addresses to the block. Keep word boundaries intact. */
static void
fill_block(uint8_t *block, size_t size, uint32_t start)
{
	uint32_t pattern_size = sizeof(start);
	uint32_t start_offset = start % pattern_size;
	uint32_t value_h = start & ~(pattern_size - 1);
	uint32_t value_n = htonl(value_h);
	uint8_t *val = (uint8_t *) &value_n;

	if (start_offset != 0) {
		size_t to_write = MIN(pattern_size - start_offset, size);
		memcpy(block, val + start_offset, to_write);
		block += to_write;
		size -= to_write;
		value_h += pattern_size;
		value_n = htonl(value_h);
	}

	while (size >= pattern_size) {
		memcpy(block, val, pattern_size);
		value_h += pattern_size;
		value_n = htonl(value_h);
		block += pattern_size;
		size -= pattern_size;
	}

	if (size > 0) {
		memcpy(block, val, size);
	}
}

static int
command_pattern_fill(int argc, char *argv[])
{
	int fd;
	size_t size;
	size_t block_size;
	uint8_t *block;
	uint32_t start_value;
	int rv;

	rv = check_and_process_params(argc, argv, O_WRONLY | O_CREAT,
	    &fd, &size, &block_size, &block, NULL, NULL, NULL, &start_value);
	if (rv != 0) {
		warnx("Error while processing parameters.\n");
		return rv;
	}

	for (size_t current = start_value;
	    current < start_value + size;
	    current += block_size) {
		size_t write_size = MIN(block_size, size-current);
		ssize_t written;
		fill_block(block, write_size, current);
		written = write(fd, block, write_size);
		if (written != (ssize_t)write_size) {
			warn("Writing failed on block at 0x%x", current);
			break;
		}
	}

	free(block);
	close(fd);

	return 0;
}

static void
print_block(uint8_t *block, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		if (i > 0 && i % 0x10 == 0) {
			printf("\n");
		}
		printf("%02x ", block[i]);
	}
	printf("\n");
}

static int
command_pattern_check(int argc, char *argv[])
{
	int fd;
	size_t size;
	size_t block_size;
	uint8_t *block;
	uint8_t *read_block;
	int rv;
	int errors = 0;
	int max_errors;
	bool short_output;
	bool last_was_error = false;
	uint32_t start_value;

	rv = check_and_process_params(argc, argv, O_RDONLY,
	    &fd, &size, &block_size, &block, &read_block,
	    &max_errors, &short_output, &start_value);
	if (rv != 0) {
		warnx("Error while processing parameters.\n");
		return rv;
	}

	for (size_t current = start_value;
	    current < size + start_value;
	    current += block_size) {
		size_t read_size = MIN(block_size, size-current);
		ssize_t received;
		fill_block(block, read_size, current);
		received = read(fd, read_block, read_size);
		if (received != (ssize_t)read_size) {
			warn("Reading failed on block at 0x%x", current);
			break;
		}
		rv = memcmp(block, read_block, read_size);
		if (short_output) {
			bool is_error = (rv != 0);
			if (last_was_error != is_error) {
				warnx("%s: 0x%x",
				    is_error ? "ERR" : "OK ",
				    current);
			}
			last_was_error = is_error;
		} else if (rv != 0) {
			warnx("Pattern wrong in block at 0x%x", current);
			warnx("Expected:");
			print_block(block, read_size);
			warnx("Got:");
			print_block(read_block, read_size);
			++errors;
			if (errors >= max_errors) {
				warnx("Too many errors. Refusing to continue.");
				break;
			}
		}
	}

	free(read_block);
	free(block);
	close(fd);

	return 0;
}

static int
command_pattern(int argc, char *argv[])
{
	if (argc < 2) {
		printf(usage);
		return -1;
	} else if (strcmp(argv[1], "fill") == 0) {
		return command_pattern_fill(argc-1, &argv[1]);
	} else if (strcmp(argv[1], "check") == 0) {
		return command_pattern_check(argc-1, &argv[1]);
	} else {
		printf(usage);
		return -1;
	}
}

#ifdef __rtems__
rtems_shell_cmd_t shell_PATTERN_Command = {
	.name = "pattern",
	.usage = usage,
	.topic = "files",
	.command = command_pattern,
};
#else /* __rtems__ */

int
main(int argc, char *argv[])
{
	return command_pattern(argc, argv);
}
#endif /* __rtems__ */
