/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (C) 2020 Chris Johns <chris@contemporary.software>
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_VFS_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_VFS_H_

#include <rtems/libio.h>

extern const rtems_filesystem_operations_table rtems_bsd_vfsops;

int rtems_bsd_vfs_mount_init(rtems_filesystem_mount_table_entry_t *mt_entry);

void rtems_bsd_vfs_mt_entry_lock(
    const rtems_filesystem_mount_table_entry_t *mt_entry);
void rtems_bsd_vfs_mt_entry_unlock(
    const rtems_filesystem_mount_table_entry_t *mt_entry);
void rtems_bsd_vfs_eval_path(rtems_filesystem_eval_path_context_t *ctx);
bool rtems_bsd_vfs_are_nodes_equal(const rtems_filesystem_location_info_t *a,
    const rtems_filesystem_location_info_t *b);
int rtems_bsd_vfs_link(const rtems_filesystem_location_info_t *parentloc,
    const rtems_filesystem_location_info_t *targetloc, const char *name,
    size_t namelen);
int rtems_bsd_vfs_fchmod(
    const rtems_filesystem_location_info_t *loc, mode_t mode);
int rtems_bsd_vfs_chown(
    const rtems_filesystem_location_info_t *loc, uid_t owner, gid_t group);
int rtems_bsd_vfs_clonenode(rtems_filesystem_location_info_t *loc);
void rtems_bsd_vfs_freenode(const rtems_filesystem_location_info_t *loc);
int rtems_bsd_vfs_mount(rtems_filesystem_mount_table_entry_t *mt_entry);
int rtems_bsd_vfs_fsmount_me(
    rtems_filesystem_mount_table_entry_t *mt_entry, const void *data);
int rtems_bsd_vfs_unmount(rtems_filesystem_mount_table_entry_t *mt_entry);
void rtems_bsd_vfs_fsunmount_me(rtems_filesystem_mount_table_entry_t *mt_entry);
int rtems_bsd_vfs_mknod(const rtems_filesystem_location_info_t *parentloc,
    const char *name, size_t namelen, mode_t mode, dev_t dev);
int rtems_bsd_vfs_rmnod(const rtems_filesystem_location_info_t *parentloc,
    const rtems_filesystem_location_info_t *loc);
int rtems_bsd_vfs_utimens(
    const rtems_filesystem_location_info_t *loc, time_t actime, time_t modtime);
int rtems_bsd_vfs_symlink(const rtems_filesystem_location_info_t *parentloc,
    const char *name, size_t namelen, const char *target);
ssize_t rtems_bsd_vfs_readlink(
    const rtems_filesystem_location_info_t *loc, char *buf, size_t bufsize);
int rtems_bsd_vfs_rename(const rtems_filesystem_location_info_t *oldparentloc,
    const rtems_filesystem_location_info_t *oldloc,
    const rtems_filesystem_location_info_t *newparentloc, const char *name,
    size_t namelen);
int rtems_bsd_vfs_statvfs(
    const rtems_filesystem_location_info_t *loc, struct statvfs *buf);

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_VFS_H_ */
