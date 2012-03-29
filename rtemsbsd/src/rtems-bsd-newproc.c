/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>
#include <stdio.h>
#include <rtems.h>


static uint32_t   networkDaemonPriority = 100; /* XXX */

/*
 * Structure passed to task-start stub
 */
struct newtask {
  void (*entry)(void *);
  void *arg;
};

/*
 * Task-start stub
 */
static void
taskEntry (rtems_task_argument arg)
{
  struct newtask t;

  /*
   * Pick up task information and free
   * the memory allocated to pass the
   * information to this task.
   */
  t = *(struct newtask *)arg;
  free ((struct newtask *)arg);

  /*
   * XXX  If we need a semaphore it should be added here
   */

  /*
   * Enter the task
   */
  (*t.entry)(t.arg);
  rtems_panic ("Network task returned!\n");
}

/*
 * Start a network task
 */
rtems_id
rtems_bsdnet_newproc (char *name, int stacksize, void(*entry)(void *), void *arg)
{
  struct newtask *t;
  char nm[4];
  rtems_id tid;
  rtems_status_code sc;

  strncpy (nm, name, 4);
  sc = rtems_task_create (rtems_build_name(nm[0], nm[1], nm[2], nm[3]),
    networkDaemonPriority,
    stacksize,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &tid
  );
  if (sc != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't create network daemon `%s': `%s'\n", name, rtems_status_text (sc));

  /*
   * Set up task arguments
   */
  t = malloc (sizeof *t);
  t->entry = entry;
  t->arg = arg;

  /*
   * Start the task
   */
  sc = rtems_task_start (tid, taskEntry, (rtems_task_argument)t);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't start network daemon `%s': `%s'\n", name, rtems_status_text (sc));

  /*
   * Let our caller know the i.d. of the new task
   */
  return tid;
}
