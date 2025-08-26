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
#ifndef __RTEMSBSD_RTEMS_MMCSD_H__
#define __RTEMSBSD_RTEMS_MMCSD_H__

#include <rtems/rtemsmmcsd.h>

#include <sys/bus.h>
#include <sys/mutex.h>

#define	MMCSD_PART_NAMELEN	(16 + 1)

struct mmcsd_part {
	struct mtx disk_mtx;
	struct mtx ioctl_mtx;
	struct mmcsd_softc *sc;
#ifndef __rtems__
	struct disk *disk;
	struct proc *p;
	struct bio_queue_head bio_queue;
	daddr_t eblock, eend;	/* Range remaining after the last erase. */
#endif /* __rtems__ */
	u_int cnt;
	u_int type;
	int running;
	int suspend;
	int ioctl;
	bool ro;
	char name[MMCSD_PART_NAMELEN];
};

struct mmcsd_softc {
	device_t dev;
	device_t mmcbus;
	struct mmcsd_part *part[MMC_PART_MAX];
	enum mmc_card_mode mode;
	u_int max_data;		/* Maximum data size [blocks] */
	u_int erase_sector;	/* Device native erase sector size [blocks] */
	uint8_t	high_cap;	/* High Capacity device (block addressed) */
	uint8_t part_curr;	/* Partition currently switched to */
	uint8_t ext_csd[MMC_EXTCSD_SIZE];
	uint16_t rca;
	uint32_t flags;
#define	MMCSD_INAND_CMD38	0x0001
#define	MMCSD_USE_TRIM		0x0002
#define	MMCSD_FLUSH_CACHE	0x0004
#define	MMCSD_DIRTY		0x0008
	uint32_t cmd6_time;	/* Generic switch timeout [us] */
	uint32_t part_time;	/* Partition switch timeout [us] */
	off_t enh_base;		/* Enhanced user data area slice base ... */
	off_t enh_size;		/* ... and size [bytes] */
	int log_count;
	struct timeval log_time;
	struct cdev *rpmb_dev;
};

#define	MMCSD_DISK_LOCK(_part)		mtx_lock(&(_part)->disk_mtx)
#define	MMCSD_DISK_UNLOCK(_part)	mtx_unlock(&(_part)->disk_mtx)
#define	MMCSD_DISK_LOCK_INIT(_part)					\
	mtx_init(&(_part)->disk_mtx, (_part)->name, "mmcsd disk", MTX_DEF)
#define	MMCSD_DISK_LOCK_DESTROY(_part)	mtx_destroy(&(_part)->disk_mtx);
#define	MMCSD_DISK_ASSERT_LOCKED(_part)					\
	mtx_assert(&(_part)->disk_mtx, MA_OWNED);
#define	MMCSD_DISK_ASSERT_UNLOCKED(_part)				\
	mtx_assert(&(_part)->disk_mtx, MA_NOTOWNED);

#define	MMCSD_IOCTL_LOCK(_part)		mtx_lock(&(_part)->ioctl_mtx)
#define	MMCSD_IOCTL_UNLOCK(_part)	mtx_unlock(&(_part)->ioctl_mtx)
#define	MMCSD_IOCTL_LOCK_INIT(_part)					\
	mtx_init(&(_part)->ioctl_mtx, (_part)->name, "mmcsd IOCTL", MTX_DEF)
#define	MMCSD_IOCTL_LOCK_DESTROY(_part)	mtx_destroy(&(_part)->ioctl_mtx);
#define	MMCSD_IOCTL_ASSERT_LOCKED(_part)				\
	mtx_assert(&(_part)->ioctl_mtx, MA_OWNED);
#define	MMCSD_IOCLT_ASSERT_UNLOCKED(_part)				\
	mtx_assert(&(_part)->ioctl_mtx, MA_NOTOWNED);

#endif /* __RTEMSBSD_RTEMS_MMCSD_H__ */
