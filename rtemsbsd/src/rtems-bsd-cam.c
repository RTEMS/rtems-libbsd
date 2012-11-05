/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  
 * All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/condvar.h>

#include <freebsd/cam/cam.h>
#include <freebsd/cam/cam_ccb.h>
#include <freebsd/cam/cam_sim.h>
#include <freebsd/cam/cam_xpt.h>
#include <freebsd/cam/cam_xpt_sim.h>
#include <freebsd/cam/cam_debug.h>

#include <freebsd/cam/scsi/scsi_all.h>

#include <rtems/media.h>
#include <rtems/libio.h>
#include <rtems/diskdevs.h>

#define BSD_CAM_DEVQ_DUMMY ((struct cam_devq *) 0xdeadbeef)

#define BSD_SCSI_TAG 0

#define BSD_SCSI_RETRIES 4

#define BSD_SCSI_TIMEOUT (60 * 1000)

#define BSD_SCSI_MIN_COMMAND_SIZE 10

MALLOC_DEFINE(M_CAMSIM, "CAM SIM", "CAM SIM buffers");

static void
rtems_bsd_sim_set_state(struct cam_sim *sim, enum bsd_sim_state state)
{
	sim->state = state;
}

static void
rtems_bsd_sim_set_state_and_notify(struct cam_sim *sim, enum bsd_sim_state state)
{
	sim->state = state;
	cv_broadcast(&sim->state_changed);
}

static void
rtems_bsd_sim_wait_for_state(struct cam_sim *sim, enum bsd_sim_state state)
{
	while (sim->state != state) {
		cv_wait(&sim->state_changed, sim->mtx);
	}
}

static void
rtems_bsd_sim_wait_for_state_and_cancel_ccb(struct cam_sim *sim, enum bsd_sim_state state)
{
	while (sim->state != state) {
		if (sim->state != BSD_SIM_BUSY) {
			cv_wait(&sim->state_changed, sim->mtx);
		} else {
			sim->ccb.ccb_h.status = CAM_SEL_TIMEOUT;
			(*sim->ccb.ccb_h.cbfcnp)(NULL, &sim->ccb);
		}
	}
}

static void
rtems_bsd_ccb_callback(struct cam_periph *periph, union ccb *ccb)
{
	struct cam_sim *sim = ccb->ccb_h.sim;

	BSD_ASSERT(periph == NULL && sim->state == BSD_SIM_INIT_BUSY);

	rtems_bsd_sim_set_state_and_notify(sim, BSD_SIM_INIT_READY);
}

static rtems_status_code
rtems_bsd_ccb_action(union ccb *ccb)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct cam_sim *sim = ccb->ccb_h.sim;

	mtx_lock(sim->mtx);

	BSD_ASSERT(sim->state == BSD_SIM_INIT);
	rtems_bsd_sim_set_state(sim, BSD_SIM_INIT_BUSY);
	(*sim->sim_action)(sim, ccb);
	rtems_bsd_sim_wait_for_state(sim, BSD_SIM_INIT_READY);
	if (ccb->ccb_h.status != CAM_REQ_CMP) {
		sc = RTEMS_IO_ERROR;
	}
	rtems_bsd_sim_set_state(sim, BSD_SIM_INIT);

	mtx_unlock(sim->mtx);

	return sc;
}

static rtems_status_code
rtems_bsd_scsi_inquiry(union ccb *ccb, struct scsi_inquiry_data *inq_data)
{
	memset(inq_data, 0, sizeof(*inq_data));

	scsi_inquiry(
		&ccb->csio,
		BSD_SCSI_RETRIES,
		rtems_bsd_ccb_callback,
		BSD_SCSI_TAG,
		(u_int8_t *) inq_data,
		sizeof(*inq_data) - 1,
		FALSE,
		0,
		SSD_MIN_SIZE,
		BSD_SCSI_TIMEOUT
	);

	return rtems_bsd_ccb_action(ccb);
}

static rtems_status_code
rtems_bsd_scsi_test_unit_ready(union ccb *ccb)
{
	scsi_test_unit_ready(
		&ccb->csio,
		BSD_SCSI_RETRIES,
		rtems_bsd_ccb_callback,
		BSD_SCSI_TAG,
		SSD_FULL_SIZE,
		BSD_SCSI_TIMEOUT
	);

	return rtems_bsd_ccb_action(ccb);
}

