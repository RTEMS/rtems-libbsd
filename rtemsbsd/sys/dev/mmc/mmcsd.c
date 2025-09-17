#include <machine/rtems-bsd-kernel-space.h>
/*
 * Copyright (c) 2025 Aaron Nyholm <aaron.nyholm@unfoldedeffective.com>.
 * All rights reserved.
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

#include <sys/malloc.h>

#include <dev/mmc/bridge.h>
#include <dev/mmc/mmcreg.h>
#include <dev/mmc/mmcvar.h>

#include <rtems/bsd/local/mmcbus_if.h>

#include <machine/rtems-bsd-support.h>
#include <rtems/bdbuf.h>
#include <rtems/diskdevs.h>
#include <rtems/libio.h>
#include <rtems/media.h>
#include <rtems/mmcsd.h>

static void rtems_mmcsd_static_attach(struct mmcsd_part*);

rtems_mmcsd_attach_hookfunction rtems_mmcsd_attach_fp =
	rtems_mmcsd_static_attach;

static rtems_status_code
rtems_bsd_mmcsd_set_block_size(device_t dev, uint32_t block_size)
{
	rtems_status_code status_code = RTEMS_SUCCESSFUL;
	struct mmc_command cmd;
	struct mmc_request req;

	memset(&req, 0, sizeof(req));
	memset(&cmd, 0, sizeof(cmd));

	req.cmd = &cmd;
	cmd.opcode = MMC_SET_BLOCKLEN;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	cmd.arg = block_size;
	MMCBUS_WAIT_FOR_REQUEST(device_get_parent(dev), dev,
	    &req);
	if (req.cmd->error != MMC_ERR_NONE) {
		status_code = RTEMS_IO_ERROR;
	}

	return status_code;
}

static int
rtems_bsd_mmcsd_disk_read_write(struct mmcsd_part *part, rtems_blkdev_request *blkreq)
{
	rtems_status_code status_code = RTEMS_SUCCESSFUL;
	struct mmcsd_softc *sc = part->sc;
	device_t dev = sc->dev;
	int shift = mmc_get_high_cap(dev) ? 0 : 9;
	int rca = mmc_get_rca(dev);
	uint32_t buffer_count = blkreq->bufnum;
	uint32_t transfer_bytes = blkreq->bufs[0].length;
	uint32_t block_count = transfer_bytes / MMC_SECTOR_SIZE;
	uint32_t opcode;
	uint32_t data_flags;
	uint32_t i;

	if (blkreq->req == RTEMS_BLKDEV_REQ_WRITE) {
		if (block_count > 1) {
			opcode = MMC_WRITE_MULTIPLE_BLOCK;
		} else {
			opcode = MMC_WRITE_BLOCK;
		}

		data_flags = MMC_DATA_WRITE;
	} else {
		BSD_ASSERT(blkreq->req == RTEMS_BLKDEV_REQ_READ);

		if (block_count > 1) {
			opcode = MMC_READ_MULTIPLE_BLOCK;
		} else {
			opcode = MMC_READ_SINGLE_BLOCK;
		}

		data_flags = MMC_DATA_READ;
	}

	MMCSD_DISK_LOCK(part);

	for (i = 0; i < buffer_count; ++i) {
		rtems_blkdev_sg_buffer *sg = &blkreq->bufs [i];
		struct mmc_request req;
		struct mmc_command cmd;
		struct mmc_command stop;
		struct mmc_data data;
		rtems_interval timeout;

		memset(&req, 0, sizeof(req));
		memset(&cmd, 0, sizeof(cmd));
		memset(&stop, 0, sizeof(stop));

		req.cmd = &cmd;

		cmd.opcode = opcode;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		cmd.data = &data;
		cmd.arg = sg->block << shift;

		if (block_count > 1) {
			data_flags |= MMC_DATA_MULTI;
			stop.opcode = MMC_STOP_TRANSMISSION;
			stop.flags = MMC_RSP_R1B | MMC_CMD_AC;
			req.stop = &stop;
		}

		data.flags = data_flags;;
		data.data = sg->buffer;
		data.mrq = &req;
		data.len = transfer_bytes;
		data.block_count = 0;

		MMCBUS_WAIT_FOR_REQUEST(device_get_parent(dev), dev,
		    &req);
		if (req.cmd->error != MMC_ERR_NONE) {
			status_code = RTEMS_IO_ERROR;
			goto error;
		}

		timeout = rtems_clock_tick_later_usec(250000);
		while (1) {
			struct mmc_request req2;
			struct mmc_command cmd2;
			uint32_t status;

			memset(&req2, 0, sizeof(req2));
			memset(&cmd2, 0, sizeof(cmd2));

			req2.cmd = &cmd2;

			cmd2.opcode = MMC_SEND_STATUS;
			cmd2.arg = rca << 16;
			cmd2.flags = MMC_RSP_R1 | MMC_CMD_AC;

			MMCBUS_WAIT_FOR_REQUEST(device_get_parent(dev), dev,
			    &req2);
			if (req2.cmd->error != MMC_ERR_NONE) {
				status_code = RTEMS_IO_ERROR;
				goto error;
			}

			status = cmd2.resp[0];
			if ((status & R1_READY_FOR_DATA) != 0
			    && R1_CURRENT_STATE(status) != R1_STATE_PRG) {
				break;
			}

			if (!rtems_clock_tick_before(timeout)) {
				status_code = RTEMS_IO_ERROR;
				goto error;
			}
		}
	}

error:

	MMCSD_DISK_UNLOCK(part);

	rtems_blkdev_request_done(blkreq, status_code);

	return 0;
}

static int
rtems_bsd_mmcsd_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{

	if (req == RTEMS_BLKIO_REQUEST) {
		struct mmcsd_part *part = rtems_disk_get_driver_data(dd);
		rtems_blkdev_request *blkreq = arg;

		return rtems_bsd_mmcsd_disk_read_write(part, blkreq);
	} else if (req == RTEMS_BLKIO_CAPABILITIES) {
		*(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
		return 0;
	} else {
		return rtems_blkdev_ioctl(dd, req, arg);
	}
}

static rtems_status_code
rtems_bsd_mmcsd_attach_worker(rtems_media_state state, const char *src, char **dest, void *arg)
{
	rtems_status_code status_code = RTEMS_SUCCESSFUL;
	struct mmcsd_part *part = arg;
	char *disk = NULL;

	if (state == RTEMS_MEDIA_STATE_READY) {
		struct mmcsd_softc *sc = part->sc;
		device_t dev = sc->dev;
		uint32_t block_count = mmc_get_media_size(dev);
		uint32_t block_size = MMC_SECTOR_SIZE;

		disk = rtems_media_create_path("/dev", src, device_get_unit(dev));
		if (disk == NULL) {
			printf("OOPS: create path failed\n");
			goto error;
		}

		/*
		 * FIXME: There is no release for this acquire. Implementing
		 * this would be necessary for:
		 * - multiple hardware partitions of eMMC chips
		 * - multiple devices on one bus
		 *
		 * On the other hand it would mean that the bus has to be
		 * acquired on every read which would decrease the performance.
		 */
		MMCBUS_ACQUIRE_BUS(device_get_parent(dev), dev);

		status_code = rtems_bsd_mmcsd_set_block_size(dev, block_size);
		if (status_code != RTEMS_SUCCESSFUL) {
			printf("OOPS: set block size failed\n");
			goto error;
		}

		status_code = rtems_blkdev_create(disk, block_size,
		    block_count, rtems_bsd_mmcsd_disk_ioctl, part);
		if (status_code != RTEMS_SUCCESSFUL) {
			goto error;
		}

		*dest = strdup(disk, M_RTEMS_HEAP);
	}

	return RTEMS_SUCCESSFUL;

