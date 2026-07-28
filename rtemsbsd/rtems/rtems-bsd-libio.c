/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright 2020 Chris Johns. All Rights Reserved.
 *
 *  Contemporary Software
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE FREEBSD PROJECT ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE FREEBSD PROJECT BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/filedesc.h>
#include <sys/proc.h>
#include <sys/vnode.h>

#include <machine/rtems-bsd-libio.h>

#include <rtems/libio.h>
#include <rtems/libio_.h>

/*
 * A libbsd descriptor is not on a file system, but it is not an invalid
 * location either.  It used to borrow rtems_filesystem_null_mt_entry, whose
 * operations refuse everything including the clone that fcntl(F_DUPFD) needs,
 * so it has a mount entry of its own.  Only the clone and the two instance
 * locks are ever reached through it; everything else keeps the refusing
 * default.  Locking stays a no-op, as it was with the null entry.
 *
 * mounted is true so that removing the last location can never let
 * rtems_filesystem_location_remove_from_mt_entry() decide this static entry is
 * ready to be unmounted.
 */
static void
rtems_bsd_libio_mt_lock_or_unlock(
    const rtems_filesystem_mount_table_entry_t *mt_entry)
{
	(void)mt_entry;
}

static const rtems_filesystem_operations_table rtems_bsd_libio_mt_ops = {
	.lock_h = rtems_bsd_libio_mt_lock_or_unlock,
	.unlock_h = rtems_bsd_libio_mt_lock_or_unlock,
	.eval_path_h = rtems_filesystem_default_eval_path,
	.link_h = rtems_filesystem_default_link,
	.are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
	.mknod_h = rtems_filesystem_default_mknod,
	.rmnod_h = rtems_filesystem_default_rmnod,
	.fchmod_h = rtems_filesystem_default_fchmod,
	.chown_h = rtems_filesystem_default_chown,
	.clonenod_h = rtems_filesystem_default_clonenode,
	.freenod_h = rtems_filesystem_default_freenode,
	.mount_h = rtems_filesystem_default_mount,
	.unmount_h = rtems_filesystem_default_unmount,
	.fsunmount_me_h = rtems_filesystem_default_fsunmount,
	.utimens_h = rtems_filesystem_default_utimens,
	.symlink_h = rtems_filesystem_default_symlink,
	.readlink_h = rtems_filesystem_default_readlink,
	.rename_h = rtems_filesystem_default_rename,
	.statvfs_h = rtems_filesystem_default_statvfs
};

static rtems_filesystem_mount_table_entry_t rtems_bsd_libio_mt_entry = {
	.location_chain = RTEMS_CHAIN_INITIALIZER_EMPTY(
	    rtems_bsd_libio_mt_entry.location_chain),
	.ops = &rtems_bsd_libio_mt_ops,
	.mt_point_node = &rtems_filesystem_global_location_null,
	.mt_fs_root = &rtems_filesystem_global_location_null,
	.mounted = true,
	.writeable = true,
	.type = "libbsd"
};

rtems_libio_t *
rtems_bsd_libio_iop_allocate(void)
{
	rtems_libio_t *iop = rtems_libio_allocate();
	if (iop != NULL) {
		iop->pathinfo.mt_entry = &rtems_bsd_libio_mt_entry;
		rtems_filesystem_location_add_to_mt_entry(&iop->pathinfo);
	}
	return iop;
}

int
rtems_bsd_libio_iop_allocate_with_file(
    struct thread *td, int fd, const rtems_filesystem_file_handlers_r *ops)
{
	rtems_libio_t *iop = rtems_bsd_libio_iop_allocate();
	int iofd = -1;
	if (iop != NULL) {
		int error = rtems_bsd_libio_iop_set_bsd_fd(td, fd, iop, ops);
		/*
		 * The fp is held and needs to be dropped and that drops the
		 * iop.
		 */
		if (error == 0) {
			rtems_libio_iop_hold(iop);
			iofd = rtems_libio_iop_to_descriptor(iop);
		} else {
			rtems_libio_free(iop);
		}
	}
	return iofd;
}