static rtems_status_code
rtems_bsd_scsi_read_capacity(union ccb *ccb, uint32_t *block_count, uint32_t *block_size)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct scsi_read_capacity_data rdcap;

	memset(&rdcap, 0, sizeof(rdcap));

	scsi_read_capacity(
		&ccb->csio,
		BSD_SCSI_RETRIES,
		rtems_bsd_ccb_callback,
		BSD_SCSI_TAG,
		&rdcap,
		SSD_FULL_SIZE,
		BSD_SCSI_TIMEOUT
	);

	sc = rtems_bsd_ccb_action(ccb);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_IO_ERROR;
	}

	*block_size = scsi_4btoul(rdcap.length);
	*block_count = scsi_4btoul(rdcap.addr) + 1;

	return RTEMS_SUCCESSFUL;
}

static void
rtems_bsd_csio_callback(struct cam_periph *periph, union ccb *ccb)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	bool done = false;
	struct cam_sim *sim = ccb->ccb_h.sim;

	BSD_ASSERT(periph == NULL && sim->state == BSD_SIM_BUSY);

	if (ccb->ccb_h.status == CAM_REQ_CMP) {
		rtems_blkdev_sg_buffer *sg = ccb->csio.sg_current;

		if (sg != ccb->csio.sg_end) {
			scsi_read_write(
				&ccb->csio,
				BSD_SCSI_RETRIES,
				rtems_bsd_csio_callback,
				BSD_SCSI_TAG,
				ccb->csio.readop,
				0,
				BSD_SCSI_MIN_COMMAND_SIZE,
				sg->block,
				sg->length / 512, /* FIXME */
				sg->buffer,
				sg->length,
				SSD_FULL_SIZE,
				BSD_SCSI_TIMEOUT
			);
			ccb->csio.sg_current = sg + 1;
			(*sim->sim_action)(sim, ccb);
		} else {
			done = true;
		}
	} else if (ccb->ccb_h.status == CAM_SEL_TIMEOUT) {
		sc = RTEMS_UNSATISFIED;
		done = true;
	} else {
		sc = RTEMS_IO_ERROR;
		done = true;
	}

	if (done) {
		rtems_blkdev_request_done(ccb->csio.req, sc);
		rtems_bsd_sim_set_state_and_notify(sim, BSD_SIM_IDLE);
	}
}

static int rtems_bsd_sim_disk_read_write(struct cam_sim *sim, rtems_blkdev_request *req)
{
	mtx_lock(sim->mtx);

	rtems_bsd_sim_wait_for_state(sim, BSD_SIM_IDLE);
	rtems_bsd_sim_set_state(sim, BSD_SIM_BUSY);

	switch (req->req) {
		case RTEMS_BLKDEV_REQ_READ:
			sim->ccb.csio.readop = TRUE;
			break;
		case RTEMS_BLKDEV_REQ_WRITE:
			sim->ccb.csio.readop = FALSE;
			break;
		default:
			mtx_unlock(sim->mtx);
			return -1;
	}

	sim->ccb.csio.sg_current = req->bufs;
	sim->ccb.csio.sg_end = req->bufs + req->bufnum;
	sim->ccb.csio.req = req;

	sim->ccb.ccb_h.status = CAM_REQ_CMP;

	rtems_bsd_csio_callback(NULL, &sim->ccb);

	mtx_unlock(sim->mtx);

	return 0;
}

static int rtems_bsd_sim_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
	struct cam_sim *sim = rtems_disk_get_driver_data(dd);

	if (req == RTEMS_BLKIO_REQUEST) {
		rtems_blkdev_request *r = arg;

		return rtems_bsd_sim_disk_read_write(sim, r);
	} else if (req == RTEMS_BLKIO_DELETED) {
		mtx_lock(sim->mtx);

		free(sim->disk, M_RTEMS_HEAP);
		sim->disk = NULL;
		rtems_bsd_sim_set_state_and_notify(sim, BSD_SIM_DELETED);

		mtx_unlock(sim->mtx);

		return 0;
	} else {
		return rtems_blkdev_ioctl(dd, req, arg);
	}
}

static void
rtems_bsd_sim_disk_initialized(struct cam_sim *sim, char *disk)
{
	mtx_lock(sim->mtx);

	sim->disk = disk;
	rtems_bsd_sim_set_state_and_notify(sim, BSD_SIM_IDLE);

	mtx_unlock(sim->mtx);
}

