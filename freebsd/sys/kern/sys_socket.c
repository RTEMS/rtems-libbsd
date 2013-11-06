#include <machine/rtems-bsd-kernel-space.h>

/*-
 * Copyright (c) 1982, 1986, 1990, 1993
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
 *	@(#)sys_socket.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/sys/param.h>
#include <sys/systm.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/proc.h>
#include <sys/protosw.h>
#include <sys/sigio.h>
#include <sys/signal.h>
#include <sys/signalvar.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/filio.h>			/* XXX */
#include <sys/sockio.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/ucred.h>

#include <net/if.h>
#include <net/route.h>
#include <net/vnet.h>

#include <security/mac/mac_framework.h>

#ifndef __rtems__
struct fileops	socketops = {
	.fo_read = soo_read,
	.fo_write = soo_write,
	.fo_truncate = soo_truncate,
	.fo_ioctl = soo_ioctl,
	.fo_poll = soo_poll,
	.fo_kqfilter = soo_kqfilter,
	.fo_stat = soo_stat,
	.fo_close = soo_close,
	.fo_flags = DFLAG_PASSABLE
};
#endif /* __rtems__ */

/* ARGSUSED */
#ifdef __rtems__
static
#endif /* __rtems__ */
int
soo_read(struct file *fp, struct uio *uio, struct ucred *active_cred,
    int flags, struct thread *td)
{
	struct socket *so = fp->f_data;
	int error;

#ifdef MAC
	error = mac_socket_check_receive(active_cred, so);
	if (error)
		return (error);
#endif
	error = soreceive(so, 0, uio, 0, 0, 0);
	return (error);
}
#ifdef __rtems__
static ssize_t
rtems_bsd_soo_read(rtems_libio_t *iop, void *buffer, size_t count)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct file *fp = rtems_bsd_iop_to_fp(iop);
	struct iovec iov = {
		.iov_base = buffer,
		.iov_len = count
	};
	struct uio auio = {
		.uio_iov = &iov,
		.uio_iovcnt = 1,
		.uio_offset = 0,
		.uio_resid = count,
		.uio_segflg = UIO_USERSPACE,
		.uio_rw = UIO_READ,
		.uio_td = td
	};
	int error;

	if (td != NULL) {
		error = soo_read(fp, &auio, NULL, 0, NULL);
	} else {
		error = ENOMEM;
	}

	if (error == 0) {
		return (count - auio.uio_resid);
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}
#endif /* __rtems__ */

/* ARGSUSED */
#ifdef __rtems__
static
#endif /* __rtems__ */
int
soo_write(struct file *fp, struct uio *uio, struct ucred *active_cred,
    int flags, struct thread *td)
{
	struct socket *so = fp->f_data;
	int error;

#ifdef MAC
	error = mac_socket_check_send(active_cred, so);
	if (error)
		return (error);
#endif
	error = sosend(so, 0, uio, 0, 0, 0, uio->uio_td);
	if (error == EPIPE && (so->so_options & SO_NOSIGPIPE) == 0) {
#ifndef __rtems__
		PROC_LOCK(uio->uio_td->td_proc);
		tdksignal(uio->uio_td, SIGPIPE, NULL);
		PROC_UNLOCK(uio->uio_td->td_proc);
#else /* __rtems__ */
		/* FIXME: Determine if we really want to use signals */
#endif /* __rtems__ */
	}
	return (error);
}
#ifdef __rtems__
static ssize_t
rtems_bsd_soo_write(rtems_libio_t *iop, const void *buffer, size_t count)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct file *fp = rtems_bsd_iop_to_fp(iop);
	struct iovec iov = {
		.iov_base = __DECONST(void *, buffer),
		.iov_len = count
	};
	struct uio auio = {
		.uio_iov = &iov,
		.uio_iovcnt = 1,
		.uio_offset = 0,
		.uio_resid = count,
		.uio_segflg = UIO_USERSPACE,
		.uio_rw = UIO_WRITE,
		.uio_td = td
	};
	int error;

	if (td != NULL) {
		error = soo_write(fp, &auio, NULL, 0, NULL);
	} else {
		error = ENOMEM;
	}

	if (error == 0) {
		return (count - auio.uio_resid);
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}
#endif /* __rtems__ */

#ifndef __rtems__
int
soo_truncate(struct file *fp, off_t length, struct ucred *active_cred,
    struct thread *td)
{

	return (EINVAL);
}
#endif /* __rtems__ */

