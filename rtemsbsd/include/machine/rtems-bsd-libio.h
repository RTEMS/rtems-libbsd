/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief LibIO interface for FreeBSD filedesc.
 */

/*
 * Copyright (c) 2020 Chrs Johns.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_LIBIO_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_LIBIO_H_

#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/proc.h>

#include <machine/rtems-bsd-vfs.h>

#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <stdint.h>

struct rtems_bsd_vfs_loc;

extern const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_nodeops;
extern const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_imfsnodeops;
extern const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_dirops;
extern const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_fileops;

static int inline rtems_bsd_error_to_status_and_errno(int error)
{
	if (error == 0) {
		return 0;
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}

static inline uint32_t
rtems_bsd_libio_fflag_to_flags(u_int fflag)
{
	uint32_t libio_flags = 0;

	if ((fflag & FREAD) == FREAD) {
		libio_flags |= LIBIO_FLAGS_READ;
	}

	if ((fflag & FWRITE) == FWRITE) {
		libio_flags |= LIBIO_FLAGS_WRITE;
	}

	if ((fflag & FNONBLOCK) == FNONBLOCK) {
		libio_flags |= LIBIO_FLAGS_NO_DELAY;
	}

	return (libio_flags);
}

static inline u_int
rtems_bsd_libio_flags_to_fflag(uint32_t libio_flags)
{
	u_int fflag = 0;

	if ((libio_flags & LIBIO_FLAGS_READ) == LIBIO_FLAGS_READ) {
		fflag |= FREAD;
	}

	if ((libio_flags & LIBIO_FLAGS_WRITE) == LIBIO_FLAGS_WRITE) {
		fflag |= FWRITE;
	}

	if ((libio_flags & LIBIO_FLAGS_NO_DELAY) == LIBIO_FLAGS_NO_DELAY) {
		fflag |= FNONBLOCK;
	}

	return (fflag);
}

static inline bool
rtems_bsd_is_libbsd_nvops(rtems_libio_t *iop)
{
	return (iop->pathinfo.handlers == &rtems_bsd_sysgen_dirops ||
	    iop->pathinfo.handlers == &rtems_bsd_sysgen_fileops);
}

static inline bool
rtems_bsd_is_libbsd_descriptor(rtems_libio_t *iop)
{
	return (iop->pathinfo.handlers == &rtems_bsd_sysgen_nodeops ||
	    rtems_bsd_is_libbsd_nvops(iop));
}

static inline rtems_libio_t *
rtems_bsd_libio_loc_to_iop(const rtems_filesystem_location_info_t *loc)
{
	return (rtems_libio_t *)RTEMS_DECONST(
	    rtems_filesystem_location_info_t *, loc)
	    ->node_access;
}

struct socket;

static inline struct socket *
rtems_bsd_libio_imfs_loc_to_so(const rtems_filesystem_location_info_t *loc)
{
	return (struct socket *)RTEMS_DECONST(
	    rtems_filesystem_location_info_t *, loc)
	    ->node_access_2;
}

static struct vnode *
rtems_bsd_libio_loc_to_vnode(const rtems_filesystem_location_info_t *loc)
{
	return (struct vnode *)RTEMS_DECONST(
	    rtems_filesystem_location_info_t *, loc)
	    ->node_access;
}

static struct vnode *
rtems_bsd_libio_loc_to_vnode_dir(const rtems_filesystem_location_info_t *loc)
{
	return (struct vnode *)RTEMS_DECONST(
	    rtems_filesystem_location_info_t *, loc)
	    ->node_access_2;
}

static inline void
rtems_bsd_libio_iop_free(rtems_libio_t *iop)
{
	rtems_libio_free(iop);
}

static int
rtems_bsd_libio_iop_to_descriptor(rtems_libio_t *iop)
{
	return (int)iop->data0;
}

static struct vnode *
rtems_bsd_libio_iop_to_vnode(rtems_libio_t *iop)
{
	return rtems_bsd_libio_loc_to_vnode(&iop->pathinfo);
}

static int
rtems_bsd_libio_fd_to_descriptor(int fd)
{
	return rtems_bsd_libio_iop_to_descriptor(rtems_libio_iop(fd));
}

static inline struct file *
rtems_bsd_libio_iop_to_file_hold(rtems_libio_t *iop, struct thread *td)
{
	struct file *fp;
	int error = fget_unlocked(td->td_proc->p_fd,
	    rtems_bsd_libio_iop_to_descriptor(iop), NULL, &fp, NULL);
	if (error != 0) {
		fp = NULL;
	}
	return fp;
}

static inline int
rtems_bsd_file_to_libio_fd(struct file *fp)
{
	return fp->f_io - rtems_libio_iops;
}

static inline void
rtems_bsd_libio_iop_set_bsd_descriptor(rtems_libio_t *iop, int fd)
{
	iop->data0 = fd;
	/* if not vnops the fstat passes a loc, need to get the iop to get the
	 * fp */
	if (!rtems_bsd_is_libbsd_nvops(iop)) {
		iop->pathinfo.node_access = iop;
	}
}

