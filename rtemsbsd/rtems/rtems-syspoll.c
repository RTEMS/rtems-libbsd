#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <rtems.h>
#include <rtems/error.h>
#include <freebsd/sys/poll.h>

struct poll_args {
        struct pollfd *fds;
        u_int   nfds;
        int     timeout;
};

int kern_poll( struct thread *td, struct poll_args *uap );


int 
__sys_poll(struct pollfd *fds, unsigned nfds, int timeout)
{
  struct poll_args uap;
  struct thread *td = rtems_get_curthread();

  uap.fds = fds;
  uap.nfds = nfds;
  uap.timeout = timeout;

  kern_poll(td, &uap);

  return -1;
}
