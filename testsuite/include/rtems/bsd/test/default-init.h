/*
 *  Shared Network Test Initialization File
 */

#ifndef RTEMS_BSD_TEST_DEFAULT_INIT_H
#define RTEMS_BSD_TEST_DEFAULT_INIT_H

#include <bsp.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtems/printer.h>
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
  rtems_printer printer;

  rtems_print_printer_printf(&printer);
  rtems_stack_checker_report_usage_with_plugin(&printer);

  if ( exit_code == 0 ) {
    puts( "*** END OF TEST " TEST_NAME " ***" );
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code sc;

  /*
   * Default the syslog priority to 'debug' to aid developers.
   */
  rtems_bsd_setlogpriority("debug");

  puts( "*** " TEST_NAME " TEST ***" );

  /*
   *  BSD must support the new "shared IRQ PIC implementation" at this point.
   *  BSPs must also provide rtems_interrupt_server_initialize() which
   *  just requires including irq-server.[ch] in their build.
   */

  on_exit( default_on_exit, NULL );

#ifdef DEFAULT_EARLY_INITIALIZATION
  early_initialization();
#endif

  /* Let other tasks run to complete background work */
  default_set_self_prio( RTEMS_MAXIMUM_PRIORITY - 1 );

  rtems_bsd_initialize();

  /* Let the callout timer allocate its resources */
  sc = rtems_task_wake_after( 2 );
  assert(sc == RTEMS_SUCCESSFUL);

  test_main();
  /* should not return */

  assert( 0 );
}

/*
 * Configure LibBSD.
 */
#define RTEMS_BSD_CONFIG_NET_PF_UNIX
#define RTEMS_BSD_CONFIG_NET_IF_LAGG
#define RTEMS_BSD_CONFIG_NET_IF_VLAN
#define RTEMS_BSD_CONFIG_INIT

#include <machine/rtems-bsd-config.h>

/*
 * Configure RTEMS.
 */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 32
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE (64 * 1024)
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 4
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (1 * 1024 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#endif /* RTEMS_BSD_TEST_DEFAULT_INIT_H */
