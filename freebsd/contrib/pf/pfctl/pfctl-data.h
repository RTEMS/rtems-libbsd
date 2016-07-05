#include <rtems/linkersets.h>

RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, extern int altqsupport);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*anchoropt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *clearopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *debugopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, extern int dev);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int first_title);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*ifaceopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int labels);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, extern int loadopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *optiopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static struct pf_anchor_global pf_anchors);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *pf_device);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static struct pf_anchor pf_main_anchor);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*rulesopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *showopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*src_node_kill[2]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int src_node_killers);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*state_kill[2]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int state_killers);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static char*tableopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *tblcmdopt);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static const char *tblcmdopt);
