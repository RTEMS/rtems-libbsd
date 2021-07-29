/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NFSClient
 *
 * @brief RTEMS bindings to the NFS client
 *
 */

/*
 * Copyright (C) 2021 Chris Johns <chris@contemporary.software>
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
#include <machine/rtems-bsd-libio.h>
#include <machine/rtems-bsd-vfs.h>

#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/resourcevar.h>
#include <sys/syscallsubr.h>
#include <sys/sysproto.h>
#include <sys/time.h>
#include <sys/vnode.h>

#include <fs/nfsclient/nfs.h>
#include <nfs/nfsproto.h>
#include <nfsclient/nfs.h>
#include <rtems/bsd/rootfs.h>
#include <stdio.h>

SYSINIT_MODULE_REFERENCE(rootfs);
SYSINIT_MODULE_REFERENCE(nfs);

#ifndef RTEMS_DEBUG
#define RTEMS_DEBUG 0
#endif

#if RTEMS_DEBUG
extern int nfscl_debuglevel;
#endif

int
rtems_nfs_initialize(
    rtems_filesystem_mount_table_entry_t *mt_entry, const void *data)
{
	struct thread *td = curthread;
	const char *fspath = NULL;
	char options[64];
	char *at;
	int error;

	if (RTEMS_DEBUG) {
		printf("nfsv4: mount: %s -> %s", mt_entry->type, mt_entry->dev,
		    mt_entry->target);
		if (data != NULL) {
			printf(" (%s)", (const char *)data);
		}
		printf("\n");
#ifdef RTEMS_NFSCL_DEBUGLEVEL
		nfscl_debuglevel = RTEMS_NFSCL_DEBUGLEVEL;
#endif
	}

	if (td == NULL) {
		if (RTEMS_DEBUG)
			printf("nfsv4: mount: no current thread\n");
		error = ENOMEM;
		goto out;
	}

	at = strchr(mt_entry->dev, '@');
	if (at != NULL) {
		if (RTEMS_DEBUG)
			printf(
			    "nfsv4: mount: user/group name in path not supported\n");
		error = EINVAL;
		goto out;
	}

	if (data != NULL) {
		size_t opts_len = strnlen((const char *)data, sizeof(options));
		if (opts_len >= sizeof(options)) {
			if (RTEMS_DEBUG)
				printf(
				    "nfsv4: mount: options string too long\n");
			error = EINVAL;
			goto out;
		}
	}

	rtems_bsd_vfs_mount_init(mt_entry);

	fspath = mt_entry->target;
	if (*fspath == '/') {
		++fspath;
	}
	if (strchr(fspath, '/') != 0) {
		error = EINVAL;
		goto out;
	}

	rtems_bsd_libio_loc_set_vnode(&mt_entry->mt_fs_root->location, NULL);
	rtems_bsd_libio_loc_set_vnode_dir(
	    &mt_entry->mt_fs_root->location, NULL);

	/*
	 * Make the mount point in the BSD root file system, mount the NFS
	 * export then find the vnode and hold it. Make sure we find the root
	 * node of the NFS share and the not the root file system's mount node.
	 */
	error = rtems_bsd_rootfs_mkdir(fspath);
	if (error == 0) {
		struct mntarg *ma = NULL;
		char errmsg[255];
		ma = mount_arg(
		    ma, "fstype", RTEMS_DECONST(char *, mt_entry->type), -1);
		ma = mount_arg(ma, "fspath", RTEMS_DECONST(char *, fspath), -1);
		ma = mount_arg(
		    ma, "from", RTEMS_DECONST(char *, mt_entry->dev), -1);
		if (mt_entry->writeable) {
			ma = mount_arg(ma, "rw", NULL, 0);
		} else {
			ma = mount_arg(ma, "ro", NULL, 0);
		}
		if (data != NULL) {
			char *opts;
			/*
			 * See `man mount_nfs` and the list of options.
			 */
			strlcpy(options, (const char *)data, sizeof(options));
			opts = &options[0];
			while (opts != NULL) {
				char *delimiter = strchr(opts, ',');
				char *opt = opts;
				if (delimiter != NULL) {
					*delimiter = '\0';
					opts = delimiter + 1;
				} else {
					opts = NULL;
				}
				delimiter = strchr(opt, '=');
				if (delimiter != NULL) {
					*delimiter = '\0';
					ma = mount_arg(
					    ma, opt, delimiter + 1, -1);
				} else {
					ma = mount_arg(ma, opt, NULL, 0);
				}
			}
		}
		memset(errmsg, 0, sizeof(errmsg));
		ma = mount_arg(ma, "errmsg", errmsg, sizeof(errmsg) - 1);
		error = kernel_mount(ma, MNT_VERIFIED);
		if (error == 0) {
			struct nameidata nd;
			vhold(rootvnode);
			NDINIT_ATVP(&nd, LOOKUP, NOFOLLOW, UIO_USERSPACE,
			    fspath, rootvnode, td);
			error = namei(&nd);
			if (error == 0) {
				rtems_bsd_libio_loc_set_vnode(
				    &mt_entry->mt_fs_root->location, nd.ni_vp);
				rtems_bsd_vfs_clonenode(
				    &mt_entry->mt_fs_root->location);
				NDFREE(&nd, NDF_NO_VP_RELE);
			} else {
				NDFREE(&nd, 0);
				rtems_bsd_libio_loc_set_vnode(
				    &mt_entry->mt_fs_root->location, NULL);
				rtems_bsd_vfs_freenode(
				    &mt_entry->mt_fs_root->location);
				rtems_bsd_rootfs_rmdir(fspath);
			}
		} else {
			if (RTEMS_DEBUG)
				printf("nfsv4: mount: error: %s\n", errmsg);
		}
	}

	rtems_bsd_libio_loc_set_vnode_dir(
	    &mt_entry->mt_fs_root->location, NULL);

out:
	if (RTEMS_DEBUG)
		printf("nfsv4: mount: (%d) %s\n", error, strerror(error));
	if (error != 0) {
		if (fspath != NULL) {
			rtems_bsd_rootfs_rmdir(fspath);
		}
	}
	return error;
}
