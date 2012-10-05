/*
 * RTEMS version of 
 */

#include <rtems.h>

__weak_reference(_pthread_main_np, pthread_main_np);

/*
 * Provide the equivalent to Solaris thr_main() function
 */
int
_pthread_main_np()
{
  /* Created and set in rtems_bsd_initialize */
  extern rtems_id rtems_init_task_id;  

  if ( rtems_init_task_id == rtems_task_self() )
    return 1;
  else 
    return 0;
}
