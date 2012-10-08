/*
 * RTEMS version of 
 */

#include <rtems.h>
#include <pthread_np.h>

__weak_reference(_pthread_main_np, pthread_main_np);

/*
 * Provide the equivalent to Solaris thr_main() function
 */
int
_pthread_main_np()
{
  if ( rtems_init_task_id == rtems_task_self() )
    return 1;
  else 
    return 0;
}
