/*
 *  Shared Network Test Initialization File
 */

#ifndef RTEMS_BSD_TEST_DEFAULT_INIT_H
#define RTEMS_BSD_TEST_DEFAULT_INIT_H

#include <bsp.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtems/stackchk.h>
#include <rtems/bsd/bsd.h>

static void default_set_self_prio( rtems_task_priority prio )
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  assert(sc == RTEMS_SUCCESSFUL);
}

static void default_on_exit( int exit_code, void *arg )
{
  rtems_stack_checker_report_usage_with_plugin(
    NULL,
    rtems_printf_plugin
  );

  if ( exit_code == 0 ) {
    puts( "*** END OF TEST " TEST_NAME " ***" );
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code sc;

  puts( "*** " TEST_NAME " TEST ***" );

  /*
   *  BSD must support the new "shared IRQ PIC implementation" at this point.
   *  BSPs must also provide rtems_interrupt_server_initialize() which
   *  just requires including irq-server.[ch] in their build.
   */

  on_exit( default_on_exit, NULL );

  /* Let other tasks run to complete background work */
  default_set_self_prio( RTEMS_MAXIMUM_PRIORITY - 1 );

  rtems_bsd_initialize_with_interrupt_server();

  /* Let the callout timer allocate its resources */
  sc = rtems_task_wake_after( 2 );
  assert(sc == RTEMS_SUCCESSFUL);

  test_main();
  /* should not return */

  assert( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 32
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (128 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/*
 * FreeBSD TCP/IP Initialization
 */

#include <machine/rtems-bsd-sysinit.h>

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

#include <rtems/bsd/test/nic-sysinit.h>

/* end of file */
#endif /* RTEMS_BSD_TEST_DEFAULT_INIT_H */
