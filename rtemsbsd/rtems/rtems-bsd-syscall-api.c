/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 *
 * File origin from FreeBSD 'lib/libc/gen/sysctlnametomib.c'.
 */

/*
 * Copyright 2001 The FreeBSD Project. All Rights Reserved.
 * Copyright 2020 Chris Johns
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

#if !defined(RTEMS_BSD_CHECK_READDIR_DIRENT)
#define RTEMS_BSD_CHECK_READDIR_DIRENT 0
#endif

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/dirent.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/proc.h>
#include <sys/syscallsubr.h>
#include <sys/sysproto.h>
#include <sys/vnode.h>

#include <machine/rtems-bsd-libio.h>
#include <machine/rtems-bsd-syscall-api.h>
#include <machine/rtems-bsd-vfs.h>

#include <errno.h>
#include <rtems/imfs.h>
#include <rtems/libio.h>
#include <rtems/seterr.h>
#include <stdio.h>

static int rtems_bsd_sysgen_open_error(
    rtems_libio_t *iop, const char *path, int oflag, mode_t mode);
static int rtems_bsd_sysgen_open(
    rtems_libio_t *iop, const char *path, int oflag, mode_t mode);
static int rtems_bsd_sysgen_close(rtems_libio_t *iop);
static ssize_t rtems_bsd_sysgen_read(
    rtems_libio_t *iop, void *buffer, size_t count);
static ssize_t rtems_bsd_sysgen_readv(
    rtems_libio_t *iop, const struct iovec *iov, int iovcnt, ssize_t total);
static ssize_t rtems_bsd_sysgen_write(
    rtems_libio_t *iop, const void *buffer, size_t count);
static ssize_t rtems_bsd_sysgen_writev(
    rtems_libio_t *iop, const struct iovec *iov, int iovcnt, ssize_t total);
static int rtems_bsd_sysgen_ioctl(
    rtems_libio_t *iop, ioctl_command_t request, void *buffer);
static off_t rtems_bsd_sysgen_lseek(
    rtems_libio_t *iop, off_t offset, int whence);
static int rtems_bsd_sysgen_vnstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf);
static int rtems_bsd_sysgen_fstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf);
static int rtems_bsd_sysgen_imfsfstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf);
static int rtems_bsd_sysgen_ftruncate(rtems_libio_t *iop, off_t length);
static int rtems_bsd_sysgen_fsync(rtems_libio_t *iop);
static int rtems_bsd_sysgen_fdatasync(rtems_libio_t *iop);
static int rtems_bsd_sysgen_fcntl(rtems_libio_t *iop, int cmd);
static int rtems_bsd_sysgen_poll(rtems_libio_t *iop, int events);
static int rtems_bsd_sysgen_kqfilter(rtems_libio_t *iop, struct knote *kn);

const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_dirops = {
	.open_h = rtems_bsd_sysgen_open,
	.close_h = rtems_bsd_sysgen_close,
	.read_h = rtems_bsd_sysgen_read,
	.write_h = rtems_filesystem_default_write,
	.ioctl_h = rtems_filesystem_default_ioctl,
	.lseek_h = rtems_filesystem_default_lseek_directory,
	.fstat_h = rtems_bsd_sysgen_vnstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate_directory,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_bsd_sysgen_fdatasync,
	.fcntl_h = rtems_filesystem_default_fcntl,
	.kqfilter_h = rtems_filesystem_default_kqfilter,
	.mmap_h = rtems_filesystem_default_mmap,
	.poll_h = rtems_filesystem_default_poll,
	.readv_h = rtems_filesystem_default_readv,
	.writev_h = rtems_filesystem_default_writev
};

const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_fileops = {
	.open_h = rtems_bsd_sysgen_open,
	.close_h = rtems_bsd_sysgen_close,
	.read_h = rtems_bsd_sysgen_read,
	.write_h = rtems_bsd_sysgen_write,
	.ioctl_h = rtems_bsd_sysgen_ioctl,
	.lseek_h = rtems_bsd_sysgen_lseek,
	.fstat_h = rtems_bsd_sysgen_vnstat,
	.ftruncate_h = rtems_bsd_sysgen_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_bsd_sysgen_fdatasync,
	.fcntl_h = rtems_bsd_sysgen_fcntl,
	.kqfilter_h = rtems_bsd_sysgen_kqfilter,
	.mmap_h = rtems_filesystem_default_mmap,
	.poll_h = rtems_bsd_sysgen_poll,
	.readv_h = rtems_bsd_sysgen_readv,
	.writev_h = rtems_bsd_sysgen_writev
};

const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_nodeops = {
	.open_h = rtems_bsd_sysgen_open_error,
	.close_h = rtems_bsd_sysgen_close,
	.read_h = rtems_bsd_sysgen_read,
	.write_h = rtems_bsd_sysgen_write,
	.ioctl_h = rtems_bsd_sysgen_ioctl,
	.lseek_h = rtems_filesystem_default_lseek,
	.fstat_h = rtems_bsd_sysgen_fstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_bsd_sysgen_fcntl,
	.poll_h = rtems_bsd_sysgen_poll,
	.kqfilter_h = rtems_bsd_sysgen_kqfilter,
	.readv_h = rtems_bsd_sysgen_readv,
	.writev_h = rtems_bsd_sysgen_writev,
	.mmap_h = rtems_filesystem_default_mmap
};

const rtems_filesystem_file_handlers_r rtems_bsd_sysgen_imfsnodeops = {
	.open_h = rtems_bsd_sysgen_open_error,
	.close_h = rtems_bsd_sysgen_close,
	.read_h = rtems_bsd_sysgen_read,
	.write_h = rtems_bsd_sysgen_write,
	.ioctl_h = rtems_bsd_sysgen_ioctl,
	.lseek_h = rtems_filesystem_default_lseek,
	.fstat_h = rtems_bsd_sysgen_imfsfstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_bsd_sysgen_fcntl,
	.poll_h = rtems_bsd_sysgen_poll,
	.kqfilter_h = rtems_bsd_sysgen_kqfilter,
	.readv_h = rtems_bsd_sysgen_readv,
	.writev_h = rtems_bsd_sysgen_writev,
	.mmap_h = rtems_filesystem_default_mmap
};

int
accept(int socket, struct sockaddr *__restrict address,
    socklen_t *__restrict address_len)
{
	struct thread *td;
	int error;
	struct accept_args ua;
	rtems_libio_t *iop;
	int afd;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: accept: %d\n", socket);
	}
	td = rtems_bsd_get_curthread_or_null();
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	int ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.name = address;
	ua.anamelen = address_len;
	error = sys_accept(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	iop = rtems_bsd_libio_iop_allocate();
	afd = td->td_retval[0];
	if (iop == NULL) {
		kern_close(td, afd);
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, afd, iop, &rtems_bsd_sysgen_nodeops);
	if (error != 0) {
		rtems_bsd_libio_iop_free(iop);
		kern_close(td, afd);
		return rtems_bsd_error_to_status_and_errno(error);
	}
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: accept: %d (%d) => %d -> %d\n", socket, ffd,
		    rtems_libio_iop_to_descriptor(iop),
		    rtems_bsd_libio_iop_to_descriptor(iop));
	}
	return rtems_libio_iop_to_descriptor(iop);
}

int
bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct bind_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: bind: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd == -1) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.name = address;
	ua.namelen = address_len;
	error = sys_bind(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct connect_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: connect: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.name = address;
	ua.namelen = address_len;
	error = sys_connect(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
getpeername(int socket, struct sockaddr *__restrict address,
    socklen_t *__restrict address_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct getpeername_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: getpeername: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.fdes = ffd;
	ua.asa = address;
	ua.alen = address_len;
	error = sys_getpeername(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
getsockname(int socket, struct sockaddr *__restrict address,
    socklen_t *__restrict address_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct getsockname_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: getsockname: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.fdes = ffd;
	ua.asa = address;
	ua.alen = address_len;
	error = sys_getsockname(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
getsockopt(int socket, int level, int option_name,
    void *__restrict option_value, socklen_t *__restrict option_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct getsockopt_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: getsockopt: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.level = level;
	ua.name = option_name;
	ua.val = (caddr_t)option_value;
	ua.avalsize = option_len;
	error = sys_getsockopt(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
kqueue(void)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct kqueue_args ua = {};
	rtems_libio_t *iop;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: kqueue:\n");
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	iop = rtems_bsd_libio_iop_allocate();
	if (iop == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	error = sys_kqueue(td, &ua);
	if (error != 0) {
		goto out;
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, td->td_retval[0], iop, &rtems_bsd_sysgen_nodeops);
	if (error == 0) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: kqueue: %d -> %d\n",
			    rtems_libio_iop_to_descriptor(iop),
			    rtems_bsd_libio_iop_to_descriptor(iop));
		}
		return rtems_libio_iop_to_descriptor(iop);
	}
	kern_close(td, rtems_libio_iop_to_descriptor(iop));
out:
	rtems_bsd_libio_iop_free(iop);
	return rtems_bsd_error_to_status_and_errno(error);
}

__weak_reference(kevent, _kevent);

int
kevent(int kq, const struct kevent *changelist, int nchanges,
    struct kevent *eventlist, int nevents, const struct timespec *timeout)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct kevent_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: kevent: %d\n", kq);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(kq, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.fd = ffd;
	ua.changelist = changelist;
	ua.nchanges = nchanges;
	ua.eventlist = eventlist;
	ua.nevents = nevents;
	ua.timeout = timeout;
	error = sys_kevent(td, &ua);
	rtems_bsd_libio_iop_drop(kq);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

int
listen(int socket, int backlog)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct listen_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: listen: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.backlog = backlog;
	error = sys_listen(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
pipe(int fildes[2])
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	rtems_libio_t *iop[2];
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: pipe: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	iop[0] = rtems_bsd_libio_iop_allocate();
	if (iop[0] == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	iop[1] = rtems_bsd_libio_iop_allocate();
	if (iop[1] == NULL) {
		rtems_bsd_libio_iop_free(iop[0]);
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	error = kern_pipe(td, fildes, 0, NULL, NULL);
	if (error != 0) {
		goto out;
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, fildes[0], iop[0], &rtems_bsd_sysgen_nodeops);
	if (error != 0) {
		goto out;
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, fildes[1], iop[1], &rtems_bsd_sysgen_nodeops);
	if (error == 0) {
		fildes[0] = rtems_libio_iop_to_descriptor(iop[0]);
		fildes[1] = rtems_libio_iop_to_descriptor(iop[1]);
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: pipe: %d -> %d, %d -> %d\n",
			    fildes[0],
			    rtems_bsd_libio_iop_to_descriptor(iop[0]),
			    fildes[1],
			    rtems_bsd_libio_iop_to_descriptor(iop[1]));
		}
		return 0;
	}
out:
	kern_close(td, rtems_bsd_libio_iop_to_descriptor(iop[0]));
	kern_close(td, rtems_bsd_libio_iop_to_descriptor(iop[1]));
	rtems_bsd_libio_iop_free(iop[0]);
	rtems_bsd_libio_iop_free(iop[1]);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct poll_args ua;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: poll: %d\n", nfds);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	/*
	 * Pass libio descriptors through as libio and bsd descriptors
	 * can be in the list at the same time.
	 */
	ua.fds = &fds[0];
	ua.nfds = nfds;
	ua.timeout = timeout;
	error = sys_poll(td, &ua);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

