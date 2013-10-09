

#include <err.h>

rtems_shell_globals_t *rtems_shell_globals;

void
rtems_shell_exit (int code)
{
  rtems_shell_globals->exit_code = code;
  longjmp (rtems_shell_globals->exit_jmp, 1);
}


