/* generated by userspace-header-gen.py */
#include <rtems/linkersets.h>
#include "rtems-bsd-route-data.h"
/* rehash.c */
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static BUCKET *hash_table[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static BUCKET nilbucket);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static EVP_MD const *evpmd);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static HENTRY nilhentry);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char const *extensions[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char const *suffixes[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int evpmdsize);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int remove_links);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int verbose);