int
pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
    const struct timespec *timeout, const sigset_t *set)
{
	struct thread *td;
	struct timeval tv;
	struct timeval *tvp;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: pselect: %d\n", nfds);
	}
	if (set != NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOSYS);
	}
	td = rtems_bsd_get_curthread_or_null();
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	if (timeout != NULL) {
		TIMESPEC_TO_TIMEVAL(&tv, timeout);
		tvp = &tv;
	} else {
		tvp = NULL;
	}
	/*
	 * Pass libio descriptors through as libio and bsd descriptors
	 * can be in the list at the same time.
	 */
	error = kern_select(
	    td, nfds, readfds, writefds, errorfds, tvp, NFDBITS);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

ssize_t
recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
    struct sockaddr *__restrict address, socklen_t *__restrict address_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct recvfrom_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: recvfrom: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.buf = buffer;
	ua.len = length;
	ua.flags = flags;
	ua.from = address;
	ua.fromlenaddr = address_len;
	error = sys_recvfrom(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

ssize_t
recvmsg(int socket, struct msghdr *message, int flags)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct recvmsg_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: recvmsg: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.msg = message;
	ua.flags = flags;
	error = sys_recvmsg(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

int
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
    struct timeval *timeout)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: select: %d\n", nfds);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	/*
	 * Pass libio descriptors through as libio and bsd descriptors
	 * can be in the list at the same time.
	 */
	error = kern_select(
	    td, nfds, readfds, writefds, errorfds, timeout, NFDBITS);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

ssize_t
sendto(int socket, const void *message, size_t length, int flags,
    const struct sockaddr *dest_addr, socklen_t dest_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct sendto_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: sendto: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.buf = (caddr_t)message;
	ua.len = length;
	ua.flags = flags;
	ua.to = dest_addr;
	ua.tolen = dest_len;
	error = sys_sendto(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_retval[0];
}

ssize_t
sendmsg(int socket, const struct msghdr *message, int flags)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct sendmsg_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: sendmsg: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.msg = message;
	ua.flags = flags;
	error = sys_sendmsg(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
setfib(int fibnum)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int error;
	if (td != NULL) {
		struct setfib_args ua = { .fibnum = fibnum };
		error = sys_setfib(td, &ua);
	} else {
		error = ENOMEM;
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
setsockopt(int socket, int level, int option_name, const void *option_value,
    socklen_t option_len)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct setsockopt_args ua;
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: setsockopt: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	ua.s = ffd;
	ua.level = level;
	ua.name = option_name;
	ua.val = __DECONST(void *, option_value);
	ua.valsize = option_len;
	error = sys_setsockopt(td, &ua);
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
shutdown(int socket, int how)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int ffd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: shutdown: %d\n", socket);
	}
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	ffd = rtems_bsd_libio_iop_hold(socket, NULL);
	if (ffd < 0) {
		return rtems_bsd_error_to_status_and_errno(EBADF);
	}
	if (rtems_bsd_is_libbsd_descriptor(rtems_libio_iop(socket))) {
		struct shutdown_args ua = { .s = ffd, .how = how };
		error = sys_shutdown(td, &ua);
	} else {
		error = ENOTSOCK;
	}
	rtems_bsd_libio_iop_drop(socket);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
socket(int domain, int type, int protocol)
{
	struct thread *td;
	rtems_libio_t *iop;
	struct socket_args ua;
	int error;
	td = rtems_bsd_get_curthread_or_null();
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	iop = rtems_bsd_libio_iop_allocate();
	if (iop == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	ua.domain = domain;
	ua.type = type;
	ua.protocol = protocol;
	error = sys_socket(td, &ua);
	if (error != 0) {
		rtems_bsd_libio_iop_free(iop);
		return rtems_bsd_error_to_status_and_errno(error);
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, td->td_retval[0], iop, &rtems_bsd_sysgen_nodeops);
	if (error != 0) {
		kern_close(td, td->td_retval[0]);
		rtems_bsd_libio_iop_free(iop);
		return rtems_bsd_error_to_status_and_errno(error);
	}
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: socket: %d -> %d\n",
		    rtems_libio_iop_to_descriptor(iop),
		    rtems_bsd_libio_iop_to_descriptor(iop));
	}
	return rtems_libio_iop_to_descriptor(iop);
}

int
socketpair(int domain, int type, int protocol, int *socket_vector)
{
	struct thread *td;
	rtems_libio_t *iop[2];
	struct socketpair_args ua;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: socketpair:\n");
	}
	td = rtems_bsd_get_curthread_or_null();
	if (td == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	iop[0] = rtems_bsd_libio_iop_allocate();
	if (iop[0] == NULL) {
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	iop[1] = rtems_bsd_libio_iop_allocate();
	if (iop[1] == NULL) {
		rtems_bsd_libio_iop_free(iop[0]);
		return rtems_bsd_error_to_status_and_errno(ENFILE);
	}
	ua.domain = domain;
	ua.type = type;
	ua.protocol = protocol;
	ua.rsv = socket_vector;
	error = sys_socketpair(td, &ua);
	if (error != 0) {
		goto out;
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, socket_vector[0], iop[0], &rtems_bsd_sysgen_nodeops);
	if (error != 0) {
		goto out;
	}
	error = rtems_bsd_libio_iop_set_bsd_fd(
	    td, socket_vector[1], iop[1], &rtems_bsd_sysgen_nodeops);
	if (error == 0) {
		socket_vector[0] = rtems_libio_iop_to_descriptor(iop[0]);
		socket_vector[1] = rtems_libio_iop_to_descriptor(iop[1]);
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: socketpair: %d -> %d, %d -> %d\n",
			    socket_vector[0],
			    rtems_bsd_libio_iop_to_descriptor(iop[0]),
			    socket_vector[1],
			    rtems_bsd_libio_iop_to_descriptor(iop[1]));
		}
		return 0;
	}
out:
	kern_close(td, rtems_bsd_libio_iop_to_descriptor(iop[0]));
	kern_close(td, rtems_bsd_libio_iop_to_descriptor(iop[1]));
	rtems_bsd_libio_iop_free(iop[0]);
	rtems_bsd_libio_iop_free(iop[1]);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
sysctl(const int *name, u_int namelen, void *oldp, size_t *oldlenp,
    const void *newp, size_t newlen)
{
	int error = EINVAL;
	if (namelen <= CTL_MAXNAME) {
		int namedup[CTL_MAXNAME];
		memcpy(namedup, name, namelen * sizeof(*name));
		error = kernel_sysctl(NULL, namedup, namelen, oldp, oldlenp,
		    RTEMS_DECONST(void *, newp), newlen, oldlenp, 0);
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

/*
 * File origin from FreeBSD 'lib/libc/gen/sysctlbyname.c'.
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.org> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 */
int
sysctlbyname(const char *name, void *oldp, size_t *oldlenp, const void *newp,
    size_t newlen)
{
	int real_oid[CTL_MAXNAME + 2];
	int error;
	size_t oidlen;
	oidlen = sizeof(real_oid) / sizeof(int);
	error = sysctlnametomib(name, real_oid, &oidlen);
	if (error < 0)
		return (error);
	error = sysctl(real_oid, oidlen, oldp, oldlenp, newp, newlen);
	return rtems_bsd_error_to_status_and_errno(error);
}

/*
 * File origin from FreeBSD 'lib/libc/gen/sysctlnametomib.c'.
 *
 * This function uses a presently undocumented interface to the kernel
 * to walk the tree and get the type so it can print the value.
 * This interface is under work and consideration, and should probably
 * be killed with a big axe by the first person who can find the time.
 * (be aware though, that the proper interface isn't as obvious as it
 * may seem, there are various conflicting requirements.
 */
int
sysctlnametomib(const char *name, int *mibp, size_t *sizep)
{
	int oid[2];
	int error;
	oid[0] = 0;
	oid[1] = 3;
	*sizep *= sizeof(int);
	error = sysctl(oid, 2, mibp, sizep, name, strlen(name));
	*sizep /= sizeof(int);
	return (error);
}

static int
rtems_bsd_sysgen_open_error(
    rtems_libio_t *iop, const char *path, int oflag, mode_t mode)
{
	return rtems_bsd_error_to_status_and_errno(ENXIO);
}

int
rtems_bsd_sysgen_open(
    rtems_libio_t *iop, const char *path, int oflag, mode_t mode)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct filedesc *fdp;
	struct file *fp;
	const bool creat = (oflag & O_CREAT) == O_CREAT;
	struct vnode *cdir;
	struct vnode *rdir;
	const char *opath;
	int opathlen;
	int fd;
	int error;

	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: open: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	fdp = td->td_proc->p_fd;

	/*
	 * There is no easy or clean means to open a vnode and follow the
	 * POSIX open semantics. You can open a vnode but the extra
	 * functionality such as create and truncate are not part of the
	 * basic vnode open. All the calls that provide that functionality
	 * take a path as the argument. As a result find the last token in
	 * the path and use the parent directory vnode to position ourselves
	 * in the parent directory. The pathloc vnode points to the '.' or
	 * '..'  directory.
	 */
	opath = path + strlen(path);
	opathlen = 0;
	while (opath != path && !rtems_filesystem_is_delimiter(opath[-1])) {
		opath--;
		opathlen++;
	}
	if (rtems_filesystem_is_current_directory(opath, opathlen) ||
	    rtems_filesystem_is_parent_directory(opath, opathlen)) {
		if (((oflag + 1) & _FWRITE) == _FWRITE) {
			if (RTEMS_BSD_SYSCALL_TRACE) {
				printf("bsd: sys: open: write to .  or ..\n");
			}
			return rtems_bsd_error_to_status_and_errno(EPERM);
		}
		opath = ".";
		cdir = rtems_bsd_libio_loc_to_vnode(&iop->pathinfo);
	} else {
		rtems_filesystem_location_info_t *rootloc =
		    &iop->pathinfo.mt_entry->mt_fs_root->location;
		cdir = rtems_bsd_libio_loc_to_vnode_dir(&iop->pathinfo);
		if (fdp->fd_cdir == NULL ||
		    rtems_bsd_libio_loc_to_vnode(&iop->pathinfo) ==
			rtems_bsd_libio_loc_to_vnode(rootloc)) {
			cdir = rtems_bsd_libio_loc_to_vnode(rootloc);
		}
	}

	FILEDESC_XLOCK(fdp);
	rdir = fdp->fd_cdir;
	fdp->fd_cdir = cdir;
	cdir = rdir;
	rdir = fdp->fd_rdir;
	fdp->fd_rdir = fdp->fd_cdir;
	FILEDESC_XUNLOCK(fdp);

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: open: path=%s opath=%s vn=%p cwd=%p"
		       " flags=%08x mode=%08x\n",
		    path, opath,
		    creat ? NULL : rtems_bsd_libio_loc_to_vnode(&iop->pathinfo),
		    fdp->fd_cdir, oflag, mode);
	}

	VREF(fdp->fd_cdir);

	error = kern_openat(td, AT_FDCWD, RTEMS_DECONST(char *, opath),
	    UIO_USERSPACE, oflag, mode);

	vrele(fdp->fd_cdir);

	if (error != 0) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: open: error = (%d) %s\n", error,
			    strerror(error));
		}
		return rtems_bsd_error_to_status_and_errno(error);
	}

	fd = td->td_retval[0];

	FILEDESC_XLOCK(fdp);
	fdp->fd_cdir = cdir;
	fdp->fd_rdir = rdir;
	if (fd < fdp->fd_nfiles) {
		struct vnode *vn;
		fp = fget_locked(fdp, fd);
		if (fp != NULL) {
			vn = fp->f_vnode;
		} else {
			vn = NULL;
		}
		rtems_bsd_libio_loc_set_vnode(&iop->pathinfo, vn);
	}
	FILEDESC_XUNLOCK(fdp);

	rtems_bsd_libio_iop_set_bsd_fd(td, fd, iop, &rtems_bsd_sysgen_fileops);

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: open: fd = %d vn=%p\n", fd,
		    rtems_bsd_libio_loc_to_vnode(&iop->pathinfo));
	}

	return 0;
}

