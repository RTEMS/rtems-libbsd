#include <rtems/linkersets.h>

RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct altqs altqs);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct gen_sc rtsc);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static struct gen_sc lssc);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl,
    static char r2sbuf[R2S_BUFS][RATESTR_MAX]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_pfctl, static int r2sidx);
