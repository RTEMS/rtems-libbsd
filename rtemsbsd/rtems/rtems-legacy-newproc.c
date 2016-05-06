/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * COPYRIGHT (c) 2012.
 * On-Line Applications Research Corporation (OAR).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include <rtems.h>
#include <rtems/bsd/bsd.h>

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
    rtems_bsd_get_task_priority(name),
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