int
rtems_bsd_sysgen_close(rtems_libio_t *iop)
{
	struct thread *td = curthread;
	int error;
	int ffd = rtems_bsd_libio_iop_to_descriptor(iop);
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: close: %d -> %d\n",
		    rtems_libio_iop_to_descriptor(iop), ffd);
	}
	if (td != NULL) {
		if (ffd >= 0) {
			rtems_libio_iop_hold(iop);
			error = kern_close(td, ffd);
		} else {
			error = EBADF;
		}
	} else {
		error = ENOMEM;
	}
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: close: %d: %d: %s\n",
		    rtems_libio_iop_to_descriptor(iop), error, strerror(error));
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

ssize_t
rtems_bsd_sysgen_read(rtems_libio_t *iop, void *buffer, size_t count)
{
	struct thread *td = curthread;
	struct vnode *vp = rtems_bsd_libio_iop_to_vnode(iop);
	int fd = rtems_bsd_libio_iop_to_descriptor(iop);
	int error;
	ssize_t size = 0;

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: read: %d -> %d: vn=%p len=%d\n",
		    rtems_libio_iop_to_descriptor(iop), fd, vp, count);
	}

	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: read: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	if (count > IOSIZE_MAX)
		return rtems_bsd_error_to_status_and_errno(EINVAL);

	if (vp != NULL && vp->v_type == VDIR) {
		off_t offset;
		error = kern_getdirentries(
		    td, fd, buffer, count, &iop->offset, NULL, UIO_USERSPACE);
		size = td->td_retval[0];
		if (RTEMS_BSD_CHECK_READDIR_DIRENT) {
			/*
			 * Helper code for integration of a file system. The
			 * FreeBSD kernel dirent and the newlib structs are not
			 * the same format.
			 */
			size_t offset = 0;
			int c = 0;
			printk(
			    "bsd: sys: readdir: buffer: %p count:%d: size=%d\n",
			    buffer, count, size);
			while (offset < size) {
				struct dirent *dp =
				    (struct dirent *)(((char *)buffer) +
					offset);
				printk(
				    "dirent: %3d: dp=%p off=%d rl=%-3d fn=%-6d name=%-3d '",
				    c, dp, (int)dp->d_off, (int)dp->d_reclen,
				    (int)dp->d_fileno, (int)dp->d_namlen);
				if (dp->d_namlen < sizeof(dp->d_name)) {
					for (int i = 0; i < dp->d_namlen; ++i) {
						printk("%c", dp->d_name[i]);
					}
				} else {
					printk("INVALID NAME LENGTH");
				}
				printk("'\n");
				if (dp->d_reclen <= 0) {
					break;
				}
				c++;
				offset += dp->d_reclen;
				if (offset > count) {
					printf("dirent: buffer overflow\n");
				}
			}
		}
	} else {
		struct iovec aiov = { .iov_base = buffer, .iov_len = count };
		struct uio auio = { .uio_iov = &aiov,
			.uio_iovcnt = 1,
			.uio_offset = iop->offset,
			.uio_resid = count,
			.uio_segflg = UIO_USERSPACE,
			.uio_rw = UIO_READ,
			.uio_td = td };
		error = kern_readv(
		    td, rtems_bsd_libio_iop_to_descriptor(iop), &auio);
		if (error == 0)
			size = td->td_retval[0];
	}

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: read: %d: %d: %s size=%i\n",
		    rtems_libio_iop_to_descriptor(iop), error, strerror(error),
		    size);
	}

	if (error != 0)
		return rtems_bsd_error_to_status_and_errno(error);

	return size;
}