int
rtems_bsd_libio_iop_set_bsd_fd(struct thread *td, int fd, rtems_libio_t *iop,
    const rtems_filesystem_file_handlers_r *ops)
{
	struct filedesc *fdp = td->td_proc->p_fd;
	int error;
	FILEDESC_XLOCK(fdp);
	if (fd < fdp->fd_nfiles) {
		struct file *fp = fget_noref(fdp, fd);
		if (fp != NULL) {
			rtems_bsd_libio_iop_set_bsd_file(iop, fp);
			rtems_libio_iop_flags_set(iop,
			    LIBIO_FLAGS_OPEN | LIBIO_FLAGS_CLOSE_BUSY |
				rtems_bsd_libio_fflag_to_flags(fp->f_flag));
			if (ops != NULL)
				iop->pathinfo.handlers = ops;
			rtems_bsd_libio_iop_set_bsd_descriptor(iop, fd);
			fdp->fd_ofiles[fd].fde_io = iop;
			error = 0;
		} else {
			error = EBADF;
		}
	} else {
		error = EBADF;
	}
	FILEDESC_XUNLOCK(fdp);
	return error;
}

void
rtems_bsd_libio_loc_set_vnode(
    rtems_filesystem_location_info_t *loc, struct vnode *vp)
{
	struct vnode *old = loc->node_access;
	int hc = 0;
	int rc = 0;
	if (vp != NULL) {
		hc = vp->v_holdcnt;
		rc = vrefcnt(vp);
	}
	int old_hc = 0;
	int old_rc = 0;
	if (old != NULL) {
		old_hc = old->v_holdcnt;
		old_rc = vrefcnt(old);
	}
	if (vp != old) {
		if (old != NULL)
			vrele(old);
		if (vp != NULL)
			VREF(vp);
		loc->node_access = vp;
	}
	int new_hc = 0;
	int new_rc = 0;
	if (vp != NULL) {
		new_hc = vp->v_holdcnt;
		new_rc = vrefcnt(vp);
	}
	int old_new_hc = 0;
	int old_new_rc = 0;
	if (old != NULL) {
		old_new_hc = old->v_holdcnt;
		old_new_rc = vrefcnt(old);
	}
	if (RTEMS_BSD_DESCRIP_TRACE)
		printf(
		    "bsd: lio: set-vode loc=%p vn=%p (%d/%d)->(%d/%d) old=%p (%d/%d)->(%d/%d)\n",
		    loc, vp, hc, rc, new_hc, new_rc, old, old_hc, old_rc,
		    old_new_hc, old_new_rc);
}

void
rtems_bsd_libio_loc_set_vnode_dir(
    rtems_filesystem_location_info_t *loc, struct vnode *dvp)
{
	struct vnode *old = loc->node_access_2;
	int hc = 0;
	int rc = 0;
	if (dvp != NULL) {
		hc = dvp->v_holdcnt;
		rc = vrefcnt(dvp);
	}
	int old_hc = 0;
	int old_rc = 0;
	if (old != NULL) {
		old_hc = old->v_holdcnt;
		old_rc = vrefcnt(old);
	}
	if (dvp != old) {
		if (old != NULL)
			vrele(old);
		if (dvp != NULL)
			VREF(dvp);
		loc->node_access_2 = dvp;
	}
	int new_hc = 0;
	int new_rc = 0;
	if (dvp != NULL) {
		new_hc = dvp->v_holdcnt;
		new_rc = vrefcnt(dvp);
	}
	int old_new_hc = 0;
	int old_new_rc = 0;
	if (old != NULL) {
		old_new_hc = old->v_holdcnt;
		old_new_rc = vrefcnt(old);
	}
	if (RTEMS_BSD_DESCRIP_TRACE)
		printf(
		    "bsd: lio: set-vode-dir loc=%p vn=%p (%d/%d)->(%d/%d) old=%p (%d/%d)->(%d/%d)\n",
		    loc, dvp, hc, rc, new_hc, new_rc, old, old_hc, old_rc,
		    old_new_hc, old_new_rc);
}
