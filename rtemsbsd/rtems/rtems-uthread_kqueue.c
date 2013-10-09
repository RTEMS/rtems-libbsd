#include <rtems/bsd/sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <rtems.h>
#include <rtems/error.h>

__weak_reference(_kqueue, kqueue);

int
_kqueue(void)
{
  rtems_panic("Unimplemented method!!!\n");
  return -1;
}
