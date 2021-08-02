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

rtems_libio_t *
rtems_bsd_libio_iop_allocate(void)
{
	rtems_libio_t *iop = rtems_libio_allocate();
	if (iop != NULL) {
		iop->pathinfo.mt_entry = &rtems_filesystem_null_mt_entry;
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
		struct file *fp = fget_locked(fdp, fd);
		if (fp != NULL) {
			rtems_bsd_libio_iop_set_bsd_file(iop, fp);
			rtems_libio_iop_flags_set(iop,
			    LIBIO_FLAGS_OPEN |
				rtems_bsd_libio_fflag_to_flags(fp->f_flag));
			if (ops != NULL)
				iop->pathinfo.handlers = ops;
			rtems_bsd_libio_iop_set_bsd_descriptor(iop, fd);
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
	if (RTEMS_BSD_DESCRIP_TRACE || true)
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
