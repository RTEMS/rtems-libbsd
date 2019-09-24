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
#include <machine/vm.h>
#include <sys/param.h>
#include <sys/conf.h>
#include <sys/mman.h>

#include <rtems/seterr.h>

#include <assert.h>

#include "test_cdev01.h"

static	d_open_t	testopen;
static	d_close_t	testclose;
static	d_read_t	testread;
static	d_write_t	testwrite;
static	d_ioctl_t	testioctl;
static	d_poll_t	testpoll;
static	d_kqfilter_t	testkqfilter;
static	d_mmap_t	testmmap;

static struct cdevsw test_cdevsw = {
	.d_version =	D_VERSION,
	.d_flags =	0,
/* FIXME: check for  D_PSEUDO | D_NEEDMINOR | D_NEEDGIANT | D_TRACKCLOSE */
	.d_name =	"test",
	.d_open =	testopen,
	.d_close =	testclose,
	.d_read =	testread,
	.d_write =	testwrite,
	.d_ioctl =	testioctl,
	.d_poll =	testpoll,
	.d_kqfilter =	testkqfilter,
	.d_mmap =	testmmap,
};

static	int
testopen(struct cdev *dev, int oflags, int devtype, struct thread *td)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_NEW);
	*state = TEST_OPEN;

	return 0;
}

static	int
testclose(struct cdev *dev, int fflag, int devtype, struct thread *td)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_MMAP);
	*state = TEST_CLOSED;

	return 0;
}

static	int
testread(struct cdev *dev, struct uio *uio, int ioflag)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_OPEN || *state == TEST_IOCTL);
	if(*state == TEST_OPEN) {
		*state = TEST_READ;
	} else {
		*state = TEST_READV;
	}

	return 0;
}

static	int
testwrite(struct cdev *dev, struct uio *uio, int ioflag)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_READ || *state == TEST_READV);
	if(*state == TEST_READ) {
		*state = TEST_WRITE;
	} else {
		*state = TEST_WRITEV;
	}

	return 0;
}

static	int
testioctl(struct cdev *dev, u_long cmd, caddr_t data, int fflag,
    struct thread *td)
{
	test_state *state = dev->si_drv1;

	assert(cmd == TEST_IOCTL_CMD);
	assert(*state == TEST_WRITE);
	*state = TEST_IOCTL;

	return 0;
}

static	int
testpoll(struct cdev *dev, int events, struct thread *td)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_WRITEV);
	*state = TEST_POLL;

	return 1;
}

static	int
testkqfilter(struct cdev *dev, struct knote *kn)
{
	test_state *state = dev->si_drv1;

	assert(*state == TEST_POLL);
	*state = TEST_KQFILTER;

	return TEST_KQ_ERRNO;
}

static int
testmmap(struct cdev *dev, vm_ooffset_t offset, vm_paddr_t *paddr,
         int nprot, vm_memattr_t *memattr)
{
	test_state *state = dev->si_drv1;

	assert(paddr != NULL);
	assert(memattr == VM_MEMATTR_DEFAULT);
	assert(nprot == (PROT_READ | PROT_WRITE));
	assert(*state == TEST_KQFILTER);
	*state = TEST_MMAP;

	return 0;
}

void
test_make_dev(test_state *state, const char *name)
{
	struct cdev *dev = NULL;

	dev = make_dev(&test_cdevsw, 0, UID_ROOT, GID_WHEEL, 0600, name);
	assert(dev != NULL);
	dev->si_drv1 = state;
}