ssize_t
rtems_bsd_sysgen_readv(
    rtems_libio_t *iop, const struct iovec *iov, int iovcnt, ssize_t total)
{
	struct thread *td = curthread;
	struct uio auio;
	int error;

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: readv: %d len=%d\n",
		    rtems_libio_iop_to_descriptor(iop), total);
	}

	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: readv: readv: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	if (total > IOSIZE_MAX)
		return rtems_bsd_error_to_status_and_errno(EINVAL);

	auio.uio_iov = RTEMS_DECONST(struct iovec *, iov);
	auio.uio_iovcnt = iovcnt;
	auio.uio_resid = total;
	auio.uio_segflg = UIO_USERSPACE;

	error = kern_readv(td, rtems_bsd_libio_iop_to_descriptor(iop), &auio);

	if (error != 0)
		return rtems_bsd_error_to_status_and_errno(error);

	return td->td_retval[0];
}

ssize_t
rtems_bsd_sysgen_write(rtems_libio_t *iop, const void *buffer, size_t count)
{
	struct thread *td = curthread;
	struct uio auio;
	struct iovec aiov;
	int error;

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: write: %d len=%d\n",
		    rtems_libio_iop_to_descriptor(iop), count);
	}

	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: write: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	if (count > IOSIZE_MAX)
		return (EINVAL);

	aiov.iov_base = RTEMS_DECONST(void *, buffer);
	aiov.iov_len = count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = count;
	auio.uio_segflg = UIO_USERSPACE;

	error = kern_writev(td, rtems_bsd_libio_iop_to_descriptor(iop), &auio);

	if (error != 0)
		return rtems_bsd_error_to_status_and_errno(error);

	return td->td_retval[0];
}

