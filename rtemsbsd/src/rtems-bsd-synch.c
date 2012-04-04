/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 * This violation is specifically for _Thread_Disable_dispatch
 * and _Thread_Enable_dispatch. Use of the critical_enter()
 * and critical_exit() routines should be reviewed.
 */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 

#include <freebsd/machine/rtems-bsd-config.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/ktr.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/proc.h>
#include <freebsd/machine/pcpu.h>

#define STATES_WAITING_FOR_SLEEP              0x40000

static int pause_wchan;

typedef struct
{
  Chain_Node node;
  void *ident;
  Thread_queue_Control queue;
}sleep_queue_control_t;

sleep_queue_control_t sleep_queue[BSD_MAXIMUM_SLEEP_QUEUES]; //this memory allocation could use _Workspace_Allocate once inside RTEMS tree
Chain_Control sleep_queue_inactive_nodes;  //chain of inactive nodes
Chain_Control sleep_queue_active_nodes;    //chain of active nodes

void
sleepinit(void)
{
  int ii;

  /* initialize the sleep queue */
  for( ii = 0; ii < BSD_MAXIMUM_SLEEP_QUEUES; ii++ )
  {
    sleep_queue[ii].ident = NULL;
    /*
     *  Initialize the queue we use to block for signals
     */
    _Thread_queue_Initialize(
      &sleep_queue[ii].queue,
      THREAD_QUEUE_DISCIPLINE_FIFO,
      STATES_WAITING_FOR_SLEEP | STATES_INTERRUPTIBLE_BY_SIGNAL,
      EAGAIN
    );
  }
  //initialize active chain
  _Chain_Initialize_empty( &sleep_queue_active_nodes );
  //initialize inactive chain
  _Chain_Initialize( &sleep_queue_inactive_nodes, sleep_queue, BSD_MAXIMUM_SLEEP_QUEUES, sizeof( sleep_queue_control_t ));
}

sleep_queue_control_t*
sleep_queue_lookup(void *ident)
{
  int ii;

  /* initialize the sleep queue */
  for( ii = 0; ii < BSD_MAXIMUM_SLEEP_QUEUES; ii++ )
  {
    if( sleep_queue[ii].ident == ident )
    {
      return &sleep_queue[ii];
    }
  }
  return NULL;
}

sleep_queue_control_t*
sleep_queue_get(void *ident)
{
  sleep_queue_control_t *sq;

  sq = sleep_queue_lookup( ident );
  if (sq == NULL)
  {
    KASSERT(!_Chain_Is_empty( &inactive_nodes ), ("sleep_queue_get"));
    //get a control from the inactive chain
    sq = ( sleep_queue_control_t * )_Chain_Get( &sleep_queue_inactive_nodes );
    sq->ident = ident;
    _Chain_Append( &sleep_queue_active_nodes, &sq->node );
  }
  return sq;
}

/*
 * Block the current thread until it is awakened from its sleep queue
 * or it times out while waiting.
 */
int
sleep_queue_timedwait(void *wchan, int pri, int timeout, int catch)
{
  sleep_queue_control_t *sq;
  Thread_Control *executing;
  ISR_Level       level;

  _Thread_Disable_dispatch();

  sq = sleep_queue_get( wchan );

  executing = _Thread_Executing;
  if( timeout )
  {
    executing->Wait.return_code = EWOULDBLOCK;
  }
  else
  {
    executing->Wait.return_code = 0;
  }
  _ISR_Disable( level );
  _Thread_queue_Enter_critical_section( &sq->queue );
  if( catch )
  {
    sq->queue.state |= STATES_INTERRUPTIBLE_BY_SIGNAL;
  }
  else
  {
    sq->queue.state &= ~STATES_INTERRUPTIBLE_BY_SIGNAL;
  }
  executing->Wait.queue = &sq->queue;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &sq->queue, timeout );
  _Thread_Enable_dispatch();
  return _Thread_Executing->Wait.return_code;
}

