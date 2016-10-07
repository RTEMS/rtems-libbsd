/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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

#include <sys/types.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fs/devfs/devfs_int.h>

#include <machine/pcpu.h>

#include <rtems/imfs.h>

#define DEVFS_ROOTINO 2

const char rtems_cdev_directory[] = RTEMS_CDEV_DIRECTORY;

struct unrhdr *devfs_inos;

static struct cdev *
devfs_imfs_get_context_by_iop(rtems_libio_t *iop)
{
	return IMFS_generic_get_context_by_iop(iop);
}

static int
devfs_imfs_open(rtems_libio_t *iop, const char *path, int oflag, mode_t mode)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int error;

	if (td != NULL) {
		error = cdev->si_devsw->d_open(cdev, oflag, 0, td);
	} else {
		error = ENOMEM;
	}

	return rtems_bsd_error_to_status_and_errno(error);
}

static int
devfs_imfs_close(rtems_libio_t *iop)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int flags = rtems_libio_to_fcntl_flags(iop->flags);
	int error;

	if (td != NULL) {
		error = cdev->si_devsw->d_close(cdev, flags, 0, td);
	} else {
		error = ENOMEM;
	}

	return rtems_bsd_error_to_status_and_errno(error);
}

static ssize_t
devfs_imfs_readv(rtems_libio_t *iop, const struct iovec *iov, int iovcnt,
    ssize_t total)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct uio uio = {
		.uio_iov = __DECONST(struct iovec *, iov),
		.uio_iovcnt = iovcnt,
		.uio_offset = 0,
		.uio_resid = total,
		.uio_segflg = UIO_USERSPACE,
		.uio_rw = UIO_READ,
		.uio_td = td
	};
	int error;

	if (td != NULL) {
		error = cdev->si_devsw->d_read(cdev, &uio,
		    rtems_libio_to_fcntl_flags(iop->flags));
	} else {
		error = ENOMEM;
	}

	if (error == 0) {
		return (total - uio.uio_resid);
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}

static ssize_t
devfs_imfs_read(rtems_libio_t *iop, void *buffer, size_t count)
{
	struct iovec iov = {
		.iov_base = buffer,
		.iov_len = count
	};

	return (devfs_imfs_readv(iop, &iov, 1, count));
}

static ssize_t
devfs_imfs_writev(rtems_libio_t *iop, const struct iovec *iov, int iovcnt,
    ssize_t total)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	struct uio uio = {
		.uio_iov = __DECONST(struct iovec *, iov),
		.uio_iovcnt = iovcnt,
		.uio_offset = 0,
		.uio_resid = total,
		.uio_segflg = UIO_USERSPACE,
		.uio_rw = UIO_WRITE,
		.uio_td = td
	};
	int error;

	if (td != NULL) {
		error = cdev->si_devsw->d_write(cdev, &uio,
		    rtems_libio_to_fcntl_flags(iop->flags));
	} else {
		error = ENOMEM;
	}

	if (error == 0) {
		return (total - uio.uio_resid);
	} else {
		rtems_set_errno_and_return_minus_one(error);
	}
}

static ssize_t
devfs_imfs_write(rtems_libio_t *iop, const void *buffer, size_t count)
{
	struct iovec iov = {
		.iov_base = __DECONST(void *, buffer),
		.iov_len = count
	};

	return (devfs_imfs_writev(iop, &iov, 1, count));
}

static int
devfs_imfs_ioctl(rtems_libio_t *iop, ioctl_command_t request, void *buffer)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int error;
	int flags = rtems_libio_to_fcntl_flags(iop->flags);

	if (td != 0) {
		error = cdev->si_devsw->d_ioctl(cdev, request, buffer, flags,
		    td);
	} else {
		error = ENOMEM;
	}

	return (rtems_bsd_error_to_status_and_errno(error));
}

static int
devfs_imfs_poll(rtems_libio_t *iop, int events)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);

	return (cdev->si_devsw->d_poll(cdev, events,
	    rtems_bsd_get_curthread_or_wait_forever()));
}