ssize_t
rtems_bsd_sysgen_writev(
    rtems_libio_t *iop, const struct iovec *iov, int iovcnt, ssize_t total)
{
	struct thread *td = curthread;
	struct uio auio;
	int error;

	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: writev: %d iocnt=%d len=%d\n",
		    rtems_libio_iop_to_descriptor(iop), iovcnt, total);
	}

	if (total > IOSIZE_MAX)
		return EINVAL;

	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: writev: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}

	auio.uio_iov = RTEMS_DECONST(struct iovec *, iov);
	auio.uio_iovcnt = iovcnt;
	auio.uio_resid = total;
	auio.uio_segflg = UIO_USERSPACE;

	error = kern_writev(td, rtems_bsd_libio_iop_to_descriptor(iop), &auio);

	if (error != 0)
		return rtems_bsd_error_to_status_and_errno(error);

	return td->td_retval[0];
}

int
rtems_bsd_sysgen_ioctl(
    rtems_libio_t *iop, ioctl_command_t request, void *buffer)
{
	struct thread *td = curthread;
	u_long com = request & 0xffffffff;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: ioctl: %d req=%08x\n",
		    rtems_libio_iop_to_descriptor(iop), com);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: ioctl: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = kern_ioctl(
	    td, rtems_bsd_libio_iop_to_descriptor(iop), com, buffer);
	return rtems_bsd_error_to_status_and_errno(error);
}

