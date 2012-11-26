#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/malloc.h>

#include <pthread.h>
#include "pthread_private.h"


static struct pthread *rtems_bsd_curpthread = NULL;


static void rtems_bsd_pthread_descriptor_dtor(void *td)
{
	// XXX are there other pieces to clean up?
	free(td, M_TEMP);
}

static struct pthread *
rtems_bsd_pthread_init( rtems_id id )
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	unsigned index = 0;
	struct pthread *td;

	td = _bsd_malloc( sizeof(struct pthread), M_TEMP, M_WAITOK | M_ZERO);
	if (td == NULL)
		return NULL;

	td->timeout = 0;
	td->data.poll_data = NULL;
	td->poll_data.nfds = 0;
	td->poll_data.fds = NULL;
	td->interrupted = 0;
        rtems_bsd_curpthread = td;

	// Now add the task descriptor as a per-task variable
	sc = rtems_task_variable_add(
		id,
		&rtems_bsd_curpthread,
		rtems_bsd_pthread_descriptor_dtor
	);
	if (sc != RTEMS_SUCCESSFUL) {
		free(td, M_TEMP);
		return NULL;
	}

  	return td;
}

/*
 */

struct pthread *
_get_curthread(void)
{
  struct pthread *td;

  /*
   * If we already have a struct thread associated with this thread,
   * obtain it. Otherwise, allocate and initialize one.
   */
  td = rtems_bsd_curpthread;
  if ( td == NULL ) {
    td = rtems_bsd_pthread_init( rtems_task_self() );
    if ( td == NULL ){
      panic("_get_curthread: Unable to create pthread\n");
    }
  }

  return td;
}

