#include <freebsd/machine/rtems-bsd-config.h>

/* Allocate space for global thread variables here: */
#define GLOBAL_PTHREAD_PRIVATE

#include <freebsd/sys/types.h>
#include <pthread.h>
#include "pthread_private.h"
