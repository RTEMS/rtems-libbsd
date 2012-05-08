/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  
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
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/callout.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_callout_chain);

static void
rtems_bsd_callout_dispatch(rtems_id id, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct callout *c = arg;

	if (c->c_lock != NULL) {
		sc = rtems_semaphore_obtain(c->c_lock->lo_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		BSD_ASSERT_SC(sc);
	}

	if (c->c_func != NULL) {
		(*c->c_func)(c->c_arg);
	}

	if (c->c_lock != NULL && (c->c_flags & CALLOUT_RETURNUNLOCKED) == 0) {
		sc = rtems_semaphore_release(c->c_lock->lo_id);
		BSD_ASSERT_SC(sc);
	}
}

void
callout_init(struct callout *c, int mpsafe)
{
	_callout_init_lock(c, mpsafe ? NULL : &Giant.lock_object, mpsafe ? CALLOUT_RETURNUNLOCKED : 0);
}

void
_callout_init_lock(struct callout *c, struct lock_object *lock, int flags)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_id id = RTEMS_ID_NONE;

	sc = rtems_timer_create(rtems_build_name('_', 'T', 'M', 'R'), &id);
	BSD_ASSERT_SC(sc);

	c->c_id = id;
	c->c_lock = lock;
	c->c_flags = flags;
	c->c_func = NULL;
	c->c_arg = NULL;

	rtems_chain_append(&rtems_bsd_callout_chain, &c->c_node);
}

int
callout_reset(struct callout *c, int to_ticks, void (*ftn)(void *), void *arg)
{
	/* FIXME: Integer conversions */

	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (to_ticks <= 0) {
		to_ticks = 1;
	}

	c->c_func = ftn;
	c->c_arg = arg;

	sc = rtems_timer_server_fire_after(c->c_id, (rtems_interval) to_ticks, rtems_bsd_callout_dispatch, c);
	BSD_ASSERT_SC(sc);

	return 0;
}

int
callout_schedule(struct callout *c, int to_ticks)
{
	return callout_reset(c, to_ticks, c->c_func, c->c_arg);
}

int
_callout_stop_safe(struct callout *c, int safe)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (!safe) {
		sc = rtems_timer_cancel(c->c_id);
		BSD_ASSERT_SC(sc);
	} else {
		sc = rtems_timer_delete(c->c_id);
		BSD_ASSERT_SC(sc);

		c->c_id = RTEMS_ID_NONE;
		rtems_chain_extract(&c->c_node);
	}

	return 0;
}
