#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <rtems.h>
#include <rtems/error.h>

__weak_reference(_kevent, kevent);

int 
_kevent(int kq, const struct kevent *changelist, int nchanges,
    struct kevent *eventlist, int nevents, const struct timespec *timeout)
{
  rtems_panic("Unimplemented method!!!\n");
  return -1;
}