static int
devfs_imfs_kqfilter(rtems_libio_t *iop, struct knote *kn)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);

	return (cdev->si_devsw->d_kqfilter(cdev, kn));
}

static const rtems_filesystem_file_handlers_r devfs_imfs_handlers = {
	.open_h = devfs_imfs_open,
	.close_h = devfs_imfs_close,
	.read_h = devfs_imfs_read,
	.write_h = devfs_imfs_write,
	.ioctl_h = devfs_imfs_ioctl,
	.lseek_h = rtems_filesystem_default_lseek_file,
	.fstat_h = rtems_filesystem_default_fstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_filesystem_default_fcntl,
	.poll_h = devfs_imfs_poll,
	.kqfilter_h = devfs_imfs_kqfilter,
	.readv_h = devfs_imfs_readv,
	.writev_h = devfs_imfs_writev,
};

static const IMFS_node_control devfs_imfs_control = IMFS_GENERIC_INITIALIZER(
    &devfs_imfs_handlers, IMFS_node_initialize_generic,
    IMFS_node_destroy_default);

struct cdev *
devfs_alloc(int flags)
{
	struct cdev *cdev;

	cdev = malloc(sizeof *cdev, M_TEMP, M_ZERO);
	if (cdev != NULL)
		return (NULL);

	memcpy(cdev->si_path, rtems_cdev_directory, sizeof(cdev->si_path));
	return (cdev);
}

void
devfs_free(struct cdev *cdev)
{
	free(cdev, M_TEMP);
}

/*
 * Create the directory for a device.
 * Note: This don't uses dirname() because this function is not defined thread
 * save in POSIX.
 */
static void
devfs_create_directory(const char *devname)
{
	char *dir = NULL;
	char *lastsep = NULL;
	int rv;
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

	dir = strdup(devname, M_TEMP);
	BSD_ASSERT(dir != NULL);

	lastsep = strrchr(dir, '/');
	if(lastsep != NULL) {
		*lastsep = 0;
		rv = rtems_mkdir(dir, mode);
		BSD_ASSERT(rv == 0);
	}

	free(dir, M_TEMP);
}

/*
 * devfs_create() and devfs_destroy() are called from kern_conf.c and
 * in both cases the devlock() mutex is held, so no further locking
 * is necesary and no sleeping allowed.
 */

void
devfs_create(struct cdev *dev)
{
	int rv;
	mode_t mode = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;

	devfs_create_directory(dev->si_path);

	rv = IMFS_make_generic_node(dev->si_path, mode, &devfs_imfs_control,
	    dev);
	BSD_ASSERT(rv == 0);
}

void
devfs_destroy(struct cdev *dev)
{
	int rv;

	rv = unlink(dev->si_path);
	BSD_ASSERT(rv == 0);

	/* FIXME: Check if directory is empty and remove it. */
}

int
devfs_dev_exists(const char *name)
{
	const size_t dirlen = sizeof(rtems_cdev_directory) - 1;
	const size_t maxnamelen = SPECNAMELEN;

	int rv;
	char fullpath[dirlen + maxnamelen + 1];

	memcpy(fullpath, rtems_cdev_directory, dirlen);
	strncpy(fullpath + dirlen, name, maxnamelen);

	rv = access(fullpath, F_OK);

	if(rv == 0)
		return 1;
	else
		return 0;
}

ino_t
devfs_alloc_cdp_inode(void)
{

	return (alloc_unr(devfs_inos));
}

void
devfs_free_cdp_inode(ino_t ino)
{

	if (ino > 0)
		free_unr(devfs_inos, ino);
}

static void
	devfs_devs_init(void *junk __unused)
{

	devfs_inos = new_unrhdr(DEVFS_ROOTINO + 1, INT_MAX, &devmtx);
}

SYSINIT(devfs_devs, SI_SUB_DEVFS, SI_ORDER_FIRST, devfs_devs_init, NULL);
