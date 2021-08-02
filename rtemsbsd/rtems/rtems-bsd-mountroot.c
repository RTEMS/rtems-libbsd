/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup FileSystems
 *
 * @brief Mount a root file system.
 */

/*
 * Copyright (C) 2020 Chris Johns <chris@contemporary.software>
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/filedesc.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/resourcevar.h>
#include <sys/vnode.h>

#include <vm/vm_extern.h>

struct vnode *rootvnode;

MALLOC_DECLARE(M_MOUNT);

static void
set_rootvnode(struct mount *mp)
{
	struct proc *p;
	int error;

	error = VFS_ROOT(mp, LK_EXCLUSIVE, &rootvnode);
	if (error != 0)
		panic("Cannot find root vnode");

	VOP_UNLOCK(rootvnode, 0);

	p = curthread->td_proc;
	FILEDESC_XLOCK(p->p_fd);

	if (p->p_fd->fd_cdir != NULL)
		vrele(p->p_fd->fd_cdir);
	p->p_fd->fd_cdir = rootvnode;
	VREF(rootvnode);

	if (p->p_fd->fd_rdir != NULL)
		vrele(p->p_fd->fd_rdir);
	p->p_fd->fd_rdir = rootvnode;
	VREF(rootvnode);

	FILEDESC_XUNLOCK(p->p_fd);
}

static void
bsd_mountroot(const char *fstype)
{
	struct vfsoptlist *opts;
	struct vfsconf *vfsp;
	struct mount *mp;
	struct ucred *cred;
	int error;

	cred = crget();
	cred->cr_uidinfo = uifind(0);
	cred->cr_ruidinfo = uifind(0);

	vfsp = vfs_byname(fstype);
	if (vfsp != NULL) {
		mp = vfs_mount_alloc(NULLVP, vfsp, "/", cred);

		crfree(cred);

		error = VFS_MOUNT(mp);
		if (error != 0)
			panic("Cannot mount root file system: %d", error);

		error = VFS_STATFS(mp, &mp->mnt_stat);
		if (error != 0)
			panic("Cannot stat root file system: %d", error);

		opts = malloc(sizeof(struct vfsoptlist), M_MOUNT, M_WAITOK);
		TAILQ_INIT(opts);
		mp->mnt_opt = opts;

		mtx_lock(&mountlist_mtx);
		TAILQ_INSERT_HEAD(&mountlist, mp, mnt_list);
		mtx_unlock(&mountlist_mtx);

		set_rootvnode(mp);
	}
}

static void
bsd_rootfs_init(const void *dummy)
{
	bsd_mountroot("rootfs");
}
SYSINIT(bsdroot, SI_SUB_KTHREAD_INIT, SI_ORDER_FIRST, bsd_rootfs_init, NULL);
