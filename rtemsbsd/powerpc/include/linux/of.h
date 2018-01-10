/*
 * Copyright (c) 2015, 2018 embedded brains GmbH
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_OF_H
#define	_LINUX_OF_H

#include <linux/types.h>

#include <libfdt.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	OF_BAD_ADDR ((uint64_t)-1)

typedef uint32_t phandle;

struct device_node {
	int offset;
	const char *full_name;
};

static inline struct device_node *
of_node_get(struct device_node *dn)
{

	return (dn);
}

static inline void
of_node_put(struct device_node *dn)
{

	(void)dn;
}

const void *of_get_property(const struct device_node *dn, const char *name,
    int *len);

int of_n_addr_cells(struct device_node *dn);

int of_n_size_cells(struct device_node *dn);

/* FIXME: If we need the property, then more work is to do */
#define	of_find_property(dn, name, len) of_get_property(dn, name, len)

int of_property_read_u32_array(const struct device_node *dn, const char *name,
    u32 *vals, size_t nz);

static inline int
of_property_read_u32(const struct device_node *dn, const char *name, u32 *val)
{

	return (of_property_read_u32_array(dn, name, val, 1));
}

uint64_t of_read_number(const uint32_t *cell, int size);

struct device_node *of_parse_phandle(struct device_node *dns,
    struct device_node *dn, const char *phandle_name, int index);

int of_count_phandle_with_args(struct device_node *dn, const char *list_name,
    const char *cells_name);

bool of_device_is_available(const struct device_node *dn);

int of_device_is_compatible(const struct device_node *dn, const char *name);

struct device_node *of_find_node_by_path(struct device_node *dns,
    const char *path);

struct device_node *of_find_compatible_node(struct device_node *dns,
    const struct device_node *dn, const char *type, const char *compatible);

#define for_each_compatible_node(dn, type, compatible) \
    for (dn = of_find_compatible_node(&of_dns, NULL, type, compatible); \
    dn != NULL; dn = of_find_compatible_node(&of_dns, dn, type, compatible))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_OF_H */