off_t
rtems_bsd_sysgen_lseek(rtems_libio_t *iop, off_t offset, int whence)
{
	struct thread *td = curthread;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: lseek: %d offset=%zu whence=%d\n",
		    rtems_libio_iop_to_descriptor(iop), offset, whence);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: lseek: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = kern_lseek(
	    td, rtems_bsd_libio_iop_to_descriptor(iop), offset, whence);
	if (error != 0) {
		return rtems_bsd_error_to_status_and_errno(error);
	}
	return td->td_uretoff.tdu_off;
}

int
rtems_bsd_sysgen_vnstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf)
{
	struct thread *td = curthread;
	struct vnode *vp = rtems_bsd_libio_loc_to_vnode(loc);
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: vnstat: %p\n", vp);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: vnstat: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	if (vp == NULL)
		error = EFAULT;
	else {
		VOP_LOCK(vp, LK_SHARED);
		error = vn_stat(vp, buf, td->td_ucred, NOCRED, td);
		VOP_UNLOCK(vp, 0);
	}
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: vnstat: exit %p\n", vp);
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_fstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf)
{
	struct thread *td = curthread;
	rtems_libio_t *iop = rtems_bsd_libio_loc_to_iop(loc);
	struct filedesc *fdp;
	struct file *fp = NULL;
	int fd;
	int error;
	if (iop == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: ffile: no iop\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENXIO);
	}
	fd = rtems_bsd_libio_iop_to_descriptor(iop);
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: fstat: %d\n", fd);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: fstat: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	fdp = td->td_proc->p_fd;
	FILEDESC_XLOCK(fdp);
	if (fd < fdp->fd_nfiles) {
		fp = fget_locked(fdp, fd);
	}
	FILEDESC_XUNLOCK(fdp);
	if (fp != NULL) {
		error = fo_stat(fp, buf, NULL, td);
	} else {
		error = EBADF;
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_imfsfstat(
    const rtems_filesystem_location_info_t *loc, struct stat *buf)
{
	struct thread *td = curthread;
	struct socket *so = rtems_bsd_libio_imfs_loc_to_so(loc);
	struct filedesc *fdp;
	struct file *fp = NULL;
	struct socket *fd_so = NULL;
	int fd;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: imfsfstat: socket=%p\n", so);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: fstat: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	fdp = td->td_proc->p_fd;
	FILEDESC_XLOCK(fdp);
	for (fd = 0; fd < fdp->fd_nfiles; fd++) {
		fp = fget_locked(fdp, fd);
		fd_so = fp->f_data;
		if (so == fd_so) {
			break;
		}
		fp = NULL;
	}
	FILEDESC_XUNLOCK(fdp);
	if (fp != NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: imfsfstat: %d\n", fd);
		}
		error = fo_stat(fp, buf, NULL, td);
	} else {
		error = EBADF;
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_ftruncate(rtems_libio_t *iop, off_t length)
{
	struct thread *td = curthread;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: ftruncate: len=%d\n", length);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: ftruncate: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = kern_ftruncate(
	    td, rtems_bsd_libio_iop_to_descriptor(iop), length);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_fsync(rtems_libio_t *iop)
{
	struct thread *td = curthread;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: fsync\n");
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: fsync: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = kern_fsync(td, rtems_bsd_libio_iop_to_descriptor(iop), true);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_fdatasync(rtems_libio_t *iop)
{
	struct thread *td = curthread;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: fdatasync\n");
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: fdatasync: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	error = kern_fsync(td, rtems_bsd_libio_iop_to_descriptor(iop), false);
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_fcntl(rtems_libio_t *iop, int cmd)
{
	struct thread *td = curthread;
	intptr_t arg;
	int error;
	if (RTEMS_BSD_SYSCALL_TRACE) {
		printf("bsd: sys: fcntl: %d cmd=%d\n",
		    rtems_libio_iop_to_descriptor(iop), cmd);
	}
	if (td == NULL) {
		if (RTEMS_BSD_SYSCALL_TRACE) {
			printf("bsd: sys: fcntl: no curthread\n");
		}
		return rtems_bsd_error_to_status_and_errno(ENOMEM);
	}
	switch (cmd) {
	case F_SETFD:
		arg = rtems_libio_to_fcntl_flags(rtems_libio_iop_flags(iop)) &
		    FD_CLOEXEC;
		break;
	case F_SETFL:
		arg = rtems_libio_to_fcntl_flags(rtems_libio_iop_flags(iop)) &
		    FCNTLFLAGS;
		break;
	default:
		arg = -1;
		error = 0;
		break;
	}
	if (arg >= 0) {
		error = kern_fcntl(
		    td, rtems_bsd_libio_iop_to_descriptor(iop), cmd, arg);
		/* no return path with the RTEMS API for get calls */
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

int
rtems_bsd_sysgen_poll(rtems_libio_t *iop, int events)
{
	printf("rtems_bsd_sysgen_poll called!\n");
	return rtems_bsd_error_to_status_and_errno(EOPNOTSUPP);
}

int
rtems_bsd_sysgen_kqfilter(rtems_libio_t *iop, struct knote *kn)
{
	printf("rtems_bsd_sysgen_kqfilter called!\n");
	return rtems_bsd_error_to_status_and_errno(EOPNOTSUPP);
}
