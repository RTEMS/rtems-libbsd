#include <rtems/linkersets.h>

RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int class_count);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct name_list classes);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int fingerprint_count);
/* There is also one static buffer called "buf". But this can be ignored. See
 * comment in source file. */
