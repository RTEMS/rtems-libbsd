/*
 *  Simple test program -- simplified version of sample test hello.
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <freebsd/bsd.h>

/* XXX temporary until in .h file */
void rtems_initialize_interfaces(void);

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

  puts( "Initializing interfaces" );
  rtems_initialize_interfaces();

  puts( "Sleeping to see what happens" );
  sleep( 5 );

  printf( "*** END OF LIBFREEBSD INITIALIZATION TEST ***\n" );
  exit( 0 );
}

/* configuration information */

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/*
 * FreeBSD TCP/IP Initialization
 */

#include <freebsd/machine/rtems-bsd-sysinit.h>

#define CONFIGURE_NEED_NET
/* only include FXP and PCI for i386/pc386 for debug on qemu (for now) */
#if defined(i386)
  #define CONFIGURE_NEED_PCIB
  #define CONFIGURE_NEED_NET_IF_FXP
#endif

/*
 * This is correct for the PC
 */
char static_hints[] = {
  "hint.fxp.0.prefer_iomap=1\0\n"
};

#include "nic-sysinit.h"

/* end of file */
