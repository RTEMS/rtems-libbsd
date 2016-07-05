#include <rtems/linkersets.h>

RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int add_opt_table_num);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int pf_opt_create_table_num);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static struct pf_rule_field pf_rule_desc[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int
    (*skip_comparitors[PF_SKIP_COUNT])(struct pf_rule *, struct pf_rule *));
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static const char *skip_comparitors_names[PF_SKIP_COUNT]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static struct pfr_buffer table_buffer);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int table_identifier);
