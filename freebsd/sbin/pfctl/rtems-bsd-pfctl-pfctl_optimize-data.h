/* generated by userspace-header-gen.py */
#include <rtems/linkersets.h>
#include "rtems-bsd-pfctl-data.h"
/* pfctl_optimize.c */
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char const *skip_comparitors_names[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int (*skip_comparitors[])(struct pfctl_rule *, struct pfctl_rule *));
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int add_opt_tablenum);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int pf_opt_create_tablenum);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int table_identifier);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct pf_rule_field pf_rule_desc[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct pfr_buffer table_buffer);
