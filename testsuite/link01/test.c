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

#include <freebsd/machine/rtems-bsd-sysinit.h>

/*
 *  User says I need XXX
 */
#define CONFIGURE_NEED_NET
#define CONFIGURE_NEED_PCIB
#define CONFIGURE_NEED_NET_IF_FXP

/*
 *  We "read" that and generate references and nexus devices
 */
#if defined(CONFIGURE_NEED_NET)
  SYSINIT_NEED_FREEBSD_CORE;
#endif

  SYSINIT_NEED_USB_CORE;
#if defined(CONFIGURE_NEED_PCIB)
  SYSINIT_NEED_PCIB;
#endif

#if defined(CONFIGURE_NEED_NET_IF_FXP)
  SYSINIT_NEED_NET_IF_FXP;
#endif
#if defined(CONFIGURE_NEED_NET_IF_DC)
  SYSINIT_NEED_NET_IF_DC;
#endif

#if 0
SYSINIT_NEED_NET_IF_BFE;
SYSINIT_NEED_NET_IF_RE;
SYSINIT_NEED_NET_IF_EM;
SYSINIT_NEED_NET_IF_IGB;
SYSINIT_NEED_NET_IF_BCE;
SYSINIT_NEED_NET_IF_LEM;

SYSINIT_NEED_NET_IF_BGE; // does not link 23 May 2012
#endif


const char *const _bsd_nexus_devices [] = {
	#if defined(CONFIGURE_NEED_PCIB)
		"pcib",
	#endif
	#if defined(CONFIGURE_NEED_NET_IF_FXP)
		"fxp",
	#endif
	NULL
};

/* end of file */
