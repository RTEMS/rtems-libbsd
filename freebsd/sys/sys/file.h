/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)file.h	8.3 (Berkeley) 1/9/95
 * $FreeBSD$
 */

#ifndef _SYS_FILE_H_
#define	_SYS_FILE_H_

#ifndef _KERNEL
#include <rtems/bsd/sys/types.h> /* XXX */
#include <sys/fcntl.h>
#include <rtems/bsd/sys/unistd.h>
#else
#include <sys/queue.h>
#include <sys/refcount.h>
#include <sys/_lock.h>
#include <sys/_mutex.h>

struct stat;
struct thread;
struct uio;
struct knote;
struct vnode;
struct socket;


#endif /* _KERNEL */

#define	DTYPE_VNODE	1	/* file */
#define	DTYPE_SOCKET	2	/* communications endpoint */
#define	DTYPE_PIPE	3	/* pipe */
#define	DTYPE_FIFO	4	/* fifo (named pipe) */
#define	DTYPE_KQUEUE	5	/* event queue */
#define	DTYPE_CRYPTO	6	/* crypto */
#define	DTYPE_MQUEUE	7	/* posix message queue */
#define	DTYPE_SHM	8	/* swap-backed shared memory */
#define	DTYPE_SEM	9	/* posix semaphore */
#define	DTYPE_PTS	10	/* pseudo teletype master device */

#ifdef _KERNEL

struct file;
struct ucred;

typedef int fo_rdwr_t(struct file *fp, struct uio *uio,
		    struct ucred *active_cred, int flags,
		    struct thread *td);
#define	FOF_OFFSET	1	/* Use the offset in uio argument */
typedef	int fo_truncate_t(struct file *fp, off_t length,
		    struct ucred *active_cred, struct thread *td);
typedef	int fo_ioctl_t(struct file *fp, u_long com, void *data,
		    struct ucred *active_cred, struct thread *td);
typedef	int fo_poll_t(struct file *fp, int events,
		    struct ucred *active_cred, struct thread *td);
typedef	int fo_kqfilter_t(struct file *fp, struct knote *kn);
typedef	int fo_stat_t(struct file *fp, struct stat *sb,
		    struct ucred *active_cred, struct thread *td);
typedef	int fo_close_t(struct file *fp, struct thread *td);
typedef	int fo_flags_t;

struct fileops {
	fo_rdwr_t	*fo_read;
	fo_rdwr_t	*fo_write;
	fo_truncate_t	*fo_truncate;
	fo_ioctl_t	*fo_ioctl;
	fo_poll_t	*fo_poll;
	fo_kqfilter_t	*fo_kqfilter;
	fo_stat_t	*fo_stat;
	fo_close_t	*fo_close;
	fo_flags_t	fo_flags;	/* DFLAG_* below */
};

#define DFLAG_PASSABLE	0x01	/* may be passed via unix sockets. */
#define DFLAG_SEEKABLE	0x02	/* seekable / nonsequential */
#endif /* _KERNEL */

#if defined(_KERNEL) || defined(_WANT_FILE)
#ifdef __rtems__
#include <rtems/libio_.h>
#include <sys/fcntl.h>
#endif /* __rtems__ */
/*
 * Kernel descriptor table.
 * One entry for each open kernel vnode and socket.
 *
 * Below is the list of locks that protects members in struct file.
 *
 * (f) protected with mtx_lock(mtx_pool_find(fp))
 * (d) cdevpriv_mtx
 * none	not locked
 */

struct file {
#ifndef __rtems__
	void		*f_data;	/* file descriptor specific data */
	struct fileops	*f_ops;		/* File operations */
	struct ucred	*f_cred;	/* associated credentials. */
	struct vnode 	*f_vnode;	/* NULL or applicable vnode */
	short		f_type;		/* descriptor type */
	short		f_vnread_flags; /* (f) Sleep lock for f_offset */
	volatile u_int	f_flag;		/* see fcntl.h */
	volatile u_int 	f_count;	/* reference count */
	/*
	 *  DTYPE_VNODE specific fields.
	 */
	int		f_seqcount;	/* Count of sequential accesses. */
	off_t		f_nextoff;	/* next expected read/write offset. */
	struct cdev_privdata *f_cdevpriv; /* (d) Private data for the cdev. */
	/*
	 *  DFLAG_SEEKABLE specific fields
	 */
	off_t		f_offset;
	/*
	 * Mandatory Access control information.
	 */
	void		*f_label;	/* Place-holder for MAC label. */
#else /* __rtems__ */
	rtems_libio_t	f_io;
#endif /* __rtems__ */
};
#ifdef __rtems__
#define f_data f_io.pathinfo.node_access