static rtems_status_code
rtems_bsd_sim_attach_worker(rtems_media_state state, const char *src, char **dest, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct cam_sim *sim = arg;
	char *disk = NULL;

	if (state == RTEMS_MEDIA_STATE_READY) {
		unsigned retries = 0;

		struct scsi_inquiry_data inq_data;
		uint32_t block_count = 0;
		uint32_t block_size = 0;

		disk = rtems_media_create_path("/dev", src, cam_sim_unit(sim));
		if (disk == NULL) {
			BSD_PRINTF("OOPS: create path failed\n");
			goto error;
		}

		sc = rtems_bsd_scsi_inquiry(&sim->ccb, &inq_data);
		if (sc != RTEMS_SUCCESSFUL) {
			BSD_PRINTF("OOPS: inquiry failed\n");
			goto error;
		}
		scsi_print_inquiry(&inq_data);

		for (retries = 0; retries <= 3; ++retries) {
			sc = rtems_bsd_scsi_test_unit_ready(&sim->ccb);
			if (sc == RTEMS_SUCCESSFUL) {
				break;
			}
		}
		if (sc != RTEMS_SUCCESSFUL) {
			BSD_PRINTF("OOPS: test unit ready failed\n");
			goto error;
		}

		sc = rtems_bsd_scsi_read_capacity(&sim->ccb, &block_count, &block_size);
		if (sc != RTEMS_SUCCESSFUL) {
			BSD_PRINTF("OOPS: read capacity failed\n");
			goto error;
		}

		BSD_PRINTF("read capacity: block count %u, block size %u\n", block_count, block_size);

		sc = rtems_blkdev_create(disk, block_size, block_count, rtems_bsd_sim_disk_ioctl, sim);
		if (sc != RTEMS_SUCCESSFUL) {
			goto error;
		}

		/* FIXME */
#if 0
		rtems_disk_device *dd = rtems_disk_obtain(dev);
		dd->block_size *= 64;
		rtems_disk_release(dd);
#endif

		rtems_bsd_sim_disk_initialized(sim, disk);

		*dest = strdup(disk, M_RTEMS_HEAP);
	}

	return RTEMS_SUCCESSFUL;

error:

	free(disk, M_RTEMS_HEAP);

	rtems_bsd_sim_disk_initialized(sim, NULL);

	return RTEMS_IO_ERROR;
}

struct cam_sim *
cam_sim_alloc(
	sim_action_func sim_action,
	sim_poll_func sim_poll,
	const char *sim_name,
	void *softc,
	u_int32_t unit,
	struct mtx *mtx,
	int max_dev_transactions,
	int max_tagged_dev_transactions,
	struct cam_devq *queue
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct cam_sim *sim = NULL;

	if (mtx == NULL) {
		return NULL;
	}

	sim = malloc(sizeof(*sim), M_CAMSIM, M_NOWAIT | M_ZERO);
	if (sim == NULL) {
		return NULL;
	}

	sim->sim_action = sim_action;
	sim->sim_poll = sim_poll;
	sim->sim_name = sim_name;
	sim->softc = softc;
	sim->mtx = mtx;
	sim->unit_number = unit;
	sim->ccb.ccb_h.sim = sim;

	cv_init(&sim->state_changed, "SIM state changed");

	sc = rtems_media_server_disk_attach(sim_name, rtems_bsd_sim_attach_worker, sim);
	BSD_ASSERT_SC(sc);

	return sim;
}

void
cam_sim_free(struct cam_sim *sim, int free_devq)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	/*
	 * The umass_detach() cancels all transfers via
	 * usbd_transfer_unsetup().  This prevents also the start of new
	 * transfers since the transfer descriptors will be removed.  Started
	 * transfers that are not in the transferring state will be canceled
	 * and the callbacks will be not called.  Thus it is necessary to do
	 * this here if we are in the BUSY state.
	 */
	rtems_bsd_sim_wait_for_state_and_cancel_ccb(sim, BSD_SIM_IDLE);

	if (sim->disk != NULL) {
		sc = rtems_media_server_disk_detach(sim->disk);
		BSD_ASSERT_SC(sc);

		rtems_bsd_sim_wait_for_state(sim, BSD_SIM_DELETED);
	}

	cv_destroy(&sim->state_changed);
	free(sim, M_CAMSIM);
}

struct cam_devq *
cam_simq_alloc(u_int32_t max_sim_transactions)
{
	return BSD_CAM_DEVQ_DUMMY;
}

void
cam_simq_free(struct cam_devq *devq)
{
	BSD_ASSERT(devq == BSD_CAM_DEVQ_DUMMY);
}

void
xpt_done(union ccb *done_ccb)
{
	(*done_ccb->ccb_h.cbfcnp)(NULL, done_ccb);
}

int32_t
xpt_bus_register(struct cam_sim *sim, device_t parent, u_int32_t bus)
{
	/*
	 * We ignore this bus stuff completely.  This is easier than removing
	 * the calls from "umass.c".
	 */

	return CAM_SUCCESS;
}

int32_t
xpt_bus_deregister(path_id_t pathid)
{
	/*
	 * We ignore this bus stuff completely.  This is easier than removing
	 * the calls from "umass.c".
	 */

	return CAM_REQ_CMP;
}
