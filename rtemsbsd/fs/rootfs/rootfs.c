/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2020 Chris Johns
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/vnode.h>

#include <fs/pseudofs/pseudofs.h>

static struct pfs_node *rootfs_root;

static int
rootfs_attr(PFS_ATTR_ARGS, int mode)
{
	vap->va_mode = 0;
	return 0;
}

static int rootfs_attr_all_rwx(PFS_ATTR_ARGS)
{
	vap->va_mode = 0777;
	return 0;
}

int
rtems_bsd_rootfs_mkdir(const char *name)
{
	struct pfs_node *dir;
	dir = pfs_create_dir(rootfs_root, name, rootfs_attr_all_rwx, NULL, NULL,
	    PFS_RDWR | PFS_NOWAIT);
	if (dir == NULL)
		return ENOENT;
	return 0;
}

int
rtems_bsd_rootfs_rmdir(const char *name)
{
	struct pfs_node *dir;
	dir = pfs_find_node(rootfs_root, name);
	if (dir == NULL)
		return ENOENT;
	return pfs_destroy(dir);
}

static int rootfs_init(PFS_INIT_ARGS)
{
	rootfs_root = pi->pi_root;
	return 0;
}

static int rootfs_uninit(PFS_INIT_ARGS)
{
	return 0;
}

PSEUDOFS(rootfs, 1, VFCF_SYNTHETIC);
