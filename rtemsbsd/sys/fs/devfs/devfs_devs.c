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
#include <sys/filedesc.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <machine/rtems-bsd-libio.h>

#include <fs/devfs/devfs_int.h>

#include <machine/pcpu.h>

#include <rtems/imfs.h>

#define DEVFS_ROOTINO 2

const char rtems_cdev_directory[] = RTEMS_CDEV_DIRECTORY;

/*
 * The one true (but secret) list of active devices in the system.
 * Locked by dev_lock()/devmtx
 */
struct cdev_priv_list cdevp_list = TAILQ_HEAD_INITIALIZER(cdevp_list);

struct unrhdr *devfs_inos;

static MALLOC_DEFINE(M_CDEVP, "DEVFS1", "DEVFS cdev_priv storage");

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
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;
	int fd = -1;

	if (td != NULL) {
		if (cdev == NULL) {
			error = ENXIO;
			goto err;
		}
		error = falloc(td, &fp, &fd, oflag);
		if (error != 0)
			goto err;
		finit(fp, FREAD | FWRITE, DTYPE_DEV, NULL, NULL);
		rtems_libio_iop_hold(iop);
		rtems_bsd_libio_iop_set_bsd_descriptor(iop, fd);
		rtems_bsd_libio_iop_set_bsd_file(iop, fp);
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = ENXIO;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		fp->f_cdevpriv = NULL;
		error = dsw->d_open(cdev, oflag + 1, 0, td);
		/* Clean up any cdevpriv upon error. */
		if (error != 0)
			devfs_clear_cdevpriv();
		curthread->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL) {
		if (error != 0)
			fdclose(td, fp, fd);
		else
			fdrop(fp, td);
	}
	return rtems_bsd_error_to_status_and_errno(error);
}

static int
devfs_imfs_close(rtems_libio_t *iop)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int flags = rtems_libio_to_fcntl_flags(iop->flags);
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;

	if (td != NULL) {
		if (cdev == NULL) {
			error = ENXIO;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = ENXIO;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_close(cdev, flags, 0, td);
		curthread->td_fpop = fpop;
		if (fp->f_cdevpriv != NULL)
			devfs_fpdrop(fp);
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
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
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;

	if (td != NULL) {
		if (cdev == NULL) {
			error = ENXIO;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = ENXIO;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_read(cdev, &uio,
		    rtems_libio_to_fcntl_flags(iop->flags));
		td->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
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
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;

	if (td != NULL) {
		if (cdev == NULL) {
			error = ENXIO;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = ENXIO;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_write(cdev, &uio,
		    rtems_libio_to_fcntl_flags(iop->flags));
		td->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
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
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;
	int flags = rtems_libio_to_fcntl_flags(iop->flags);

	if (td != 0) {
		if (cdev == NULL) {
			error = ENXIO;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = ENXIO;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_ioctl(cdev, request, buffer, flags,
		    td);
		td->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
	return rtems_bsd_error_to_status_and_errno(error);
}

static int
devfs_imfs_fstat(const rtems_filesystem_location_info_t *loc, struct stat *buf)
{
	int rv = 0;
	const IMFS_jnode_t *the_dev = loc->node_access;

	if (the_dev != NULL) {
		buf->st_mode = the_dev->st_mode;
	} else {
		rv = rtems_filesystem_default_fstat(loc, buf);
	}

	return rv;
}

static int
devfs_imfs_poll(rtems_libio_t *iop, int events)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_wait_forever();
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;

	if (td != 0) {
		if (cdev == NULL) {
			error = POLLERR;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = POLLERR;
			goto err;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_poll(cdev, events, td);
		td->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
	return error;
}

static int
devfs_imfs_kqfilter(rtems_libio_t *iop, struct knote *kn)
{
	struct cdev *cdev = devfs_imfs_get_context_by_iop(iop);
	struct thread *td = rtems_bsd_get_curthread_or_wait_forever();
	struct file *fp = NULL;
	struct cdevsw *dsw = NULL;
	struct file *fpop;
	int error, ref;

	if (td != 0) {
		if (cdev == NULL) {
			error = EINVAL;
		}
		fp = rtems_bsd_libio_iop_to_file_hold(iop, td);
		if (fp == NULL) {
			error = EBADF;
			goto err;
		}
		if (cdev->si_flags & SI_ALIAS) {
			cdev = cdev->si_parent;
		}
		dsw = dev_refthread(cdev, &ref);
		if (dsw == NULL) {
			error = EINVAL;
		}
		fpop = td->td_fpop;
		curthread->td_fpop = fp;
		error = dsw->d_kqfilter(cdev, kn);
		td->td_fpop = fpop;
	} else {
		error = ENOMEM;
	}

err:
	if (dsw != NULL)
		dev_relthread(cdev, ref);
	if (td != NULL && fp != NULL)
		fdrop(fp, td);
	return error;
}

static const rtems_filesystem_file_handlers_r devfs_imfs_handlers = {
	.open_h = devfs_imfs_open,
	.close_h = devfs_imfs_close,
	.read_h = devfs_imfs_read,
	.write_h = devfs_imfs_write,
	.ioctl_h = devfs_imfs_ioctl,
	.lseek_h = rtems_filesystem_default_lseek_file,
	.fstat_h = devfs_imfs_fstat,
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
	struct cdev_priv *cdp;
	struct cdev *cdev;

	cdp = malloc(sizeof *cdp, M_CDEVP, M_ZERO |
	    ((flags & MAKEDEV_NOWAIT) ? M_NOWAIT : M_WAITOK));
	if (cdp == NULL)
		return (NULL);

	cdev = &cdp->cdp_c;
	LIST_INIT(&cdev->si_children);

	memcpy(cdev->si_path, rtems_cdev_directory, sizeof(cdev->si_path));
	return (cdev);
}

void
devfs_free(struct cdev *cdev)
{
	struct cdev_priv *cdp;

	cdp = cdev2priv(cdev);
	devfs_free_cdp_inode(cdp->cdp_inode);
	free(cdp, M_CDEVP);
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
	struct cdev_priv *cdp;
	int rv;
	mode_t mode = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;

	devfs_create_directory(dev->si_path);

	rv = IMFS_make_generic_node(dev->si_path, mode, &devfs_imfs_control,
	    dev);
	BSD_ASSERT(rv == 0);

	cdp = cdev2priv(dev);
	cdp->cdp_flags |= CDP_ACTIVE;
	cdp->cdp_inode = alloc_unrl(devfs_inos);
	dev_refl(dev);
	TAILQ_INSERT_TAIL(&cdevp_list, cdp, cdp_list);
}

void
devfs_destroy(struct cdev *dev)
{
	struct cdev_priv *cdp;
	int rv;

	cdp = cdev2priv(dev);
	cdp->cdp_flags &= ~CDP_ACTIVE;

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
