#include <machine/rtems-bsd-kernel-space.h>

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (C) 2012-2016 Intel Corporation
 * All rights reserved.
 * Copyright (C) 2018 Alexander Motin <mav@FreeBSD.org>
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

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/queue.h>
#include <sys/systm.h>

#include <dev/nvme/nvme_private.h>

#include <stdatomic.h>
#include <rtems/blkdev.h>
#include <rtems/thread.h>

#define NVD_STR		"nvd"

struct nvd_controller;

static int nvd_load(void);
static void *nvd_new_disk(struct nvme_namespace *ns, void *ctrlr);
static void *nvd_new_controller(struct nvme_controller *ctrlr);
static void nvd_controller_fail(void *ctrlr);

MALLOC_DEFINE(M_NVD, "nvd", "nvd(4) allocations");

struct nvd_disk {
	struct nvd_controller	*ctrlr;
	struct nvme_namespace	*ns;
	uint32_t		lb_per_media_block;
	u_int			unit;
	TAILQ_ENTRY(nvd_disk)	global_tailq;
	TAILQ_ENTRY(nvd_disk)	ctrlr_tailq;
};

struct nvd_controller {
	TAILQ_ENTRY(nvd_controller)	tailq;
	TAILQ_HEAD(, nvd_disk)		disk_head;
};

static struct mtx			nvd_lock;
static TAILQ_HEAD(, nvd_controller)	ctrlr_head;
static TAILQ_HEAD(disk_list, nvd_disk)	disk_head;

static int
nvd_modevent(module_t mod, int type, void *arg)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		error = nvd_load();
		break;
	default:
		break;
	}

	return (error);
}

moduledata_t nvd_mod = {
	NVD_STR,
	nvd_modevent,
	0
};

DECLARE_MODULE(nvd, nvd_mod, SI_SUB_DRIVERS, SI_ORDER_ANY);
MODULE_VERSION(nvd, 1);
MODULE_DEPEND(nvd, nvme, 1, 1, 1);

static int
nvd_load(void)
{

	mtx_init(&nvd_lock, "nvd_lock", NULL, MTX_DEF);
	TAILQ_INIT(&ctrlr_head);
	TAILQ_INIT(&disk_head);

	nvme_register_consumer(nvd_new_disk,
	    nvd_new_controller, NULL, nvd_controller_fail);

	return (0);
}

static void *
nvd_new_controller(struct nvme_controller *ctrlr)
{
	struct nvd_controller *nvd_ctrlr;

	nvd_ctrlr = malloc(sizeof(*nvd_ctrlr), M_NVD,
	    M_ZERO | M_WAITOK);

	TAILQ_INIT(&nvd_ctrlr->disk_head);
	mtx_lock(&nvd_lock);
	TAILQ_INSERT_TAIL(&ctrlr_head, nvd_ctrlr, tailq);
	mtx_unlock(&nvd_lock);

	return (nvd_ctrlr);
}

#define NVD_BUFNUM_SHIFT 16

#define NVD_BUFNUM_DEC (UINT32_C(1) << NVD_BUFNUM_SHIFT)

static void
nvd_request_done(rtems_blkdev_request *req)
{
	uint32_t prev;

	prev = atomic_fetch_sub_explicit(&req->bufnum, NVD_BUFNUM_DEC,
	    memory_order_relaxed);

	if ((prev >> NVD_BUFNUM_SHIFT) == 1) {
		rtems_blkdev_request_done(req, req->status);
	}
}

static void
nvd_completion(void *arg, const struct nvme_completion *status)
{
	rtems_blkdev_request *req;

	req = arg;

	if (nvme_completion_is_error(status)) {
		if (req->status == RTEMS_SUCCESSFUL) {
			req->status = RTEMS_IO_ERROR;
		}
	}

	nvd_request_done(req);
}

static int
nvd_request(struct nvd_disk *ndisk, rtems_blkdev_request *req,
    uint32_t media_block_size)
{
	uint32_t i;
	uint32_t bufnum;

	BSD_ASSERT(req->req == RTEMS_BLKDEV_REQ_READ ||
	    req->req == RTEMS_BLKDEV_REQ_WRITE);
	BSD_ASSERT(rtems_event_transient_receive(RTEMS_NO_WAIT, 0) == RTEMS_UNSATISFIED);
	BSD_ASSERT(req->bufnum < NVD_BUFNUM_DEC);

	req->status = RTEMS_SUCCESSFUL;
	bufnum = req->bufnum;
	req->bufnum |= bufnum << NVD_BUFNUM_SHIFT;

	for (i = 0; i < bufnum; ++i) {
		rtems_blkdev_sg_buffer *sg;
		uint32_t lb_count;
		int error;

		sg = &req->bufs[i];
		lb_count = (sg->length / media_block_size) *
		    ndisk->lb_per_media_block;

		if (req->req == RTEMS_BLKDEV_REQ_READ) {
			error = nvme_ns_cmd_read(ndisk->ns, sg->buffer,
			    sg->block * ndisk->lb_per_media_block, lb_count,
			    nvd_completion, req);
		} else {
			error = nvme_ns_cmd_write(ndisk->ns, sg->buffer,
			    sg->block * ndisk->lb_per_media_block, lb_count,
			    nvd_completion, req);
		}

		if (error != 0) {
			req->status = RTEMS_NO_MEMORY;
			nvd_request_done(req);
		}
	}

	return (0);
}

