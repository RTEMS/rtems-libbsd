/*
 *  Simple test program -- simplified version of sample test hello.
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <freebsd/bsd.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  printf( "\n\n*** LIBFREEBSD INITIALIZATION TEST ***\n" );
  /*
   *  BSD must support the new "shared IRQ PIC implementation" at this point.
   *  BSPs must also provide rtems_interrupt_server_initialize() which
   *  just requires including irq-server.[ch] in their build.
   */

  rtems_bsd_initialize_with_interrupt_server();

  printf( "*** END OF LIBFREEBSD INITIALIZATION TEST ***\n" );
  exit( 0 );
}

/* configuration information */

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#include <freebsd/machine/rtems-bsd-sysinit.h>

SYSINIT_NEED_FREEBSD_CORE;
SYSINIT_NEED_NET_IF_BFE;
SYSINIT_NEED_NET_IF_RE;
SYSINIT_NEED_NET_IF_EM;
SYSINIT_NEED_NET_IF_IGB;
SYSINIT_NEED_NET_IF_LEM;
SYSINIT_NEED_NET_IF_BCE;
SYSINIT_NEED_NET_IF_BGE;
SYSINIT_NEED_NET_IF_FXP;
SYSINIT_NEED_NET_IF_DC;

const char *const _bsd_nexus_devices [] = {
	NULL
};

/* end of file */
