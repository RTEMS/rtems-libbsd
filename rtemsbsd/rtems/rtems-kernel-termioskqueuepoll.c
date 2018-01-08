/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.
 * All rights reserved.
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

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/event.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/pcpu.h>
#include <sys/poll.h>
#include <sys/selinfo.h>

#include <rtems/termiostypes.h>
#include <rtems/irq-extension.h>

SYSINIT_REFERENCE(irqs);

typedef struct {
	rtems_termios_tty *tty;
	struct selinfo sel;
	rtems_interrupt_server_request request;
} termios_selinfo;

static bool
termios_is_eol(const rtems_termios_tty *tty, char c)
{

	return (c == '\n' || c == tty->termios.c_cc[VEOF] ||
	    c == tty->termios.c_cc[VEOL] ||
	    c == tty->termios.c_cc[VEOL2]);
}

static bool
termios_can_read(rtems_termios_tty *tty)
{
	rtems_termios_device_context *ctx;
	rtems_interrupt_lock_context lock_context;
	unsigned int i;
	unsigned int size;
	unsigned int raw_content_size;
	bool can;

	if (tty->handler.mode == TERMIOS_POLLED) {
		return (true);
	}

	if (tty->cindex != tty->ccount) {
		return (true);
	}

	ctx = tty->device_context;
	rtems_termios_device_lock_acquire(ctx, &lock_context);
	i = tty->rawInBuf.Head;
	size = tty->rawInBuf.Size;
	raw_content_size = (tty->rawInBuf.Tail - i) % size;

	if ((tty->termios.c_lflag & ICANON) != 0) {
		unsigned int todo = raw_content_size;

		/*
		 * FIXME: What to do in case of a raw input buffer overflow?
		 * For now, indicated that we can read.  However, this has
		 * problems in case an erase takes place.
		 */
		can = raw_content_size == (size - 1);

		while (todo > 0 && !can) {
			char c;

			i = (i + 1) % size;
			c = tty->rawInBuf.theBuf[i];
			can = termios_is_eol(tty, c);
			--todo;
		}
	} else {
		cc_t vmin = tty->termios.c_cc[VMIN];

		if (vmin == 0) {
			vmin = 1;
		}

		can = raw_content_size >= vmin;
	}

	if (!can) {
		tty->tty_rcvwakeup = false;
	}

	rtems_termios_device_lock_release(ctx, &lock_context);
	return (can);
}

static bool
termios_can_write(const rtems_termios_tty *tty)
{
	rtems_termios_device_context *ctx;
	rtems_interrupt_lock_context lock_context;
	bool can;

	if (tty->handler.mode == TERMIOS_POLLED) {
		return (true);
	}

	ctx = tty->device_context;
	rtems_termios_device_lock_acquire(ctx, &lock_context);
	can = ((tty->rawOutBuf.Tail - tty->rawOutBuf.Head - 1) %
	    tty->rawOutBuf.Size) > 0;
	rtems_termios_device_lock_release(ctx, &lock_context);
	return (can);
}

static void
termios_receive_wakeup(void *arg)
{
	termios_selinfo *ts;
	rtems_termios_tty *tty;

	ts = arg;
	tty = ts->tty;

	rtems_mutex_lock(&tty->isem);
	selwakeup(&ts->sel);
	rtems_mutex_unlock(&tty->isem);
}

static void
termios_transmit_wakeup(void *arg)
{
	termios_selinfo *ts;
	rtems_termios_tty *tty;

	ts = arg;
	tty = ts->tty;

	rtems_mutex_lock(&tty->osem);
	selwakeup(&ts->sel);
	rtems_mutex_unlock(&tty->osem);
}

static void
termios_wakeup(struct termios *term, void *arg)
{
	termios_selinfo *ts = arg;

	rtems_interrupt_server_request_submit(&ts->request);
}

static struct selinfo *
termios_get_selinfo(rtems_termios_tty *tty, struct ttywakeup *wk,
    rtems_interrupt_handler handler)
{
	termios_selinfo *ts = wk->sw_arg;

	if (ts == NULL) {
		BSD_ASSERT(wk->sw_pfn == NULL);
		ts = malloc(sizeof(*ts), M_TEMP, M_WAITOK | M_ZERO);
		ts->tty = tty;
		rtems_interrupt_server_request_initialize(RTEMS_INTERRUPT_SERVER_DEFAULT,
		    &ts->request, handler, ts);
		wk->sw_arg = ts;
		wk->sw_pfn = termios_wakeup;
	} else {
		BSD_ASSERT(wk->sw_pfn == termios_wakeup);
	}

	return (&ts->sel);
}

int
rtems_termios_kqfilter(rtems_libio_t *iop, struct knote  *kn)
{

	return (EINVAL);
}

int
rtems_termios_poll(rtems_libio_t *iop, int events)
{
	struct thread *td = rtems_bsd_get_curthread_or_wait_forever();
	struct selinfo *sel;
	rtems_termios_tty *tty;
	int revents;

	revents = 0;
	tty = iop->data1;

	if ((events & (POLLIN | POLLRDNORM)) != 0) {
		sel = termios_get_selinfo(tty, &tty->tty_rcv,
		    termios_receive_wakeup);

		rtems_mutex_lock(&tty->isem);

		if (termios_can_read(tty)) {
			revents |= events & (POLLIN | POLLRDNORM);
		} else {
			selrecord(td, sel);
		}

		rtems_mutex_unlock(&tty->isem);
	}

	if ((events & (POLLOUT | POLLWRNORM)) != 0) {
		sel = termios_get_selinfo(tty, &tty->tty_snd,
		    termios_transmit_wakeup);

		rtems_mutex_lock(&tty->osem);

		if (termios_can_write(tty)) {
			revents |= events & (POLLOUT | POLLWRNORM);
		} else {
			selrecord(td, sel);
		}

		rtems_mutex_unlock(&tty->osem);
	}

	return (revents);
}

static void
termioskqueuepoll_sysinit(void)
{

	/* Do nothing */
}

SYSINIT(termioskqueuepoll, SI_SUB_TUNABLES, SI_ORDER_ANY,
    termioskqueuepoll_sysinit, NULL);