static void
nvd_sync_completion(void *arg, const struct nvme_completion *status)
{
	rtems_binary_semaphore *sync;

	sync = arg;
	rtems_binary_semaphore_post(sync);
}

static int
nvd_sync(struct nvd_disk *ndisk)
{
	rtems_binary_semaphore sync;
	int error;

	rtems_binary_semaphore_init(&sync, "nvd sync");

	error = nvme_ns_cmd_flush(ndisk->ns, nvd_sync_completion, &sync);
	if (error == 0) {
		rtems_binary_semaphore_wait(&sync);
	}

	rtems_binary_semaphore_destroy(&sync);
	return (error);
}

static int
nvd_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
	struct nvd_disk *ndisk;

	ndisk = rtems_disk_get_driver_data(dd);

	if (req == RTEMS_BLKIO_REQUEST) {
		return (nvd_request(ndisk, arg, dd->media_block_size));
	}

	if (req == RTEMS_BLKDEV_REQ_SYNC) {
		return (nvd_sync(ndisk));
	}

	if (req == RTEMS_BLKIO_CAPABILITIES) {
		*(uint32_t *)arg = RTEMS_BLKDEV_CAP_SYNC;
		return (0);
	}

	if (req == RTEMS_BLKIO_DELETED) {
		panic("nvd_ioctl");
		return (0);
	}

	return (rtems_blkdev_ioctl(dd, req, arg));
}

static void *
nvd_new_disk(struct nvme_namespace *ns, void *arg)
{
	char path[64];
	struct nvd_disk *ndisk;
	struct nvd_disk *tnd;
	struct nvd_controller *ctrlr;
	int unit;
	rtems_status_code sc;
	uint32_t block_size;
	uint32_t min_page_size;
	rtems_blkdev_bnum block_count;

	ctrlr = arg;
	ndisk = malloc(sizeof(*ndisk), M_NVD, M_ZERO | M_WAITOK);
	ndisk->ctrlr = ctrlr;
	ndisk->ns = ns;

	mtx_lock(&nvd_lock);
	unit = 0;
	TAILQ_FOREACH(tnd, &disk_head, global_tailq) {
		if (tnd->unit > unit) {
			break;
		}
		unit = tnd->unit + 1;
	}
	ndisk->unit = unit;
	if (tnd != NULL) {
		TAILQ_INSERT_BEFORE(tnd, ndisk, global_tailq);
	} else {
		TAILQ_INSERT_TAIL(&disk_head, ndisk, global_tailq);
	}
	TAILQ_INSERT_TAIL(&ctrlr->disk_head, ndisk, ctrlr_tailq);
	mtx_unlock(&nvd_lock);

	min_page_size = ndisk->ns->ctrlr->page_size;
	block_size = nvme_ns_get_sector_size(ns);

	if (block_size < min_page_size) {
		ndisk->lb_per_media_block = min_page_size / block_size;
		block_size = min_page_size;
	} else {
		ndisk->lb_per_media_block = 1;
	}

	block_count = nvme_ns_get_size(ns) / block_size;
	snprintf(path, sizeof(path), "/dev/nvd%i", unit);
	sc = rtems_blkdev_create(path, block_size, block_count, nvd_ioctl,
	    ndisk);
	if (sc != RTEMS_SUCCESSFUL) {
		panic("nvd_new_disk");
	}

	return (ndisk);
}

static void
nvd_gone(struct nvd_disk *ndisk)
{

	panic("nvd_gone");
}

static void
nvd_controller_fail(void *ctrlr_arg)
{
	struct nvd_controller	*ctrlr = ctrlr_arg;
	struct nvd_disk		*ndisk;

	mtx_lock(&nvd_lock);
	TAILQ_REMOVE(&ctrlr_head, ctrlr, tailq);
	TAILQ_FOREACH(ndisk, &ctrlr->disk_head, ctrlr_tailq)
		nvd_gone(ndisk);
	while (!TAILQ_EMPTY(&ctrlr->disk_head))
		msleep(&ctrlr->disk_head, &nvd_lock, 0, "nvd_fail", 0);
	mtx_unlock(&nvd_lock);
	free(ctrlr, M_NVD);
}

