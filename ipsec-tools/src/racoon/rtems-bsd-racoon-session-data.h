/* generated by userspace-header-gen.py */
#include <rtems/linkersets.h>
#include "rtems-bsd-racoon-data.h"
/* session.c */
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_racoon, static fd_set *allocated_active_mask);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_racoon, static fd_set *allocated_preset_mask);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_racoon, static int nfds);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_racoon, static struct fd_monitor *allocated_fd_monitors);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_racoon, static struct fd_monitor_list fd_monitor_tree[]);