#ifdef __rtems__
static
#endif /* __rtems__ */
int
soo_ioctl(struct file *fp, u_long cmd, void *data, struct ucred *active_cred,
    struct thread *td)
{
	struct socket *so = fp->f_data;
	int error = 0;

	switch (cmd) {
	case FIONBIO:
		SOCK_LOCK(so);
		if (*(int *)data)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		SOCK_UNLOCK(so);
		break;

	case FIOASYNC:
		/*
		 * XXXRW: This code separately acquires SOCK_LOCK(so) and
		 * SOCKBUF_LOCK(&so->so_rcv) even though they are the same
		 * mutex to avoid introducing the assumption that they are
		 * the same.
		 */
		if (*(int *)data) {
			SOCK_LOCK(so);
			so->so_state |= SS_ASYNC;
			SOCK_UNLOCK(so);
			SOCKBUF_LOCK(&so->so_rcv);
			so->so_rcv.sb_flags |= SB_ASYNC;
			SOCKBUF_UNLOCK(&so->so_rcv);
			SOCKBUF_LOCK(&so->so_snd);
			so->so_snd.sb_flags |= SB_ASYNC;
			SOCKBUF_UNLOCK(&so->so_snd);
		} else {
			SOCK_LOCK(so);
			so->so_state &= ~SS_ASYNC;
			SOCK_UNLOCK(so);
			SOCKBUF_LOCK(&so->so_rcv);
			so->so_rcv.sb_flags &= ~SB_ASYNC;
			SOCKBUF_UNLOCK(&so->so_rcv);
			SOCKBUF_LOCK(&so->so_snd);
			so->so_snd.sb_flags &= ~SB_ASYNC;
			SOCKBUF_UNLOCK(&so->so_snd);
		}
		break;

	case FIONREAD:
		/* Unlocked read. */
		*(int *)data = so->so_rcv.sb_cc;
		break;

	case FIONWRITE:
		/* Unlocked read. */
		*(int *)data = so->so_snd.sb_cc;
		break;

	case FIONSPACE:
		if ((so->so_snd.sb_hiwat < so->so_snd.sb_cc) ||
		    (so->so_snd.sb_mbmax < so->so_snd.sb_mbcnt))
			*(int *)data = 0;
		else
			*(int *)data = sbspace(&so->so_snd);
		break;

	case FIOSETOWN:
		error = fsetown(*(int *)data, &so->so_sigio);
		break;

	case FIOGETOWN:
		*(int *)data = fgetown(&so->so_sigio);
		break;

	case SIOCSPGRP:
		error = fsetown(-(*(int *)data), &so->so_sigio);
		break;

	case SIOCGPGRP:
		*(int *)data = -fgetown(&so->so_sigio);
		break;

	case SIOCATMARK:
		/* Unlocked read. */
		*(int *)data = (so->so_rcv.sb_state & SBS_RCVATMARK) != 0;
		break;
	default:
		/*
		 * Interface/routing/protocol specific ioctls: interface and
		 * routing ioctls should have a different entry since a
		 * socket is unnecessary.
		 */
		if (IOCGROUP(cmd) == 'i')
			error = ifioctl(so, cmd, data, td);
		else if (IOCGROUP(cmd) == 'r') {
			CURVNET_SET(so->so_vnet);
			error = rtioctl_fib(cmd, data, so->so_fibnum);
			CURVNET_RESTORE();
		} else {
			CURVNET_SET(so->so_vnet);
			error = ((*so->so_proto->pr_usrreqs->pru_control)
			    (so, cmd, data, 0, td));
			CURVNET_RESTORE();
		}
		break;
	}
	return (error);
}
#ifdef __rtems__
static int
rtems_bsd_soo_ioctl(rtems_libio_t *iop, ioctl_command_t request, void *buffer)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct file *fp = rtems_bsd_iop_to_fp(iop);
	int error;

	if (td != NULL) {
		error = soo_ioctl(fp, request, buffer, NULL, td);
	} else {
		error = ENOMEM;
	}

	return rtems_bsd_error_to_status_and_errno(error);
}
#endif /* __rtems__ */

#ifdef __rtems__
static
#endif /* __rtems__ */
int
soo_poll(struct file *fp, int events, struct ucred *active_cred,
    struct thread *td)
{
	struct socket *so = fp->f_data;
#ifdef MAC
	int error;