error:
	free(disk, M_RTEMS_HEAP);

	return RTEMS_IO_ERROR;
}

static void
rtems_mmcsd_static_attach(struct mmcsd_part *part)
{
	device_t dev = part->sc->dev;
	uint32_t block_count = mmc_get_media_size(dev);
	uint32_t block_size = MMC_SECTOR_SIZE;
	char path[MMCSD_PART_NAMELEN + 4];

	/*
	 * FIXME: There is no release for this acquire. Implementing
	 * this would be necessary for:
	 * - multiple hardware partitions of eMMC chips
	 * - multiple devices on one bus
	 *
	 * On the other hand it would mean that the bus has to be
	 * acquired on every read which would decrease the performance.
	 */
	MMCBUS_ACQUIRE_BUS(device_get_parent(dev), dev);

	rtems_status_code status_code =
		rtems_bsd_mmcsd_set_block_size(dev, block_size);
	BSD_ASSERT(status_code == RTEMS_SUCCESSFUL);

	snprintf(path, sizeof(path), "/dev/%s%d", part->name, part->cnt);
	status_code = rtems_blkdev_create(path, block_size,
			block_count, rtems_bsd_mmcsd_disk_ioctl, part);
	BSD_ASSERT(status_code == RTEMS_SUCCESSFUL);
}

static void
rtems_mmcsd_media_server_attach(struct mmcsd_part *part)
{
	rtems_status_code status_code = rtems_media_server_disk_attach(
			part->name, rtems_bsd_mmcsd_attach_worker, part);
	BSD_ASSERT(status_code == RTEMS_SUCCESSFUL);
}

int rtems_mmcsd_attach_set_hook(rtems_mmcsd_attach_hookfunction hookfp) {
	rtems_mmcsd_attach_fp = hookfp;
	return 0;
}

void rtems_mmcsd_use_media_server() {
	rtems_mmcsd_attach_set_hook(rtems_mmcsd_media_server_attach);
}

void rtems_mmcsd_attach(struct mmcsd_part *part) {
	if ( rtems_mmcsd_attach_fp ) {
		(*rtems_mmcsd_attach_fp)(part);
	}
}
