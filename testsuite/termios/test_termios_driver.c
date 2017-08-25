/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <sys/types.h>
#include <sys/conf.h>
#include <sys/tty.h>
#include <sys/serial.h>
#include <assert.h>
#include <string.h>
#include <rtems/seterr.h>

#include "../termios/test_termios_driver.h"

static tsw_open_t test_termios_open;
static tsw_close_t test_termios_close;
static tsw_ioctl_t test_termios_ioctl;
static tsw_modem_t test_termios_modem;
static tsw_param_t test_termios_param;
static tsw_outwakeup_t test_termios_outwakeup;
static tsw_inwakeup_t test_termios_inwakeup;
static tsw_free_t test_termios_free;

static struct ttydevsw test_termios_class = {
  .tsw_flags = TF_INITLOCK | TF_CALLOUT,
  .tsw_open = test_termios_open,
  .tsw_close = test_termios_close,
  .tsw_outwakeup = test_termios_outwakeup,
  .tsw_inwakeup = test_termios_inwakeup,
  .tsw_ioctl = test_termios_ioctl,
  .tsw_param = test_termios_param,
  .tsw_modem = test_termios_modem,
  .tsw_free = test_termios_free,
};

struct test_termios_softc tt_softc;

static int
test_termios_open(struct tty *tp)
{
  return (0);
}

static void
test_termios_close(struct tty *tp)
{
}

static void
test_termios_outwakeup(struct tty *tp)
{
}

static void
test_termios_inwakeup(struct tty *tp)
{
}

static int
test_termios_ioctl(struct tty *tp, u_long cmd, caddr_t data, struct thread *td)
{
  return (ENOIOCTL);
}

static int
test_termios_param(struct tty *tp, struct termios *t)
{
  return (0);
}

static int
test_termios_modem(struct tty *tp, int sigon, int sigoff)
{
  struct test_termios_softc *sc = tty_softc(tp);

  if ((sigon == 0) && (sigoff == 0)) {
    if (sc->sc_mcr & SER_DTR) {
      sigon |= SER_DTR;
    }
    if (sc->sc_mcr & SER_RTS) {
      sigon |= SER_RTS;
    }
    if (sc->sc_msr & SER_CTS) {
      sigon |= SER_CTS;
    }
    if (sc->sc_msr & SER_DCD) {
      sigon |= SER_DCD;
    }
    if (sc->sc_msr & SER_DSR) {
      sigon |= SER_DSR;
    }
    if (sc->sc_msr & SER_RI) {
      sigon |= SER_RI;
    }
    return (sigon);
  }
  if (sigon & SER_DTR) {
    sc->sc_mcr |= SER_DTR;
  }
  if (sigoff & SER_DTR) {
    sc->sc_mcr &= ~SER_DTR;
  }
  if (sigon & SER_RTS) {
    sc->sc_mcr |= SER_RTS;
  }
  if (sigoff & SER_RTS) {
    sc->sc_mcr &= ~SER_RTS;
  }
  return (0);
}

static void
test_termios_free(void *xsc)
{
}

void
test_termios_make_dev(void)
{
  struct tty *tp = NULL;

  memset( &tt_softc, 0, sizeof( tt_softc ));
  tt_softc.sc_msr = SER_DCD | SER_CTS | SER_DSR;

  tp = tty_alloc_mutex(&test_termios_class, &tt_softc, NULL);
  assert(tp != NULL);
  tt_softc.tp = tp;

  tty_makedev( tp, NULL, "%s", TERMIOS_TEST_DRIVER_TTY_PREFIX TERMIOS_TEST_DRIVER_TTY_UNIT);
  assert(tp != NULL);
}