	error = mac_socket_check_poll(active_cred, so);
	if (error)
		return (error);
#endif
#ifndef __rtems__
	return (sopoll(so, events, fp->f_cred, td));
#else /* __rtems__ */
	return (sopoll(so, events, NULL, td));
#endif /* __rtems__ */
}
#ifdef __rtems__
static int
rtems_bsd_soo_poll(rtems_libio_t *iop, int events)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct file *fp = rtems_bsd_iop_to_fp(iop);
	int error;

	if (td != NULL) {
		error = soo_poll(fp, events, NULL, td);
	} else {
		error = ENOMEM;
	}

	return error;
}
#endif /* __rtems__ */

#ifndef __rtems__
int
soo_stat(struct file *fp, struct stat *ub, struct ucred *active_cred,
    struct thread *td)
{
	struct socket *so = fp->f_data;
#else /* __rtems__ */
static int
soo_stat(struct socket *so, struct stat *ub)
{
#endif /* __rtems__ */
#ifdef MAC
	int error;
#endif

#ifndef __rtems__
	bzero((caddr_t)ub, sizeof (*ub));
#endif /* __rtems__ */
	ub->st_mode = S_IFSOCK;
#ifdef MAC
	error = mac_socket_check_stat(active_cred, so);
	if (error)
		return (error);
#endif
	/*
	 * If SBS_CANTRCVMORE is set, but there's still data left in the
	 * receive buffer, the socket is still readable.
	 */
	SOCKBUF_LOCK(&so->so_rcv);
	if ((so->so_rcv.sb_state & SBS_CANTRCVMORE) == 0 ||
	    so->so_rcv.sb_cc != 0)
		ub->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
	ub->st_size = so->so_rcv.sb_cc - so->so_rcv.sb_ctl;
	SOCKBUF_UNLOCK(&so->so_rcv);
	/* Unlocked read. */
	if ((so->so_snd.sb_state & SBS_CANTSENDMORE) == 0)
		ub->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
#ifndef __rtems__
	ub->st_uid = so->so_cred->cr_uid;
	ub->st_gid = so->so_cred->cr_gid;
#else /* __rtems__ */
	ub->st_uid = BSD_DEFAULT_UID;
	ub->st_gid = BSD_DEFAULT_GID;
#endif /* __rtems__ */
	return (*so->so_proto->pr_usrreqs->pru_sense)(so, ub);
}
#ifdef __rtems__
static int
rtems_bsd_soo_stat(
	const rtems_filesystem_location_info_t *loc,
	struct stat *buf
)
{
	struct socket *so = rtems_bsd_loc_to_f_data(loc);
	int error = soo_stat(so, buf);

	return rtems_bsd_error_to_status_and_errno(error);
}
#endif /* __rtems__ */

/*
 * API socket close on file pointer.  We call soclose() to close the socket
 * (including initiating closing protocols).  soclose() will sorele() the
 * file reference but the actual socket will not go away until the socket's
 * ref count hits 0.
 */
/* ARGSUSED */
#ifdef __rtems__
static
#endif /* __rtems__ */
int
soo_close(struct file *fp, struct thread *td)
{
	int error = 0;
	struct socket *so;

#ifdef __rtems__
	/* FIXME: Move this to the RTEMS close() function */
	knote_fdclose(td, rtems_bsd_fp_to_fd(fp));
#endif /* __rtems__ */

	so = fp->f_data;
#ifndef __rtems__
	fp->f_ops = &badfileops;
#else /* __rtems__ */
	fp->f_io.pathinfo.handlers = &rtems_filesystem_handlers_default;
#endif /* __rtems__ */
	fp->f_data = NULL;

	if (so)
		error = soclose(so);
	return (error);
}
#ifdef __rtems__
static int
rtems_bsd_soo_close(rtems_libio_t *iop)
{
	struct file *fp = rtems_bsd_iop_to_fp(iop);
	int error = soo_close(fp, NULL);

	return rtems_bsd_error_to_status_and_errno(error);
}

const rtems_filesystem_file_handlers_r socketops = {
	.open_h = rtems_filesystem_default_open,
	.close_h = rtems_bsd_soo_close,
	.read_h = rtems_bsd_soo_read,
	.write_h = rtems_bsd_soo_write,
	.ioctl_h = rtems_bsd_soo_ioctl,
	.lseek_h = rtems_filesystem_default_lseek,
	.fstat_h = rtems_bsd_soo_stat,
	.ftruncate_h = rtems_filesystem_default_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_filesystem_default_fcntl,
	.poll_h = rtems_bsd_soo_poll,
	.kqfilter_h = rtems_bsd_soo_kqfilter
};
#endif /* __rtems__ */