static inline void
rtems_bsd_libio_iop_set_bsd_file(rtems_libio_t *iop, struct file *fp)
{
	fp->f_io = iop;
}

/*
 * The fd is a libio file descriptor.
 *
 * Return -1 if the descriptor is closed or not valid. The descriptor is not
 * held.
 *
 * If open hold the descriptor. If the descriptor referneces a BSD
 * descriptor return the BSD descriptor else return the libio descriptor.
 *
 * Optionally return the iop in *iopp if the descriptor if a libio descriptor
 * else return NULL.
 */
static inline int
rtems_bsd_libio_iop_hold(int fd, rtems_libio_t **iopp)
{
	rtems_libio_t *iop = NULL;
	unsigned int flags = 0;
	int ffd = -1;
	if (fd < rtems_libio_number_iops) {
		iop = rtems_libio_iop(fd);
		flags = rtems_libio_iop_hold(iop);
		if ((flags & LIBIO_FLAGS_OPEN) != 0) {
			if (rtems_bsd_is_libbsd_descriptor(iop)) {
				ffd = rtems_bsd_libio_iop_to_descriptor(iop);
				if (iopp != NULL) {
					*iopp = NULL;
				}
			} else {
				ffd = fd;
				if (iopp != NULL) {
					*iopp = iop;
				}
			}
		} else {
			rtems_libio_iop_drop(iop);
		}
		if (RTEMS_BSD_DESCRIP_TRACE)
			flags = iop->flags;
	} else {
		*iopp = NULL;
	}
	if (RTEMS_BSD_DESCRIP_TRACE)
		printf("bsd: iop: hold: fd=%d ffd=%d refs=%d iop=%p by %p\n",
		    fd, ffd, flags >> 12, iop, __builtin_return_address(0));
	return ffd;
}

static inline int
rtems_bsd_libio_iop_drop(int fd)
{
	if (RTEMS_BSD_DESCRIP_TRACE)
		printf("bsd: iop: drop: fd=%d refs=%d by %p\n", fd,
		    rtems_libio_iop(fd)->flags >> 12,
		    __builtin_return_address(0));
	rtems_libio_iop_drop(rtems_libio_iop(fd));
	return 0;
}

static inline int
rtems_bsd_libio_fo_poll(int fd, struct file *fp, int events,
    struct ucred *active_cred, struct thread *td)
{
	int error;
	if (fp == NULL) {
		rtems_libio_t *iop = rtems_libio_iop(fd);
		error = (*iop->pathinfo.handlers->poll_h)(iop, events);
	} else {
		error = (*fp->f_ops->fo_poll)(fp, events, active_cred, td);
		fd = rtems_bsd_file_to_libio_fd(fp);
	}
	rtems_bsd_libio_iop_drop(fd);
	return error;
}

static inline void
rtems_bsd_libio_iop_to_knote(struct knote *kn, rtems_libio_t *iop)
{
	kn->kn_fp = (struct file *)iop;
}

static inline struct kqueue *
rtems_bsd_libio_knote_to_kq(struct knote *kn)
{
	struct kqueue *kq = kn->kn_kq;
	if ((kn->kn_status & KN_FP_IS_IOP) == 0) {
		if (kq != kn->kn_fp->f_data)
			panic("libio kq wrong\n");
	}
	return kq;
}

/*
 * Returns an iop with null file system mount or NULL is ENFILE.
 */
rtems_libio_t *rtems_bsd_libio_iop_allocate(void);

/*
 * Returns the libio descriptor or -1 if ENFILE.
 */
int rtems_bsd_libio_iop_allocate_with_file(
    struct thread *td, int fd, const rtems_filesystem_file_handlers_r *ops);

/*
 * Set the BSD file descriptor in the iop. Returns 0 if successful or an error
 * number,
 */
int rtems_bsd_libio_iop_set_bsd_fd(struct thread *td, int fd,
    rtems_libio_t *iop, const rtems_filesystem_file_handlers_r *ops);

/*
 * Set the vnode in the libio location.
 */
void rtems_bsd_libio_loc_set_vnode(
    rtems_filesystem_location_info_t *loc, struct vnode *vn);
void rtems_bsd_libio_loc_set_vnode_dir(
    rtems_filesystem_location_info_t *loc, struct vnode *dvn);

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_LIBIO_H_ */
