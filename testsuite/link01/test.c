/*
 *  Simple test program -- simplified version of sample test hello.
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <freebsd/bsd.h>

/* 
 * Methods used to make sure the nic drivers
 * are pulled into the executable.
 */
extern int _bsd_re_pcimodule_sys_init();
extern int _bsd_fxp_pcimodule_sys_init();
extern int _bsd_dc_pcimodule_sys_init();
extern int _bsd_em_pcimodule_sys_init();
extern int _bsd_igb_pcimodule_sys_init();
extern int _bsd_bce_pcimodule_sys_init();
extern int _bsd_lem_pcimodule_sys_init();
extern int _bsd_bfe_pcimodule_sys_init();
extern int _bsd_bge_pcimodule_sys_init();

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

  printf("Nic Driver Addresses\n");
  printf("RealTek %p\n", &_bsd_re_pcimodule_sys_init );
  printf("EtherExpress %p\n", &_bsd_fxp_pcimodule_sys_init );
  printf("DEC tulip %p\n", &_bsd_dc_pcimodule_sys_init );
  printf("Broadcom BCM570XX/BCM571XX %p\n", &_bsd_bce_pcimodule_sys_init );
  printf("Broadcom BCM4401 %p\n", &_bsd_bfe_pcimodule_sys_init );

#if 0
  printf("Broadcom BCM570x %p\n", &_bsd_bge_pcimodule_sys_init );
  printf("E1000 XXX %p\n", &_bsd_igb_pcimodule_sys_init );
  printf("E1000 XXX %p\n", &_bsd_em_pcimodule_sys_init );
#endif

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
/* end of file */