/*
 * General sleep call.  Suspends the current thread until a wakeup is
 * performed on the specified identifier.  The thread will then be made
 * runnable with the specified priority.  Sleeps at most timo/hz seconds
 * (0 means no timeout).  If pri includes PCATCH flag, signals are checked
 * before and after sleeping, else signals are not checked.  Returns 0 if
 * awakened, EWOULDBLOCK if the timeout expires.  If PCATCH is set and a
 * signal needs to be delivered, ERESTART is returned if the current system
 * call should be restarted if possible, and EINTR is returned if the system
 * call should be interrupted by the signal (return EINTR).
 *
 * The lock argument is unlocked before the caller is suspended, and
 * re-locked before _sleep() returns.  If priority includes the PDROP
 * flag the lock is not re-locked before returning.
 */
int
_sleep(void *ident, struct lock_object *lock, int priority, const char *wmesg, int timo)
{
  struct thread *td;
  struct proc *p;
  struct lock_class *class;
  int catch, flags, lock_state, pri, rval;

  td = curthread;
  p = td->td_proc;
#ifdef KTRACE
  if (KTRPOINT(td, KTR_CSW))
    ktrcsw(1, 0);
#endif
  KASSERT(timo != 0 || mtx_owned(&Giant) || lock != NULL,
      ("sleeping without a lock"));
  KASSERT(p != NULL, ("msleep1"));
  KASSERT(ident != NULL && TD_IS_RUNNING(td), ("msleep"));
  if (priority & PDROP)
    KASSERT(lock != NULL && lock != &Giant.lock_object,
        ("PDROP requires a non-Giant lock"));
  if (lock != NULL)
    class = LOCK_CLASS(lock);
  else
    class = NULL;

  if (cold) {
    /*
     * During autoconfiguration, just return;
     * don't run any other threads or panic below,
     * in case this is the idle thread and already asleep.
     * XXX: this used to do "s = splhigh(); splx(safepri);
     * splx(s);" to give interrupts a chance, but there is
     * no way to give interrupts a chance now.
     */
    if (lock != NULL && priority & PDROP)
      class->lc_unlock(lock);
    return (0);
  }
  catch = priority & PCATCH;
  pri = priority & PRIMASK;

  CTR5(KTR_PROC, "sleep: thread %ld (pid %ld, %s) on %s (%p)",
      td->td_tid, p->p_pid, td->td_name, wmesg, ident);

  if (lock == &Giant.lock_object)
    mtx_assert(&Giant, MA_OWNED);
  DROP_GIANT();
  if (lock != NULL && lock != &Giant.lock_object &&
      !(class->lc_flags & LC_SLEEPABLE)) {
    lock_state = class->lc_unlock(lock);
  } else
    /* GCC needs to follow the Yellow Brick Road */
    lock_state = -1;

  if (lock != NULL && class->lc_flags & LC_SLEEPABLE) {
    lock_state = class->lc_unlock(lock);
  }

  rval = sleep_queue_timedwait(ident, pri, timo, catch);

#ifdef KTRACE
  if (KTRPOINT(td, KTR_CSW))
    ktrcsw(0, 0);
#endif
  PICKUP_GIANT();
  if (lock != NULL && lock != &Giant.lock_object && !(priority & PDROP)) {
    class->lc_lock(lock, lock_state);
  }
  return (rval);
}

/*
 * pause() is like tsleep() except that the intention is to not be
 * explicitly woken up by another thread.  Instead, the current thread
 * simply wishes to sleep until the timeout expires.  It is
 * implemented using a dummy wait channel.
 */
int
pause(const char *wmesg, int timo)
{

  KASSERT(timo != 0, ("pause: timeout required"));
  return (tsleep(&pause_wchan, 0, wmesg, timo));
}

/*
 * Make all threads sleeping on the specified identifier runnable.
 */
void
wakeup(void *ident)
{
  sleep_queue_control_t *sq;
  Thread_Control *the_thread;

  sq = sleep_queue_lookup( ident );
  if (sq == NULL)
  {
    return (0);
  }

  while ( (the_thread = _Thread_queue_Dequeue(&sq->queue)) )
  {
  }
  return 0;
}

