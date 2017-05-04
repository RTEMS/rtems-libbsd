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
  default_set_self_prio( RTEMS_MAXIMUM_PRIORITY - 2 );

  rtems_bsd_initialize();

  /* Let the callout timer allocate its resources */
  sc = rtems_task_wake_after( 2 );
  assert(sc == RTEMS_SUCCESSFUL);

  test_main();
  /* should not return */

  assert( 0 );
}

/*
 * Configure RTEMS.
 */
#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_DRIVERS 32

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE (64 * 1024)
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (256 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/*
 * Configure LIBBSD device.
 */
 #include <bsp/nexus-devices.h>

/*
 * Configure RTEMS Shell.
 */
#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_STTY_Command, \
  &rtems_shell_SYSCTL_Command

#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE
#define CONFIGURE_SHELL_COMMAND_PROFREPORT

#define CONFIGURE_SHELL_COMMAND_CP
#define CONFIGURE_SHELL_COMMAND_PWD
#define CONFIGURE_SHELL_COMMAND_LS
#define CONFIGURE_SHELL_COMMAND_LN
#define CONFIGURE_SHELL_COMMAND_LSOF
#define CONFIGURE_SHELL_COMMAND_CHDIR
#define CONFIGURE_SHELL_COMMAND_CD
#define CONFIGURE_SHELL_COMMAND_MKDIR
#define CONFIGURE_SHELL_COMMAND_RMDIR
#define CONFIGURE_SHELL_COMMAND_CAT
#define CONFIGURE_SHELL_COMMAND_MV
#define CONFIGURE_SHELL_COMMAND_RM
#define CONFIGURE_SHELL_COMMAND_MALLOC_INFO

#define CONFIGURE_SHELL_COMMAND_BLKSTATS
#define CONFIGURE_SHELL_COMMAND_BLKSYNC
#define CONFIGURE_SHELL_COMMAND_MOUNT
#define CONFIGURE_SHELL_COMMAND_UNMOUNT

#include <rtems/shellconfig.h>

#endif /* RTEMS_BSD_TEST_DEFAULT_INIT_H */