static inline struct file *
rtems_bsd_iop_to_fp(rtems_libio_t *iop)
{
	return (struct file *) iop;
}

static inline struct file *
rtems_bsd_fd_to_fp(int fd)
{
	return rtems_bsd_iop_to_fp(&rtems_libio_iops[fd]);
}

static inline int
rtems_bsd_fp_to_fd(struct file *fp)
{
	return fp - rtems_bsd_iop_to_fp(&rtems_libio_iops[0]);
}

static inline void *
rtems_bsd_loc_to_f_data(const rtems_filesystem_location_info_t *loc)
{
	return loc->node_access;
}

static inline uint32_t
rtems_bsd_fflag_to_libio_flags(u_int fflag)
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

static int inline
rtems_bsd_error_to_status_and_errno(int error)
{
	if (error == 0) {
		return 0;
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}
#endif /* __rtems__ */

#define	FOFFSET_LOCKED       0x1
#define	FOFFSET_LOCK_WAITING 0x2		 

#endif /* _KERNEL || _WANT_FILE */

/*
 * Userland version of struct file, for sysctl
 */
struct xfile {
	size_t	xf_size;	/* size of struct xfile */
	pid_t	xf_pid;		/* owning process */
	uid_t	xf_uid;		/* effective uid of owning process */
	int	xf_fd;		/* descriptor number */
	void	*xf_file;	/* address of struct file */
	short	xf_type;	/* descriptor type */
	int	xf_count;	/* reference count */
	int	xf_msgcount;	/* references from message queue */
	off_t	xf_offset;	/* file offset */
	void	*xf_data;	/* file descriptor specific data */
	void	*xf_vnode;	/* vnode pointer */
	u_int	xf_flag;	/* flags (see fcntl.h) */
};

#ifdef _KERNEL

#ifdef MALLOC_DECLARE
MALLOC_DECLARE(M_FILE);
#endif

extern struct fileops vnops;
extern struct fileops badfileops;
extern struct fileops socketops;
extern int maxfiles;		/* kernel limit on number of open files */
extern int maxfilesperproc;	/* per process limit on number of open files */
extern volatile int openfiles;	/* actual number of open files */

#ifndef __rtems__
int fget(struct thread *td, int fd, struct file **fpp);
#else /* __rtems__ */
struct file *rtems_bsd_get_file(int fd);

static inline int
fget(struct thread *td, int fd, struct file **fpp)
{
	struct file *fp = rtems_bsd_get_file(fd);

	(void) td;

	*fpp = fp;

	return fp != NULL ? 0 : EBADF;
}
#endif /* __rtems__ */
int fget_read(struct thread *td, int fd, struct file **fpp);
int fget_write(struct thread *td, int fd, struct file **fpp);
int _fdrop(struct file *fp, struct thread *td);

/*
 * The socket operations are used a couple of places.
 * XXX: This is wrong, they should go through the operations vector for
 * XXX: sockets instead of going directly for the individual functions. /phk
 */
fo_rdwr_t	soo_read;
fo_rdwr_t	soo_write;
fo_truncate_t	soo_truncate;
fo_ioctl_t	soo_ioctl;
fo_poll_t	soo_poll;
fo_kqfilter_t	soo_kqfilter;
fo_stat_t	soo_stat;
fo_close_t	soo_close;

#ifndef __rtems__
void finit(struct file *, u_int, short, void *, struct fileops *);
#else /* __rtems__ */
static inline void
finit(struct file *fp, u_int fflag, short type, void *data,
    const rtems_filesystem_file_handlers_r *ops)
{
	rtems_filesystem_location_info_t *pathinfo = &fp->f_io.pathinfo;

	(void) type;

	fp->f_data = data;
	fp->f_io.flags |= rtems_bsd_fflag_to_libio_flags(fflag);

	pathinfo->handlers = ops;
	pathinfo->mt_entry = &rtems_filesystem_null_mt_entry;
	rtems_filesystem_location_add_to_mt_entry(pathinfo);
}
#endif /* __rtems__ */
int fgetvp(struct thread *td, int fd, struct vnode **vpp);
int fgetvp_read(struct thread *td, int fd, struct vnode **vpp);
int fgetvp_write(struct thread *td, int fd, struct vnode **vpp);

int fgetsock(struct thread *td, int fd, struct socket **spp, u_int *fflagp);
void fputsock(struct socket *sp);

#define	fhold(fp)							\
	(refcount_acquire(&(fp)->f_count))
#ifndef __rtems__
#define	fdrop(fp, td)							\
	(refcount_release(&(fp)->f_count) ? _fdrop((fp), (td)) : 0)
#else /* __rtems__ */
#define	fdrop(fp, td) do { } while (0)
#endif /* __rtems__ */

#ifndef __rtems__
static __inline fo_rdwr_t	fo_read;
static __inline fo_rdwr_t	fo_write;
static __inline fo_truncate_t	fo_truncate;
static __inline fo_ioctl_t	fo_ioctl;
static __inline fo_poll_t	fo_poll;
static __inline fo_kqfilter_t	fo_kqfilter;
static __inline fo_stat_t	fo_stat;
static __inline fo_close_t	fo_close;

static __inline int
fo_read(fp, uio, active_cred, flags, td)
	struct file *fp;
	struct uio *uio;
	struct ucred *active_cred;
	int flags;
	struct thread *td;
{

	return ((*fp->f_ops->fo_read)(fp, uio, active_cred, flags, td));
}

static __inline int
fo_write(fp, uio, active_cred, flags, td)
	struct file *fp;
	struct uio *uio;
	struct ucred *active_cred;
	int flags;
	struct thread *td;
{

	return ((*fp->f_ops->fo_write)(fp, uio, active_cred, flags, td));
}

static __inline int
fo_truncate(fp, length, active_cred, td)
	struct file *fp;
	off_t length;
	struct ucred *active_cred;
	struct thread *td;
{

	return ((*fp->f_ops->fo_truncate)(fp, length, active_cred, td));
}
#endif /* __rtems__ */

static __inline int
fo_ioctl(fp, com, data, active_cred, td)
	struct file *fp;
	u_long com;
	void *data;
	struct ucred *active_cred;
	struct thread *td;
{

#ifndef __rtems__
	return ((*fp->f_ops->fo_ioctl)(fp, com, data, active_cred, td));
#else /* __rtems__ */
	int rv;

	(void) active_cred;
	(void) td;

	errno = 0;
	rv = ((*fp->f_io.pathinfo.handlers->ioctl_h)(&fp->f_io, com, data));
	if (rv == 0) {
		return (0);
	} else {
		return (errno);
	}
#endif /* __rtems__ */
}

#ifndef __rtems__
static __inline int
fo_poll(fp, events, active_cred, td)
	struct file *fp;
	int events;
	struct ucred *active_cred;
	struct thread *td;
{

	return ((*fp->f_ops->fo_poll)(fp, events, active_cred, td));
}

static __inline int
fo_stat(fp, sb, active_cred, td)
	struct file *fp;
	struct stat *sb;
	struct ucred *active_cred;
	struct thread *td;
{

	return ((*fp->f_ops->fo_stat)(fp, sb, active_cred, td));
}

static __inline int
fo_close(fp, td)
	struct file *fp;
	struct thread *td;
{

	return ((*fp->f_ops->fo_close)(fp, td));
}

static __inline int
fo_kqfilter(fp, kn)
	struct file *fp;
	struct knote *kn;
{

	return ((*fp->f_ops->fo_kqfilter)(fp, kn));
}
#endif /* __rtems__ */

#endif /* _KERNEL */

#endif /* !SYS_FILE_H */
